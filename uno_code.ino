#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include<MemoryFree.h>
#include <avr/wdt.h>
#include <Info.h>

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

/******************** define button *******************/
#define PAUSE_BUTTON 2
#define FORWARD_BUTTON 5
#define BACKWARD_BUTTON 9
#define REC_BUTTON 8
#define VOLUME_PIN A0
#define LED_PIN 10

/***************** forward backward variable *******************/
unsigned int countForwardButton = 0;
unsigned int countBackwardButton = 0;

/***************** recording ******************************/
File recording;  // the file we will save our recording to
#define RECBUFFSIZE 128  // 64 or 128 bytes.
uint8_t recording_buffer[RECBUFFSIZE];
uint8_t isRecording = false;

/*************** file list ****************************/
Info info = NULL;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Adafruit VS1053 Library Test"));
  
  pinMode(PAUSE_BUTTON,INPUT_PULLUP);
  pinMode(FORWARD_BUTTON,INPUT_PULLUP);
  pinMode(BACKWARD_BUTTON, INPUT_PULLUP);
  pinMode(REC_BUTTON,INPUT_PULLUP);
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  
  countForwardButton = 0;
  countBackwardButton = 0;

  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  //musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
 
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println(F("SD OK!"));
  
   countFiles(SD.open("/"));
   setFilelist(SD.open("/"));
   //printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);
  
  //Serial.print("free memory: "); Serial.println(freeMemory());
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT))
    Serial.println(F("DREQ pin is not an interrupt pin"));
    
}

void loop() {  
     
   countBackwardButton = 0;
   countForwardButton = 0;
  
  if( !isRecording && musicPlayer.playingMusic == false)
  { 
     Serial.print(F("free memory: ")); Serial.println(freeMemory());
   
    musicPlayer.reset();
    Serial.println(info.getFileName(info.getCurrentFileIndex()));
    if(info.getCurrentFileIndex() > info.getTotalFile())
    {
      Serial.println(F("end list"));
      while(1);
    }
    
    if (! musicPlayer.startPlayingFile(info.getFileName(info.getCurrentFileIndex()))) {
      Serial.println(F("Could not open file track001.mp3"));
      while (1);
    }
     info.currentFileIndexInc();
  }
 
  while (musicPlayer.playingMusic) {
  
    countBackwardButton = 0;
    countForwardButton = 0;
    delay(50);
    if(digitalRead(REC_BUTTON) == LOW)
    {
       musicPlayer.stopPlaying();
       delay(600);
       
       if(musicPlayer.stopped() && !isRecording)
       {
         
       if (! musicPlayer.prepareRecordOgg("v44k1q05.img")) {
           Serial.println(F("Couldn't load plugin!"));
         while (1);    
        }
        break;
       }
       
    }
    
     /********************* pause and play *************************/
     if(digitalRead(PAUSE_BUTTON) == LOW)
     {  
        Serial.println(F("PAUSE"));
        musicPlayer.pausePlaying(true); 
        delay(100);
        while (digitalRead(PAUSE_BUTTON)){// while input pin is LOW
          
        }
        delay(100);
        Serial.println(F("PLAY"));
        musicPlayer.pausePlaying(false);
     } 
     
       /******************** Foward ***********************/
       while(!digitalRead(FORWARD_BUTTON))
       {
             countForwardButton++;
             delay(80);
             if(countForwardButton > 10)
             { 
                  if(musicPlayer.currentTrack.position() <= 0 || musicPlayer.currentTrack.size() == 0)
                  {  
                      delay(1000);
                      musicPlayer.reset();
                      musicPlayer.stopPlaying();
                      break;
                  }
                  
                  if(musicPlayer.currentTrack.available())
                  {  
                      cli();
                      musicPlayer.currentTrack.flush();
                      musicPlayer.currentTrack.seek(musicPlayer.currentTrack.position() + 1024);
                      sei();
                  }
             }
       }

       /**************** skip forward ************************/
       if(countForwardButton > 1 && countForwardButton <= 10)
       {
         musicPlayer.stopPlaying();
         delay(1000);
         musicPlayer.reset();
         break;
       }
       
       /********************** Backward *******************/
       while(!digitalRead(BACKWARD_BUTTON))
       {
             countBackwardButton++;
             delay(10);
           
             if(countBackwardButton > 50)
             {
                  
                  if(musicPlayer.currentTrack.position() <= 0 || musicPlayer.currentTrack.size() == 0)
                  {  
                      delay(1000);
                      musicPlayer.stopPlaying();
                      musicPlayer.reset();
                      if((info.getCurrentFileIndex() - 2) < 0)
                      {
                           info.initCurrentFileIndex();
                      }
                      else
                      {
                         info.setCurrentFileIndex(info.getCurrentFileIndex()-2);
                      }
                      break;
                  }
                  
                  if(musicPlayer.currentTrack.available())
                  {  
                      cli();
                      musicPlayer.currentTrack.flush();
                      musicPlayer.currentTrack.seek(musicPlayer.currentTrack.position() - 4096);
                      sei();
                  }
                  
             }
       }
       
        /**************** skip backward ************************/
       if(countBackwardButton > 10 && countBackwardButton <= 49)
       {
         musicPlayer.stopPlaying();
         delay(1000);
         musicPlayer.reset();
         if((info.getCurrentFileIndex() - 2) < 0)
         {
              info.initCurrentFileIndex();
         }
         else
            info.setCurrentFileIndex(info.getCurrentFileIndex()-2);
         break;
       }
       
       int val = analogRead(VOLUME_PIN);// Analog read for dial. 0 to 1023.
        musicPlayer.setVolume(val*100/1000,val*100/1000); 
  }
  
  /******************** recording code **********************/
  if (!isRecording && !digitalRead(REC_BUTTON)) {
    Serial.println(F("Begin recording"));
    isRecording = true;
    
    // Check if the file exists already
    char filename[15];
    strcpy(filename, "R00.OGG");
    for (uint8_t i = 0; i < 100; i++) {
      filename[1] = '0' + i/10;
      filename[2] = '0' + i%10;
      // create if does not exist, do not open existing, write, sync after write
      if (! SD.exists(filename)) {
        Serial.println(F("exist filename"));
        break;
      }
    }
    
    Serial.print(F("Recording to ")); Serial.println(filename);
    recording = SD.open(filename, FILE_WRITE);
    if (! recording) {
       //Serial.print(F("Recording value: ")); Serial.println(recording);
       Serial.println(F("Couldn't open file to record!"));
       while (1);
    }
    musicPlayer.startRecordOgg(false); // use microphone (for linein, pass in 'false')
    //delay(10);
  }
  
 if (isRecording)
    saveRecordedData(isRecording);
    
 if (isRecording && digitalRead(REC_BUTTON)) {
    Serial.println(F("End recording"));
    musicPlayer.stopRecordOgg();
    isRecording = false;
    // flush all the data!
    saveRecordedData(isRecording);
    // close it up
    recording.close();
   // delay(100);
    musicPlayer.reset();
    info.deleteFileName();
    software_Reboot();
  }
}

