// makeAndWriteMsgQueue.c

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

int makeMsgQueue ()
{
	 int toReturn;
	 struct msqid_ds msgQInfoBuffer;
	 
	 do
	 {
		 int key = getMsgKey();
		 toReturn = msgget(key,0660 | IPC_CREAT);
	 }
	 while (toReturn < 0);
	 
	 msgctl(toReturn,IPC_STAT,&msgQInfoBuffer);
	 msgQInfoBuffer.msg_qbytes = 4096;
	 msgctl(toReturn,IPC_SET,&msgQInfoBuffer);
	 
	 return(toReturn);
}

void writeMsgs (int msgQId)
{
	 struct AMessage aMsg;
	 char format[TEXT_LEN];
	 char line[TEXT_LEN * 2];
	 aMsg.msgType_ = 1;
	 snprintf(format,TEXT_LEN,"%%f %%d %%%ds",TEXT_LEN);
	 while (1)
 	 {
		 aMsg.floatPt_ = 0.0;
		 aMsg.integer_ = 0;
		 memset(aMsg.text_,'\0',TEXT_LEN);
		 printf("Please enter a float, int and word (or blank line to quit): ");
		 fgets(line,TEXT_LEN*2,stdin);
		 if (sscanf(line,format,&aMsg.floatPt_,&aMsg.integer_,aMsg.text_) <= 0)
 		 	{
				 msgsnd(msgQId,&aMsg,sizeof(aMsg)-sizeof(long),0);
				 break;
 			}
		 aMsg.text_[TEXT_LEN-1] = '\0';
		 msgsnd(msgQId,&aMsg,sizeof(aMsg)-sizeof(long),0);
 	}
}

int main ()
{
	 int msgQId= makeMsgQueue();
	 writeMsgs(msgQId);
	 return(EXIT_SUCCESS);
}