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

// These are the pins used for the breakout example
#define SHIELD_RESET  -1     // VS1053 reset pin (output)
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


/******************** define button *******************/
#define PAUSE_BUTTON 2
#define FORWARD_BUTTON 5
#define BACKWARD_BUTTON 11
//#define REC_BUTTON 12
#define VOLUME_PIN A0
/******************* variable *****************/
int state_pause = 0;
int countForwardButton = 0;
int countBackwardButton = 0;

/***************** play files *******************/
String* filesArr = NULL;
char* str = NULL; 
File entry;
File root; 

/************** recording variable *************/
File recording;  // the file we will save our recording to
#define RECBUFFSIZE 128  // 64 or 128 bytes.
uint8_t recording_buffer[RECBUFFSIZE];
boolean isRecording = false;

int currentSong = 0;
int fileCount = 0;

#define DATALENTH 32   // default read data size



void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Library Test");
  
  pinMode(PAUSE_BUTTON,INPUT);
  digitalWrite(PAUSE_BUTTON,HIGH);
  pinMode(FORWARD_BUTTON,INPUT);
  digitalWrite(FORWARD_BUTTON,HIGH);

  pinMode(BACKWARD_BUTTON, INPUT);
 digitalWrite(BACKWARD_BUTTON, HIGH);
  
  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  
  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  

  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);

  
  /***** Two interrupt options! *******/ 
  // This option uses timer0, this means timer1 & t2 are not required
  // (so you can use 'em for Servos, etc) BUT millis() can lose time
  // since we're hitchhiking on top of the millis() tracker
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
  
  // This option uses a pin interrupt. No timers required! But DREQ
  // must be on an interrupt pin. For Uno/Duemilanove/Diecimilla
  // that's Digital #2 or #3
  // See http://arduino.cc/en/Reference/attachInterrupt for other pins
  // *** This method is preferred
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
    
    root = SD.open("/");
    countFiles(root);
    filesArr = new String[fileCount];
      // list files
    printDirectory(SD.open("/"), 0);
    currentSong = 0;
    while (currentSong < fileCount){
        int len = filesArr[currentSong].length() +1;
        str = new char[len]; //error: invalid conversion from ‘char*’ to ‘char’ [-fpermissive]
        filesArr[currentSong].toCharArray(str,len); //error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive] 
        Serial.print("playing "); Serial.println(str);
        musicPlayer.startPlayingFile(str);
        //while(!musicPlayer.startPlayingFile(str));
        
        currentSong++;
     //   Serial.println(count);
        delete [] str; //error: type ‘char’ argument given to ‘delete’, expected pointer
        str = NULL;
        
        while (! musicPlayer.stopped() ){
            loop();

            int val = analogRead(VOLUME_PIN);// Analog read for dial. 0 to 1023.
            musicPlayer.setVolume(val*100/1000,val*100/1000); 
            //Volume set up will be constant depending on the dials' current postion while a song plays
        }
        
    }
}


void loop() {  
    
      
        if(currentSong > fileCount)
        {
          Serial.println("it's over");
          while(1){
          }
       } 
        countForwardButton = 0; 
        countBackwardButton = 0;
        /********************* pause and play *************************/
       if(digitalRead(PAUSE_BUTTON) == LOW)
       {  
          Serial.println("PAUSE");
          musicPlayer.pausePlaying(true); 
          delay(200);
          while (digitalRead(PAUSE_BUTTON)){// while input pin is LOW
          }
          delay(200);
          Serial.println("PLAY");
          musicPlayer.pausePlaying(false);
       }    
       
       /******************** Foward ***********************/
       while(!digitalRead(FORWARD_BUTTON))
       {
             countForwardButton++;
             delay(100);
             if(countForwardButton > 10)
             {
                  Serial.println("FF");
                  Serial.println(musicPlayer.currentTrack.position());
             }
       }
       
       
       /********************** Backward *******************/
        while(!digitalRead(BACKWARD_BUTTON))
       {
             countBackwardButton++;
             delay(100);
             if(countBackwardButton > 10)
             {
                  Serial.println("BB");
                  Serial.println(musicPlayer.currentTrack.position());
             }
       }
       
       if(countForwardButton == 1)
       {
         /**************** skip forward ************************/
         musicPlayer.stopped();
         Serial.println("FORWARD");
         
         int len = filesArr[currentSong].length() +1;
         str = new char[len]; //error: invalid conversion from ‘char*’ to ‘char’ [-fpermissive]
         filesArr[currentSong].toCharArray(str,len); //error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive] 
         Serial.print("RE PLAYING "); Serial.println(str);
         musicPlayer.startPlayingFile(str);
         delete [] str; //error: type ‘char’ argument given to ‘delete’, expected pointer
         str = NULL;
         currentSong++;
       }
       
       if(countBackwardButton == 1)
       {
         /**************** skip backward ************************/
         musicPlayer.stopped();
         Serial.println("BACKWARD");
         currentSong = currentSong -2;
         int len = filesArr[currentSong].length() +1;
         str = new char[len]; //error: invalid conversion from ‘char*’ to ‘char’ [-fpermissive]
         filesArr[currentSong].toCharArray(str,len); //error: invalid conversion from ‘char’ to ‘char*’ [-fpermissive] 
         Serial.print("BACKWARD RE PLAYING "); Serial.println(str);
         musicPlayer.startPlayingFile(str);
         delete [] str; //error: type ‘char’ argument given to ‘delete’, expected pointer
         str = NULL;
         currentSong++;
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
            if(String(entry.name()).equals("V44K1Q05.IMG"))
                    continue;
            else
            {
              fileCount++;
            }
        }
    }
}

/// File listing helper
void printDirectory(File dir, int numTabs) {
   int numFiles = 0;
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
      Serial.print(entry.name());

     
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
       if(String(entry.name()).equals("V44K1Q05.IMG"))
                    continue;
       filesArr[numFiles++] = entry.name();
     }
     entry.close();
   }
}
