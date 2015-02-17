#ifndef Info_h
#define Info_h

#include "Arduino.h"
 
class Info
{

  private:
    char** fileName;
    int totalFile;
	int currentFile;
	int Extension;
  public:
    Info(int count);
    void inputFileName(int idx, char* name);
	char* getFileName(int idx);
	int getCurrentFileIndex();
	int getTotalFile();
	int currentFileIndexInc();
	int setCurrentFileIndex(int);
	void initCurrentFileIndex();
	void deleteFileName();

};
 
#endif