uint16_t saveRecordedData(boolean isrecord) {
  uint16_t written = 0;
  //delay(10);
    // read how many words are waiting for us
  uint16_t wordswaiting = musicPlayer.recordedWordsWaiting();

  
  // try to process 256 words (512 bytes) at a time, for best speed
  while (wordswaiting > 256) {
   
    // for example 128 bytes x 4 loops = 512 bytes
    for (int x=0; x < 512/RECBUFFSIZE; x++) {
      // fill the buffer!
      for (uint16_t addr=0; addr < RECBUFFSIZE; addr+=2) {
        uint16_t t = musicPlayer.recordedReadWord();
        recording_buffer[addr] = t >> 8; 
        recording_buffer[addr+1] = t;
      }
      if (! recording.write(recording_buffer, RECBUFFSIZE)) {
            Serial.print(F("Couldn't write ")); Serial.println(RECBUFFSIZE); 
            while (1);
      }
    }
    // flush 512 bytes at a time
    recording.flush();
    written += 256;
    wordswaiting -= 256;
  }
  
  
  wordswaiting = musicPlayer.recordedWordsWaiting();
  if (!isrecord) {
    //Serial.print(wordswaiting); Serial.println(" remaining");
    // wrapping up the recording!
    uint16_t addr = 0;
    for (int x=0; x < wordswaiting-1; x++) {
      // fill the buffer!
      uint16_t t = musicPlayer.recordedReadWord();
      recording_buffer[addr] = t >> 8; 
      recording_buffer[addr+1] = t;
      if (addr > RECBUFFSIZE) {
          if (! recording.write(recording_buffer, RECBUFFSIZE)) {
                Serial.println(F("Couldn't write!"));
                while (1);
          }
          recording.flush();
          addr = 0;
      }
    }
    if (addr != 0) {
      if (!recording.write(recording_buffer, addr)) {
        Serial.println(F("Couldn't write!")); while (1);
      }
      written += addr;
    }
    musicPlayer.sciRead(VS1053_SCI_AICTRL3);
    if (! (musicPlayer.sciRead(VS1053_SCI_AICTRL3) & _BV(2))) {
       recording.write(musicPlayer.recordedReadWord() & 0xFF);
       written++;
    }
    recording.flush();
  }

  return written;
}

/// File listing helper
void countFiles(File dir) {
   int count = 0;
 
   //dir.rewindDirectory();
   while(true) {
     File entry =  dir.openNextFile();
    
     if (! entry) {
       break;
     }
     if (entry.isDirectory()) {
     } else {
      if(!String(entry.name()).equals("V44K1Q05.IMG"))
       {
                count++;
        }
     }
     entry.close();
   }
   dir.close();
   info = Info(count);   
  // Serial.print(F("total Count: "));
  // Serial.println(totalFile);
}

void setFilelist(File dir)
{
  
  int num = 0;
  while(true) {
     File entry =  dir.openNextFile();
   
     if (! entry) {
       break;
     }

     if (entry.isDirectory()) {
     } else {
      if(!String(entry.name()).equals("V44K1Q05.IMG"))
       {
            info.inputFileName(num,entry.name());
            num++;
        }
     }
     entry.close();
   }
 
  Serial.println(info.getTotalFile());
  
  dir.close();
}

void software_Reboot()
{
    wdt_enable(WDTO_15MS);
    while(1)
    {
    }
}
