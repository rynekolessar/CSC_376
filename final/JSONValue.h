/*-------------------------------------------------------------------------*
 *---									---*
 *---		JSONValue.h						---*
 *---									---*
 *---	    This file declares classes for reading JSON text.		---*
 *---	See https://www.json.org/.					---*
 *---									---*
 *---		This is a declassified version of an original		---*
 *---			CONFIDENTIAL source file owned by:		---*
 *---			Applied Philosophy of Science			---*
 *---									---*
 *---	----	----	----	----	----	----	----	-----	---*
 *---									---*
 *---	Version 2		2019 October		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	<string>
#include	<vector>
#include	<map>

#define		QUOTE_STRING		"\""


  //  PURPOSE:  To distinguish among the derived classes of JSONValue.
  typedef	enum
		{
		  STRING_JSON,
		  NUMBER_JSON,
		  TRUE_JSON,
		  FALSE_JSON,
		  NULL_JSON,
		  ARRAY_JSON,
		  OBJECT_JSON,

		  LO_LEGAL_JSON	= STRING_JSON,
		  HI_LEGAL_JSON	= OBJECT_JSON
		}
		json_ty;


  //  PURPOSE:  To return 'true' if 'json' is a legal value in 'json_ty' or
  //  	'false' otherwise.
  inline
  bool		isLegalJsonType	(int	json
  				)
				{
				  return( (json >= LO_LEGAL_JSON) &&
				  	  (json <= HI_LEGAL_JSON)
					);
				}

  class		JSONSyntacticElement;


  //  PURPOSE:  To serve as the base class of all JSON classes.
  class	JSONValue
  {
    //  0.  Internal classes:

    //  PURPOSE:  To manage the low-level obtaining of chars to parse.
    class	InputCharStream
    {
      //  0.  Internal consts:
      enum
      {
        BUFFER_LEN	= LINE_BUFFER_LEN
      };

      //  I.  Member vars:
      //  PURPOSE:  To hold the file descriptor from which to read.
      int			inFd_;

      //  PURPOSE:  To hold the chars that have been read.
      char			buffer_[BUFFER_LEN];

      //  PURPOSE:  To hold the address of the char to process next.
      const char*		bufCursor_;

      //  PURPOSE:  To hold the address of one char beyond the last char to
      //	process.
      const char*		bufJustBeyondLast_;

      //  PURPOSE:  To hold 'true' if '*this' should 'close()' 'inFd_'
      //	upon the destruction of '*this', or 'false' otherwise.
      bool			shouldCloseFd_;

      //  PURPOSE:  To hold 'true' after EOF read, of 'false' otherwise.
      bool			haveSeenEof_;

      //  II.  Disallowed auto-generated methods:
      //  No default constructor:
      InputCharStream		();

      //  No copy constructor:
      InputCharStream		(const InputCharStream&
				);

      //  No copy assignment op:
      InputCharStream&
		operator=	(const InputCharStream&
				);

    protected :
      //  III.  Protected:

    public :
      //  IV.  Constructor(s):
      //  PURPOSE:  To initialize '*this' to read from file descriptor
      //	'newInFd'.  If 'newShouldCloseFd' is 'true' then 'newInFd' will
      //	be closed when '*this' is destroyed.  No return value.
      InputCharStream		(int		newInFd,
				 bool		newShouldCloseFd	= true
				) :
				inFd_(newInFd),
				bufCursor_(buffer_),
				bufJustBeyondLast_(buffer_),
				shouldCloseFd_(newShouldCloseFd),
				haveSeenEof_(false)
				{ }

      //  PURPOSE:  To initialize '*this' to read from 'sourceCPtr'.
      //	No return value.
      InputCharStream		(const char*	sourceCPtr
				) :
				inFd_(-1),
				bufCursor_(sourceCPtr),
				bufJustBeyondLast_
					(sourceCPtr + strlen(sourceCPtr)),
				shouldCloseFd_(false),
				haveSeenEof_(true)
				{ }

      //  PURPOSE:  To release the resources of '*this'.  No parameters.
      //	No return value.
      ~InputCharStream		()
				{
				  if  (shouldCloseFd_)
				  {
				    close(inFd_);
				  }
				}

      //  V.  Accessors:

      //  VI.  Mutators:

      //  VII.  Methods that do main and misc work of class:
      //  PURPOSE:  To return the current char, or 'EOF' if there are no more.
      //	No parameters.
      int	peek		();

      //  PURPOSE:  To return 'true' if at eof-of-input, or 'false' otherwise.
      bool	isAtEnd		()
				{
				  return( peek() == EOF );
				}

      //  PURPOSE:  To advance to the next char (if not already at end).  No
      //	parameters.  No return value.
      void	advance		();

    };


    //  PURPOSE:  To return the next token.
    class	Tokenizer
    {
      //  I.  Member vars:
      //  PURPOSE:  To represent the occurence of EOF.
      static
      JSONSyntacticElement	eof__;

      //  PURPOSE:  To represent the occurence of begin curly brace.
      static
      JSONSyntacticElement	beginCurlyBrace__;

      //  PURPOSE:  To represent the occurence of end curly brace.
      static
      JSONSyntacticElement	endCurlyBrace__;

      //  PURPOSE:  To represent the occurence of begin square bracket.
      static
      JSONSyntacticElement	beginSquareBracket__;

      //  PURPOSE:  To represent the occurence of end square bracket.
      static
      JSONSyntacticElement	endSquareBracket__;

      //  PURPOSE:  To represent the occurence of comma.
      static
      JSONSyntacticElement	comma__;

      //  PURPOSE:  To represent the occurence of colon.
      static
      JSONSyntacticElement	colon__;

      //  PURPOSE:  To do the low-level work of getting the next char
      InputCharStream		inputStream_;

      //  PURPOSE:  To hold the address of the last parsed JSONValue.
      JSONValue*		lastParsedPtr_;

      //  II.  Disallowed auto-generated methods:
      //  No default constructor:
      Tokenizer			();

      //  No copy constructor:
      Tokenizer			(const Tokenizer&
				);

      //  No copy assignment op:
      Tokenizer&
		operator=	(const Tokenizer&
				);

    protected :
      //  III.  Protected:
      //  PURPOSE:  To return a pointer to a JSONNumber representing the
      //  	number scanned from 'inputStream_' whose first char begins
      //	with 'c'.
      JSONValue*
		scanNumber	(char	firstC
				);

      //  PURPOSE:  To return a pointer to a JSONValue representing the string
      //  	scanned from 'inputStream_' whose beginning QUOTE_CHAR has
      //	already been read.
      JSONValue*
		scanString	();

      //  PURPOSE:  To scan 'null' or 'true' or 'false' appearing outside of
      //	quotes.  Return address of JSONValue instance corresponding to
      //	what was scanned.  'c' tells the character already scanned.
      JSONValue*
		scanNullTrueFalse
				(char		c
				);

      //  PURPOSE:  To return a pointer representing a scanned JSONValue.
      //	No parameters.
      JSONValue*
		scanner		();

    public :
      //  IV.  Constructor(s):
      //  PURPOSE:  To initialize '*this' to read JSON tokens from file
      //	descriptor 'newInFd'.  If 'newShouldCloseFd' is 'true' then
      //	'newInFd' will be closed when '*this' is destroyed.  No return
      //	value.
      Tokenizer			(int		newInFd,
				 bool		newShouldCloseFd	= true
				) :
				inputStream_(newInFd,newShouldCloseFd),
				lastParsedPtr_(NULL)
				{ }

      //  PURPOSE:  To initialize '*this' to read JSON tokens from C-string
      //	'sourceCPtr'.  No return value.
      Tokenizer			(const char*	sourceCPtr
				) :
				inputStream_(sourceCPtr),
				lastParsedPtr_(NULL)
				{ }

      //  PURPOSE:  To release the resources of '*this'.  No parameters.
      //	No return value.
      ~Tokenizer		()
				{ }

      //  V.  Accessors:

      //  VI.  Mutators:

      //  VII.  Methods that do main and misc work of class:
      //  PURPOSE:  To return an integer to identity the type of the current
      //	token in the input stream.  No parameters.
      int	peek		()
      				{
				  if  (lastParsedPtr_ == NULL)
				    lastParsedPtr_	= scanner();

				  return(lastParsedPtr_->getType());
				}

      //  PURPOSE:  To return the pointer to the old JSONValue that was at the
      //	front of the symbol stream, and then to internally advance to
      //	the next JSONValue instance (if not already at the end).  No
      //	parameters.
      JSONValue*
		advance		()
				{
				  JSONValue* toReturn
						  = (lastParsedPtr_ == NULL)
				  	     	    ? scanner()
						    : lastParsedPtr_;

				  lastParsedPtr_  = scanner();
				  return(toReturn);
				}

    };

    //  I.  Member vars:

    //  II.  Disallowed auto-generated methods:
  protected :
    //  III.  Protected methods:
    //  PURPOSE:  To return the address of JSONObject read from 'tokenizer'.
    static
    JSONValue*	parseObject	(Tokenizer&	tokenizer
				);

    //  PURPOSE:  To return the address of JSONArray read from 'tokenizer'.
    static
    JSONValue*	parseArray	(Tokenizer&	tokenizer
				);

    //  PURPOSE:  To return the address of the next JSONValue read from
    //	'tokenizer'.
    static
    JSONValue*	parseValue	(Tokenizer&	tokenizer
				);

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this'.  No return value.
    JSONValue			()
				{ }

    //  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
    JSONValue			(const JSONValue&	source
				)
				{ }

    //  PURPOSE:  To release the resources of '*this', make '*this' a copy of
    //		'source', and to return a reference to '*this'.
    JSONValue&	operator=	(const JSONValue&	source
				)
				{
				  //  I.  Application validity check:
				  if  (this == &source)
				  {
				    return(*this);
				  }

				  //  II.  Release resources:

				  //  III.  Copy 'source':

				  //  IV.  Finished:
				  return(*this);
				}

    //  PURPOSE:  To return the address of a JSONValue instance read from
    //		file descriptor 'fd'.  If 'shouldCloseFd' is 'true' then
    //		'close()'s 'fd' when finished.  Leaves 'fd' open otherwise.
    //		Returns 'NULL' if no JSON value available.
    static
    JSONValue*	factory		(int		fd,
				 bool		shouldCloseFd
    				);

    //  PURPOSE:  To return the address of a JSONValue instance read from
    //		'sourceCPtr'.  Returns 'NULL' if no JSON value available.
    static
    JSONValue*	factory		(const char*	sourceCPtr
    				);

    //  PURPOSE:  To release the resources of '*this'.  No parameters.  No
    //		return value.
    virtual
    ~JSONValue			();

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    virtual
    int		getType		()
				const
				= 0;

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    virtual
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const;

    //  PURPOSE:  To return the length of '*this' array.  No parameters.
    virtual
    size_t	getLength	()
				const
				{ return(0); }

    //  PURPOSE:  To return the 'i'-th item in '*this' array, or 'NULL' if it
    //  	is out-of-range.
    virtual
    const JSONValue*
		getElement	(size_t		i
				)
				const
				{ return(NULL); }

    //  PURPOSE:  To return the value to which 'key' maps '*this', or 'NULL'
    //  	if 'key' maps to no value.
    virtual
    const JSONValue*
		getElement	(const std::string&	key
				)
				const
				{ return(NULL); }

    //  PURPOSE:  To return the integer implicit by 
    virtual
    long long	getInteger	()
				const
				{ return(0); }

    //  PURPOSE:  To return the float 
    virtual
    double	getFloat 	()
				const
				{ return(0.0); }

    //  PURPOSE:  To set 'integer' to the integer in '*this' if '*this'
    //		represents an integer, or to set 'real' to the floating pt
    //		number in '*this' if '*this' represents a floating pt number.
    //		Returns 'true' if '*this' represents a float or 'false'
    //		otherwise.
    virtual
    bool	isInteger	(long long&	integer,
    				 double&	real
				)
				const
				{
				  real	= getFloat();
				  return(false);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent a JSON number.
  class	JSONNumber : public JSONValue
  {
    //  I.  Member vars:
    //  PURPOSE:  To hold the text of the parsed number.
    std::string			text_;

    //  PURPOSE:  To hold the integer represented by '*this', if '*this'
    //		indeed represents an integer.  Results are undefined otherwise.
    long long			integer_;

    //  PURPOSE:  To hold the floating pt number represented by '*this', if
    //		'*this' indeed represents a float.  Results are undefined
    //		otherwise.
    double			float_;

    //  PURPOSE:  To hold 'true' if 'text_' represents an integer, or 'false'
    //		otherwise.
    bool			isInteger_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' number-representing JSON value has text
    //		'newText' representing integer 'newInteger'.  No return value.
    JSONNumber			(std::string&	newText,
				 long long	newInteger
				) :
				JSONValue(),
				text_(newText),
				integer_(newInteger),
				float_(0.0),
				isInteger_(true)
				{ }

    //  PURPOSE:  To note that '*this' number-representing JSON value has text
    //		'newText' representing floating pt number 'newFloat'.  No
    //		return value.
    JSONNumber			(std::string&	newText,
				 double	newFloat
				) :
				JSONValue(),
				text_(newText),
				integer_(0),
				float_(newFloat),
				isInteger_(false)
				{ }

    //  PURPOSE:  To note that '*this' number-representing JSON value has text
    //		'newText' representing integer 'newInteger'.  No return value.
    JSONNumber			(long long	newInteger
				) :
				JSONValue(),
				text_(),
				integer_(newInteger),
				float_(0.0),
				isInteger_(true)
				{ }

    //  PURPOSE:  To note that '*this' number-representing JSON value
    //		represents floating pt number 'newFloat'.  No return value.
    JSONNumber			(double	newFloat
				) :
				JSONValue(),
				text_(),
				integer_(0),
				float_(newFloat),
				isInteger_(false)
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(NUMBER_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const;

    //  PURPOSE:  To return the integer implicit by 
    long long	getInteger	()
				const
				{ return(integer_); }

    //  PURPOSE:  To return the float 
    double	getFloat 	()
				const
				{ return(float_); }

    //  PURPOSE:  To set 'integer' to the integer in '*this' if '*this'
    //		represents an integer, or to set 'real' to the floating pt
    //		number in '*this' if '*this' represents a floating pt number.
    //		Returns 'true' if '*this' represents a float or 'false'
    //		otherwise.
    bool	isInteger	(long long&	integer,
    				 double&	real
				)
				const
				{
				  if  (isInteger_)
				  {
				    integer	= getInteger();
				    return(true);
				  }

				  real	= getFloat();
				  return(false);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent a JSON string.
  class	JSONString : public JSONValue
  {
    //  I.  Member vars:
    //  PURPOSE:  To hold the parsed text.
    std::string			text_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' string-representing JSON value has text
    //		'newText'.
    JSONString			(const std::string&	newText
				) :
				JSONValue(),
				text_(newText)
				{ }

    //  PURPOSE:  To note that '*this' string-representing JSON value has text
    //		'newTextCPtr'.
    JSONString			(const char*	newTextCPtr
				) :
				JSONValue(),
				text_(newTextCPtr)
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(STRING_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const
				{
				  return( shouldQuoteText
					  ?(QUOTE_STRING + text_ + QUOTE_STRING)
				  	  :text_
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent the JSON value "true"
  class	JSONTrue : public JSONValue
  {
    //  I.  Member vars:

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' 'true'-representing JSON value.
    JSONTrue			() :
				JSONValue()
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(TRUE_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const
				{
				  return( shouldQuoteText
				  	  ? std::string
					    (QUOTE_STRING "true" QUOTE_STRING)
					  : std::string("true")
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent the JSON value "false"
  class	JSONFalse : public JSONValue
  {
    //  I.  Member vars:

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' 'false'-representing JSON value:.
    JSONFalse			() :
				JSONValue()
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(FALSE_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const
				{
				  return( shouldQuoteText
				  	  ? std::string
					    (QUOTE_STRING "false" QUOTE_STRING)
					  : std::string("false")
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent the JSON value "null"
  class	JSONNull : public JSONValue
  {
    //  I.  Member vars:

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' 'null'-representing JSON value.
    JSONNull			() :
				JSONValue()
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(NULL_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const
				{
				  return( shouldQuoteText
				  	  ? std::string
					    (QUOTE_STRING "null" QUOTE_STRING)
					  : std::string("null")
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To represent JSON arrays.
  class	JSONArray : public JSONValue
  {
    //  I.  Member vars:
    //  PURPOSE:  To hold the components of the array.
    std::vector<JSONValue*>	array_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' array-representing JSON value.
    JSONArray			() :
				JSONValue()
				{ }

    //  PURPOSE:  To release the resources of '*this'.  No parameters.  No
    //		return value.
    ~JSONArray			()
				{
				  size_t	limit	= array_.size();

				  for  (size_t i = 0;  i < limit;  i++)
				  {
				    delete(array_[i]);
				  }
				}

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(ARRAY_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const;

    //  PURPOSE:  To return the length of '*this' array.  No parameters.
    size_t	getLength	()
				const
				{ return(array_.size()); }

    //  PURPOSE:  To return the 'i'-th item in '*this' array, or 'NULL' if it
    //  	is out-of-range.
    const JSONValue*
		getElement	(size_t		i
				)
				const
				{
				  return( (i >= getLength())
					  ? NULL
					  : array_[i]
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:
    //  PURPOSE:  To add 'jsonElePtr' to the end of '*this'.  No return
    //		value.
    void	add		(JSONValue*	jsonElePtr
				)
				{
				  array_.push_back(jsonElePtr);
				}

  };


  //  PURPOSE:  To represent JSON objects..
  class	JSONObject : public JSONValue
  {
    //  I.  Member vars:
    //  PURPOSE:  To hold map.
    std::map<std::string,JSONValue*>
				map_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' object-representing JSON value.
    JSONObject			() :
				JSONValue()
				{ }

    //  PURPOSE:  To release the resources of '*this'.  No parameters.  No
    //		return value.
    ~JSONObject			();

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(OBJECT_JSON); }

    //  PURPOSE:  To return the string representation of '*this'.  Quotes
    //		text if 'shouldQuoteText' == 'true'.
    const std::string
		getString	(bool	shouldQuoteText	= true
				)
				const;

    //  PURPOSE:  To return the value to which 'key' maps '*this', or 'NULL'
    //  	if 'key' maps to no value.
    const JSONValue*
		getElement	(const std::string&	key
				)
				const
				{
				  std::map<std::string,JSONValue*>::
					const_iterator	iter = map_.find(key);

				  return( (iter == map_.end())
					  ? NULL
					  : iter->second
					);
				}

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:
    //  PURPOSE:  To add the mapping 'key' => '*valuePtr' to '*this'.  No
    //		return value.
    void	add		(const std::string&	key,
				 JSONValue*		valuePtr
				);

  };


  //  PURPOSE:  To represent syntactic elements, but not JSON values.
  class	JSONSyntacticElement : public JSONValue
  {
    //  I.  Member vars:
    //  PURPOSE:  To hold an integer representation of the element.
    int		syntacticElement_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To note that '*this' 'true'-representing JSON value.
    JSONSyntacticElement	(int		newSyntacticElement
				) :
				JSONValue(),
				syntacticElement_(newSyntacticElement)
				{ }

    //  V.  Accessors:
    //  PURPOSE:  To return the 'json_ty' or 'char' or 'int' value
    //		corresponding to the identity of '*this'.  No parameters.
    int		getType		()
				const
				{ return(syntacticElement_); }

    //  VI.  Mutators:

    //  VII.  Methods that do main and misc. work of class:

  };


  //  PURPOSE:  To define a path (a sequence of string attributes and/or
  //	integer indices) from root JSONValue to an internal node (e.g. leaf)
  class		JSONPath
  {
    //  0.  Internal classes:
    //  PURPOSE:  To serve as the base class of both 'IntegerIndex' and
    //		'StringIndex'.
    class	Index
    {
      //  I.  Member vars:

      //  II.  Disallowed auto-generated methods:

    protected :
      //  III.  Protected methods:

    public :
      //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
      //  PURPOSE:  To initialize '*this'.  No parameters.  No return value.
      Index			()
				{ }

      //  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
      Index			(const Index&	source
				)
				{ }

      //  PURPOSE:  To release the resources of '*this', make '*this' a copy
      //	of 'source', and return a reference to '*this'.
      Index&	operator=	(const Index&	source
				)
				{
				  //  I.  Application validity check:
				  if  (this == &source)
				  {
				    return(*this);
				  }

				  //  II.  Release resources:

				  //  III.  Copy 'source':

				  //  IV.  Finished:
				  return(*this);
				}

      //  PURPOSE:  To release the resources of '*this'.  No parameters.
      //	No return value.
      virtual
      ~Index			()
				= 0;

      //  V.  Accessors:

      //  VI.  Mutators:

      //  VII.  Methods that do main and misc work of class:
      //  PURPOSE:  To return the result of following '*sourcePtr' indexed
      //	by the index of '*this'.
      virtual
      const JSONValue*
		follow		(const JSONValue*	sourcePtr
				)
				= 0;

      //  PURPOSE:  To return a copy of '*this'.  No parameters.
      virtual
      Index*	copy		()
      				= 0;

    };


    //  PURPOSE:  To serve as an index into JSONArray.
    class	IntegerIndex : public Index
    {
      //  I.  Member vars:
      //  PURPOSE: To hold the index stored at '*this'.
      size_t			index_;

      //  II.  Disallowed auto-generated methods:
      //  No default constructor:
      IntegerIndex		();

    protected :
      //  III.  Protected methods:

    public :
      //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
      //  PURPOSE:  To initialize '*this' to follow index 'newIndex'.  No
      //	return value.
      IntegerIndex		(size_t		newIndex
				) :
      				Index(),
				index_(newIndex)
				{ }

      //  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
      IntegerIndex		(const IntegerIndex&	source
				) :
      				Index(source),
				index_(source.getIndex())
				{ }

      //  PURPOSE:  To release the resources of '*this', make '*this' a copy
      //	of 'source',  and return a reference to '*this'.
      IntegerIndex&
		operator=	(const IntegerIndex&	source
				)
				{
				  //  I.  Applicability validity check:
				  if  (this == &source)
				  {
				    return(*this);
				  }

				  //  II.  Release resources:
				  *(Index*)this = source;

				  //  III.  Copy 'source':
				  index_    = source.getIndex();

				  //  IV.  Finished:
				  return(*this);
				}

      //  PURPOSE:  To release the resources of '*this'.  No parameters.
      //	No return value.
      ~IntegerIndex		()
				{ }

      //  V.  Accessors:
      //  PURPOSE:  To return the index stored at '*this'.  No parameters.
      size_t	getIndex	()
				const
				{ return(index_); }

      //  VI.  Mutators:

      //  VII.  Methods that do main and misc work of class:
      //  PURPOSE:  To return the result of following '*sourcePtr' indexed
      //	by the index of '*this'.
      const JSONValue*
		follow		(const JSONValue*	sourcePtr
				)
				{
				  return(sourcePtr->getElement(getIndex()));
				}

      //  PURPOSE:  To return a copy of '*this'.  No parameters.
      Index*	copy		()
      				{ return(new IntegerIndex(*this)); }

    };


    //  PURPOSE:  To serve as an index into JSONObject.
    class	StringIndex : public Index
    {
      //  I.  Member vars:
      //  PURPOSE: To hold the index stored at '*this'.
      std::string		index_;

      //  II.  Disallowed auto-generated methods:
      //  No default constructor:
      StringIndex		();

    protected :
      //  III.  Protected methods:

    public :
      //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
      //  PURPOSE:  To initialize '*this' to follow index 'newIndex'.  No
      //	return value.
      StringIndex		(const std::string&	newIndex
				) :
      				Index(),
				index_(newIndex)
				{ }

      //  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
      StringIndex		(const StringIndex&	source
				) :
      				Index(source),
				index_(source.getIndex())
				{ }

      //  PURPOSE:  To release the resources of '*this', make '*this' a copy
      //	of 'source',  and return a reference to '*this'.
      StringIndex&
		operator=	(const StringIndex&	source
				)
				{
				  //  I.  Applicability validity check:
				  if  (this == &source)
				  {
				    return(*this);
				  }

				  //  II.  Release resources:
				  *(Index*)this = source;

				  //  III.  Copy 'source':
				  index_    = source.getIndex();

				  //  IV.  Finished:
				  return(*this);
				}

      //  PURPOSE:  To release the resources of '*this'.  No parameters.
      //	No return value.
      ~StringIndex		()
				{ }

      //  V.  Accessors:
      //  PURPOSE:  To return the index stored at '*this'.  No parameters.
      const std::string&
		getIndex	()
				const
				{ return(index_); }

      //  VI.  Mutators:

      //  VII.  Methods that do main and misc work of class:
      //  PURPOSE:  To return the result of following '*sourcePtr' indexed
      //	by the index of '*this'.
      const JSONValue*
		follow		(const JSONValue*	sourcePtr
				)
				{
				  return(sourcePtr->getElement(getIndex()));
				}

      //  PURPOSE:  To return a copy of '*this'.  No parameters.
      Index*	copy		()
      				{ return(new StringIndex(*this)); }

    };


    //  I.  Member vars:
    //  PURPOSE:  To keep track of a path thru a JSONValue tree.
    std::vector<Index*>		pathDs_;

    //  II.  Disallowed auto-generated methods:

  protected :
    //  III.  Protected methods:

  public :
    //  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
    //  PURPOSE:  To initialize '*this' to an empty path.  No parameters.
    //		No return value.
    JSONPath			() :
				pathDs_()
				{ }

    //  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
    JSONPath			(const JSONPath&	source
				);

    //  PURPOSE:  To release the resources of '*this',  make '*this' a copy
    //		of 'source', and return a reference to '*this'.
    JSONPath&	operator=	(const JSONPath&	source
    				);

    //  PURPOSE:  To release the resources of '*this'.  No parameters.
    //		No return value.
    ~JSONPath			();

    //  V.  Accessors:
    //  PURPOSE:  To return the length of '*this' path.  No parameters.
    size_t	getLength	()
				const
				{ return(pathDs_.size()); }

    //  VI.  Mutators:
    //  PURPOSE:  To add integer index 'index' to the end of '*this' path.
    //		No return value.
    void	addInt		(size_t		index
    				)
				{
				  pathDs_.push_back(new IntegerIndex(index));
				}

    //  PURPOSE:  To add string index 'index' to the end of '*this' path.
    //		No return value.
    void	addStr		(const std::string&	index
    				)
				{
				  pathDs_.push_back(new StringIndex(index));
				}

    //  PURPOSE:  To add C-string index 'indexCPtr' to the end of '*this' path.
    //		No return value.
    void	addStr		(const char*	indexCPtr
    				)
				{
				  pathDs_.push_back
					(new StringIndex(std::string(indexCPtr))
					);
				}

    //  VII.  Methods that do main and misc work of class:
    //  PURPOSE:  To return the address of an internal JSONValue instance
    //		that results from following '*this' path, or 'NULL' if the
    //		path ends along the way.
    const JSONValue*
		follow		(const JSONValue*	valuePtr
				);

  };
