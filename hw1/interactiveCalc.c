/*-------------------------------------------------------------------------*
 *---																	---*
 *---						interactiveCalc.c							---*
 *---																	---*
 *---	 				 An interactive calculator.						---*
 *---																	---*
 *---	----	----	----	----	----	----	----	----	---*
 *---																	---*
 *---		Version 1a		2020 January 8		Joseph Phillips			---*
 *---																	---*
 *-------------------------------------------------------------------------*/

//
//	Compile with:
//	$ g++ interactiveCalc.c -o interactiveCalc -lncurses -g
//
//	NOTE:
//	If this program crashes then you may not see what you type.
//	If that happens, just type:
//
//		stty sane
//
//	to your terminal, even though you cannot even see it.

//---												---//
//---			Header file inclusions:				---//
//---												---//
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>		// For alarm()
#include	<errno.h>
#include	<sys/socket.h>
#include	<sys/types.h>	// For creat()
#include	<sys/wait.h>	// For creat(), wait()
#include	<sys/stat.h>	// For creat()
#include	<fcntl.h>		// For creat()
#include	<signal.h>
#include	<ncurses.h>


//---												---//
//---			Definitions of constants:			---//
//---												---//
#define		DEFAULT_CALC_PROGRAM	"/usr/bin/bc"
#define		CALC_ARG1				"-l"
#define		BC_QUIT_CMD				"quit"

const int	HEIGHT					= 5;
const int	WIDTH					= 64;
const int	INIT_TEXT_LEN			= 1024;
const int	MAX_TEXT_LEN			= 65536;
const char	STOP_CHAR				= (char)27;
const char	CALC_CHAR				= (char)'\r';
const int	BUFFER_LEN				= 4096;
const int	TYPING_WINDOW_BAR_Y		= 0;
const int	CHECKING_WINDOW_BAR_Y	= TYPING_WINDOW_BAR_Y   + HEIGHT + 1;
const int	MESSAGE_WINDOW_BAR_Y	= CHECKING_WINDOW_BAR_Y + HEIGHT + 1;
const int	NUM_MESSAGE_SECS		= 4;


//---												---//
//---			Definitions of global vars:			---//
//---												---//
WINDOW*		typingWindow;
WINDOW*		answerWindow;
WINDOW*		messageWindow;
int			shouldRun		= 1;
int			hasChildStopped	= 0;
//  YOUR CODE HERE: add some global variable(s)

int pfd[2];
int cfd[2];

char buffer[BUFFER_LEN];


//---												---//
//---			Definitions of global fncs:			---//
//---												---//

//  PURPOSE:  To turn 'ncurses' on.  No parameters.  No return value.
void onNCurses()
{
	//  I.  Application validity check:

	//  II.  Turn 'ncurses' on:
	const int	LINE_LEN	= 80;

	char		line[LINE_LEN];

	initscr();
	cbreak();
	noecho();
	nonl();
	//intrflush(stdscr, FALSE);
	//keypad(stdscr, TRUE);
	typingWindow		= newwin(HEIGHT,WIDTH,TYPING_WINDOW_BAR_Y+1,1);
	answerWindow		= newwin(HEIGHT,WIDTH,CHECKING_WINDOW_BAR_Y+1,1);
	messageWindow		= newwin(     1,WIDTH,MESSAGE_WINDOW_BAR_Y,1);
	scrollok(typingWindow,TRUE);
	scrollok(answerWindow,TRUE);

	snprintf(line,LINE_LEN,"'Esc' to quit. Enter to calculate");
	mvaddstr(TYPING_WINDOW_BAR_Y,0,line);
	mvaddstr(CHECKING_WINDOW_BAR_Y,0,"Answers:");
	refresh();
	wrefresh(typingWindow);	// moves cursor back to 'typingWindow':

	//  III.  Finished:
}


//  PURPOSE:  To handle the 'SIGCHLD' signal.  'sig' will be 'SIGCHLD' and
//	will be ignored.  No return value.
void sigChildHandler(int sig)
{
	int status;

	wait(&status);
	hasChildStopped	= 1;
}


//  PURPOSE:  To handle the 'SIGALRM' signal.  'sig' will be 'SIGALRM' and
//	will be ignored.  No return value.
void sigAlarmHandler(int sig)
{
	//  Do not worry, literally does nothing
}


