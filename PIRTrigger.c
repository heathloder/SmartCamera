/*	PIRTrigger.c 
*
*	Authors: Patrick Dzioba
*			 Heath Loder
*
*	Compile Command: gcc -o PIRTrigger PIRTrigger.c -lwiringPi -lquickmail -Wl,-rpath=/usr/local/lib
*
*	Usage: sudo ./PIRTrigger
*
*/

#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>			/* For getopts interface */
#include <wiringPi.h>		/* WiringPi GPIO library */
#include <quickmail.h>		/* quickmail email library */

// GPIO Pinout
#define PIR_SENSOR	7

// Email server settings
#define SMTPSERVER "smtp.gmail.com"
#define SMTPPORT 465				/* For SSL */
//#define SMTPPORT 587				/* For TLS/StartTLS */
#define SMTPUSER "hepabepa@gmail.com"
#define SMTPPASS "Email1Still2Works3"
#define FROM "hepabepa@gmail.com"
#define TO "hepabepa@gmail.com"

static void checkPIRClear ();
static void checkPIRTrigger ();
static void setupPIR (void);

int eflag=0, sflag=0;
char recipient[255];
char server[33];

int main (int argc, char **argv)
{
	int c;
	// While loop to handle when someone enters optional args (options).
	while ((c = getopt (argc, argv, "e:s:")) != -1) {
		switch (c) {
			case 'e':
				eflag=1;
				strcpy(recipient, optarg);
// Optionally perform validation on recipient
				break;
			case 's':
				sflag=1;
				strcpy(server, optarg);
// Optionally perform validation on server name.
				break;
			case '?':
				if (optopt == 'e' || optopt == 's')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				return 1;
		}
	}	
	if (optind != argc) {
		printf ("Usage: sudo ./PIRTrigger [-e email-address] [-s server-name]\n");
		return 1;
	}
	
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
	char fname[36];  // Overflow MIGHT happen here... keep an eye out. [min=32]
	struct tm *timeinfo;
	time_t seconds;
	const char* errmsg;
	
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
	snprintf(fname, sizeof(fname),"capture-%s.jpg", pictime);

	/* Take the still image and save it with the appropriate filename */
	if(system(command) < 0)
		printf ("Error capturing picture.\n");
	else
		printf ("Image captured at %s.\n", pictime);
	
	/* Optionally upload and/or email photo. */
	// Check for Internet connection.
	if (eflag==1) {		/* If an email address was provided... */
		/* Initialize email and send */
		quickmail_initialize();
		quickmail mailobj = quickmail_create(FROM, "Motion Sensor Alert from Raspberry Pi");
		quickmail_add_to (mailobj, recipient);
		quickmail_set_body(mailobj, "Something has set off the motion sensor alarm on the Pi.\nSee attached photo for more details.");
		printf("Attaching: %s...\n", fname);
		quickmail_add_attachment_file(mailobj, fname, NULL);
		if ((errmsg = quickmail_send_secure(mailobj, SMTPSERVER, SMTPPORT, SMTPUSER, SMTPPASS)) != NULL)
			fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
		quickmail_destroy(mailobj);
	}
	if (sflag==1) {		/* If a server name was provided... */
		// initialize connection and attempt to save
	}
}


static void setupPIR (void)
{
	pinMode (PIR_SENSOR, INPUT);
	pullUpDnControl (PIR_SENSOR, PUD_UP);
	checkPIRClear();
}
