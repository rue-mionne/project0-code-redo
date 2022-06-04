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
	while(isLineIncomplete())
	{
		if(isThereABitLeft())
			addWordToAggregation();
		else
			finishLineCreation(LINE_INCOMPLETE);
	}
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
	/*
	 *
	 *TODO: Finish line handling
	 *
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