//  PURPOSE:  To launch the 'bc' calculator.  If 'argc' is at least 2 then
//	'argv[1]' will be run.  If 'argc' is less then 2 then
//	'DEFAULT_CALC_PROGRAM' will be run.  No return value.
void launchBc(int argc, char* argv[])
{
	//  I.  Application validity check:

	//  II.  Attempt to launch 'bc'
	//  YOUR CODE HERE (see (3))
	if (pipe(pfd) < 0 || pipe(cfd) < 0)
	{
		fprintf(stderr,"pipe() failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	pid_t cPid = fork();
	
	if (cPid < 0) 
	{
		fprintf(stderr,"fork() failed: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	if (cPid == 0) 
	{
		close(pfd[0]);
		close(cfd[1]);
		
		dup2(pfd[1], STDOUT_FILENO);
		dup2(cfd[0], STDIN_FILENO);
		
		if (argc >= 2)
		{
			execl(argv[1], argv[1], CALC_ARG1, NULL);
			fprintf(stderr,"Cannot execl(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else 
		{
			execl(DEFAULT_CALC_PROGRAM,DEFAULT_CALC_PROGRAM,CALC_ARG1,NULL));
			fprintf(stderr,"Cannot execl():	%s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
	} else {
		close(pfd[1]);
		close(cfd[0]);
	}
}
		
	//  III.  Finished: 
}


//  PURPOSE:  To save the 'lineIndex' chars at the beginning of 'line' to
//	to position '*endTextPtrPtr' in buffer '*bufferPtrPtr' of length
//	'*bufferLenPtr' and with end '*endBufferPtrPtr'.  If there is not
//	enough space in '*bufferPtrPtr' then '*bufferLenPtr' will be doubled,
//	and '*bufferPtrPtr' will be 'realloc()'-ed to this new length.
//	No return value.
void saveLine	(size_t*		bufferLenPtr,
				 char**			bufferPtrPtr,
				 char**			endTextPtrPtr,
				 char**			endBufferPtrPtr,
				 const char*	line,
				 int   			lineIndex
				)
{
	//  I.  Application validity check:

	//  II.  Save 'line' to '*bufferPtrPtr':
	//  II.A.  Allocate more space if needed:
	if  (lineIndex >= (*endBufferPtrPtr - *endTextPtrPtr + 1) )
	{
		size_t	textLen	 	= *endTextPtrPtr - *bufferPtrPtr;

		(*bufferLenPtr)		*= 2;
		(*bufferPtrPtr)	 	 = (char*)realloc(*bufferPtrPtr,*bufferLenPtr);
		(*endTextPtrPtr)	 = *bufferPtrPtr + textLen;
		(*endBufferPtrPtr)	 = *bufferPtrPtr + *bufferLenPtr;
	}

	//  II.B.  Save 'line' to '*bufferPtrPtr':
	memcpy(*endTextPtrPtr,line,lineIndex);
	(*endTextPtrPtr)	+= lineIndex;

	//  III.  Finished:
}



//  PURPOSE:  To attempt to send the text pointed to by 'bufferPtr' to the
//	bc-running child process.  'endTextPtr' points to one char beyond
//	the end of the text to send in 'bufferPtr'.  No return value.
//
//	SIDE EFFECT: Prints to 'answerWindow'
void calculate	(const char* outputBufferPtr, const char* endTextPtr)
{
  	//  I.  Application validity check:

 	//  II.  Send 'outputBufferPtr' to calculator:
  	int	numBytes;
  	char	inputBuffer[BUFFER_LEN];

  	//  See (4)
  	//  YOUR CODE HERE TO SEND endTextPtr-outputBufferPtr BYTES POINTED TO BY outputBufferPtr
	write(cfd[1], outputBufferPtr, (endTextPtr-outputBufferPtr));
	alarm(1);
  	numBytes	= read(pfd[0], inputBuffer, BUFFER_LEN); // CHANGE THAT 0.
					 // YOUR CODE HERE TO RECEIVE RESPONSE INTO inputBuffer[]
  	alarm(0);

  	if  (numBytes > 0)
  	{
		if  ( (endTextPtr[-1] == '\n') ||  (endTextPtr[-1] == '\r') )
		{
	  		endTextPtr--;
		}

		waddnstr(answerWindow,outputBufferPtr,endTextPtr-outputBufferPtr);
		waddstr (answerWindow," = ");
		inputBuffer[numBytes]	= '\0';
		waddstr(answerWindow,inputBuffer);
		wrefresh(answerWindow);
		wrefresh(typingWindow);	// moves cursor back to 'typingWindow':
  	}

  	//  III.  Finished:
}


//  PURPOSE:  To allow the user to type, display what they type in
//	'typingWindow', and to send what they type to the spell checking
//	process upon pressing 'Enter'.  'vPtr' comes in, perhaps pointing
//	to something.  Returns 'NULL'.
void* type (void* vPtr)
{
	//  I.  Application validity check:

	//  II.  Handle user typing:
	unsigned int	c;
	char		line[WIDTH+1];
	int			index			= 0;
	size_t		bufferLen		= INIT_TEXT_LEN;
	char*		bufferPtr		= (char*)malloc(bufferLen);
	char*		endTextPtr		= bufferPtr;
	char*		endBufferPtr	= bufferPtr + bufferLen;

	//  II.A.  Each iteration handles another typed char:
	while  ( (c = getch()) != STOP_CHAR )
  	{
		//  II.A.1.  Handle special chars:
		if  (c == '\r')
		{
			//  II.A.1.a.  Treat carriage return like newline:
			c = '\n';
		}
		else if  ( (c == 0x7) || (c == 127) )
		{
			//  II.A.1.b.  Handle backspace:
			int	col	= getcurx(typingWindow);

			if  (col > 0)
			{
				index--;
				wmove(typingWindow,getcury(typingWindow),col-1);
				wrefresh(typingWindow);
	  		}

	  		continue;
		}
		else if  (c == ERR)
		{
	 	 	continue;
		}

		//  II.A.2.  Print and record the char:
		waddch(typingWindow,c);
		wrefresh(typingWindow);
		line[index++]	= c;

		//  II.A.3.  Handle when save 'line':
		if  (c == '\n')
		{
	 		//  II.A.3.a.  Save 'line' when user types newline:
	  		size_t	textLen	= endTextPtr - bufferPtr;

	  		saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
	  		index = 0;
	  		calculate(bufferPtr,endTextPtr);
	  		endTextPtr	= bufferPtr + textLen;
	  		continue;
		}
		else if  (index == WIDTH-1)
		{
	  		//  II.A.3.b.  Save 'line' when at last column:
	  		line[index] = '\n';
	  		index++;
	  		saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
	  		index = 0;
	  		waddch(typingWindow,'\n');
	  		wrefresh(typingWindow);
		}
  	}

	//  III.  Finished:
	saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
	free(bufferPtr);
	//  YOUR CODE HERE TO TELL THE bc PROCESS TO END ITSELF, see (5)
	write(cfd[1], BC_QUIT_CMD, sizeof(BC_QUIT_CMD));
	return(NULL);
}


//  PURPOSE:  To turn off 'ncurses'.  No parameters.  No return value.
void offNCurses()
{
	sleep(1);
	nl();
	echo();
	refresh();
	delwin(messageWindow);
	delwin(typingWindow);
	delwin(answerWindow);
	endwin();
}



//  PURPOSE:  To do the spell-checking word-processor.  Ignores command line
//	arguments.  Return 'EXIT_SUCCESS' to OS.
int main (int argc, char* argv[])
{
	//  YOUR CODE HERE (see (2))
	
	struct sigaction cSignal;
	
	memset(&cSignal, '\0', sizeof(cSignal));
	cSignal.sa_flags = 0;
	cSignal.sa_handler = sigChildHandler;
	sigaction(SIGCHLD, &cSignal, NULL);

	struct sigaction alarmSignal;

	memset(&alarmSignal, '\0', sizeof(alarmSignal));
	alarmSignal.sa_flags = 0;
	alarmSignal.sa_handler = sigAlarmHandler;
	sigaction(SIGALRM, &alarmSignal, NULL);

	onNCurses();
	launchBc(argc,argv);
	type(NULL);
	offNCurses();

	while  (!hasChildStopped)
	{
		sleep(1);
	}

	return(EXIT_SUCCESS);
}
