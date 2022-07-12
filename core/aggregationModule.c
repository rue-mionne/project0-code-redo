#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int didTheWordEnd(char);
int isItNotAWord(int, char**);

char* findWord(char** line){
	int bitCount=0;
	while(!didTheWordEnd((*line)[bitCount])){
		bitCount++;
	}
	if(isItNotAWord(bitCount, line)){
		char* tempLine=(*line)+1;
		return findWord(&tempLine);
	}
	char* word=calloc(sizeof(char), bitCount+1);
	strncpy(word, *line, bitCount);
	return word;
}

int didTheWordEnd(char letter){
	if(letter==' '||letter=='\0'||letter=='\t')
		return 1;
	else
		return 0;
}

int isItNotAWord(int bitCount, char** line){
	if(bitCount==0&&(*line)[0]!='\0')
		return 1;
	else
		return 0;
}


