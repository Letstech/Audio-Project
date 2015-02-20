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
int inputPin1 = 3; //!!!!!!!reassign the pins here to test out the pause/play and stop functions     
int inputPin2 = 2;
//Swapping these pins to 3, 2 results in:
    //btn1: brief pause
    //btn2: fast forward


int count = 0;

boolean buttonPressed1 = false;
boolean buttonPressed2 = false;//boolean testing 

// char *names[3]={"sample 1", "sample 2", "sample 3"};
char *files[3]={"sample1.mp3", "sample2.mp3", "sample3.mp3"};

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

    SD.begin(CARDCS);    // initialise the SD card

    // Set volume for left, right channels. lower numbers == louder volume!
    musicPlayer.setVolume(5,5);

    // Timer interrupts are not suggested, better to use DREQ interrupt!
    //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

    // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

    while (count < 3){

        Serial.print("Playing ");
        Serial.println(files[count]);
        musicPlayer.startPlayingFile(files[count]);
        count ++ ;
        Serial.println(count);

        while (!musicPlayer.stopped()){
            loop();
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

    Serial.print("Button1 input: ");
    Serial.println( digitalRead(inputPin1));
    Serial.print("Button2 input: ");
    Serial.println( digitalRead(inputPin2));

    if (digitalRead(inputPin1) == HIGH){
        buttonPressed1 = true;
        Serial.println("Button 1 Pressed");
    }

    if(digitalRead(inputPin2) == HIGH){  //This doesn't appear to trigger
        buttonPressed2 = true;
        Serial.println("Button 2 Pressed"); 
    }

    /**TESTING Debounce
    int btnRead;
    btnRead = digitalRead(inputPin2);
    int btn2State = LOW;
    int lastBtn2State = LOW;
    long lastDebounceTime = 0;
    long debounceDelay = 200;  

    if (btnRead != lastBtn2State){
        lastDebounceTime = millis();
        Serial.println("lastDebounceTime: ");
        Serial.println(lastDebounceTime);
    }
    if ((millis() - lastDebounceTime) > debounceDelay){
        btn2State = btnRead;
        Serial.println("Button 2 Pressed");
        lastDebounceTime = millis();
        buttonPressed2 = true;
    }
    lastBtn2State = btnRead;
    **/




    if (buttonPressed2) { //This doesn't appear to trigger
        delay(500);// delay is used as a fail safe to prevent rapid button input when held down.
        Serial.println("Track Stopped");
        // musicPlayer.stopPlaying();
        musicPlayer.pausePlaying(true); //instead of stopPlaying, let's call pause and then start playing next file
        count++;
        
        buttonPressed2 = false;
    }

    if (buttonPressed1){
        Serial.println("Paused");
        musicPlayer.pausePlaying(true); 
        delay(300);         
        while (!digitalRead(inputPin1)){ // while input pin is LOW stay on pause
        }
    delay(300);
    Serial.println("Resumed");
    musicPlayer.pausePlaying(false);
    buttonPressed1 = false;
    }
// }
delay(100);
}