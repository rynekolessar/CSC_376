/*--------------------------------------------------------------------------*
 *----								               		               	----*
 *----							simpleHttpServer.cpp					----*
 *----																	----*
 *----	   			 This file defines a simple HTTP server.			----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----				Version 1A		2019 May 7		Joseph Phillips		----*
 *----																	----*
 *--------------------------------------------------------------------------*/

// Compile with:
// $ g++ simpleHttpServer.cpp JSONValue.cpp -o simpleHttpServer -lpthread -lcurl

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <errno.h>		// errno
#include <pthread.h>	// pthread_create()
#include <signal.h>		// sigaction()
#include <unistd.h>		// close()
#include <sys/types.h>  // socket(), accept(), bind(), open()
#include <sys/socket.h> //socket(),accept(),bind(),getaddrinfo()
#include <sys/stat.h>   // open()
#include <fcntl.h>		// open()
#include <wait.h>		// wait()
#include <signal.h>		// sigaction()
#include <netinet/in.h> // sockaddr_in, inet_ntoa()
#include <netinet/ip.h> // sockaddr_in
#include <netdb.h>		// getaddrinfo()
#include <arpa/inet.h>  // inet_ntoa()
#include <set>			// std::set
#include <vector>		// std::vector
#include <list>			// std::list
#include <map>			// std::map
#include <curl/curl.h>

const int ERROR_PORT = -1;

const int ERROR_FILE_DESCRIPTOR = -1;

const char QUOTE_CHAR = 0x22;

#define DIRECTORY_SEPARATORY_STR "/"

const char HOME_DIRECTORY_CHAR = '~';

#define CURRENT_DIRECTORY_STR "."

const size_t CURRENT_DIRECTORY_LEN =
	sizeof(CURRENT_DIRECTORY_STR) - 1;

#define CURRENT_DIRECTORY_PATH_STR \
	CURRENT_DIRECTORY_STR          \
	DIRECTORY_SEPARATORY_STR

const size_t CURRENT_DIRECTORY_PATH_LEN = sizeof(CURRENT_DIRECTORY_PATH_STR) - 1;

#define PARENT_DIRECTORY_STR ".."

const size_t PARENT_DIRECTORY_LEN =
	sizeof(PARENT_DIRECTORY_STR) - 1;

#define PARENT_DIRECTORY_PATH_STR \
	PARENT_DIRECTORY_STR          \
	DIRECTORY_SEPARATORY_STR

const size_t PARENT_DIRECTORY_PATH_LEN = sizeof(PARENT_DIRECTORY_PATH_STR) - 1;

//  PURPOSE:  To tell the beginning JSON brace
const char BEGIN_JSON_BRACE = '{';

//  PURPOSE:  To tell the ending JSON brace
const char END_JSON_BRACE = '}';

//  PURPOSE:  To tell the beginning JSON array
const char BEGIN_JSON_ARRAY = '[';

//  PURPOSE:  To tell the ending JSON array
const char END_JSON_ARRAY = ']';

//  PURPOSE:  To tell the ending JSON separator
const char JSON_SEPARATOR = ',';

//  PURPOSE:  To tell the ending JSON mapping char
const char JSON_MAPPER = ':';

#define JSON_TYPE_KEY "type"

#define SOM_ERROR_MSG_JSON_TYPE_VALUE "SOM error"

#define JSON_MESSAGE_KEY "message"

const int MAX_TINY_ARRAY_LEN = 256;

const int FILE_BUFFER_LEN = 64 * 1024;

const int LINE_BUFFER_LEN = 4096;

const char BETWEEN_COOKIE_SEPARATORY_CHAR = ';';

#define NAME_VAR "session.name"
#define ID_VAR "session.id"
#define CONTENT_VAR "session.content"

#define PYTHON_PROGNAME "symMath.py"

#define WEATHER_URL \
	"https://api.weather.gov/gridpoints/TOP/76,73"
//  76,73 Corresponds to Chicago according to
//  https://api.weather.gov/points/41.9,-87.6

#define SIMPLE_WEB_TEMPLATE_EXT ".swt"

const char QUERY_CHAR = '?';
const char FRAGMENT_CHAR = '#';
const char QUERY_ASSIGNMENT_CHAR = '=';
const char QUERY_SEGMENT_CHAR = '&';
const char QUERY_HEX_ESCAPE_CHAR = '%';
const char QUERY_SPACE_CHAR = '+';
const int COOKIE_LEN = 32;
const char *HEXADIGIT_ARRAY = "0123456789ABCDEF";
const char *DEFAULT_CONTENT_FILEPATH = "./content.txt";
const char *LOGIN_PAGE_NAME = "login";
const char *LOGOUT_PAGE_NAME = "logout";
const char *WELCOME_PAGE_NAME = "/welcome" SIMPLE_WEB_TEMPLATE_EXT;
const char *LIN_FIT_PAGE_NAME = "linFit" SIMPLE_WEB_TEMPLATE_EXT;
const char *POOR_FIT_PAGE_NAME = "poorFit" SIMPLE_WEB_TEMPLATE_EXT;
const char *ERROR_PAGE_NAME = "error" SIMPLE_WEB_TEMPLATE_EXT;
const char *USERNAME_HTML_VARNAME = "name";
const char *ID_HTML_VARNAME = "id";
const int AUTOLOGOUT_TIME_SECS = 60 * 15;
const int ASCTIME_BUFFER_LEN = 26; // From man pages
const int R_PORT = 58384;
const float CORREL_THRESHOLD = 0.5;
#define SERVER_NAME "SOMWebServer"
#define SERVER_VERSION "1.0"
#define INDEX_PAGE_NAME "index.html"
#define IMPLICIT_INDEX_URL "/"
#define EXPLICIT_INDEX_URL DIRECTORY_SEPARATORY_STR INDEX_PAGE_NAME
#define DEFAULT_MIME_FORMAT "text/html"
#define SIMPLE_WEB_TEMPLATE_EXT ".swt"
#define COOKIE_HEADER_TEXT "Cookie:"
#define SESSION_COOKIE_NAME "session"
#define BEGIN_TEMPLATE_VAR "<$"
#define END_TEMPLATE_VAR "$>"
#define USER_NAME_VAR "session.userName"
const int NEW_CLIENT_BUFFER_LEN = 64;
const int MIN_PORT_NUM = 1024;
const int MAX_PORT_NUM = 65535;
const int DEFAULT_PORT_NUM = 8080;
const int OS_CLIENT_QUEUE_LEN = 8;
const int NUM_CLIENT_HANDLING_THREADS = 16;
#define SYM_EVAL_PAGE_NAME "symEval.swt"

#define END_PYTHON_CONST_CPTR "quit()\n"

const size_t END_PYTHON_LEN = sizeof(END_PYTHON_CONST_CPTR) - 1;

#define PYTHON_PROMPT ">>> "

const size_t PYTHON_PROMPT_LEN = sizeof(PYTHON_PROMPT) - 1;

typedef enum
{
	BAD_HTTP_METH = -1,
	GET_HTTP_METH,
	PUT_HTTP_METH,
	DELETE_HTTP_METH,
	POST_HTTP_METH,
	HEAD_HTTP_METH,

	NUM_HTTP_METH
} httpMethod_ty;

const char *HTTP_METHOD_NAME_ARRAY[] = {
	"GET",	// GET_HTTP_METH
	"PUT",	// PUT_HTTP_METH
	"DELETE", // DELETE_HTTP_METH
	"POST",   // POST_HTTP_METH
	"HEAD"	// HEAD_HTTP_METH
};

typedef enum
{
	OK_HTTP_RET_CODE = 200,
	BAD_REQUEST_HTTP_RET_CODE = 400,
	UNAUTHORIZED_HTTP_RET_CODE = 401,
	FORBIDDEN_HTTP_RET_CODE = 403,
	NOT_FOUND_HTTP_RET_CODE = 404,
	METHOD_NOT_ALLOWED_HTTP_RET_CODE = 405
} httpReturnCode_ty;

#define WITH_COOKIE_HEADER_TEMPLATE                  \
	"HTTP/1.0 %d %s\r\n"                             \
	"Server: " SERVER_NAME "/" SERVER_VERSION "\r\n" \
	"Content-Type: %s\r\n"                           \
	"Content-Length: %zu\r\n"                        \
	"Set-Cookie: %s=%s\r\n"                          \
	"Date: %s"                                       \
	"\r\n"

#define WITHOUT_COOKIE_HEADER_TEMPLATE               \
	"HTTP/1.0 %d %s\r\n"                             \
	"Server: " SERVER_NAME "/" SERVER_VERSION "\r\n" \
	"Content-Type: %s\r\n"                           \
	"Content-Length: %zu\r\n"                        \
	"Date: %s"                                       \
	"\r\n"

#define BAD_REQUEST_PAGE                       \
	"<!DOCTYPE HTML>"                          \
	"<html lang=\"en\">"                       \
	" <head><title>Bad request</title></head>" \
	" <body>"                                  \
	"  <h1>Bad request</h1>"                   \
	" </body>"                                 \
	"</html>"

#define UNAUTHORIZED_PAGE                       \
	"<!DOCTYPE HTML>"                           \
	"<html lang=\"en\">"                        \
	" <head><title>Unauthorized</title></head>" \
	" <body>"                                   \
	"  <h1>Unauthorized</h1>"                   \
	" </body>"                                  \
	"</html>"

#define FORBIDDEN_PAGE                       \
	"<!DOCTYPE HTML>"                        \
	"<html lang=\"en\">"                     \
	" <head><title>Forbidden</title></head>" \
	" <body>"                                \
	"  <h1>Forbidden</h1>"                   \
	" </body>"                               \
	"</html>"

#define NOT_FOUND_PAGE                       \
	"<!DOCTYPE HTML>"                        \
	"<html lang=\"en\">"                     \
	" <head><title>Not found</title></head>" \
	" <body>"                                \
	"  <h1>Not found</h1>"                   \
	" </body>"                               \
	"</html>"

#define METHOD_NOT_ALLOWED_PAGE                       \
	"<!DOCTYPE HTML>"                                 \
	"<html lang=\"en\">"                              \
	" <head><title>Method not allowed</title></head>" \
	" <body>"                                         \
	"  <h1>Method not allowed</h1>"                   \
	" </body>"                                        \
	"</html>"

#include "JSONValue.h"

const char *CACHE_CONTROL[] = {
	"max-age=",
	"s-maxage=",
	"public",
	"private",
	"no-cache",
	"no-store",
};

typedef std::map<std::string, std::string> StringToString;
typedef std::map<std::string, std::string>::iterator StringToStringIter;

/*----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----		    		Declaration of functions and macros:			----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*/

//  PURPOSE:  To tell the port with which to communicate with R.
int rPort;

//  PURPOSE:  To hold 'true' after Python asked to quit, or 'false' beforehand.
bool haveAskedPythonToQuit = false;

//  PURPOSE:  To hold 'true' when Python is running or 'false' when it is not.
bool isPythonRunning = false;

//  PURPOSE:  To handle 'SIGCHLD'.  'sigNum' is ignored.  No return value.
void sigChildHandler(int sigNum)
{
	int status;
	const char *cPtr;

	wait(&status);
	isPythonRunning = false;

	if (WIFEXITED(status))
		if (WEXITSTATUS(status) == EXIT_SUCCESS)
			cPtr = "ended successfully";
		else
			cPtr = "ended with errors";
	else
		cPtr = "crashed";

	if (haveAskedPythonToQuit)
		printf("Python %s normally.\n", cPtr);
	else
		printf("Python %s unexpectedly.\n", cPtr);
}

//  PURPOSE:  To wait for Python to respond to the most recent command sent
//	to it.  We know Python has completed its response after we receive
//	'PYTHON_PROMPT'.  'fromPythonFd' is the file descriptor for
//	listening to Python.  Returns response from Python, without the
//	'PYTHON_PROMPT'.
std::string waitForPrompt(int fromPythonFd)
{
	std::string totalBuffer;
	char buffer[LINE_BUFFER_LEN];
	int numBytes;

	//  YOUR CODE HERE
	return ("CHANGE THIS");
}

struct MemoryStruct
{
	char *memory;
	size_t size;
};

