// readAndDelMsgQueue.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>

#include "msg.h"

int getMsgKey ()
{
	 char text[TEXT_LEN];
	 int toReturn;
	 printf("Please enter a msg id integer: ");
	 fgets(text,TEXT_LEN,stdin);
	 toReturn = strtol(text,NULL,0);
	 return(toReturn);
}

int main()
{
	 struct AMessage msg;
	 int msgKey = getMsgKey();
	 int msgQId = msgget(msgKey,0);
	 if (msgQId < 0)
 	 {
		 fprintf(stderr,"Sorry!\n");
		 exit(EXIT_FAILURE);
 	 }
 	 while (1)
 	 {
	 	 if ( msgrcv(msgQId,&msg,sizeof(msg),1,0) < 0)
		 	 break;
 	 	 if ((msg.floatPt_ == 0.0) && (msg.integer_ == 0) && (msg.text_[0] == '\0'))
 			 break;
 		 printf("Float:\t%g\nInt:\t%d\nWord:\t%s\n\n", msg.floatPt_,msg.integer_,msg.text_);
 	 }
 	 msgctl(msgQId,IPC_RMID,NULL);
 	 
 	 return(EXIT_SUCCESS);
}