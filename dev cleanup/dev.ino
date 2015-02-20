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
int inputPin1 = 2;     
int inputPin2 = 3;
boolean buttonPressed = false;//boolean testing for one of the buttons

// char *names[3]={"sample 1", "sample 2", "sample 3"};
char *files[3]={"sample1.mp3", "sample2.mp3", "sample3.mp3"};
  
  
void setup() {
    pinMode(inputPin1, INPUT);//pin mode setup for both buttons  
    pinMode(inputPin2, INPUT);

    Serial.begin(9600);
    Serial.println("Adafruit VS1053 Audio Player");

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



    // int count = 0;

    // while (count < 3){

    //     Serial.print("Playing ");
    //     Serial.println(files[count]);
        // musicPlayer.startPlayingFile(files[count]);

    //     while (! musicPlayer.stopped()){
    //         loop();
    //     }
    // count ++ ;
    // }
}

//button calling
void loop() {
    // File is playing in the background
    if (musicPlayer.stopped()) {
        Serial.println("Done playing music");
        while (1);
    }

    if(digitalRead(inputPin1) == HIGH){
        Serial.println("Button 1 Pressed");
        buttonPressed = true;
    }
    
    if(Serial.available()) {
        if(buttonPressed){
            if(!musicPlayer.paused()){
                Serial.println("Pausing...");
                musicPlayer.pausePlaying(true);
            }
            else if(musicPlayer.paused()){
                Serial.println("Resuming...");
                musicPlayer.pausePlaying(false);
            }
        }
    }
    //State:
        //Playing
            //musicPlayer.playingMusic()
            //button 1 pressed: pause
            //button 2 pressed: stop
        //Paused
            //musicPlayer.paused()
            //button 1 pressed: play
            //button 2 pressed: stop
        //Stopped
            //musicPlayer.stopped();
            //button 1 pressed: play
            //button 2 pressed: next file




    // if (Serial.available()) {

    //     if (buttonPressed) {
    //         musicPlayer.stopPlaying();
    //         while (1);
    //     }

    //     if (digitalRead(inputPin1) == LOW) {
    //         if (! musicPlayer.paused()) {
    //             Serial.println("Paused");
    //             musicPlayer.pausePlaying(true);
    //         } 
    //         else { 
    //             Serial.println("Button 1 Pressed");
    //             Serial.println("Resumed");
    //             musicPlayer.pausePlaying(false);
    //         }
    //     }
    // }
    // if(digitalRead(inputPin2) == HIGH){
    //     Serial.println("Button 2 Pressed");
    //     buttonPressed = true;
    // }
    delay(100);
}