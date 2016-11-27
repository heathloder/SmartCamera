/*	PIRTrigger.c 
*
*	Authors: Patrick Dzioba
*			 Heath Loder
*
*	Compile Command: gcc -o PIRTrigger PIRTrigger.c -lwiringPi
*
*	Usage: sudo ./PIRTrigger
*
*/

#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

#define PIR_SENSOR	7

static void checkPIRClear ();
static void checkPIRTrigger ();
static void setupPIR (void);

int main (void)
{
	/* Set up PIR Sensor */
	printf ("Starting Setup.\n");
	wiringPiSetup();	/* Prepare GPIO Pins */
	setupPIR();			/* Assign proper GPIO Pin to PIR Sensor */

	/* Loop through sensor check indefinitely */
	while(1)
	{
		checkPIRTrigger();
		checkPIRClear();
	}
	return 0;
}
 
static void checkPIRClear ()
{
	printf ("Waiting for PIR input to clear...\n");
	fflush (stdout);
	while (digitalRead (PIR_SENSOR) == HIGH)
		delay (10);
	printf ("PIR ready.\n");
}


static void checkPIRTrigger ()
{
	char pictime[19];
	char command[60];
	struct tm *timeinfo;
	time_t seconds;
	
	printf ("Awaiting sensor input...\n");
	fflush (stdout);
	delay (2000);

	while (digitalRead (PIR_SENSOR) == LOW)
		delay(10);
	
	/* Sensor activated.  */
	seconds = time(NULL);
	timeinfo = localtime(&seconds);
	strftime(pictime, 20, "%F-%I-%M-%S", timeinfo);
	printf ("PIR Sensor triggered.\n");
	snprintf(command, sizeof(command),"raspistill -t 50 -o capture-%s.jpg", pictime);

	/* Take the still image and save it with the appropriate filename */
	if(system(command) < 0)
		printf ("Error capturing picture.\n");
	else
		printf ("Image captured at %s.\n", pictime);
	
	/*  */
}


static void setupPIR (void)
{
	pinMode (PIR_SENSOR, INPUT);
	pullUpDnControl (PIR_SENSOR, PUD_UP);
	checkPIRClear();
}
