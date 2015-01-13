#include <SD.h>

File root;
File entry;
int fileCount = 0; 
String* filesArr = NULL;
char* fileList = NULL;


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
    Serial.print("EXTRA FINAL Files Array: \n");
    for(int t = 0; t < fileCount; t++){
        Serial.print(t);
        Serial.print("\n");
        Serial.print(fileList[t]);
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

    root = SD.open("/");
    
    filesArr = new String[fileCount];
    fileList = new char[fileCount];

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
        }
    }
    entry.close();




    for(int j = 0; j < fileCount; j++){
        String temp = NULL;
        temp = filesArr[j];
        char str[sizeof(temp)];
        temp.toCharArray(str,sizeof(str));
        fileList[j] = temp;
    }
} 