//  PURPOSE:  To
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);

	if (ptr == NULL)
	{
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

//  PURPOSE:  To return the C++ string instance of the element of interest
//	in the JSON text pointed to by 'chunkPtr'.
std::string parseJson(struct MemoryStruct *chunkPtr)
{
	//  I.  Application validity check:

	//  II.  Attempt to parse the JSON in  '*chunkPtr':

	//  III.  Finished:
	return ("CHANGE THIS");
}

//  PURPOSE:  To return a random integer in a thread-safe fashion.  No
//	parameters.
extern int safeRand();

//  PURPOSE:  To return the address of text telling the name of the guess
//	of the MIME format of a file with path 'filepathCPtr' and with
//	contents 'contentsCPtr'.
extern const char *getMimeGuess(const char *filepathCPtr, const char *contentsCPtr);

#define safeDelete(p)    \
	{                    \
		if ((p) != NULL) \
		{                \
			delete (p);  \
			(p) = NULL;  \
		}                \
	}

#define safeFree(p)      \
	{                    \
		if ((p) != NULL) \
		{                \
			free(p);     \
			(p) = NULL;  \
		}                \
	}

/*----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----						Definition of global vars:					----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*/

static bool shouldRun = true;

static pthread_mutex_t safeRandLock__;

/*----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----						Declaration of classes:						----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*/

class Cookie
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the distinguishing value of '*this' cookie.
	char textValue_[COOKIE_LEN + 1];

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initalize '*this' to an blank cookie.  No parameters.
	//	No return value.
	Cookie()
	{
		memset(textValue_, '\0', COOKIE_LEN + 1);
	}

	//  PURPOSE:  To initialize '*this' to a random cookie.  'seedPtr' points to
	//	the random number seed to use.  No return value.
	Cookie(unsigned int *seedPtr)
	{
		for (size_t i = 0; i < COOKIE_LEN; i++)
		{
			textValue_[i] =
				HEXADIGIT_ARRAY[safeRand() % 16];
		}

		textValue_[COOKIE_LEN] = '\0';
	}

	//  PURPOSE:  To copy the cookie value from 'cPtr'.  No return value.
	Cookie(const char *cPtr)
	{
		char *destPtr = textValue_;
		char *destEndPtr = destPtr + COOKIE_LEN;

		while ((*cPtr == ' ') || (*cPtr == '\t'))
		{
			cPtr++;
		}

		for (;
			 (destPtr < destEndPtr) && isxdigit(*cPtr);
			 destPtr++, cPtr++)
		{
			*destPtr = *cPtr;
		}

		for (; destPtr <= destEndPtr; destPtr++)
		{
			*destPtr = '\0';
		}
	}

	//  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
	Cookie(const Cookie &source)
	{
		memcpy(textValue_,
			   source.getCPtr(),
			   COOKIE_LEN + 1);
	}

	//  PURPOSE:  To release the resources of '*this', make '*this' a copy of
	//	'source', and return a reference to '*this'.
	Cookie &operator=(const Cookie &source)
	{
		//  I.  Application validity check:
		if (this == &source)
		{
			return (*this);
		}

		//  II.  Release resources:

		//  III.  Copy 'source':
		memcpy(textValue_,
			   source.getCPtr(),
			   COOKIE_LEN + 1);

		//  IV.  Finished:
		return (*this);
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~Cookie()
	{
	}

	//  V.  Accessors:
	//  PURPOSE:  To return the text value of '*this'.  No parameters.
	const char *getCPtr()
		const
	{
		return (textValue_);
	}

	//  PURPOSE:  To return 'true' if '*this' has a useful value or 'false'
	//	otherwise.  No parameters.
	bool isDefined()
		const
	{
		return (textValue_[0] != '\0');
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
};

class User
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the name.
	std::string name_;

	//  PURPOSE:  To hold the id.
	std::string id_;

	//  PURPOSE:  To hold the content.
	std::string content_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to default values.  No parameters.
	//	No return value.
	User() : name_(""),
			 id_(""),
			 content_("")
	{
	}

	//  PURPOSE:  To initialize '*this' to record name 'name', id 'id' and
	//	content 'content'.  No return value.
	User(const std::string &name,
		 const std::string &id,
		 const std::string &content) : name_(name),
									   id_(id),
									   content_(content)
	{
	}

	//  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
	User(const User &source) : name_(source.getName()),
							   id_(source.getId()),
							   content_(source.getContent())
	{
	}

	//  PURPOSE:  To release the resources of '*this', make '*this' a copy of
	//	'source', and return a reference to '*this'.
	User &operator=(const User &source)
	{
		//  I.  Application validity check:
		if (this == &source)
		{
			return (*this);
		}

		//  II.  Release resources:

		//  III.  Copy 'source':
		name_ = source.getName();
		id_ = source.getId();
		content_ = source.getContent();

		//  IV.  Finished:
		return (*this);
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	virtual ~User()
	{
	}

	//  V.  Accessors:
	//  PURPOSE:  To return the name.  No parameters.
	const std::string &
	getName()
		const
	{
		return (name_);
	}

	//  PURPOSE:  To return the id.  No parameters.
	const std::string &
	getId()
		const
	{
		return (id_);
	}

	//  PURPOSE:  To return the content.  No parameters.
	const std::string &
	getContent()
		const
	{
		return (content_);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
};

class SessionStore
{
	//  0.  Local classes:
	class Session
	{
		//  I.  Member vars:
		//  PURPOSE:  To tell the time when '*this' was created.
		time_t creationTime_;

		//  PURPOSE:  To tell the time when '*this' was last used.
		time_t lastUsedTime_;

		//  PURPOSE:  To hold the address of User logged in '*this' Session.
		//		'*this' does not own the pointed-to instance.
		const User *userPtr_;

		//  II.  Disallowed auto-generate methods:

	protected:
		//  III.  Protected methods:

	public:
		//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
		//  PURPOSE:  To initialize '*this' to garbage, useless values.  No
		//		parameters.  No return value.
		Session() : creationTime_(),
					lastUsedTime_(),
					userPtr_(NULL)
		{
		}

		//  PURPOSE:  To record "now" as both the creation and last usage time of
		//		'*this' Session for user '*userPtr'.
		Session(const User *userPtr) : userPtr_(userPtr)
		{
			creationTime_ = lastUsedTime_ = time(NULL);
		}

		//  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
		Session(const Session &source) : creationTime_(source.getCreationTime()),
										 lastUsedTime_(source.getLastUsedTime()),
										 userPtr_(source.getUserPtr())
		{
		}

		//  PURPOSE:  To release the resources of '*this', make '*this' a copy of
		//		'source', and return a reference to '*this'.
		Session &operator=(const Session &source)
		{
			//  I.  Application validity check:
			if (this == &source)
			{
				return (*this);
			}

			//  II.  Release the resources of '*this':

			//  III.  Copy 'source':
			creationTime_ = source.getCreationTime();
			lastUsedTime_ = source.getLastUsedTime();
			userPtr_ = source.getUserPtr();

			//  IV.  Finished:
			return (*this);
		}

		//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
		//		return value.
		~Session()
		{
			//  Do _not_ 'delete()' 'userPtr'.
			//  '*this' does not own it.
		}

		//  V.  Accessors:
		//  PURPOSE:  To return the time when '*this' was created.
		time_t getCreationTime()
			const
		{
			return (creationTime_);
		}

		//  PURPOSE:  To return the time when '*this' was last used.
		time_t getLastUsedTime()
			const
		{
			return (lastUsedTime_);
		}

		//  PURPOSE:  To return the address of User logged in '*this' Session.
		const User *getUserPtr()
			const
		{
			return (userPtr_);
		}

		//  VI.  Mutators:

		//  VII.  Methods that do main and misc. work of class:
		//  PURPOSE:  To update 'lastUsedTime_' to "now".  No parameters.  No
		//		return value.
		void touch()
		{
			lastUsedTime_ = time(NULL);
		}
	};

	//  I.  Member vars:
	//  PURPOSE:  To map from a Cookie to the Session to which it corresponds.
	std::map<Cookie, Session> cookieToSessionMap_;

	//  PURPOSE:  To lock '*this' to make it thread safe.
	pthread_mutex_t lock_;

	//  II.  Disallowed auto-generate methods:
	//  No copy constructor.
	SessionStore(const SessionStore &);

	//  No copy assignment operator.
	SessionStore &operator=(const SessionStore &);

protected:
	//  III.  Protected methods:
	//  PURPOSE:  To return 'true' if 'cookie' is in '*this', or 'false'
	//	otherwise.  Not thread safe.
	bool doesExist_NTS(const Cookie &cookie)
		const
	{
		return (cookieToSessionMap_.find(cookie) !=
				cookieToSessionMap_.end());
	}

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to an empty store.  No parameters.  No
	//	return value.
	SessionStore() : cookieToSessionMap_()
	{
		pthread_mutex_init(&lock_, NULL);
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~SessionStore()
	{
		pthread_mutex_destroy(&lock_);
	}

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To create and return a cookie.  'seedPtr' points to a random
	//	number seed to use to create the cookie.
	Cookie createSession(unsigned int *seedPtr,
						 const User *userPtr)
	{
		Cookie cookie(seedPtr);

		pthread_mutex_lock(&lock_);

		while (doesExist_NTS(cookie))
		{
			cookie = Cookie(seedPtr);
		}

		cookieToSessionMap_[cookie] = Session(userPtr);
		pthread_mutex_unlock(&lock_);
		return (cookie);
	}

	//  PURPOSE:  To return 'true' if 'cookie' is in '*this', or 'false'
	//	otherwise.
	bool doesExist(const Cookie &cookie)
	{
		bool toReturn;

		pthread_mutex_lock(&lock_);
		toReturn = doesExist_NTS(cookie);
		pthread_mutex_unlock(&lock_);

		return (toReturn);
	}

	//  PURPOSE:  To return the address of the 'User' entry for the Session
	//	with Cookie 'cookie', or 'NULL' if there is no such Session.
	const User *getUserPtr(const Cookie &cookie)
	{
		const User *toReturn = NULL;

		pthread_mutex_lock(&lock_);
		std::map<Cookie, Session>::iterator
			iter = cookieToSessionMap_.find(cookie);

		if (iter != cookieToSessionMap_.end())
		{
			toReturn = iter->second.getUserPtr();
		}

		pthread_mutex_unlock(&lock_);

		return (toReturn);
	}

	//  PURPOSE:  To return 'true' if the session implied by 'cookie' had been
	//	updated with the current time, or to return 'false' otherwise.
	bool didTouch(const Cookie &cookie)
	{
		bool toReturn;

		pthread_mutex_lock(&lock_);
		std::map<Cookie, Session>::iterator
			iter = cookieToSessionMap_.find(cookie);

		if (iter == cookieToSessionMap_.end())
		{
			toReturn = false;
		}
		else
		{
			toReturn = true;
			iter->second.touch();
		}

		pthread_mutex_unlock(&lock_);

		return (toReturn);
	}

	//  PURPOSE:  To remove the session with Cookie 'cookie', if it is exists,
	//	from '*this'.  Returns 'true' if session was deleted or 'false'
	//	otherwise.
	bool didDeleteSession(const Cookie &cookie)
	{
		bool toReturn = false;

		pthread_mutex_lock(&lock_);
		std::map<Cookie, Session>::iterator
			iter = cookieToSessionMap_.find(cookie);

		if (iter != cookieToSessionMap_.end())
		{
			cookieToSessionMap_.erase(iter);
			toReturn = true;
		}

		pthread_mutex_unlock(&lock_);
		return (toReturn);
	}
};

class UserContent
{
	//  I.  Member vars:
	//  PURPOSE:  To map from user names to content about that user.
	std::map<std::string, User> userNameToContentMap_;

	//  PURPOSE:  To hold the process id of the Python program.
	pid_t pythonPid_;

	//  PURPOSE:  To hold the file descriptor for talking to Python.
	int toPythonFd_;

	//  PURPOSE:  To hold the file descriptor for listening to Python.
	int fromPythonFd_;

	//  PURPOSE:  To hold the handle for querying outside content.
	CURL *curlHandle_;

	//  II.  Disallowed auto-generate methods:
	//  No default constructor:
	UserContent();

	//  No copy constructor:
	UserContent(const UserContent &);

	//  No copy assignment op:
	UserContent &operator=(const UserContent &);

protected:
	//  III.  Protected methods:
	//  PURPOSE:  To tell the Python program to quit.  No parameters.
	//	No return value.
	void endPython()
	{
		haveAskedPythonToQuit = true;
		//  YOUR CODE HERE
	}

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To read the definition of '*this' from 'filepathCPtr'.  No
	//	return value.
	UserContent(const char *filepathCPtr,
				const char *pythonProgNameCPtr,
				const char *externalContentUrlCPtr);

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~UserContent();

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return a pointer to the content for the user named
	//  	'userNameCPtr', or to return 'NULL' if there is no such user.
	const User *getContentFor(const char *userNameCPtr)
		const
	{
		std::map<std::string, User>::const_iterator
			iter = userNameToContentMap_.find(userNameCPtr);

		return ((iter == userNameToContentMap_.end())
					? NULL
					: &(iter->second));
	}

	//  PURPOSE:  To get Python to evaluate the expression in 'toEvalStr',
	//	and to return the evaluation generated by Python.
	std::string evalExpression(std::string toEvalStr)
	{
		//  I.  Application validity check:

		//  II.  Evaluate 'toEvalStr':
		printf("Attempt to evaluate \"%s\"\n\n",
			   toEvalStr.c_str());

		//  YOUR CODE HERE

		return ("CHANGE THIS");
	}

	//  PURPOSE:  To use libcurl handle 'curlHandle' to query the registered
	//	URL for its JSON content, to parse that content, and to pull out
	//	the desired element and return it as a C++ string instance.
	std::string getExternalContentStr()
	{
		//  I.  Applicability validity check:
		if (curlHandle_ == NULL)
		{
			return ("(no content provider specified)");
		}

		//  II.  Get external content:
		struct MemoryStruct chunk;
		CURLcode res;
		std::string toReturn;

		//  YOUR CODE HERE

		//  III.  Finished:
		return ("CHANGE THIS");
	}
};

class RequestHeader
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the cookie from the request.
	Cookie cookie_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to a blank request header.
	RequestHeader() : cookie_()
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~RequestHeader()
	{
	}

	//  V.  Accessors:

	//  VI.  Mutators:
	//  PURPOSE:  To note that Cookie 'cookie' was seen in '*this' header.
	//	No return value.
	void setCookie(const Cookie &cookie)
	{
		cookie_ = cookie;
	}

	//  PURPOSE:  To clear '*this'.
	void clearCookie()
	{
		cookie_ = Cookie();
	}

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the cookie from the request.  No parameters.
	const Cookie &getCookie()
		const
	{
		return (cookie_);
	}
};

class Request
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the HTTP method that was requested.
	httpMethod_ty method_;

	//  PURPOSE:  To hold the path that was requested.
	char path_[LINE_BUFFER_LEN];

	//  PURPOSE:  To hold the HTTP version of the client.
	char version_[MAX_TINY_ARRAY_LEN];

	//  PURPOSE:  To hold the query.
	std::map<std::string, std::string>
		query_;

	//  PURPOSE:  To hold the relevant information of the header of the request.
	RequestHeader header_;

	//  PURPOSE:  To hold the address of the actual content of request,
	//	or 'NULL' if there is no content.
	const char *contentPtr_;

	//  PURPOSE:  To hold the address of the user (if one is defined for '*this'
	//	Request), or 'NULL' otherwise.
	const User *userPtr_;

	//  PURPOSE:  To hold 'true' if the cookie was just recently deleted,
	//  	or 'false' otherwise.
	bool wasCookieRecentlyDeleted_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To create a blank Request.  No parameters.  No return value.
	Request() : method_(BAD_HTTP_METH),
				query_(),
				header_(),
				contentPtr_(NULL),
				userPtr_(NULL),
				wasCookieRecentlyDeleted_(false)
	{
	}

	//  PURPOSE:  To initialize '*this' using the 'requestBufferLen' bytes
	//	'read()' from 'inFd' and placed into 'requestBuffer'.  'sessionStore'
	//	keeps track of the Session information.  No return value.
	void initialize(int inFd,
					char *requestBuffer,
					int requestBufferLen,
					SessionStore &sessionStore);

	//  PURPOSE:  To set all member vars to blank values.
	void clear()
	{
		method_ = BAD_HTTP_METH;
		path_[0] = '\0';
		version_[0] = '\0';
		query_.clear();
		header_.clearCookie();
		contentPtr_ = NULL;
		userPtr_ = NULL;
		wasCookieRecentlyDeleted_ = false;
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~Request()
	{
	}

	//  V.  Accessors:
	//  PURPOSE:  To return the HTTP method that was requested.  No parameters.
	httpMethod_ty getMethod()
		const
	{
		return (method_);
	}

	//  PURPOSE:  To return the path that was requested.  No parameters.
	const char *getPath()
		const
	{
		return (path_);
	}

	//  PURPOSE:  To return the HTTP version of the client.  No parameters.
	const char *getVersion()
		const
	{
		return (version_);
	}

	//  PURPOSE:  To return a reference to the query.
	std::map<std::string, std::string> &
	getQuery()
	{
		return (query_);
	}

	//  PURPOSE:  To return a reference to the query.
	const std::map<std::string, std::string> &
	getQuery()
		const
	{
		return (query_);
	}

	//  PURPOSE:  To return the cookie from '*this' request.  No parameters.
	const Cookie &getCookie()
		const
	{
		return (header_.getCookie());
	}

	//  PURPOSE:  To hold the address of the actual content of request,
	//	or 'NULL' if there is no content.  No parameters.
	const char *getContentPtr()
		const
	{
		return (contentPtr_);
	}

	//  PURPOSE:  To return the address of the user (if one is defined for
	//	'*this' Request), or 'NULL' otherwise.
	const User *getUserPtr()
		const
	{
		return (userPtr_);
	}

	//  PURPOSE:  To return 'true' if the cookie was just recently deleted,
	//  	or 'false' otherwise.  No parameters.
	bool getWasCookieRecentlyDeleted()
		const
	{
		return (wasCookieRecentlyDeleted_);
	}

	//  VI.  Mutators:
	//  PURPOSE:  To note that the user for '*this' Request is '*userPtr' and
	//	to set the cookie to 'cookie'.  No return value.
	void setUserPtrAndCookie(const User *userPtr,
							 const Cookie &cookie)
	{
		userPtr_ = userPtr;
		header_.setCookie(cookie);
	}

	//  PURPOSE:  To remove the user and cookie from '*this' Request.
	//	No parameters.
	void clearUserPtrAndCookie()
	{
		userPtr_ = NULL;
		header_.clearCookie();
		wasCookieRecentlyDeleted_ = true;
	}

	//  VII.  Methods that do main and misc. work of class:
};

class Page
{
	//  I.  Member vars:

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:
	Page()
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	virtual ~Page();

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the number of bytes in '*this' page given Request
	//	'request'.
	virtual size_t getNumBytes(const Request &request) = 0;

	//  PURPOSE:  To return the MIME format of '*this' page.  No parameters.
	virtual const char *getMimeCPtr() = 0;

	//  PURPOSE:  To 'write()' '*this' page for file descriptor 'outFd' given
	//	Request 'request'.
	virtual int write(int outFd,
					  const Request &request) = 0;

	//  PURPOSE:  To perhaps get rid of '*this' page after it has been
	//	'write()'n.  No parameters.  No return value.
	virtual void perhapsDispose()
	{
	}
};

class FixedPage : public Page
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the length of '*this' page in bytes.
	size_t numBytes_;

	//  PURPOSE:  To hold the address of the beginning of contents of '*this'
	//	page.
	const char *contentsCPtr_;

	//  PURPOSE:  To hold the address of the MIME format name.
	const char *mimeCPtr_;

	//  PURPOSE:  To hold 'true' if '*this' should be disposed when
	//	'perhapsDispose()' is called, or 'false' otherwise.
	bool shouldDispose_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to have fixed context (e.g. text)
	//	pointed to by 'newContentsCPtr' and MIME format name 'newMimeCPtr'.
	//	No return value.
	FixedPage(const char *newContentsCPtr,
			  const char *newMimeCPtr = DEFAULT_MIME_FORMAT) : Page(),
															   numBytes_(strlen(newContentsCPtr)),
															   contentsCPtr_(newContentsCPtr),
															   mimeCPtr_(newMimeCPtr),
															   shouldDispose_(false)
	{
	}

	//  PURPOSE:  To initialize '*this' to have fixed context (e.g. text)
	//	pointed to by 'newContentsCPtr' of length 'newNumBytes' and MIME format
	//	name 'newMimeCPtr'.  No return value.
	FixedPage(size_t newNumBytes,
			  const char *newContentsCPtr,
			  const char *newMimeCPtr = DEFAULT_MIME_FORMAT) : Page(),
															   numBytes_(newNumBytes),
															   contentsCPtr_(newContentsCPtr),
															   mimeCPtr_(newMimeCPtr),
															   shouldDispose_(false)
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~FixedPage()
	{
		//  Do _not_ 'free(mimeCPtr_)',
		//  '*this' does not own it.

		//  Do _not_ 'free(contentsCPtr_)',
		//  '*this' does not own it.
	}

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the number of bytes in '*this' page given Request
	//	'request'.
	size_t getNumBytes(const Request &request)
	{
		return (numBytes_);
	}

	//  PURPOSE:  To return the MIME format of '*this' page.  No parameters.
	const char *getMimeCPtr()
	{
		return (mimeCPtr_);
	}

	//  PURPOSE:  To 'write()' '*this' page for file descriptor 'outFd' given
	//	Request 'request'.
	int write(int outFd,
			  const Request &request)
	{
		return (::write(outFd,
						contentsCPtr_,
						getNumBytes(request)));
	}

	//  PURPOSE:  To perhaps get rid of '*this' page after it has been
	//	'write()'n.  No parameters.  No return value.
	void perhapsDispose()
	{
		if (shouldDispose_)
		{
			delete (this);
		}
	}
};

//  PURPOSE:  To act as the base class for both 'FilePage' and 'FixedFormPage'.
class BaseFilePage : public Page
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the address of the filepath from which '*this' page
	//	was 'read()'.
	char *filepathCPtr_;

	//  PURPOSE:  To hold the length of '*this' page in bytes.
	size_t numBytes_;

	//  PURPOSE:  To hold the address of the contents of '*this' page.
	char *contentsCPtr_;

	//  PURPOSE:  To hold the last modification time.
	struct timespec lastModificationTime_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:
	//  PURPOSE:  To return the address of the filepath from which '*this' page
	//	was 'read()'.  No parameters.
	const char *getFilepathCPtr()
		const
	{
		return (filepathCPtr_);
	}

	//  PURPOSE:  To return the address of the contents of '*this' page.  No
	//	parameters.
	const char *getContentsCPtr()
		const
	{
		return (contentsCPtr_);
	}

	//  PURPOSE:  To read the definition of '*this' from 'filepathCPtr_'.
	//	'statBuffer' tells the meta-data about the current definition in the
	//	file-system.
	void load(struct stat &statBuffer)
	{
		int fd = open(filepathCPtr_, O_RDONLY);

		if (fd < 0)
		{
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		numBytes_ = statBuffer.st_size;
		lastModificationTime_ = statBuffer.st_mtim;
		contentsCPtr_ = (char *)malloc(numBytes_ + 1);

		if (read(fd, contentsCPtr_, numBytes_) < 0)
		{
			close(fd);
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		close(fd);
		contentsCPtr_[numBytes_] = '\0';
	}

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to be the page read() from the file
	//	'filepathCPtr'.  No return value.
	BaseFilePage(const char *filepathCPtr) : Page(),
											 filepathCPtr_(strdup(filepathCPtr)),
											 numBytes_(0),
											 contentsCPtr_(NULL),
											 lastModificationTime_()
	{
		struct stat statBuffer;

		if ((stat(getFilepathCPtr(),
				  &statBuffer) != 0) ||
			!S_ISREG(statBuffer.st_mode))
		{
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		load(statBuffer);
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~BaseFilePage()
	{
		safeFree(contentsCPtr_);
		safeFree(filepathCPtr_);
	}

	//  V.  Accessors:
	//  PURPOSE:  To hold 'true' if '*this' should 'delete()' itself in
	//	'perhapsDispose()', or 'false' otherwise.
	virtual bool getShouldSelfDispose()
		const
	{
		return (false);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the number of bytes in '*this' page given Request
	//	'request'.
	size_t getNumBytes(const Request &request)
	{
		return (numBytes_);
	}

	//  PURPOSE:  To perhaps get rid of '*this' page after it has been
	//	'write()'n.  No parameters.  No return value.
	void perhapsDispose()
	{
		if (getShouldSelfDispose())
		{
			unlink(filepathCPtr_);
			delete (this);
		}
	}

	//  PURPOSE:  To perhaps re-load the definition of '*this' from the
	//	file-system.  No parameters.  Returns 'true' if have reloaded or
	//	'false' otherwise.
	virtual bool didPerhapsUpdate()
	{
		struct stat statBuffer;

		if ((stat(getFilepathCPtr(),
				  &statBuffer) != 0) ||
			!S_ISREG(statBuffer.st_mode))
		{
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		if ((statBuffer.st_mtim.tv_sec <
			 lastModificationTime_.tv_sec) ||
			((statBuffer.st_mtim.tv_sec ==
			  lastModificationTime_.tv_sec) &&
			 (statBuffer.st_mtim.tv_nsec <=
			  lastModificationTime_.tv_nsec)))
		{
			return (false);
		}

		safeFree(contentsCPtr_);
		load(statBuffer);
		return (true);
	}
};

//  PURPOSE:  To represent static pages (e.g. .html and images).
class FilePage : public BaseFilePage
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the address of the MIME format name.
	const char *mimeCPtr_;

	//  PURPOSE:  To hold 'true' if '*this' should 'delete()' itself in
	//	'perhapsDispose()', or 'false' otherwise.
	bool shouldSelfDispose_;

	//  II.  Disallowed auto-generate methods:
	//  No default constructor:
	FilePage();

	//  No copy constructor:
	FilePage(const FilePage &);

	//  No copy assignment op:
	FilePage &operator=(const FilePage &);

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to be the page read() from the file
	//	'filepathCPtr'.  If 'shouldSelfDispose' is 'true' then '*this' page
	//	will 'delete()' itself when told to do so in 'perhapsDispose()'.
	//	It will not do so if 'shouldSelfDispose' is 'false'.  No return value.
	FilePage(const char *filepathCPtr,
			 bool shouldSelfDispose = false) : BaseFilePage(filepathCPtr),
											   mimeCPtr_(getMimeGuess(getFilepathCPtr(),
																	  getContentsCPtr())),
											   shouldSelfDispose_(shouldSelfDispose)
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~FilePage()
	{
		//  Do _not_ 'free(mimeCPtr_)',
		//  '*this' does not own it.
	}

	//  V.  Accessors:
	//  PURPOSE:  To hold 'true' if '*this' should 'delete()' itself in
	//	'perhapsDispose()', or 'false' otherwise.
	bool getShouldSelfDispose()
		const
	{
		return (shouldSelfDispose_);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the MIME format of '*this' page.  No parameters.
	const char *getMimeCPtr()
	{
		return (mimeCPtr_);
	}

	//  PURPOSE:  To 'write()' '*this' page for file descriptor 'outFd' given
	//	Request 'request'.
	int write(int outFd,
			  const Request &request)
	{
		return (::write(outFd,
						getContentsCPtr(),
						getNumBytes(request)));
	}
};

class AppPage : public Page
{
};

class FixedFormPage : public BaseFilePage
{
	//  0.  Local types:
	//  PURPOSE:  To be the base class of both TextSegment and VarSegment.
	class Segment
	{
		//  I.  Member vars:

		//  II.  Disallowed auto-generated methods:
		//  No copy constructor:
		Segment(const Segment &);

		//  No copy assignment op:
		Segment &operator=(const Segment &);

	protected:
		//  III.  Protected methods:

	public:
		//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
		//  PURPOSE:  To initialize '*this'.  No parameters.  No return value.
		Segment()
		{
		}

		//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
		//		return value.
		virtual ~Segment();

		//  V.  Accessors:

		//  VI.  Mutators:

		//  VII.  Methods that do main and misc. work of class:
		//  PURPOSE:  To write the segment of '*this' to memory to address 'cPtr'
		//		given query 'query'.  Returns address just after written
		//		segment.
		virtual char *memWrite(const Request &request,
							   char *cPtr)
			const = 0;

		//  PURPOSE:  To return the length of the segment of '*this' given
		//		query 'query'.
		virtual size_t getLength(const Request &request)
			const = 0;
	};

	//  PURPOSE:  To represent a sequence of text to place in a Page.
	class TextSegment : public Segment
	{
		//  I.  Member vars:
		//  PURPOSE:  To hold the address of the start of segment.
		const char *textCPtr_;

		//  PURPOSE:  To hold the length of the segment.
		size_t length_;

		//  II.  Disallowed auto-generated methods:
		//  No copy constructor:
		TextSegment(const TextSegment &);

		//  No copy assignment op:
		TextSegment &
		operator=(const TextSegment &);

	protected:
		//  III.  Protected methods:

	public:
		//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
		//  PURPOSE:  To initialize '*this' to the text segment starting at
		//		address 'textCPtr' and having length 'length'.  No return value.
		TextSegment(const char *textCPtr,
					size_t length) : Segment(),
									 textCPtr_(textCPtr),
									 length_(length)
		{
		}

		//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
		//		return value.
		~TextSegment()
		{
			//  Do *not* 'delete()' 'textCPtr_',
			//  '*this' does not own it.
		}

		//  V.  Accessors:

		//  VI.  Mutators:

		//  VII.  Methods that do main and misc. work of class:
		//  PURPOSE:  To write the segment of '*this' to memory to address 'cPtr'
		//		given query 'query'.  Returns address just after written
		//		segment.
		char *memWrite(const Request &request,
					   char *cPtr)
			const
		{
			memcpy(cPtr, textCPtr_, length_);
			return (cPtr + length_);
		}

		//  PURPOSE:  To return the length of the segment of '*this' given
		//		query 'query'.
		size_t getLength(const Request &request)
			const
		{
			return (length_);
		}
	};

	//  PURPOSE:  To represent a variable whose value should be filled in on
	//	a Page.
	class VarSegment : public Segment
	{
		//  I.  Member vars:
		//  PURPOSE:  To the name of the variable whose value should be obtained.
		std::string varName_;

		//  II.  Disallowed auto-generated methods:
		//  No copy constructor:
		VarSegment(const VarSegment &);

		//  No copy assignment op:
		VarSegment &operator=(const VarSegment &);

	protected:
		//  III.  Protected methods:
		//  PURPOSE:  To return the mapped-to string for '*this' given query
		//		'query'.
		std::string getStringValue(const Request &request)
			const
		{
			if (request.getUserPtr() != NULL)
			{
				const char *varNameCPtr = varName_.c_str();

				if (strcmp(varNameCPtr, NAME_VAR) == 0)
				{
					return (request.getUserPtr()->getName());
				}

				if (strcmp(varNameCPtr, ID_VAR) == 0)
				{
					return (request.getUserPtr()->getId());
				}

				if (strcmp(varNameCPtr, CONTENT_VAR) == 0)
				{
					return (request.getUserPtr()->getContent());
				}
			}

			std::map<std::string, std::string>::const_iterator
				iter = request.getQuery().find(varName_);

			if (iter == request.getQuery().end())
			{
				return (std::string(""));
			}

			return (iter->second);
		}

	public:
		//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
		//  PURPOSE:  To initialize '*this' to the note that the value of the
		//		variable named 'varName' should be filled-in.  No return value.
		VarSegment(const std::string &varName) : Segment(),
												 varName_(varName)
		{
		}

		//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
		//		return value.
		~VarSegment()
		{
		}

		//  V.  Accessors:

		//  VI.  Mutators:

		//  VII.  Methods that do main and misc. work of class:
		//  PURPOSE:  To write the segment of '*this' to memory to address 'cPtr'
		//		given query 'query'.  Returns address just after written
		//		segment.
		char *memWrite(const Request &request,
					   char *cPtr)
			const
		{
			std::string value = getStringValue(request);
			size_t length = value.length();

			memcpy(cPtr, value.c_str(), length);
			return (cPtr + length);
		}

		//  PURPOSE:  To return the length of the segment of '*this' given
		//		query 'query'.
		size_t getLength(const Request &request)
			const
		{
			return (getStringValue(request).length());
		}
	};

	//  PURPOSE:  To keep track of the contents of a filled-in page, and how
	//	many bytes are in the contents.
	struct ContentsAndNumBytes
	{
		char *contentsCPtr_;
		size_t numBytes_;

		ContentsAndNumBytes(char *contentsCPtr,
							size_t numBytes) : contentsCPtr_(contentsCPtr),
											   numBytes_(numBytes)
		{
		}

		~ContentsAndNumBytes()
		{
			safeFree(contentsCPtr_);
		}
	};

	//  I.  Member vars:
	//  PURPOSE:  To map from thread ids to the addresses of filled-in pages made
	//	for the request they were serving.
	std::map<pthread_t, ContentsAndNumBytes *>
		threadIdToPagePtrMap_;

	//  PURPOSE:  To act as a lock on access to 'threadIdToPagePtrMap_'.
	pthread_mutex_t pageLock_;

	//  PURPOSE:  To hold the addresses of Segment instances that comprise
	//	'*this' template.
	std::list<Segment *> segmentPtrDS_;

	//  PURPOSE:  To hold the "end" iterator value of 'segmentPtrDS_'
	std::map<pthread_t, ContentsAndNumBytes *>::const_iterator
		end_;

	//  II.  Disallowed auto-generate methods:
	//  No default constructor:
	FixedFormPage();

	//  No copy constructor:
	FixedFormPage(const FixedFormPage &);

	//  No copy assignment op:
	FixedFormPage &
	operator=(const FixedFormPage &);

protected:
	//  III.  Protected methods:
	//  PURPOSE:  To clear the contents of 'segmentPtrDS_'.  No parameters.
	//	No return value.
	void clearSegmentPtrDS()
	{
		//  I.  Application validity check:

		//  II.  Clear 'segmentPtrDS_':
		//  II.A.  'delete()' Segment instances:
		std::list<Segment *>::iterator
			iter = segmentPtrDS_.begin();
		std::list<Segment *>::iterator
			end = segmentPtrDS_.end();

		for (; iter != end; iter++)
		{
			safeDelete(*iter);
		}

		//  II.B.  Clear 'segmentPtrDS_':
		segmentPtrDS_.clear();

		//  III.  Finished:
	}

	//  PURPOSE:  To re-populate 'segmentPtrDS_'.  No parameters.  No return
	//	value.
	void computeSegmentPtrDS();

	//  PURPOSE:  To return the length that would result from instantiating
	//	'*this' given Request 'request'.
	size_t getInstantiatedLength(const Request &request)
		const
	{
		size_t toReturn = 0;
		std::list<Segment *>::const_iterator
			iter = segmentPtrDS_.begin();
		std::list<Segment *>::const_iterator
			end = segmentPtrDS_.end();

		for (; iter != end; iter++)
		{
			toReturn += (*iter)->getLength(request);
		}

		return (toReturn);
	}

	//  PURPOSE: To instantiate '*this' page given Request 'request'.  Returns
	//	address of 'FixedPage' telling instantiation.
	char *getInstantiatedContent(size_t instantiatedLength,
								 const Request &request)
		const
	{
		char *cPtr;
		char *instantiatedContentsCPtr = (char *)
			malloc(instantiatedLength + 1);
		std::list<Segment *>::const_iterator
			iter = segmentPtrDS_.begin();
		std::list<Segment *>::const_iterator
			end = segmentPtrDS_.end();

		for (cPtr = instantiatedContentsCPtr;
			 iter != end;
			 iter++)
		{
			cPtr = (*iter)->memWrite(request, cPtr);
		}

		*cPtr = '\0';
		return (instantiatedContentsCPtr);
	}

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to be the page read() from the file
	//	'filepathCPtr'.
	FixedFormPage(const char *filepathCPtr) : BaseFilePage(filepathCPtr),
											  segmentPtrDS_(),
											  end_(threadIdToPagePtrMap_.end())
	{
		pthread_mutex_init(&pageLock_, NULL);
		computeSegmentPtrDS();
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~FixedFormPage();

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the number of bytes in '*this' page given Request
	//	'request'.
	size_t getNumBytes(const Request &request);

	//  PURPOSE:  To return the MIME format of '*this' page.  No parameters.
	const char *getMimeCPtr()
	{
		return (DEFAULT_MIME_FORMAT);
	}

	//  PURPOSE:  To 'write()' '*this' page for file descriptor 'outFd' given
	//	Request 'request'.
	int write(int outFd,
			  const Request &request);

	//  PURPOSE:  To perhaps get rid of '*this' page after it has been
	//	'write()'n.  No parameters.  No return value.
	void perhapsDispose();

	//  PURPOSE:  To perhaps re-load the definition of '*this' from the
	//	file-system.  No parameters.  Return 'true' if did re-load or 'false'
	//	otherwise.
	bool didPerhapsUpdate()
	{
		if (BaseFilePage::didPerhapsUpdate())
		{
			pthread_mutex_lock(&pageLock_);
			computeSegmentPtrDS();
			pthread_mutex_unlock(&pageLock_);
			return (true);
		}

		return (false);
	}
};

//
class PageStore
{
	//  I.  Member vars:
	//  PURPOSE:  To map from URI strings to addresses of FilePage instances.
	std::map<std::string, FilePage *>
		uriToFilePagePtrMap_;

	//  PURPOSE:  To map from URI string to addresses of FixedFormPage instances.
	std::map<std::string, FixedFormPage *>
		uriToFixedFormPagePtrMap_;

	//  PURPOSE:  To hold the bad request page.
	FixedPage badRequestPage_;

	//  PURPOSE:  To hold the unauthorized page.
	FixedPage unauthorizedPage_;

	//  PURPOSE:  To hold the forbidden page.
	FixedPage forbiddenPage_;

	//  PURPOSE:  To hold the not found page.
	FixedPage notFoundPage_;

	//  PURPOSE:  To hold the method not allowed page.
	FixedPage methodNotAllowedPage_;

	//  II.  Disallowed auto-generate methods:

protected:
	//  III.  Protected methods:
	//  PURPOSE:  To load 'query' with <attribute,value> pairs parsed from
	//	'sourceCPtr'.
	void parseQuery(const char *sourceCPtr,
					std::map<std::string, std::string> &
						query);

	//  PURPOSE:  To translate the URI in 'uriCPtr' into the file path in
	//	'filepathSpace' of length 'filepathSpaceLen'.  No return value.
	void translateUriToFilepath(const char *uriCPtr,
								char *filepathSpace,
								size_t filepathSpaceLen,
								std::map<std::string, std::string> &
									query);

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to a PageStore empty of user-provided
	//	content.  No parameters.  No return value.
	PageStore();

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~PageStore();

	//  V.  Accessors:
	//  PURPOSE:  To return the address of the bad request page.
	FixedPage *getBadRequestPage()
	{
		return (&badRequestPage_);
	}

	//  PURPOSE:  To return the address of the unauthorized page.
	FixedPage *getUnauthorizedPage()
	{
		return (&unauthorizedPage_);
	}

	//  PURPOSE:  To return the address of the forbidden page.
	FixedPage *getForbiddenPage()
	{
		return (&forbiddenPage_);
	}

	//  PURPOSE:  To return the address of the not found page.
	FixedPage *getNotFoundPage()
	{
		return (&notFoundPage_);
	}

	//  PURPOSE:  To return the address of the method not allowed page.
	FixedPage *getMethodNotAllowedPage()
	{
		return (&methodNotAllowedPage_);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
	//  PURPOSE:  To return the address of a Page referenced by 'request'.
	Page *getPagePtr(Request &request,
					 UserContent &content,
					 SessionStore &sessionStore);

	//  PURPOSE:  To return the address of the page corresponding to 'errCode'.
	Page *getErrorPage(httpReturnCode_ty errCode)
	{
		Page *toReturn;

		switch (errCode)
		{
		case BAD_REQUEST_HTTP_RET_CODE:
			toReturn = getBadRequestPage();
			break;

		case UNAUTHORIZED_HTTP_RET_CODE:
			toReturn = getUnauthorizedPage();
			break;

		case FORBIDDEN_HTTP_RET_CODE:
			toReturn = getForbiddenPage();
			break;

		case METHOD_NOT_ALLOWED_HTTP_RET_CODE:
			toReturn = getMethodNotAllowedPage();
			break;

		case NOT_FOUND_HTTP_RET_CODE:
		default:
			toReturn = getNotFoundPage();
			break;
		}

		return (toReturn);
	}
};

//  PURPOSE:  To hold file descriptors and addresses of clients that have
//	been 'accept()'ed but not yet served.
class NewClientBuffer
{
	//  0.  Local types:
	class BufferElement
	{
		//  I.  Member vars:
		//  PURPOSE:  To tell the file descriptor with which to communicate with
		//		the client.
		int fd_;

		//  PURPOSE:  To tell the address from which the client connects.
		struct sockaddr_in addr_;

		//  PURPOSE:  To tell the length of the data in 'addr_'.
		socklen_t addrLen_;

		//  II.  Disallowed auto-generated methods:

	protected:
		//  III.  Protected methods:

	public:
		//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
		//  PURPOSE:  To initialize '*this' to blank values.  No parameters.  No
		//		return value.
		BufferElement() : fd_(-1),
						  addr_(),
						  addrLen_(sizeof(struct sockaddr_in))
		{
		}

		//  PURPOSE:  To initialize '*this' to note that file descriptor 'fd'
		//		should be used to communicate with the client at address
		//		'addr', and that the length of the info in 'addr' is
		//		'addrLen'.  No return value.
		BufferElement(int fd,
					  const struct sockaddr_in &addr,
					  int addrLen) : fd_(fd),
									 addr_(addr),
									 addrLen_(addrLen)
		{
		}

		//  PURPOSE:  To make '*this' a copy of 'source'.  No return value.
		BufferElement(const BufferElement &source) : fd_(source.getFd()),
													 addr_(source.getAddr()),
													 addrLen_(source.getAddrLen())
		{
		}

		//  PURPOSE:  To release the resources of '*this', make '*this' a copy
		//		of 'source', and return a reference to '*this'.
		BufferElement &
		operator=(const BufferElement &source)
		{
			//  I.  Application validity check:
			if (this == &source)
			{
				return (*this);
			}

			//  II.  Release resources:

			//  III.  Copy 'source':
			fd_ = source.getFd();
			addr_ = source.getAddr();
			addrLen_ = source.getAddrLen();

			//  IV.  Finished:
			return (*this);
		}

		//  PURPOSE:  To release the resources of '*this'.  No parameters.
		//		No return value.
		~BufferElement()
		{
		}

		//  V.  Accessors:
		//  PURPOSE:  To return the file descriptor with which to communicate
		//		with the client.  No parameters.
		int getFd()
			const
		{
			return (fd_);
		}

		//  PURPOSE:  To return the address from which the client connects.
		//		No parameters.
		const struct sockaddr_in &
		getAddr()
			const
		{
			return (addr_);
		}

		//  PURPOSE:  To tell the length of the data in 'addr_'.
		socklen_t getAddrLen()
			const
		{
			return (addrLen_);
		}

		//  VI.  Mutators:
		//  PURPOSE:  To set '*this' to note that file descriptor 'fd'
		//		should be used to communicate with the client at address
		//		'addr', and that the length of the info in 'addr' is
		//		'addrLen'.  No return value.
		void set(int fd,
				 const struct sockaddr_in &addr,
				 int addrLen)
		{
			fd_ = fd;
			addr_ = addr;
			addrLen_ = addrLen;
		}

		//  VII.  Methods that do main and misc. work of class:
		//  PURPOSE:  To set 'fd', 'addr' and 'addrLen' to the values stored
		//		in '*this'.  No return value.
		void get(int &fd,
				 struct sockaddr_in &addr,
				 socklen_t &addrLen)
			const
		{
			fd = fd_;
			addr = addr_;
			addrLen = addrLen_;
		}
	};

	//  I.  Member vars:
	//  PURPOSE:  To hold the BufferElements.
	BufferElement array_[NEW_CLIENT_BUFFER_LEN];

	//  PURPOSE:  To hold the index of where to put the next item in.
	size_t inIndex_;

	//  PURPOSE:  To hold the index of where to get the next item.
	size_t outIndex_;

	//  PURPOSE:  To hold how many items are in '*this'.
	size_t count_;

	//  PURPOSE:  To hold the mutex to regulate access to '*this'.
	pthread_mutex_t lock_;

	//  PURPOSE:  To be used to tell threads when '*this' is no longer empty.
	pthread_cond_t notEmpty_;

	//  II.  Disallowed auto-generated methods:
	//  No copy constructor:
	NewClientBuffer(const NewClientBuffer &);

	//  No copy assignment op:
	NewClientBuffer &
	operator=(const NewClientBuffer &);

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to an empty buffer.  No parameters.
	//	No return value.
	NewClientBuffer() : inIndex_(0),
						outIndex_(0),
						count_(0)
	{
		pthread_mutex_init(&lock_, NULL);
		pthread_cond_init(&notEmpty_, NULL);
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.
	//	No return value.
	~NewClientBuffer()
	{
		pthread_cond_destroy(&notEmpty_);
		pthread_mutex_destroy(&lock_);
	}

	//  V.  Accessors:

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc work of class
	//  PURPOSE:  To insert file descriptor 'fd', address 'addr' and the
	//	length of the data in 'addr' noted in 'addrLen' in '*this', *ONLY*
	//	if there is space.  No return value.
	void put(int fd,
			 const struct sockaddr_in &addr,
			 socklen_t addrLen)
	{
		pthread_mutex_lock(&lock_);

		if (count_ < NEW_CLIENT_BUFFER_LEN)
		{
			array_[inIndex_].set(fd, addr, addrLen);

			if (++inIndex_ >= NEW_CLIENT_BUFFER_LEN)
			{
				inIndex_ = 0;
			}

			count_++;
			pthread_cond_signal(&notEmpty_);
		}

		pthread_mutex_unlock(&lock_);
	}

	//  PURPOSE:  To wait until data is available, and then to set 'fd', 'addr'
	//	and 'addrLen' equal to the next available values stored in '*this'.
	//	No return value.
	void get(int &fd,
			 struct sockaddr_in &addr,
			 socklen_t &addrLen)
	{
		pthread_mutex_lock(&lock_);

		while (shouldRun && (count_ == 0))
		{
			pthread_cond_wait(&notEmpty_, &lock_);

			if (!shouldRun)
			{
				printf("Ending thread %lu\n",
					   pthread_self());
				pthread_mutex_unlock(&lock_);
				pthread_exit(NULL);
			}
		}

		array_[outIndex_].get(fd, addr, addrLen);

		if (++outIndex_ >= NEW_CLIENT_BUFFER_LEN)
		{
			outIndex_ = 0;
		}

		count_--;
		pthread_mutex_unlock(&lock_);
	}

	//  PURPOSE:  To notify all waiting threads to wake up.  No parameters.
	//	No return value.
	void wakeWaiters()
	{
		pthread_mutex_lock(&lock_);
		pthread_cond_broadcast(&notEmpty_);
		pthread_mutex_unlock(&lock_);
	}
};

//  PURPOSE:  To hold information needed by the thread that listens for new
//	clients.
class InfoForListeningThread
{
	//  I.  Member vars:
	//  PURPOSE:  To hold the file descriptor used to listen for clients.
	int listenFd_;

	//  PURPOSE:  To hold a reference to the buffer into which the file
	//	descriptor and address of clients should be placed.
	NewClientBuffer &newClientBuffer_;

	//  II.  Disallowed auto-generated methods:
	//  No default constructor:
	InfoForListeningThread();

	//  No copy constructor:
	InfoForListeningThread(const InfoForListeningThread &);

	//  No copy assignment op:
	InfoForListeningThread &
	operator=(const InfoForListeningThread &);

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this'.
	InfoForListeningThread(int listenFd,
						   NewClientBuffer &newClientBuffer) : listenFd_(listenFd),
															   newClientBuffer_(newClientBuffer)
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.
	//	No return value.
	~InfoForListeningThread()
	{
	}

	//  V.  Accessors:
	//  PURPOSE:  To return the file descriptor used to listen for clients.
	int getListenFd()
		const
	{
		return (listenFd_);
	}

	//  PURPOSE:  To a reference to the buffer into which the file
	//	descriptor and address of clients should be placed.
	NewClientBuffer &
	getNewClientBuffer()
		const
	{
		return (newClientBuffer_);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc work of class:
};

//  PURPOSE:  To hold info to give to client-serving threads.
class InfoForClientServingThread
{
	//  I.  Member vars:
	//  PURPOSE:  To hold a reference to the user content storage instance.
	UserContent &contentStore_;

	//  PURPOSE:  To hold a reference to the session storage instance.
	SessionStore &sessionStore_;

	//  PURPOSE:  To hold a reference to the page storage instance.
	PageStore &pageStore_;

	//  PURPOSE:  To hold a reference to the buffer of file descriptors and
	//	addresses of clients.
	NewClientBuffer &newClientBuffer_;

	//  II.  Disallowed auto-generated methods:
	//  No default constructor:
	InfoForClientServingThread();

	//  No copy constructor:
	InfoForClientServingThread(const InfoForClientServingThread &);

	//  No copy assignment op:
	InfoForClientServingThread &
	operator=(const InfoForClientServingThread &);

protected:
	//  III.  Protected methods:

public:
	//  IV.  Constructor(s), assignment op(s), factory(s) and destructor:
	//  PURPOSE:  To initialize '*this' to hold references to 'content',
	//	'sessionStore', 'pageStore' and 'newClientBuffer'.  No return value.
	InfoForClientServingThread(UserContent &content,
							   SessionStore &sessionStore,
							   PageStore &pageStore,
							   NewClientBuffer &newClientBuffer) : contentStore_(content),
																   sessionStore_(sessionStore),
																   pageStore_(pageStore),
																   newClientBuffer_(newClientBuffer)
	{
	}

	//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
	//	return value.
	~InfoForClientServingThread()
	{
	}

	//  V.  Accessors:
	//  PURPOSE:  To return a reference to the user content storage instance.
	UserContent &getContentStore()
		const
	{
		return (contentStore_);
	}

	//  PURPOSE:  To return a reference to the session storage instance.
	SessionStore &getSessionStore()
		const
	{
		return (sessionStore_);
	}

	//  PURPOSE:  To return a reference to the page storage instance.
	PageStore &getPageStore()
		const
	{
		return (pageStore_);
	}

	//  PURPOSE:  To hold the address of the buffer of file descriptors and
	//	addresses of clients.
	NewClientBuffer &
	getNewClientBuffer()
		const
	{
		return (newClientBuffer_);
	}

	//  VI.  Mutators:

	//  VII.  Methods that do main and misc. work of class:
};

/*----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----					Definition of helper functions:					----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*/

//  PURPOSE:  To return a random integer in a thread-safe fashion.  No
//	parameters.
int safeRand()
{
	int toReturn;

	pthread_mutex_lock(&safeRandLock__);
	toReturn = rand();
	pthread_mutex_unlock(&safeRandLock__);

	return (toReturn);
}

//  PURPOSE:  To return the value of hexadecimal digit 'hexDigit'.
//	NOTE: Assumes 'isxdigit(hexDigit)' returns non-0.
int hexDigitValue(char hexDigit)
{
	if (isdigit(hexDigit))
		return (hexDigit - '0');

	if (isupper(hexDigit))
		return (hexDigit - 'A' + 10);

	return (hexDigit - 'a' + 10);
}

//  PURPOSE:  To which ever of 'run0' and 'run1' comes earlier.
const char *firstCPtr(const char *run0,
					  const char *run1)
{
	if (run0 == NULL)
		return (run1);

	if (run1 == NULL)
		return (run0);

	return ((run0 < run1) ? run0 : run1);
}

//  PURPOSE:  To return a 'std::string' instance of the URL query fragment
//	pointed to by 'cPtr'.  Advances 'cPtr' passed query fragment.
std::string translateUrlCPtr(const char *&cPtr)
{
	std::string toReturn;
	char hiNibble;
	char loNibble;
	bool shouldContinue = true;

	while (shouldContinue)
	{
		switch (*cPtr)
		{
		case '\0':
		case FRAGMENT_CHAR:
		case QUERY_ASSIGNMENT_CHAR:
		case QUERY_SEGMENT_CHAR:
			shouldContinue = false;
			break;

		case QUERY_SPACE_CHAR:
			toReturn += ' ';
			cPtr++;
			break;

		case QUERY_HEX_ESCAPE_CHAR:
			cPtr++; // Go past QUERY_HEX_ESCAPE_CHAR
			hiNibble = *cPtr++;
			loNibble = *cPtr++;

			if (!isxdigit(hiNibble) || !isxdigit(loNibble))
			{
				throw BAD_REQUEST_HTTP_RET_CODE;
			}

			toReturn += (char)((hexDigitValue(hiNibble) << 4) | hexDigitValue(loNibble));
			break;

		default:
			toReturn += *cPtr++;
			break;
		}
	}

	return (toReturn);
}

//  PURPOSE:  To be the "robust" version of read() in the manner advocated by
//	Bryant and O'Hallaron.  'fd' tells the file descriptor from which to
//	read.  'usrbuf' tells the buffer into which to read.  'n' tells the
//	length of 'usrbuf'.  Returns number of bytes read or -1 on some type
//	of error.
ssize_t rio_read(int fd,
				 char *usrbuf,
				 size_t n)
{
	//  I.  Application validity check:

	//  II.  Attempt to read:
	ssize_t nread;
	size_t nleft = n;
	char *bufp = usrbuf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR) // interrupted by sig handler return
			{
				nread = 0; // and call read() again
			}
			else
			{
				return -1; // errno set by read()
			}
		}
		else if (nread == 0)
		{
			break; // EOF
		}

		nleft -= nread;
		bufp += nread;
	}

	//  III.  Finished:
	return (n - nleft); // return >= 0
}

//  PURPOSE:  To return the 'httpMethod_ty' value whose name is pointed to
//	by 'methodNameCPtr', or 'BAD_HTTP_METH' if no match is found.
httpMethod_ty getHttpMethod(const char *methodNameCPtr)
{
	//  I.  Application validity check:

	//  II.  Look for 'methodNameCPtr':

	for (int index = 0; index < (int)NUM_HTTP_METH; index++)
	{
		if (strcasecmp(methodNameCPtr, HTTP_METHOD_NAME_ARRAY[index]) == 0)
		{
			return ((httpMethod_ty)index);
		}
	}

	//  III.  Finished:
	return (BAD_HTTP_METH);
}

//  PURPOSE:  To
const char *getReturnCodeCPtr(httpReturnCode_ty returnCode)
{
	const char *toReturn;

	switch (returnCode)
	{
	case OK_HTTP_RET_CODE:
		toReturn = "OK";
		break;
	case BAD_REQUEST_HTTP_RET_CODE:
		toReturn = "Bad request";
		break;
	case UNAUTHORIZED_HTTP_RET_CODE:
		toReturn = "Unauthorized";
		break;
	case FORBIDDEN_HTTP_RET_CODE:
		toReturn = "Forbidden";
		break;
	case NOT_FOUND_HTTP_RET_CODE:
		toReturn = "Not found";
		break;
	case METHOD_NOT_ALLOWED_HTTP_RET_CODE:
		toReturn = "Method not allowed";
		break;
	}

	return (toReturn);
}

//  PURPOSE:  To return 'true' if 'source' begins with substring 'beginning'
//	(ignoring case), or 'false' otherwise.
bool beginsWith(const char *source,
				const char *beginning)
{
	size_t sourceLen = strlen(source);
	size_t beginningLen = strlen(beginning);

	if (sourceLen < beginningLen)
	{
		return (false);
	}

	return (strncasecmp(source, beginning, beginningLen) == 0);
}

//  PURPOSE:  To return 'true' if 'source' ends with substring 'ending'
//	(ignoring case), or 'false' otherwise.
bool endsWith(const char *source,
			  const char *ending)
{
	size_t sourceLen = strlen(source);
	size_t endingLen = strlen(ending);

	if (sourceLen < endingLen)
	{
		return (false);
	}

	return (strcasecmp(source + sourceLen - endingLen, ending) == 0);
}

//  PURPOSE:  To return 'true' if 'filepathCPtr' has an extension implying
//	that it is a static file (as opposed to a program or template), or to
//	return 'false' otherwise.
bool appearsToBeStaticFile(const char *filepathCPtr)
{
	//  I.  Application validity check:

	//  II.  Return value:
	return (endsWith(filepathCPtr, ".css") ||
			endsWith(filepathCPtr, ".csv") ||
			endsWith(filepathCPtr, ".htm") ||
			endsWith(filepathCPtr, ".html") ||
			endsWith(filepathCPtr, ".ico") ||
			endsWith(filepathCPtr, ".gif") ||
			endsWith(filepathCPtr, ".jpg") ||
			endsWith(filepathCPtr, ".jpeg") ||
			endsWith(filepathCPtr, ".png"));
}

//  PURPOSE:  To return the address of text telling the name of the guess
//	of the MIME format of a file with path 'filepathCPtr' and with
//	contents 'contentsCPtr'.
const char *getMimeGuess(const char *filepathCPtr,
						 const char *contentsCPtr)
{
	//  I.  Application validity check:

	//  II.  :
	const char *toReturn = DEFAULT_MIME_FORMAT;

	if (endsWith(filepathCPtr, ".json"))
	{
		toReturn = "application/json";
	}
	else if (endsWith(filepathCPtr, ".pdf"))
	{
		toReturn = "application/pdf";
	}
	else if (endsWith(filepathCPtr, ".zip"))
	{
		toReturn = "application/zip";
	}
	if (endsWith(filepathCPtr, ".tar"))
	{
		toReturn = "application/x-tar";
	}
	else if (endsWith(filepathCPtr, ".oga"))
	{
		toReturn = "audio/ogg";
	}
	else if (endsWith(filepathCPtr, ".jpg") || endsWith(filepathCPtr, ".jpeg") ||
			 endsWith(filepathCPtr, ".png") || endsWith(filepathCPtr, ".ico"))
	{
		if (memcmp(contentsCPtr, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) == 0)
		{
			toReturn = "image/png";
		}
		else if ((memcmp(contentsCPtr, "\x47\x49\x46\x38\x37\x61", 6) == 0) ||
				 (memcmp(contentsCPtr, "\x47\x49\x46\x38\x39\x61", 6) == 0))
		{
			toReturn = "image/gif";
		}
		else if ((memcmp(contentsCPtr, "\xFF\xD8\xFF\xDB", 4) == 0) ||
				 (memcmp(contentsCPtr,
						 "\xFF\xD8\xFF\xE0\x00\x10\x4A\x46\x49\x46\x00\x01",
						 10) == 0) ||
				 (memcmp(contentsCPtr, "\xFF\xD8\xFF\xEE", 4) == 0) ||
				 ((memcmp(contentsCPtr + 0, "\xFF\xD8\xFF\xE1", 4) == 0) &&
				  (memcmp(contentsCPtr + 6, "\x45\x78\x69\x66\x00\x00", 6) == 0)))
		{
			toReturn = "image/jpeg";
		}
		else if ((memcmp(contentsCPtr, "\x00\x01\x00\x00", 4) == 0) ||
				 (memcmp(contentsCPtr, "\x00\x00\x01\x00", 4) == 0))
		{
			toReturn = "image/x-icon";
		}
	}
	else if (endsWith(filepathCPtr, ".html") || endsWith(filepathCPtr, ".htm"))
	{
		toReturn = DEFAULT_MIME_FORMAT;
	}
	else if (endsWith(filepathCPtr, ".otf"))
	{
		toReturn = "font/otf";
	}
	if (endsWith(filepathCPtr, ".csv"))
	{
		toReturn = "text/csv";
	}
	else if (endsWith(filepathCPtr, ".txt"))
	{
		toReturn = "text/plain";
	}
	else if (endsWith(filepathCPtr, ".ogv"))
	{
		toReturn = "video/ogg";
	}
	else
	{
		const char *extensionPtr = strrchr(filepathCPtr, '.');

		if (extensionPtr == NULL)
		{
			if (beginsWith(contentsCPtr, "<!DOCTYPE html") ||
				beginsWith(contentsCPtr, "<!DOCTYPE\thtml") ||
				beginsWith(contentsCPtr, "<html"))
			{
				toReturn = DEFAULT_MIME_FORMAT;
			}
		}
	}

	//  III.  Finished:
	return (toReturn);
}

/*----	----	----	----	----	----	----	----	----	----*
 *----																	----*
 *----						Definition of class members:				----*
 *----																	----*
 *----	----	----	----	----	----	----	----	----	----*/

//  PURPOSE:  To return 'true' if 'lhs' should be ordered before 'rhs', or
//	'false' otherwise.
bool operator<(const Cookie &lhs,
			   const Cookie &rhs)
{
	//  I.  Application validity check:

	//  II.  Return value:
	return (strcmp(lhs.getCPtr(), rhs.getCPtr()) < 0);
}

//  PURPOSE:  To read the definition of '*this' from 'filepathCPtr'.  No
//	return value.
UserContent::UserContent(const char *filepathCPtr,
						 const char *pythonProgNameCPtr,
						 const char *externalContentUrlCPtr) : userNameToContentMap_(),
															   pythonPid_(-1),
															   toPythonFd_(-1),
															   fromPythonFd_(-1),
															   curlHandle_(NULL)
{
	//  I.  Application validity check:

	//  II.  Initialize member vars:
	//  II.A.  Attempt to read 'filepathCPtr':
	FILE *filePtr = fopen(filepathCPtr, "r");

	if (filePtr != NULL)
	{
		char line[LINE_BUFFER_LEN];
		char user[LINE_BUFFER_LEN];
		char id[LINE_BUFFER_LEN];
		char content[LINE_BUFFER_LEN];
		int lineNum = 0;

		while (fgets(line, sizeof(line), filePtr) != NULL)
		{
			lineNum++;

			if (sscanf(line, "%s %s \"%[^\"]s\"", user, id, content) != 3)
			{
				fprintf(stderr,
						"Content file syntax error on line %d of %s.\n"
						"Syntax:\t<username> <id> \"content text\"",
						lineNum, filepathCPtr);
				fclose(filePtr);
				exit(EXIT_FAILURE);
			}

			userNameToContentMap_[user] = User(user, id, content);
		}

		fclose(filePtr);
	}

	//  II.B.  Attempt to launch Python program:
	if (pythonProgNameCPtr != NULL)
	{
		int toPythonArray[2];
		int fromPythonArray[2];

		//  YOUR CODE HERE TODO

		waitForPrompt(fromPythonFd_);
	}

	//  II.C.  Attempt to initialize libcurl:
	if (externalContentUrlCPtr != NULL)
	{

		//  YOUR CODE HERE
	}

	//  III.  Finished:
}

//  PURPOSE:  To release the resources of '*this'.  No parameters.
//	No return value.
UserContent::~UserContent()
{
	//  I.  Application validity check:

	//  II.  Release resources:
	//  II.A.  End libcurl:
	if (curlHandle_ != NULL)
	{
		//  YOUR CODE HERE
		curlHandle_ = NULL;
	}

	//  II.B.  Tell python to quit:
	if (pythonPid_ > 0)
	{
		endPython();

		while (isPythonRunning)
			sleep(1);

		pythonPid_ = -1;
	}

	//  III.  Finished:
}

//  PURPOSE:  To initialize '*this' using the 'requestBufferLen' bytes
//	'read()' from 'inFd' and placed into 'requestBuffer'.  'sessionStore'
//	keeps track of the Session information.  No return value.
void Request::initialize(int inFd,
						 char *requestBuffer,
						 int requestBufferLen,
						 SessionStore &sessionStore)
{
	//  I.  Application validity check:

	//  II.  Initialize member vars:
	//  II.A.  Finish making '*this' blank:
	//  (Assume .clear()) has already been called.

	//  II.B.  Attempt to parse first line:
	char formatArray[MAX_TINY_ARRAY_LEN];
	char tempCArray0[MAX_TINY_ARRAY_LEN];
	char tempCArray1[MAX_TINY_ARRAY_LEN];

	snprintf(formatArray, MAX_TINY_ARRAY_LEN, "%%%lus %%%lus %%%lus",
			 sizeof(tempCArray0) - 1, sizeof(path_) - 1, sizeof(version_) - 1);

	if ((sscanf(requestBuffer, formatArray, tempCArray0, path_, version_) < 1) ||
		((method_ = getHttpMethod(tempCArray0)) == BAD_HTTP_METH))
	{
		throw BAD_REQUEST_HTTP_RET_CODE;
	}

	//  II.C.  Parse rest of header:
	const char *carriageReturnCPtr;
	const char *newlineCPtr;
	const char *nextLineCPtr = requestBuffer;
	const char *requestBufferEndCPtr = requestBuffer + requestBufferLen;

	//  II.C.1.  Each iteration handles one more header line:
	while (nextLineCPtr < requestBufferEndCPtr)
	{
		//  II.C.1.a.  Look for end of current line:
		//  II.C.1.a.I.  Look for next carriage-return and new-line chars:
		carriageReturnCPtr = strchr(nextLineCPtr, '\r');
		newlineCPtr = strchr(nextLineCPtr, '\n');

		//  II.C.1.a.II.  Update 'nextLineCPtr' based upon
		//		      location of '\r' and '\n':
		if ((carriageReturnCPtr + 1) == newlineCPtr)
		{
			//  II.A.  Expected case "\r\n":
			nextLineCPtr = newlineCPtr + 1;
		}
		else if (carriageReturnCPtr == (newlineCPtr + 1))
		{
			//  II.B.  Inverse of expected case "\n\r":
			nextLineCPtr = carriageReturnCPtr + 1;
		}
		else if (carriageReturnCPtr == NULL)
		{
			//  II.C.  No '\r'
			if (newlineCPtr == NULL)
			{
				//  II.C.1.  No '\r' or '\n': nothing else to parse:
				nextLineCPtr = NULL;
				break;
			}
			else
			{
				//  II.C.2.  No '\r' but yes '\n': use 'newlineCPtr':
				nextLineCPtr = newlineCPtr + 1;
			}
		}
		else if (newlineCPtr == NULL)
		{
			//  II.D.  No '\n' but yes '\r': use 'carriageReturnCPtr':
			nextLineCPtr = carriageReturnCPtr + 1;
		}
		else
		{
			//  II.E.  Both '\n' and '\r' exist, but separated.  Use whichever is 1st:
			nextLineCPtr = firstCPtr(carriageReturnCPtr, newlineCPtr) + 1;
		}

		//  II.C.1.a.III.  Quit loop if see blank line, or end-of-input:
		if ((*nextLineCPtr == '\r') ||
			(*nextLineCPtr == '\n') ||
			(*nextLineCPtr == '\0'))
		{
			break;
		}

		//  II.C.1.b.  Attempt to interpret header line:
		//  II.C.1.b.I.  Fastforward past spaces:
		while ((*nextLineCPtr == ' ') || (*nextLineCPtr == '\t'))
		{
			nextLineCPtr++;
		}

		//  II.C.1.b.II.
		if (sscanf(nextLineCPtr, "%s", tempCArray0) != 1)
		{
			continue;
		}

		if (strncasecmp(tempCArray0,
						COOKIE_HEADER_TEXT,
						sizeof(COOKIE_HEADER_TEXT) - 1) == 0)
		{
			nextLineCPtr += sizeof(COOKIE_HEADER_TEXT) - 1;
			tempCArray0[0] = '\0';
			tempCArray1[0] = '\0';

			if ((sscanf(nextLineCPtr, " %[^= ] = %s", tempCArray0, tempCArray1) == 2) &&
				(strcasecmp(tempCArray0, SESSION_COOKIE_NAME) == 0))
			{
				header_.setCookie(Cookie(tempCArray1));
				userPtr_ = sessionStore.getUserPtr(header_.getCookie());
			}
		}
	}

	//  II.D.  Set 'contentPtr_':
	if ((nextLineCPtr != NULL) && (nextLineCPtr < requestBufferEndCPtr))
	{
		//  Go past blank lines:
		while ((nextLineCPtr < requestBufferEndCPtr) &&
			   ((*nextLineCPtr == '\r') || (*nextLineCPtr == '\n')))
		{
			nextLineCPtr++;
		}

		//  Get ptr to content, if anything is there:
		contentPtr_ = ((*nextLineCPtr != '\0') &&
					   (nextLineCPtr < requestBufferEndCPtr))
						  ? nextLineCPtr
						  : NULL;
	}
	else
	{
		contentPtr_ = NULL;
	}

	//  III.  Finished:
}

//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//	return value.
Page::~Page()
{
}

//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//		return value.
FixedFormPage::Segment::~Segment()
{
}

//  PURPOSE:  To re-populate 'segmentPtrDS_'.  No parameters.  No return value.
void FixedFormPage::computeSegmentPtrDS()
{
	//  I.  Application validity check:

	//  II.  Recompute 'segmentPtrDS_':
	//  II.A.  Clear any current contents:
	clearSegmentPtrDS();

	//  II.B.  Each iteration looks for the next variable:
	const char *runCPtr;
	const char *nextVarCPtr;
	const char *endVarCPtr;

	for (runCPtr = getContentsCPtr();
		 *runCPtr != '\0';
		 runCPtr += sizeof(END_TEMPLATE_VAR) - 1)
	{
		//  II.B.1.  Look for next variable:
		nextVarCPtr = strstr(runCPtr, BEGIN_TEMPLATE_VAR);

		//  II.B.2.  Stop loop if no more vars:
		if (nextVarCPtr == NULL)
		{
			break;
		}

		//  II.B.3.  Have found potential var beginning
		//  II.B.3.a.  Handle text between 'runCPtr' and beginning of var:
		if (runCPtr < nextVarCPtr)
		{
			segmentPtrDS_.push_back(new TextSegment(runCPtr, nextVarCPtr - runCPtr));
			runCPtr = nextVarCPtr;
		}

		//  II.B.3.b.  Do error basic checking:
		runCPtr += sizeof(BEGIN_TEMPLATE_VAR) - 1;
		nextVarCPtr = strstr(runCPtr, BEGIN_TEMPLATE_VAR);
		endVarCPtr = strstr(runCPtr, END_TEMPLATE_VAR);

		if ((endVarCPtr == NULL) ||
			((nextVarCPtr != NULL) && (nextVarCPtr < endVarCPtr)))
		{
			fprintf(stderr, "WARNING: Template %s ill-defined.\n", getFilepathCPtr());
			break;
		}

		//  II.B.4.  Gather var name:
		std::string varName;

		while (isspace(*runCPtr))
		{
			runCPtr++;
		}

		while (isalnum(*runCPtr) || (*runCPtr == '_') || (*runCPtr == '.'))
		{
			varName += *runCPtr++;
		}

		while (isspace(*runCPtr))
		{
			runCPtr++;
		}

		if (strncmp(runCPtr, END_TEMPLATE_VAR, sizeof(END_TEMPLATE_VAR) - 1) != 0)
		{
			fprintf(stderr, "WARNING: Template %s ill-defined.\n", getFilepathCPtr());
			break;
		}

		//  II.B.5.  Add segment for var:
		segmentPtrDS_.push_back(new VarSegment(varName));
	}

	//  II.C.  Handle ending text:
	if ((runCPtr != NULL) && (*runCPtr != '\0'))
	{
		segmentPtrDS_.push_back(new TextSegment(runCPtr, strlen(runCPtr)));
	}

	//  III.  Finished:
}

//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//	return value.
FixedFormPage::~FixedFormPage()
{
	//  I.  Application validity check:

	//  II.  Release resources:
	std::map<pthread_t, ContentsAndNumBytes *>::iterator
		iter = threadIdToPagePtrMap_.begin();
	std::map<pthread_t, ContentsAndNumBytes *>::iterator
		end = threadIdToPagePtrMap_.end();

	for (; iter != end; iter++)
	{
		safeDelete(iter->second);
	}

	clearSegmentPtrDS();
	pthread_mutex_destroy(&pageLock_);

	//  III.  Finished:
}

//  PURPOSE:  To return the number of bytes in '*this' page given Request
//	'request'.
size_t FixedFormPage::getNumBytes(const Request &request)
{
	//  I.  Application validity check:

	//  II.  Get the number of bytes:
	//  II.A.  See if value is already stored:
	ContentsAndNumBytes *foundDataPtr = NULL;
	pthread_t threadId = pthread_self();

	//  II.A.1.  Look in 'threadIdToPagePtrMap_':
	pthread_mutex_lock(&pageLock_);
	std::map<pthread_t, ContentsAndNumBytes *>::const_iterator
		iter = threadIdToPagePtrMap_.find(threadId);

	if (iter != end_)
	{
		foundDataPtr = iter->second;
	}

	pthread_mutex_unlock(&pageLock_);

	//  II.A.2.  Return answer if found:
	if (foundDataPtr != NULL)
	{
		return (foundDataPtr->numBytes_);
	}

	//  II.B.  No answer cached, compute:
	//  II.B.1.  Compute answer:
	size_t numBytes = getInstantiatedLength(request);

	//  II.B.2.  Cache answer:
	pthread_mutex_lock(&pageLock_);
	threadIdToPagePtrMap_[threadId] = new ContentsAndNumBytes(NULL, numBytes);
	pthread_mutex_unlock(&pageLock_);

	//  III.  Finished:
	return (numBytes);
}

//  PURPOSE:  To 'write()' '*this' page for file descriptor 'outFd' given
//	Request 'request'.
int FixedFormPage::write(int outFd,
						 const Request &request)
{
	//  I.  Application valdity check:

	//  II.  Write page:
	//  II.A.  See if page info is already stored:
	ContentsAndNumBytes *foundDataPtr = NULL;
	pthread_t threadId = pthread_self();

	//  II.A.1.  Look in 'threadIdToPagePtrMap_':
	pthread_mutex_lock(&pageLock_);
	std::map<pthread_t, ContentsAndNumBytes *>::const_iterator
		iter = threadIdToPagePtrMap_.find(threadId);

	if (iter != end_)
	{
		foundDataPtr = iter->second;
	}

	pthread_mutex_unlock(&pageLock_);

	//  II.A.2.  Write page if info found:
	if ((foundDataPtr != NULL) && (foundDataPtr->contentsCPtr_ != NULL))
	{
		FixedPage page(foundDataPtr->numBytes_, foundDataPtr->contentsCPtr_);

		return (page.write(outFd, request));
	}

	//  II.B.  No answer cached, create page info:
	//  II.B.1.  Create content:
	size_t numBytes = (foundDataPtr == NULL)
						  ? getNumBytes(request)
						  : foundDataPtr->numBytes_;
	char *contentsCPtr = getInstantiatedContent(numBytes, request);

	//  II.B.2.  Cache answer:
	pthread_mutex_lock(&pageLock_);

	if (foundDataPtr == NULL)
	{
		threadIdToPagePtrMap_[threadId] =
			new ContentsAndNumBytes(contentsCPtr, numBytes);
	}
	else
	{
		foundDataPtr->contentsCPtr_ = contentsCPtr;
	}

	pthread_mutex_unlock(&pageLock_);

	//  III.  Finished:
	FixedPage page(foundDataPtr->numBytes_, contentsCPtr);

	return (page.write(outFd, request));
}

//  PURPOSE:  To perhaps get rid of '*this' page after it has been
//	'write()'n.  No parameters.  No return value.
void FixedFormPage::perhapsDispose()
{
	//  I.  Application validity check:

	//  II.  Remove entry for current thread:
	pthread_t threadId = pthread_self();

	pthread_mutex_lock(&pageLock_);
	std::map<pthread_t, ContentsAndNumBytes *>::iterator
		iter = threadIdToPagePtrMap_.find(threadId);

	if (iter != threadIdToPagePtrMap_.end())
	{
		safeDelete(iter->second);
		threadIdToPagePtrMap_.erase(iter);
	}

	pthread_mutex_unlock(&pageLock_);

	//  III.  Finished:
}

//  PURPOSE:  To load 'query' with <attribute,value> pairs parsed from
//	'sourceCPtr'.
void PageStore::parseQuery(const char *sourceCPtr,
						   std::map<std::string, std::string> &
							   query)
{
	//  I.  Application validity check:

	//  II.  Parse query:
	while ((*sourceCPtr != '\0') && (*sourceCPtr != FRAGMENT_CHAR))
	{
		sourceCPtr++; // Go past QUERY_CHAR or QUERY_SEGMENT_CHAR

		std::string name = translateUrlCPtr(sourceCPtr);

		if (*sourceCPtr != QUERY_ASSIGNMENT_CHAR)
		{
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		sourceCPtr++; // Go past QUERY_ASSIGNMENT_CHAR

		std::string value = translateUrlCPtr(sourceCPtr);

		if ((*sourceCPtr != QUERY_SEGMENT_CHAR) &&
			(*sourceCPtr != FRAGMENT_CHAR) &&
			(*sourceCPtr != '\0'))
		{
			throw BAD_REQUEST_HTTP_RET_CODE;
		}

		query[name] = value;
	}

	//  III.  Finished:
}

//  PURPOSE:  To translate the URI in 'uriCPtr' into the file path in
//	'filepathSpace' of length 'filepathSpaceLen'.  No return value.
void PageStore::translateUriToFilepath(const char *uriCPtr,
									   char *filepathSpace,
									   size_t filepathSpaceLen,
									   std::map<std::string, std::string> &
										   query)
{
	//  I.  Application valdity check:

	//  II.  Do translation:
	const char *queryCPtr;
	const char *fragmentCPtr;
	const char *limitCPtr;

	//  II.A.  Translate implicit paths to explicit paths:
	if (strcmp(uriCPtr, IMPLICIT_INDEX_URL) == 0)
	{
		uriCPtr = EXPLICIT_INDEX_URL;
	}

	//  II.B.  Go beyond initial '/':
	if (*uriCPtr == '/')
	{
		uriCPtr++;
	}

	//  II.C.  Look for special URI chars:
	queryCPtr = strchr(uriCPtr, QUERY_CHAR);
	fragmentCPtr = strchr(uriCPtr, FRAGMENT_CHAR);
	limitCPtr = firstCPtr(queryCPtr, fragmentCPtr);

	//  II.C.1.  Do translation:
	if (limitCPtr == NULL)
	{
		//  II.C.1.a.  If no special chars then accept path as-is:
		strncpy(filepathSpace, uriCPtr, filepathSpaceLen);
	}
	else
	{
		//  II.C.1.b.  Handle when have query and/or segment:
		size_t len = limitCPtr - uriCPtr;

		//  II.C.1.b.I.  Obtain path portion:
		strncpy(filepathSpace, uriCPtr, len);
		filepathSpace[len] = '\0';

		//  II.C.1.b.II.  Obtain query portion:
		if (queryCPtr != NULL)
		{
			parseQuery(queryCPtr, query);
		}
	}

	//  II.D.  Check validity of path:

	//  III.  Finished:
}

//  PURPOSE:  To initialize '*this' to a PageStore empty of user-provided
//	content.  No parameters.  No return value.
PageStore::PageStore() : uriToFilePagePtrMap_(),
						 badRequestPage_(BAD_REQUEST_PAGE),
						 unauthorizedPage_(UNAUTHORIZED_PAGE),
						 forbiddenPage_(FORBIDDEN_PAGE),
						 notFoundPage_(NOT_FOUND_PAGE),
						 methodNotAllowedPage_(METHOD_NOT_ALLOWED_PAGE)
{
	//  I.  Application validity check:

	//  II.  Initialize:

	//  III.  Finished:
}

//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//	return value.
PageStore::~PageStore()
{
	//  I.  Application validity check:

	//  II.  Release resources:
	//  II.A.  Release 'uriToFixedFormPagePtrMap_':
	std::map<std::string, FixedFormPage *>::iterator
		formMapIter = uriToFixedFormPagePtrMap_.begin();
	std::map<std::string, FixedFormPage *>::iterator
		formMapEnd = uriToFixedFormPagePtrMap_.end();

	for (; formMapIter != formMapEnd; formMapIter++)
	{
		safeDelete(formMapIter->second);
	}

	//  II.B.  Release 'uriToFilePagePtrMap_':
	std::map<std::string, FilePage *>::iterator
		fileMapIter = uriToFilePagePtrMap_.begin();
	std::map<std::string, FilePage *>::iterator
		fileMapEnd = uriToFilePagePtrMap_.end();

	for (; fileMapIter != fileMapEnd; fileMapIter++)
	{
		safeDelete(fileMapIter->second);
	}

	//  III.  Finished:
}

//  PURPOSE:  To return 'true' if the HTML variable with name 'varName'
//  	is found in 'map', *and* it is interpretable as a floating pt number.
//  	If it is interpretable as a floating pt number then 'value' is set
//  	to that number.  Returns 'false' otherwise.
bool isLegalNumber(const std::map<std::string, std::string> &
					   map,
				   const std::string &varName,
				   float &value)
{
	float temp;
	char *cPtr;
	std::map<std::string, std::string>::const_iterator
		iter = map.find(varName);

	if (iter == map.end())
		return (false);

	temp = strtod(iter->second.c_str(), &cPtr);

	if ((*cPtr != '\0') || (cPtr == iter->second.c_str()))
		return (false);

	value = temp;
	return (true);
}

//  PURPOSE:  To return 'true' if the HTML variable with name 'varNameCPtr'
//  	is found in 'map', *and* it is interpretable as a floating pt number.
//  	If it is interpretable as a floating pt number then 'value' is set
//  	to that number.  Returns 'false' otherwise.
bool isLegalNumber(const std::map<std::string, std::string> &
					   map,
				   const char *varNameCPtr,
				   float &value)
{
	return (isLegalNumber(map, std::string(varNameCPtr), value));
}

//  PURPOSE:  To return the address of a Page referenced by 'request'.
Page *PageStore::getPagePtr(Request &request,
							UserContent &content,
							SessionStore &sessionStore)
{
	//  I.  Application validity check:

	//  II.  Obtain address of page:
	char filePath[LINE_BUFFER_LEN];

	//  II.A.  Parse parts of URL:
	translateUriToFilepath(request.getPath(),
						   filePath, sizeof(filePath),
						   request.getQuery());

	//  II.B.  Obtain page:
	Page *toReturn = NULL;

	if (strncmp(filePath, SYM_EVAL_PAGE_NAME, strlen(SYM_EVAL_PAGE_NAME)) == 0)
	{
		//  YOUR CODE HERE

		//  YOUR CODE HERE
	}
	else if (strcmp(filePath, LOGIN_PAGE_NAME) == 0)
	{
		const char *cPtr;
		std::map<std::string, std::string>::const_iterator
			varIter = request.getQuery().find(USERNAME_HTML_VARNAME);
		std::string userName = (varIter == request.getQuery().end())
								   ? std::string("")
								   : varIter->second;
		const User *userPtr = userName.empty()
								  ? NULL
								  : content.getContentFor(userName.c_str());
		std::string id = ((varIter =
							   request.getQuery().find(ID_HTML_VARNAME)) == request.getQuery().end())
							 ? std::string("")
							 : varIter->second;

		if ((userPtr != NULL) && !id.empty() && (userPtr->getId() == id))
		{
			//  Set welcome page
			cPtr = WELCOME_PAGE_NAME;

			//  Create cookie
			unsigned int seed = pthread_self();
			Cookie cookie = sessionStore.createSession(&seed, userPtr);

			request.setUserPtrAndCookie(userPtr, cookie);
		}
		else
		{
			cPtr = EXPLICIT_INDEX_URL;
		}

		if (*cPtr == '/')
		{
			cPtr++;
		}

		strcpy(filePath, cPtr);
	}
	else if (strcmp(filePath, LOGOUT_PAGE_NAME) == 0)
	{
		const char *cPtr;

		if (request.getUserPtr() == NULL)
		{
			cPtr = EXPLICIT_INDEX_URL;
		}
		else
		{
			sessionStore.didDeleteSession(request.getCookie());
			request.clearUserPtrAndCookie();
			cPtr = EXPLICIT_INDEX_URL;
		}

		if (*cPtr == '/')
		{
			cPtr++;
		}

		strcpy(filePath, cPtr);
	}

	if (appearsToBeStaticFile(filePath))
	{
		//  II.B.1.  Handle static pages:
		//  II.B.1.a.  See if already have file in memory:
		FilePage *pagePtr;
		std::string uriStr = filePath;
		std::map<std::string, FilePage *>::iterator
			found = uriToFilePagePtrMap_.find(uriStr);

		if (found == uriToFilePagePtrMap_.end())
		{
			//  II.B.1.a.I.  Page is in not memory, load it:
			pagePtr = new FilePage(filePath);
			uriToFilePagePtrMap_[uriStr] = pagePtr;
		}
		else
		{
			//  II.B.1.a.II.  Page is in memory, get it:
			pagePtr = found->second;
			pagePtr->didPerhapsUpdate();
		}

		toReturn = pagePtr;
	}
	else if (endsWith(filePath, SIMPLE_WEB_TEMPLATE_EXT))
	{
		//  II.B.2.  Handle template pages:
		//  II.B.2.a.  See if already have file in memory:
		FixedFormPage *pagePtr;
		std::string uriStr = filePath;
		std::map<std::string, FixedFormPage *>::iterator
			found = uriToFixedFormPagePtrMap_.find(uriStr);

		if (found == uriToFixedFormPagePtrMap_.end())
		{
			//  II.B.2.a.I.  Page is in not memory, load it:
			pagePtr = new FixedFormPage(filePath);
			uriToFixedFormPagePtrMap_[uriStr] = pagePtr;
		}
		else
		{
			//  II.B.2.a.II.  Page is in memory, get it:
			pagePtr = found->second;
			pagePtr->didPerhapsUpdate();
		}

		toReturn = pagePtr;
	}
	//else
	//if  ()
	//{
	//  //  II.B.3.  Handle programs:
	//  int	outFd	= creat("temp.html",0600);
	//
	//  if  (fork() == 0)
	//  {
	//    char						temp[256];
	//    std::map<std::string,std::string>::iterator	iter	= query.begin();
	//
	//    snprintf(temp,256,"%s=%s",iter->first.c_str(),iter->second.c_str());
	//    dup2(outFd,STDOUT_FILENO);
	//    execl(filePath,filePath,temp,NULL);
	//  }
	//
	//  close(outFd);
	//  wait(NULL);
	//  pagePtr	= new FilePage("temp.html",true);
	//}
	else
	{
		throw NOT_FOUND_HTTP_RET_CODE;
	}

	//  III.  Finished:
	return (toReturn);
}

//  PURPOSE:  To show how to use this program to 'outFilePtr'.  No return
//	value.
void showUsage(FILE *outFilePtr)
{
	fprintf(outFilePtr,
			"Usage:\tsimpleHttpServer <httpPort> <rPort>\n"
			"Where:\n"
			"  <httpPort> is the port to bind: int in [%d..%d], default is %d\n"
			"  <rPort> is the port for talking to R: int in [%d..%d], default is %d\n",
			MIN_PORT_NUM, MAX_PORT_NUM, DEFAULT_PORT_NUM,
			MIN_PORT_NUM, MAX_PORT_NUM, R_PORT);
}

//  PURPOSE:  To attempt to bind port 'portNum'.  Returns file descriptor of
//	socket to which to listen on success or '-1' on error.
int obtainListeningSocketFd(int port)
{
	// Create a socket
	int socketDescriptor = socket(AF_INET,	 // AF_INET domain
								  SOCK_STREAM, // Reliable TCP
								  0);

	// We'll fill in this datastruct
	struct sockaddr_in socketInfo;
	// Fill socketInfo with 0's
	memset(&socketInfo, '\0', sizeof(socketInfo));

	// Use std TCP/IP
	socketInfo.sin_family = AF_INET;

	// Tell port in network endian with htons()
	socketInfo.sin_port = htons(port);

	// Allow machine to connect to this service
	socketInfo.sin_addr.s_addr = INADDR_ANY;

	// Try to bind socket with port and other specifications
	int status = bind(socketDescriptor, // from socket()
					  (struct sockaddr *)&socketInfo,
					  sizeof(socketInfo));

	if (status < 0)
	{
		fprintf(stderr, "Could not bind to port %d\n", port);
		exit(EXIT_FAILURE);
	}

	listen(socketDescriptor, OS_CLIENT_QUEUE_LEN);
	return (socketDescriptor);
}

//  PURPOSE:  To write the response header to 'clientFd'.  'returnCode' tells
//	the return code, 'request' tells the Request for which the response
//	is given, and 'pagePtr' has the address of the page to write.
void writeHeader(int clientFd,
				 httpReturnCode_ty returnCode,
				 const Request &request,
				 Page *pagePtr)
{
	//  I.  Application validity check:

	//  II.  Write header:
	//  II.A.  Get data to write in header:
	char timeString[ASCTIME_BUFFER_LEN];
	const Cookie &cookie = request.getCookie();
	size_t resourceLen = pagePtr->getNumBytes(request);
	const char *mimeCPtr = pagePtr->getMimeCPtr();
	time_t now = time(NULL);

	asctime_r(gmtime(&now), timeString);

	//  II.B.  Compose into header as string:
	char header[FILE_BUFFER_LEN];
	int headerLen = (cookie.isDefined() ||
					 request.getWasCookieRecentlyDeleted())
						? snprintf(header, sizeof(header),
								   WITH_COOKIE_HEADER_TEMPLATE,
								   returnCode, getReturnCodeCPtr(returnCode),
								   mimeCPtr,
								   resourceLen,
								   SESSION_COOKIE_NAME, cookie.getCPtr(),
								   timeString)
						: snprintf(header, sizeof(header),
								   WITHOUT_COOKIE_HEADER_TEMPLATE,
								   returnCode, getReturnCodeCPtr(returnCode),
								   mimeCPtr,
								   resourceLen,
								   timeString);

	//  II.C.  Write 'header' to 'clientFd':
	write(clientFd, header, headerLen);
	printf("\n\nSending:\n");
	write(STDOUT_FILENO, header, headerLen);

	//  III.  Finished:
}

void sigPipeHandler(int sigNum)
{
	fprintf(stderr, "Fine!  No input for you!\n");
}

void sigIntHandler(int sigNum)
{
	shouldRun = false;
}

//  PURPOSE:  To do the listening to the server socket for clients.
//
void *listenToServerSocket(void *vPtr)
{
	//  I.  Application validity check:

	//  II.  Do listening:
	InfoForListeningThread *infoPtr = (InfoForListeningThread *)vPtr;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	int clientFd;

	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	while (shouldRun)
	{
		clientAddrLen = sizeof(clientAddr);
		clientFd = accept(infoPtr->getListenFd(),
						  (sockaddr *)&clientAddr,
						  &clientAddrLen);

		if (clientFd < 0)
		{
			if (shouldRun)
			{
				perror("accept()");
			}

			break;
		}

		if (!shouldRun)
		{
			break;
		}

		printf("accept() from host %s, port %d.\n",
			   inet_ntoa(clientAddr.sin_addr), (int)ntohs(clientAddr.sin_port));
		infoPtr->getNewClientBuffer().put(clientFd, clientAddr, clientAddrLen);
	}

	//  III.  Finished:
	printf("listenToServerSocket() finishing\n");
	return (NULL);
}

void *serveClients(void *vPtr)
{
	//  I.  Application validity check:

	//  II.  Do serving:
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	InfoForClientServingThread *infoPtr = (InfoForClientServingThread *)vPtr;
	Request request;
	fd_set originalSet;
	struct timeval time;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen;
	int clientFd;

	time.tv_sec = AUTOLOGOUT_TIME_SECS;
	time.tv_usec = 0;

	while (shouldRun)
	{
		char fromClient[FILE_BUFFER_LEN];
		int numBytes;
		Page *pagePtr = NULL;
		httpReturnCode_ty returnCode;

		request.clear();

		infoPtr->getNewClientBuffer().get(clientFd, clientAddr, clientAddrLen);

		FD_ZERO(&originalSet);
		FD_SET(clientFd, &originalSet);

		//  II.C.1.a.  Accept connection to client:
		if (select(clientFd + 1, &originalSet, NULL, NULL, &time) < 0)
		{
			if (errno == EINTR)
			{
				//  No biggie, just SIGINT or something
				printf("select() interrupted by signal\n");
				continue;
			}
			else
			{
				//  Ruh-roh!  More serious
				perror("select()");
				shouldRun = false;
				break;
			}
		}

		printf("Now serving host %s, port %d.\n",
			   inet_ntoa(clientAddr.sin_addr), (int)ntohs(clientAddr.sin_port));

		if (!FD_ISSET(clientFd, &originalSet))
		{
			printf("Time-out!\n");
			continue; // Probably just timed out
		}

		//  II.C.1.b.  read() request from client:
		if ((numBytes = read(clientFd, fromClient, sizeof(fromClient) - 1)) < 0)
		{
			if (!shouldRun)
			{
				break;
			}

			perror("read()");
			returnCode = NOT_FOUND_HTTP_RET_CODE;
			pagePtr = infoPtr->getPageStore().getErrorPage(returnCode);

			writeHeader(clientFd, returnCode, request, pagePtr);
			pagePtr->write(clientFd, request);
			pagePtr->write(STDOUT_FILENO, request);
			pagePtr->perhapsDispose();
			close(clientFd);
			continue;
		}

		fromClient[numBytes] = '\0',

		printf("Received: %d\n", numBytes);
		write(STDOUT_FILENO, fromClient, numBytes);
		write(STDOUT_FILENO, "\n\n", 2);

		//  II.C.1.c.  Compose response for client:
		httpMethod_ty method;

		try
		{
			request.initialize(clientFd,
							   fromClient,
							   numBytes,
							   infoPtr->getSessionStore());
			pagePtr = infoPtr->getPageStore().getPagePtr(request,
														 infoPtr->getContentStore(),
														 infoPtr->getSessionStore());
			returnCode = OK_HTTP_RET_CODE;
			method = request.getMethod();
		}
		catch (httpReturnCode_ty errCode)
		{
			returnCode = errCode;
		}

		if (pagePtr == NULL)
		{
			pagePtr = infoPtr->getPageStore().getErrorPage(returnCode);
		}

		//  II.C.1.d.  Send response to client:
		writeHeader(clientFd, returnCode, request, pagePtr);

		if (method != HEAD_HTTP_METH)
		{
			pagePtr->write(clientFd, request);

			if (strstr(pagePtr->getMimeCPtr(), "text/") != NULL)
			{
				pagePtr->write(STDOUT_FILENO, request);
				write(STDOUT_FILENO, "\n\n", 2);
			}
		}

		//  II.C.1.e.  Clean up after sending response:
		pagePtr->perhapsDispose();
		close(clientFd);
	}

	//  III.  Finished:
	printf("Ending thread %lu\n", pthread_self());
	return (NULL);
}

//  PURPOSE:  To do the main work of the Web server program.  'argc' tells
//	the number of command line arguments.  'argv[]' is an array of
//	pointers to each.  Returns 'EXIT_SUCCESS' on success or 'EXIT_FAILURE'
//	otherwise.
int main(int argc,
		 char *argv[])
{
	//  I.  Application validity check:

	//  II.  Do web serving:
	//  II.A.  Get parameters:
	int port = DEFAULT_PORT_NUM;
	const char *contentFilepathCPtr = DEFAULT_CONTENT_FILEPATH;

	rPort = R_PORT;

	switch (argc)
	{
	case 0:
		showUsage(stderr);
		exit(EXIT_FAILURE);

	case 3:
	default:
	{
		char *cPtr;

		rPort = strtol(argv[2], &cPtr, 0);

		if ((*cPtr != '\0') || (rPort < MIN_PORT_NUM) || (rPort > MAX_PORT_NUM))
		{
			fprintf(stderr, "Bad R port value\n");
			showUsage(stderr);
			exit(EXIT_FAILURE);
		}
	}

		// Do _not_ 'break', after case 3, do case 2:

	case 2:
	{
		char *cPtr;

		port = strtol(argv[1], &cPtr, 0);

		if ((*cPtr != '\0') || (port < MIN_PORT_NUM) || (port > MAX_PORT_NUM))
		{
			fprintf(stderr, "Bad port value\n");
			showUsage(stderr);
			exit(EXIT_FAILURE);
		}
	}

		// Do _not_ 'break', after case 2, do case 1:

	case 1:
		break;
	}

	//  II.B.  Do pre-webserving setup:
	//  II.B.1.  Obtain listening socket file descriptor:
	int listenFd = obtainListeningSocketFd(port);

	//  II.B.2.  Install signal handlers:
	struct sigaction action;

	memset(&action, '\0', sizeof(action));
	action.sa_handler = sigPipeHandler;
	sigaction(SIGPIPE, &action, NULL);
	action.sa_handler = sigIntHandler;
	sigaction(SIGINT, &action, NULL);
	action.sa_handler = sigChildHandler;
	sigaction(SIGCHLD, &action, NULL);

	//  II.B.3.  Create content and session stores:
	UserContent content(contentFilepathCPtr, PYTHON_PROGNAME, WEATHER_URL);
	SessionStore sessionStore;

	//  II.B.4.  Announce readiness:
	printf("Please connect to http://127.0.0.1:%d\n"
		   "Press Ctrl-C to stop.\n",
		   port);

	//  II.C.  Serve webpages:
	PageStore pageStore;
	NewClientBuffer newClientBuffer;
	//pthread_attr_t	pthreadAttr;

	pthread_mutex_init(&safeRandLock__, NULL);

	//pthread_attr_init(&pthreadAttr);
	//pthread_attr_setdetachstate(&pthreadAttr,PTHREAD_CREATE_DETACHED);

	//  II.C.1.  Start socket listening thread:
	pthread_t listeningThread;
	InfoForListeningThread listenThreadInfo(listenFd, newClientBuffer);

	pthread_create(&listeningThread,
				   NULL, //&pthreadAttr,
				   listenToServerSocket,
				   &listenThreadInfo);

	//  II.C.2.  Start client serving threads:
	pthread_t servingThread[NUM_CLIENT_HANDLING_THREADS];
	InfoForClientServingThread servingThreadInfo(content,
												 sessionStore,
												 pageStore,
												 newClientBuffer);

	for (int i = 0; i < NUM_CLIENT_HANDLING_THREADS; i++)
	{
		pthread_create(servingThread + i,
					   NULL, //&pthreadAttr,
					   serveClients,
					   &servingThreadInfo);
	}

	//  II.C.3.  Main thread hangs-out and handles external signals:
	while (shouldRun)
	{
		sleep(1);
	}

	//  II.D.  Shutdown:
	printf("Shutting down.\n");

	//  II.D.1.  Tell listening thread to stop:
	int toShutdownFd = socket(AF_INET, SOCK_STREAM, 0);
	struct addrinfo *hostPtr;
	struct sockaddr_in serverAddr;

	getaddrinfo("localhost", NULL, NULL, &hostPtr);
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = ((struct sockaddr_in *)hostPtr->ai_addr)->sin_addr.s_addr;
	connect(toShutdownFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	pthread_join(listeningThread, NULL);

	//  II.D.2.  Tell client-serving threads to stop:
	newClientBuffer.wakeWaiters();

	for (int i = 0; i < NUM_CLIENT_HANDLING_THREADS; i++)
	{
		pthread_join(servingThread[i], NULL);
	}

	//  II.D.3.  Do misc cleanup:
	close(listenFd);
	pthread_mutex_destroy(&safeRandLock__);

	//  III.  Finished:
	return (EXIT_SUCCESS);
}
