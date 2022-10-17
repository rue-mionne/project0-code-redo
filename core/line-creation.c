#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line-creation.h"

char** aggregation;
char** positionInAggregation;
int sumWithSpaces, wordLenghtWithSpaces, wordCount;
char** positionInSource;
lineStatus lineExitStatus;

void createLine(char** sourceBuffer){
	initiateLineCreation(sourceBuffer);
	fillAggregation();
}

void initiateLineCreation(char** sourceBuffer){
	createAggregation();
	setPositionInAggregation();
	sumWithSpaces=0;
	wordLenghtWithSpaces=1;
	wordCount=0;
	positionInSource=sourceBuffer;
}

void createAggregation(){
	if((aggregation=(char**)calloc((lineLenght+1), sizeof(char*)))==NULL){
			fprintf(stderr, "Allocation error!\n");
			finishLineCreation(ALLOCATION_ERROR);
	}	
}

void setPositionInAggregation(){
	positionInAggregation=aggregation;
}

void fillAggregation(){
	if(isThereABitLeft())
		addWordToAggregation();
	else
		finishLineCreation(LINE_INCOMPLETE);
}

int isLineIncomplete(){
	if(sumWithSpaces<lineLenght)
		return 1;
	else
		return 0;
}

int isThereABitLeft(){
	if(**positionInSource=='\0')
		return 0;
	else
		return 1;
}

void addWordToAggregation(){
	int wordLenght;
	wordLenght=findNextWord();
	if(isThereOverflow(wordLenght)||isWordEmpty(wordLenght))
		handleAbnormalWord(wordLenght);
	else{
		updateAggregation(wordLenght);
		/*
		 *
		 *TODO: Find true word length (separated from putWord function from original file); WARNING: remove while loop being Aggregation control - modify updateAggregation to be recursion
		 *
		 *
		 */
	}
}

int findNextWord(){
	int bitCount=0;
	while(bitCount<(lineLenght*2)){
		if(didTheWordEnd()){
			return finishWordCreation(bitCount);
		}
		bitCount++;
		positionInSource++;
	}
	return -1; //TODO: Abnormal source exception
}

int didTheWordEnd(){
	if(**positionInSource==' '||**positionInSource=='\0')
		return 1;
	else
		return 0;
}

int finishWordCreation(int bitCount){
	if(bitCount==0)
		return -2;
	positionInSource-=bitCount;
	return bitCount;
}

int isThereOverflow(int wordLenght)
{
	if((sumWithSpaces+wordLenght+1) >= lineLenght)
		return 1;
	else
		return 0;
}

int isWordEmpty(int wordLenght){
	if(wordLenght==-2)
		return 1;
	else
		return 0;
}

void handleAbnormalWord(int wordLenght){
	if(isThereOverflow(wordLenght))
		formatLine();
	if(isWordEmpty(wordLenght))
		*positionInAggregation++;

}

void updateAggregation(int wordLenght){
	positionInAggregation=createWord(wordLenght);
	positionInAggregation++;
	updateWordLenght(wordLenght);
}

char** createWord(int wordLenght){
	char** word;
	char* tempWord=(char*)calloc((wordLenght+1), sizeof(char));
	word=&tempWord;
	strncpy(*word,*positionInSource, wordLenght);
	return word;
}

void updateWordLenght(int wordLenght){
	int currentBitCount=0;
	int utf8BitCount=0;
	for(;currentBitCount<wordLenght;currentBitCount++){
		if(isItUtf8((*positionInAggregation)[currentBitCount])){
			currentBitCount++;
			utf8BitCount++;
		}//TODO: export if to different function
	}
}

int isItUtf8(char fisrtBite){
	/*
	 * TODO:checking for utf8
	 */
}

void formatLine(){
/*
 *
 * TODO:line formatting
 *
 */
}

void finishLineCreation(lineStatus exitStatus){
	freeAggregation();
	positionInAggregation=NULL;
	lineExitStatus=exitStatus;
}

void freeAggregation(){
	if(areWordsInAggregation())
	{
		freeWords();
	}
	freeAggregation();
	aggregation=NULL;
}

int areWordsInAggregation(){
	if(wordCount>0)
		return 1;
	else
		return 0;
}

void freeWords(){
	for (; wordCount>=0; --wordCount) 
	{
		free(aggregation[wordCount]);
	}
}

lineStatus getLineStatus(){
	return lineExitStatus;
}
