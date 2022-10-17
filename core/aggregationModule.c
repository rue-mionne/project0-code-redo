#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "aggregationModule.h"

int whiteChars=0;
wchar_t* aggregation[CHARACTERLIMIT];
int aggregationLoad=0;
enum buffer{FULL=0, ENDED=1};

int didTheWordEnd(wchar_t);
int isItNotAWord(int, wchar_t**);
void clearAggregation();
void loadWord(wchar_t***, wchar_t**);
void updateAggregationLoad(wchar_t***);
int isAggregationFull();
wchar_t* moveLinePointer(wchar_t*, wchar_t*);
enum buffer isBufferEmpty(wchar_t);

wchar_t* findWord(wchar_t** line){
	int bitCount=0;
	while(!didTheWordEnd((*line)[bitCount])){
		bitCount++;
	}
	if(isItNotAWord(bitCount, line)){
		wchar_t* tempLine=(*line)+1;
		whiteChars++;
		return findWord(&tempLine);
	}
	wchar_t* word=calloc(sizeof(wchar_t), bitCount+1);
	wcsncpy(word, *line, bitCount);
	return word;
}

int didTheWordEnd(wchar_t letter){
	if(letter==' '||letter=='\0'||letter=='\t')
		return 1;
	else
		return 0;
}

int isItNotAWord(int bitCount, wchar_t** line){
	if(bitCount==0&&(*line)[0]!='\0')
		return 1;
	else
		return 0;
}

wchar_t** formAggregation(wchar_t *line){
	clearAggregation();
	wchar_t** aggregationPointer=aggregation;
	while(!isAggregationFull()){
	loadWord(&aggregationPointer, &line);
	}
	aggregationLoad=0;
	return aggregation;
}

void clearAggregation(){
	for(int i = 0; i<CHARACTERLIMIT; i++ )
	{
		aggregation[i]=(wchar_t*)0;
	}

}

void loadWord(wchar_t*** aggregationPointer, wchar_t** line){
	whiteChars=0;
	**aggregationPointer=findWord(line);
	*line=moveLinePointer(**aggregationPointer, *line);
	updateAggregationLoad(aggregationPointer);
	++*aggregationPointer;
}

void updateAggregationLoad(wchar_t*** aggregationPointer){
	aggregationLoad=aggregationLoad+wcslen(**aggregationPointer)+1;
}

int isAggregationFull(){
	if(aggregationLoad<=CHARACTERLIMIT)
		return 0;
	else
		return 1;	
}

wchar_t* moveLinePointer(wchar_t* word, wchar_t* line){
	return line+(wcslen(word)+whiteChars);
}

enum buffer isBufferEmpty(wchar_t character){
	if(character=='\0')
		return ENDED;
	else
		return FULL;
}
