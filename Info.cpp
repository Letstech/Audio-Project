#include "Arduino.h"
#include "Info.h"

Info::Info(int count)
{
    fileName = new char*[count];
	for(int i=0; i<count;i++)
	{
		fileName[i] = new char[12];
		memset(fileName[i],0,12);
	}

	totalFile = count;
	currentFile = 0;
}

void Info::inputFileName(int idx, char* name)
{
	strcpy(fileName[idx], name);
}


char* Info::getFileName(int idx)
{
	return fileName[idx];
}

int Info::getCurrentFileIndex()
{
	return currentFile;
}

int Info::getTotalFile()
{
	return totalFile;
}

int Info::currentFileIndexInc()
{
	return currentFile++;
}

int Info::setCurrentFileIndex(int idx)
{
	currentFile = idx;
}
void Info::initCurrentFileIndex()
{
	currentFile = 0;

}

void Info::deleteFileName()
{
	for(int i=0; i<totalFile;i++)
	{
		delete[] fileName[i];
	}
	delete[] fileName;

}

