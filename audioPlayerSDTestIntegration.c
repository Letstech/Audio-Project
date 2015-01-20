
//Ray Ortiz
//Audio Project

/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
// create breakout-example object!
//Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
// create shield-example object!
Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//initial setup of pins (buttons)

int inputPin1 = 2; // Digital pin assignment for push buttons
int inputPin2 = 1;

int potPin = 0;   // Analog pin assignment for Potentiometer Dial

boolean buttonPressed1 = false;
boolean buttonPressed2 = false;//boolean testing 

File root;
File entry;
int fileCount = 0; 
String* filesArr = NULL;
char str = NULL; 
  
void setup() {
  
    pinMode(inputPin1, INPUT);//pin mode setup for both buttons  
    pinMode(inputPin2, INPUT);

    Serial.begin(9600);
    Serial.println("Adafruit VS1053 Simple Test");

    if (! musicPlayer.begin()) { // initialise the music player
        Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
        while (1);
    }
    Serial.println(F("VS1053 found"));

    // SD.begin(CARDCS);    // initialise the SD card REDUNDANDT?


    Serial.print("Initializing SD card...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin 
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
    // or the SD library functions will not work. 
    pinMode(4, OUTPUT);

    if (!SD.begin(4)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");

    //reopens the root file for each op to avoid a bug indicating there are no more files to read.
    //http://stackoverflow.com/questions/16971084/how-to-recheck-file-in-arduino-sd-card
    root = SD.open("/");

    if(root) {
        writeDirectory(root);        
        root.close();
    }
    else {
        Serial.println("Failed to open directory");
    }


    // Set volume for left, right channels. lower numbers == louder volume!
    //musicPlayer.setVolume(5,5);

    // Timer interrupts are not suggested, better to use DREQ interrupt!
    //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

    // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

    int count = 0;

    while (count <= fileCount){

        // Serial.print("Playing ");
        // Serial.println(filesArr[count]);

        int len = filesArr[count].length();
        str = new char[len]; 
        filesArr[count].toCharArray(str,len);

        Serial.print("playing ");
        Serial.print(str);

        musicPlayer.startPlayingFile(str);
        count ++ ;
        Serial.println(count);
        delete [] str;
        str = NULL;

        while (! musicPlayer.stopped() ){
            loop();

            int val = analogRead(potPin);// Analog read for dial. 0 to 1023.
            musicPlayer.setVolume(val*100/1000,val*100/1000); 
            //Volume set up will be constant depending on the dials' current postion while a song plays

            /*
            Serial.print("Knob value "); //Serial testing of dial output
            Serial.println(val);
            */
        }
    }
}

//button calling
void loop() {
    // File is playing in the background

    if (musicPlayer.stopped()) {
        Serial.println("Done playing music");
        while (1);
    }

    // Serial Port Testing
    Serial.print("Button1 input ");
    Serial.println( digitalRead(inputPin1));
    Serial.print("Button2 input ");
    Serial.println( digitalRead(inputPin2));

    if (digitalRead(inputPin1) == HIGH){
        buttonPressed1 = true;
        Serial.println("Button 1 Pressed");
    }

    if(digitalRead(inputPin2) == HIGH){
        buttonPressed2 = true;
        Serial.println("Button 2 Pressed"); 
    }

    if (buttonPressed2) {
        delay(500);// delay is used as a fail safe to prevent rapid button input when held down.
        Serial.println("Track Stopped");
        musicPlayer.stopPlaying();
        buttonPressed2 = false;
    }

    //while (! musicPlayer.paused()){
    if (buttonPressed1){
        Serial.println("Paused");
        musicPlayer.pausePlaying(true); 
        delay(300);         
        while (!digitalRead(inputPin1)){// while input pin is LOW
        }
        delay(300);
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
        buttonPressed1 = false;
    }
    delay(100);
}

int countFiles(File dir) {
    Serial.println("Counting files...");
    while(true) {
     
        entry =  dir.openNextFile();
        if (!entry) {
            // no more files
            Serial.println(fileCount);
            Serial.println("No more files to count.");
            break;
        }
            if (entry.isDirectory()) {
            //do nothing
        } else if (entry.size() != 0){
            fileCount++;
        }
    }
}

void writeDirectory(File dir) {
    countFiles(root);
    root.close();

    // root = SD.open("/");
    
    filesArr = new String[fileCount];

    Serial.println("Writing files...");
    for(int i = 0; i < fileCount; i++){
        entry =  dir.openNextFile();
        if (entry.isDirectory()) {
            Serial.print("is a Dir");
            //do nothing
        } else if (entry.size() != 0){
            // files have sizes - directories do not
            Serial.print(entry.name());
            filesArr[i] = entry.name();
        } else{
            Serial.print("ERROR!! ");
            Serial.print(entry.name());
        }
    }
    entry.close();
    Serial.print("Done writing files");
} 
