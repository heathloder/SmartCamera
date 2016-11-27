# SmartCamera
### STEP 1: Connect the PIR sensor and camera to the Raspberry Pi

### STEP 2: Install wiringPi GPIO access library on the Raspberry Pi
This program uses the wiringPi GPIO access library, available here:
http://wiringpi.com/download-and-install/

### STEP 3: Install libcurl
`$ sudo apt-get update`
`$ sudo apt-get install libcurl4-openssl-dev`

### STEP 4: Install libquickmail - C library for sending email
https://sourceforge.net/projects/libquickmail/ 

*NOTE: After following the instructions for libquickmail, I still needed to run:*

`$ sudo cp /path/to/where/you/compiled/quickmail.h /usr/local/include/`
     
### STEP 5: Compile software and run
With wiringPi installed, compile the PIRTrigger.c program as follows:

`$ gcc -o PIRTrigger PIRTrigger.c -lwiringPi -lquickmail -lcurl -Wl,-rpath=/usr/local/lib`

### STEP 6: run with sudo:
`$ sudo ./PIRTrigger [-e emailaddress@domain.com] [-s servername]`

#Info for presentation:
Email: hepabepa@gmail.com
