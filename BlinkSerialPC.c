/* BlinkSerialPC.c
Written by Arthur Jones 2 November 2018
Updated by Louise Brown 15 June 2023
Program to blink LED on Arduino by sending serial commands and receiving
acknowledgement

Loosely based on programs demo_tx.c and demo_rx.c by Teunis van Beelen,
https://www.teuniz.net/RS-232/index.html licensed under GPL3.0

Uses RS-232 library downloadable from https://www.teuniz.net/RS-232/index.html */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "rs232.h"

#if defined(__APPLE__) || defined(__linux__)
#include <stdbool.h>
#define FALSE false
#define TRUE true
int mingw_gettimeofday(struct timeval *tp, void *tzp){
   return gettimeofday(tp, tzp);}
#endif

#define BUF_SIZE 4095
#define BAUD_RATE 115200  // Must match the Arduino program

int getStringRS232(int cport_nr, unsigned char * buf, unsigned char * combinedBuf, int * stringRead);
long currentTimeMillis();

int main()
{
  int i=0, j=0, numRead;

#if defined(__APPLE__) || defined(__linux__)
  int cport_nr=RS232_GetPortnr("tty.usbmodem1101");   // set com port to name of port
#else
  int cport_nr=2; // com port number
#endif  

  unsigned char buf[BUF_SIZE+1];  /* Buffer used to receive characters from serial port. Add 1 to size to allow for string terminator */
  unsigned char combinedBuf[BUF_SIZE+1];  /* Buffer used to combine attempts at reading string from port */
  int stringRead = FALSE;    /* True if a \n-terminated string is read, otherwise wait for remainder */

  char mode[]={'8','N','1',0};
  char str[2][10]; 
  char checkPort; 
  
  long prevMillisSend = currentTimeMillis();
  long prevMillisRec = currentTimeMillis();
  long sendInterval = 1000;  /* Period for transmission loop */
  long recInterval = 10; /* Period for receive loop */

  printf("Serial port is set to COM%d, enter 'y' if correct: ", cport_nr+1);
  scanf("%c", &checkPort);
  if ( checkPort != 'y' && checkPort != 'Y' )
  {
      printf("Serial port set incorrectly, please correct\n");
      printf("Terminating program\n");
      return(0);
  }

  if(RS232_OpenComport(cport_nr, BAUD_RATE, mode, 0))
  {
    printf("Cannot open comport\n");
    return(0);
  }

  /* Strings to send to Arduino to make "blinking" happen */
  strcpy(str[0], "on\n");
  strcpy(str[1], "off\n");

  // Clear buffers for strings returned from port
  strncpy((char*)buf, "", BUF_SIZE);
  strncpy((char*)combinedBuf, "", BUF_SIZE);

  while(1)
  {
    /* Timed loop for transmission from PC to Arduino */
    if (currentTimeMillis() - prevMillisSend > sendInterval)
    {
        RS232_cputs(cport_nr, str[i]);
        printf("sent: %s\n", str[i]);
        prevMillisSend = currentTimeMillis();
        i=1-i;  // Toggle i between 1 and 0
    }

    /* Timed loop for polling for signals from Arduino on PC to Arduino */
    if (currentTimeMillis() - prevMillisRec > recInterval)
    {
        numRead = getStringRS232(cport_nr, buf, combinedBuf, &stringRead); 
        if (numRead > 0)
        {
          for(j=0; j < numRead; j++)
          {
            if(buf[j] < 32)  /* replace unreadable control-codes by dots */
            {
              buf[j] = '.';
            }
          }
          printf("%s\n", (char *)buf);
        }
        prevMillisRec = currentTimeMillis();
    }
  }

  return(0);
}

// Get a string from the RS232 port
int getStringRS232(int cport_nr, unsigned char * buf, unsigned char * combinedBuf, int * stringRead)
{
    int numRead = RS232_PollComport(cport_nr, buf, BUF_SIZE);

    if(numRead > 0)
    {
      buf[numRead] = '\0';   /* always put a "null" at the end of a string! */

      /* Handle situation where received string is split over two attempts at polling
         serial buffer.  Will fail if multiple strings sent in rapid succession so they
         occur in single buffer, or if buffer is not read before next serial read,
         but unlikely if only expected return value is "ok" or "error" */
      if( buf[numRead-1]=='\n' )  // Complete string
      {
         buf[numRead-1] = '\0';   /* effectively strip off unwanted \n character */
         strncat((char *)combinedBuf, (char *)buf, BUF_SIZE); // Add string just read to the combined buffer
         *stringRead=TRUE;
      }
      else   // String was incomplete
      {
          strncpy((char *)combinedBuf, (char *)buf, BUF_SIZE);   // Save the string into the buffer for combining with next string
          printf("Incomplete string received: %s , %d long\n", (char*)buf, (int)strlen((char*)buf));
      }

      if( *stringRead )
      {
        strncpy((char *)buf, (char *)combinedBuf, BUF_SIZE);  // Put the final string into buf
        strncpy((char*)combinedBuf, "", BUF_SIZE);   // and clear the buffer for combining strings
        *stringRead=FALSE;
        return strlen((char*)buf);
      }
    }
    return 0;
}

// Function to return the current time in milliseconds
long currentTimeMillis()
{
     struct timeval time;  // Structure for storing times. tv_sec member returns whole seconds, tv_usec returns part seconds in microseconds
     mingw_gettimeofday( &time, NULL );
     return time.tv_sec*1000 + time.tv_usec/1000;
}
