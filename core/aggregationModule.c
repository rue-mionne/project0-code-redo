#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int didTheWordEnd(char);

char* findWord(char** line){
	int bitCount=0;
	while(!didTheWordEnd((*line)[bitCount])){
		bitCount++;
	}
	char* word=calloc(sizeof(char), bitCount+1);
	strncpy(word, *line, bitCount);
	return word;
}

int didTheWordEnd(char letter){
	if(letter==' '||letter=='\0')
		return 1;
	else
		return 0;
}
