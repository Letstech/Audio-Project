/*
SD card basic file example

This example shows how to create and destroy an SD card file 	
The circuit:
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 4

created   Nov 2010
by David A. Mellis
modified 9 Apr 2012
by Tom Igoe

This example code is in the public domain.
	 
*/
#include <SD.h>

File root;
int count = 0; 

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}

	Serial.print("Initializing SD card...");
	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	// Note that even if it's not used as the CS pin, the hardware SS pin 
	// (10 on most Arduino boards, 53 on the Mega) must be left as an output 
	// or the SD library functions will not work. 
	pinMode(4, OUTPUT);

	if (!SD.begin(10)) {
		Serial.println("initialization failed!");
		return;
	}
	Serial.println("initialization done.");

	root = SD.open("/");

	countFiles(root);
	// char *files[count];
	Serial.println(count);
	getFiles(root);

	Serial.println("done!");
}

void loop()
{
	// nothing happens after setup finishes.
}

int countFiles(File dir) {
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			//Serial.println("**nomorefiles**");
			break;
		}
		if (entry.isDirectory()) {
			//do nothing
		} else {
			Serial.print(entry.name());
			count++; //count the file
		}
		entry.close();
	}
	return count;
}

void getFiles(File dir) {
	char *files[count];
	int i = 0;
	while(true) {
		File entry = dir.openNextFile();
		if(!entry){
			break;
		}
		if(entry.isDirectory()){
			//do nothing
		}
		else {
			Serial.println(entry.name());
			files[i] = entry.name(); //write the file name to the array
			i++;
		}
	}
	char *pointertoarray = &files[0];
	for(int j= 0; pointertoarray[j] != '\0'; j++)
	{
   		Serial.println(*pointertoarray[j]);
	}
}