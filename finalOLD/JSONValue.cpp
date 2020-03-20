
/*-------------------------------------------------------------------------*
 *---									---*
 *---		JSONValue.cpp						---*
 *---									---*
 *---	    This file defines members for classes for reading JSON	---*
 *---	text.  See https://www.json.org/.				---*
 *---									---*
 *---		This is a declassified version of an original		---*
 *---			CONFIDENTIAL source file owned by:		---*
 *---			Applied Philosophy of Science			---*
 *---									---*
 *---	----	----	----	----	----	----	----	-----	---*
 *---									---*
 *---	Version 2		2019 October 22		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"serverHeader.h"

//  PURPOSE:  To return the current char, or -1 if there are no more.
//	No parameters.
int		JSONValue::InputCharStream::peek
				()
{
  //  I.  Application validity check:

  //	II.  Attempt to get current char:
  //  II.A.  Each iteration handles when chars are not immediately available:
  while  (bufCursor_ >= bufJustBeyondLast_)
  {
    //  II.A.1.  Handle when know there are no more chars:
    if  (haveSeenEof_)
    {
      return(EOF);
    }

    //  II.A.2.  Attempt to get next buffer of chars from 'inFd_':
    int	numChars;

    if  ( (numChars = read(inFd_,buffer_,BUFFER_LEN)) < 0 )
    {
      fprintf(stderr,"read() failed for JSON source file: %s\n",
	      strerror(errno)
	     );
      exit(EXIT_FAILURE);
    }

    haveSeenEof_	 = (numChars < BUFFER_LEN);
    bufCursor_	 = buffer_;
    bufJustBeyondLast_ = buffer_ + numChars;
  }

  //  III.  Finished:
  return((int)*bufCursor_);
}


//  PURPOSE:  To advance to the next char (if not already at end).  No
//	parameters.  No return value.
void		JSONValue::InputCharStream::advance
				()
{
  //  I.  Application validity check:

  //  II.  Advance '*this':
  int	c	= peek();

  if  (c != EOF)
  {
    bufCursor_++;
  }

  //  III.  Finished:
}


//  PURPOSE:  To represent the occurence of EOF.
JSONSyntacticElement
		JSONValue::Tokenizer::eof__(EOF);


//  PURPOSE:  To represent the occurence of begin curly brace.
JSONSyntacticElement
		JSONValue::Tokenizer::beginCurlyBrace__((int)'{');


//  PURPOSE:  To represent the occurence of end curly brace.
JSONSyntacticElement
		JSONValue::Tokenizer::endCurlyBrace__((int)'}');


//  PURPOSE:  To represent the occurence of begin square bracket.
JSONSyntacticElement
		JSONValue::Tokenizer::beginSquareBracket__((int)'[');


//  PURPOSE:  To represent the occurence of end square bracket.
JSONSyntacticElement
		JSONValue::Tokenizer::endSquareBracket__((int)']');


//  PURPOSE:  To represent the occurence of comma.
JSONSyntacticElement
		JSONValue::Tokenizer::comma__((int)',');


//  PURPOSE:  To represent the occurence of colon.
JSONSyntacticElement
		JSONValue::Tokenizer::colon__((int)':');


//  PURPOSE:  To return a pointer to a JSONNumber representing the number
//  	scanned from 'inputStream_' whose first char begins with 'c'.
JSONValue*	JSONValue::Tokenizer::scanNumber
				(char	firstC
				)
{
  //  I.  Application validity check:

  //  II.  Read number:
  bool		isInteger	  = true;
  bool		isNegative	  = false;
  bool		haveSeenDecimalPt = false;
  bool		haveSeenExponentE = false;
  bool		haveSeenDigit	  = false;
  long long	integer		  = 0;
  std::string		text;

  //  II.A.  Handle 'firstC':
  text	+= firstC;

  if  (firstC == '-')
  {
    isNegative	= true;
  }
  else
  if  (firstC == '+')
  {
  }
  else
  if  ( isdigit(firstC) )
  {
    integer		= firstC - '0';
    haveSeenDigit	= true;
  }
  else
  if  (firstC == '.')
  {
    isInteger		= false;
    haveSeenDecimalPt	= true;
  }
  else
  {
    fprintf(stderr,
	    "Non-handled char '%c' passed to "
	    "JSONValue::Tokenizer::scanNumber()",
	     firstC
	    );
    exit(EXIT_FAILURE);
  }

  //  II.B.  Handle subsequent chars:
  char	subsequentC;
  bool	isFinishedReading	= false;

  //  II.B.1.  Handle reading digits of integer:
  if  (isInteger)
  {

    while  ( !inputStream_.isAtEnd() )
    {
      subsequentC	= inputStream_.peek();

      if  ( isdigit(subsequentC) )
      {
	haveSeenDigit	 = true;
	integer		 = (10 * integer) + (subsequentC - '0');
	text		+= subsequentC;
	inputStream_.advance();
      }
      else
      if  (subsequentC == '.')
      {
	text			+= subsequentC;
	haveSeenDecimalPt	 = true;
	isInteger		 = false;
	inputStream_.advance();
	break;
      }
      else
      if  ( (subsequentC == 'e')  ||  (subsequentC == 'E') )
      {
	text			+= subsequentC;
	haveSeenExponentE	 = true;
	isInteger		 = false;
	inputStream_.advance();
	break;
      }
      else
      {
	isFinishedReading	= true;
	break;
      }

    }
  }

  //  II.B.1.  Handle reading chars of floating point:
  if  (!isFinishedReading)
  {
    while  ( !inputStream_.isAtEnd() )
    {
      char	prevC	= subsequentC;

      subsequentC	= inputStream_.peek();

      if  ( isdigit(subsequentC) )
      {
	haveSeenDigit	 = true;
	text		+= subsequentC;
	inputStream_.advance();
      }
      else
      if  (subsequentC == '.')
      {
	if  ( haveSeenDecimalPt || haveSeenExponentE )
	{
	  break;
	}

	text			+= subsequentC;
	haveSeenDecimalPt	 = true;
	inputStream_.advance();
      }
      else
      if  ( (subsequentC == 'e')  ||  (subsequentC == 'E') )
      {
	if  ( !haveSeenDigit )
	{
	  fprintf(stderr,
		  "Missing digit(s) before exponent of JSON floating pt number"
		 );
	  exit(EXIT_FAILURE);
	}

	if  (haveSeenExponentE)
	{
	  break;
	}

	haveSeenDigit		 = false;
	text			+= subsequentC;
	haveSeenExponentE	 = true;
	inputStream_.advance();
      }
      else
      if  ( (subsequentC == '+') || (subsequentC == '-') )
      {
	if  ( (prevC != 'e')  &&  (prevC != 'E') )
	{
	  break;
	}

	text			+= subsequentC;
	inputStream_.advance();	  
      }
      else
      {
	isFinishedReading	= true;
	break;
      }
    }
  }

  //  III.  Finished:
  if  ( !haveSeenDigit )
  {
    if  ( haveSeenExponentE )
    {
      fprintf(stderr,
	      "Missing digit(s) in exponent of JSON floating pt number"
	     );
      exit(EXIT_FAILURE);
    }
    else
    {
      fprintf(stderr,"Missing digits in JSON number");
      exit(EXIT_FAILURE);
    }
  }

  return( isInteger
	  ? new JSONNumber
			(text,
			 isNegative
			  ? (long long)-integer
			  : (long long)+integer
			)
	  : new JSONNumber(text,strtod(text.c_str(),NULL))
	);
}


//  PURPOSE:  To return a pointer to a JSONValue representing the string
//  	scanned from 'inputStream_' whose beginning QUOTE_CHAR has
//	already been read.
JSONValue*	JSONValue::Tokenizer::scanString
				()
{
  //  I.  Application validity check:

  //  II.  Parse string:
  std::string	text;
  char		c;

  while  ( !inputStream_.isAtEnd() )
  {
    c	= inputStream_.peek();

    inputStream_.advance();

    if  (c == QUOTE_CHAR)
    {
      break;
    }
    else
    if  (c == '\\')
    {
      if  ( inputStream_.isAtEnd() )
      {
	fprintf(stderr,"Illegal escape char sequence");
	exit(EXIT_FAILURE);
      }

      c	= inputStream_.peek();
      inputStream_.advance();

      switch  (c)
      {
      case QUOTE_CHAR :
	text += QUOTE_CHAR;
	break;
      case '\\' :
	text += '\\';
	break;
      case '/' :
	text += '/';
	break;
      case 'b' :
	text += '\b';
	break;
      case 'f' :
	text += '\f';
	break;
      case 'n' :
	text += '\n';
	break;
      case 'r' :
	text += '\r';
	break;
      case 't' :
	text += '\t';
	break;
      default :
	fprintf(stderr,"Illegal escape char sequence");
	exit(EXIT_FAILURE);
      }

    }
    else
    {
      text	+= c;
    }
  }

  //  III.  Finished:
  if  (text == "true")
  {
    return(new JSONTrue());
  }

  if  (text == "false")
  {
    return(new JSONFalse());
  }

  if  (text == "null")
  {
    return(new JSONNull());
  }

  return(new JSONString(text));
}


//  PURPOSE:  To scan 'null' or 'true' or 'false' appearing outside of quotes.
//	Return address of JSONValue instance corresponding to what was scanned.
//	'c' tells the character already scanned.
JSONValue*	JSONValue::Tokenizer::scanNullTrueFalse
				(char		c
				)
{
  //  I.  Application validity check:

  //  II.  Scan text:
  std::string		text;

  text	+= c;

  while  ( !inputStream_.isAtEnd() && isalpha(c = inputStream_.peek()) )
  {
    text	+= c;
    inputStream_.advance();
  }

  //  III.  Finished:
  if  (text == "true")
  {
    return(new JSONTrue());
  }

  if  (text == "false")
  {
    return(new JSONFalse());
  }

  if  (text == "null")
  {
    return(new JSONNull());
  }

  fprintf(stderr,"Unexpected non-quoted text '%s' while reading JSON.\n",
	  text.c_str()
	 );
  exit(EXIT_FAILURE);
}


//  PURPOSE:  To return a pointer representing a scanned JSONValue.
//	No parameters.
JSONValue*	JSONValue::Tokenizer::scanner
				()
{
  //  I.  Application validity check:

  //  II.  Get next token:
  //  II.A.  Fast-forward past spaces:
  while  ( !inputStream_.isAtEnd() )
  {
    char	c = inputStream_.peek();

    inputStream_.advance();

    switch  (c)
    {
    case ' ' :
    case '\n' :
    case '\t' :
    case '\v' :
    case '\f' :
    case '\r' :
      continue;

    case '-' :
    case '+' :
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      return(scanNumber(c));

    case 'n' :
    case 't' :
    case 'f' :
      return(scanNullTrueFalse(c));

    case QUOTE_CHAR :
      return(scanString());

    case '{' :
      return(&beginCurlyBrace__);

    case '}' :
      return(&endCurlyBrace__);

    case '[' :
      return(&beginSquareBracket__);

    case ']' :
      return(&endSquareBracket__);

    case ',' :
      return(&comma__);

    case ':' :
      return(&colon__);

    case '\0' :
      return(&eof__);

    default :
      fprintf(stderr,"Unexpected char '%c' (ASCII = %d) while reading JSON.\n",
	      c,c
	     );
      exit(EXIT_FAILURE);
    }
  }

  //  III.  Finished:
  return(&eof__);
}


//  PURPOSE:  To return the address of a JSONObject read from 'tokenizer'.
JSONValue*	JSONValue::parseObject
				(Tokenizer&	tokenizer
				)
{
  //  I.  Application validity check:

  //  II.  Parse the JSON object:
  JSONValue*	readPtr;
  JSONObject*	toReturn	= new JSONObject();

  if  (tokenizer.peek() == '}')
  {
    tokenizer.advance();
  }
  else
  {
    std::string	key;

    while  (true)
    {
      //  Get key string:
      readPtr = parseValue(tokenizer);
      key	= readPtr->getString(false);
      delete(readPtr);

      //  Get colon:
      if  (tokenizer.peek() != ':')
      {
	fprintf(stderr,"Expected ':' while reading JSON");
	exit(EXIT_FAILURE);
      }

      tokenizer.advance();

      //  Get value:
      readPtr = parseValue(tokenizer);

//    if  ( !isLegalJsonType(readPtr->getType()) )
//    {
//	fprintf(stderr,"Expected JSON value");
//	exit(EXIT_FAILURE);
//    }

      //  Add to map:
      toReturn->add(key,readPtr);

      //  Get next syntactic element to see if should continue or quit:
      if  (tokenizer.peek() == ',')
      {
	tokenizer.advance();
	continue;
      }
      else
      if  (tokenizer.peek() == '}')
      {
	tokenizer.advance();
	break;
      }
      else
      {
	fprintf(stderr,"Expected '}' or ',' while reading JSON object");
	exit(EXIT_FAILURE);
      }

    }

  }

  //  III.  Finished:
  return(toReturn);
}


//  PURPOSE:  To return the address of JSONArray read from 'tokenizer'.
JSONValue*	JSONValue::parseArray
				(Tokenizer&	tokenizer
				)
{
  //  I.  Application validity check:

  //  II.  Parse the JSON array:
  JSONValue*	readPtr;
  JSONArray*	toReturn	= new JSONArray();

  if  (tokenizer.peek() == ']')
  {
    tokenizer.advance();
  }
  else
  {
    while  (true)
    {
      //  Get value:
      readPtr = parseValue(tokenizer);

//	if  ( !isLegalJsonType(readPtr->getType()) )
//	{
//	  fprintf(stderr,"Expected JSON value");
//	  exit(EXIT_FAILURE);
//	}

      //  Add to array:
      toReturn->add(readPtr);

      //  Get next syntactic element to see if should continue or quit:
      if  (tokenizer.peek() == ',')
      {
	tokenizer.advance();
	continue;
      }
      else
      if  (tokenizer.peek() == ']')
      {
	tokenizer.advance();
	break;
      }
      else
      {
	fprintf(stderr,"Expected ']' or ',' while parsing JSON");
	exit(EXIT_FAILURE);
      }

    }

  }

  //  III.  Finished:
  return(toReturn);
}


//  PURPOSE:  To return the address of the next JSONValue read from 'tokenizer'.
JSONValue*	JSONValue::parseValue
				(Tokenizer&	tokenizer
				)
{
  //  I.  Application validity check:

  //  II.  Get the next JSON value from 'tokenizer':
  JSONValue*	readPtr	= tokenizer.advance();

  switch  (readPtr->getType())
  {
  case STRING_JSON :
  case NUMBER_JSON :
  case TRUE_JSON :
  case FALSE_JSON :
  case NULL_JSON :
  case EOF :
    break;

  case '{' :
    return(parseObject(tokenizer));

  case '[' :
    return(parseArray(tokenizer));

  default :
    fprintf(stderr,"Expected JSON value");
  }

  //  III.  Finished:
  return(readPtr);
}


//  PURPOSE:  To return the address of a JSONValue instance read from
//	file descriptor 'fd'.  If 'shouldCloseFd' is 'true' then
//	'close()'s 'fd' when finished.  Leaves 'fd' open otherwise.
//	Returns 'NULL' if no JSON value available.
JSONValue*	JSONValue::factory
				(int		fd,
				 bool		shouldCloseFd
    				)
{
  //  I.  Application validity check:

  //  II.  Read the JSON value from 'fd':
  Tokenizer	tokenizer(fd,shouldCloseFd);

  JSONValue*	valuePtr	= parseValue(tokenizer);

  //  III.  Finished:
  return( (valuePtr->getType() == EOF) ? NULL : valuePtr );
}


//  PURPOSE:  To return the address of a JSONValue instance read from
//	'sourceCPtr'.  Returns 'NULL' if no JSON value available.
JSONValue*	JSONValue::factory
				(const char*	sourceCPtr
    				)
{
  //  I.  Application validity check:

  //  II.  Read the JSON value from 'fd':
  Tokenizer	tokenizer(sourceCPtr);

  JSONValue*	valuePtr	= parseValue(tokenizer);

  //  III.  Finished:
  return( (valuePtr->getType() == EOF) ? NULL : valuePtr );
}


//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//		return value.
JSONValue::~JSONValue		()
{
  //  I.  Application validity check:

  //  II.  Release resources:

  //  III.  Finished:
}


//  PURPOSE:  To return the string representation of '*this'.  Quotes
//		text if 'shouldQuoteText' == 'true'.
const std::string
		JSONValue::getString
				(bool	shouldQuoteText	// = true
				)
				const
{
  //  I.  Application validity check:
  fprintf(stderr,"Attempt to use non-string as key in JSON object");
  exit(EXIT_FAILURE);
}


//  PURPOSE:  To return the string representation of '*this'.  Quotes text
//	if 'shouldQuoteText' == 'true'.
const std::string
		JSONNumber::getString
				(bool	shouldQuoteText // = true
				)
				const
{
  //  I.  Application validity check:

  //  II.  Get string
  //  II.A.  Use 'text_' if already defined:
  if  ( !text_.empty() )
  {
    return(text_);
  }

  //  II.B.  Create text:
  char	text[MAX_TINY_ARRAY_LEN];

  if  (isInteger_)
  {
    snprintf(text,MAX_TINY_ARRAY_LEN,"%ld",getInteger());
  }
  else
  {
    snprintf(text,MAX_TINY_ARRAY_LEN,"%g",getFloat());
  }

  //  III.  Finished:
  return(std::string(text));
}


//  PURPOSE:  To return the string representation of '*this'.  Quotes
//	text if 'shouldQuoteText' == 'true'.
const std::string
		JSONArray::getString
				(bool	shouldQuoteText	// = true
				)
				const
{
  //  I.  Application validity check:

  //  II.  Compose string:
  size_t	length	= array_.size();
  std::string	toReturn;

  toReturn	+= BEGIN_JSON_ARRAY;

  for  (size_t index = 0;  index < length;  index++)
  {
    if  (index > 0)
    {
      toReturn	+= JSON_SEPARATOR;
    }

    toReturn		+= array_[index]->getString(shouldQuoteText);
  } 

  toReturn	+= END_JSON_ARRAY;

  //  III.  Finished:
  return(toReturn);
}


//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//	return value.
JSONObject::~JSONObject	()
{
  //  I.  Application validity check:

  //  II.  Remove resources:
  std::map<std::string,JSONValue*>::iterator	iter	= map_.begin();
  std::map<std::string,JSONValue*>::iterator	end	= map_.end();

  for  ( ;  iter != end;  iter++)
  {
    delete(iter->second);
  }

  //  III.  Finished:
}


//  PURPOSE:  To return the string representation of '*this'.  Quotes
//	text if 'shouldQuoteText' == 'true'.
const std::string
		JSONObject::getString
				(bool	shouldQuoteText	// = true
				)
				const
{
  //  I.  Application validity check:

  //  II.  Compose string:
  bool					     isFirstIter= true;
  std::map<std::string,JSONValue*>::const_iterator iter	= map_.begin();
  std::map<std::string,JSONValue*>::const_iterator end	= map_.end();
  std::string					     toReturn;

  toReturn	+= BEGIN_JSON_BRACE;

  for  ( ;  iter != end;  iter++)
  {
    if  (isFirstIter)
    {
      isFirstIter	= false;
    }
    else
    {
      toReturn	+= JSON_SEPARATOR;
    }

    toReturn	+= QUOTE_STRING + iter->first + QUOTE_STRING;
    toReturn	+= JSON_MAPPER;
    toReturn	+= iter->second->getString(shouldQuoteText);
  } 

  toReturn	+= END_JSON_BRACE;

  //  III.  Finished:
  return(toReturn);
}


//  PURPOSE:  To add the mapping 'key' => '*valuePtr' to '*this'.  No
//	return value.
void		JSONObject::add	(const std::string&	key,
				 JSONValue*		valuePtr
				)
{
  //  I.  Application validity check:

  //  II.  Insert 'key' => 'valuePtr' in *this:
  //  II.A.  Look for existing mapping:
  std::map<std::string,JSONValue*>::iterator	iter	= map_.find(key);

  if  (iter != map_.end())
  {
    delete(iter->second);
    iter->second	= valuePtr;
  }
  else
  {
    map_[key]		= valuePtr;
  }

  //  III.  Finished:
}


//  PURPOSE:  To release the resources of '*this'.  No parameters.
//	No return value.
JSONPath::Index::~Index	()
{
  //  I.  Application validity check:

  //  II.  Release resources:

  //  III.  Finished:
}


//  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
JSONPath::JSONPath		(const JSONPath&	source
				) :
				pathDs_()
{
  //  I.  Applicability validity check:

  //  II.  Copy 'source':
  size_t	length	= source.getLength();

  for  (size_t dsIndex = 0;  dsIndex < length;  dsIndex++)
  {
    pathDs_.push_back(source.pathDs_[dsIndex]->copy());
  }

  //  III.  Finished:
}


//  PURPOSE:  To release the resources of '*this',  make '*this' a copy
//	of 'source', and return a reference to '*this'.
JSONPath&	JSONPath::operator=
				(const JSONPath&	source
    				)
{
  //  I.  Application validity check:
  if  (this == &source)
  {
    return(*this);
  }

  //  II.  Release resources:
  size_t	dsIndex;
  size_t	length	= getLength();

  for  (dsIndex = 0;  dsIndex < length;  dsIndex++)
  {
    delete(pathDs_[dsIndex]);
  }

  //  III.  Copy 'source':
  length	= source.getLength();

  for  (dsIndex = 0;  dsIndex < length;  dsIndex++)
  {
    pathDs_.push_back(source.pathDs_[dsIndex]->copy());
  }

  //  IV.  Finished:
  return(*this);
}


//  PURPOSE:  To release the resources of '*this'.  No parameters.
//	No return value.
JSONPath::~JSONPath		()
{
  //  I.  Applicability validity check:

  //  II.  Release resources:
  size_t	length	= getLength();

  for  (size_t dsIndex = 0;  dsIndex < length;  dsIndex++)
  {
    delete(pathDs_[dsIndex]);
  }

  //  III.  Finished:
}


//  PURPOSE:  To return the address of an internal JSONValue instance
//	that results from following '*this' path, or 'NULL' if the
//	path ends along the way.
const JSONValue*
		JSONPath::follow(const JSONValue*	valuePtr
				)
{
  //  I.  Application validity check:
  if  (valuePtr == NULL)
  {
    return(NULL);
  }

  //  II.  Follow path:
  size_t	length	= getLength();

  for  (size_t dsIndex = 0;  dsIndex < length;  dsIndex++)
  {
    valuePtr = pathDs_[dsIndex]->follow(valuePtr);

    if  (valuePtr == NULL)
    {
      return(NULL);
    }
  }

  //  III.  Finished:
  return(valuePtr);
}
