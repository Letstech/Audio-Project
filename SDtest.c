#include <SD.h>

File root;
File entry;
int count = 0;
String* filesArr = NULL;


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

    if (!SD.begin(4)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

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
    Serial.print("Final Files Array: \n");
    for(int t = 0; t < count; t++){
        Serial.print(t);
        Serial.print("\n");
        Serial.print(filesArr[t]);
        Serial.print("\n");
    }


}

void loop()
{
    // nothing happens after setup finishes.
}

int countFiles(File dir) {
    Serial.println("Counting files...");
    while(true) {
     
        entry =  dir.openNextFile();
        if (!entry) {
            // no more files
            Serial.println(count);
            Serial.println("No more files to count.");
            break;
        }
            if (entry.isDirectory()) {
            //do nothing
        } else if (entry.size() != 0){
            // files have sizes, directories do not
            count++;
            // Serial.println(count);
            // Serial.print("\n");
        }
    }
}

void writeDirectory(File dir) {
    countFiles(root);
    root.close();

    root = SD.open("/");
    
    filesArr = new String[count];

    Serial.println("Writing files...");
    for(int i = 0; i < count; i++){
        entry =  dir.openNextFile();
        if (entry.isDirectory()) {
            Serial.print("is a Dir");
            //do nothing
        } else if (entry.size() != 0){
            // files have sizes - directories do not
            // Serial.print("Entry: ");
            Serial.print(entry.name());
            // Serial.print("\n");
            // Serial.print("File Arr:  ");
            filesArr[i] = entry.name();
            // Serial.print(filesArr[i]);
            // Serial.print("\n");
        } else{
            Serial.print("ERROR!! ");
        }
    }
    entry.close();
    // Serial.print("Final Files Array: \n");
    // for(int t = 0; t < count; t++){
    //     Serial.print(t);
    //     Serial.print("\n");
    //     Serial.print(filesArr[t]);
    //     Serial.print("\n");
    // }
    // delete [] filesArr;
    // filesArr = NULL;
}