#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int didTheWordEnd(char);
int isItOnlySpace(int);
void skipBite();

char* findWord(char** line){
	int bitCount=0;
	while(!didTheWordEnd((*line)[bitCount])){
		bitCount++;
	}
	if(isItOnlySpace(bitCount)){
		skipBite();//TODO
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

int isItOnlySpace(int bitCount){
	if(bitCount==1)
		return 1;
	else
		return 0;
}
