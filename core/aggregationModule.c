#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int didTheWordEnd(wchar_t);
int isItNotAWord(int, wchar_t**);


wchar_t* findWord(wchar_t** line){
	int bitCount=0;
	while(!didTheWordEnd((*line)[bitCount])){
		bitCount++;
	}
	if(isItNotAWord(bitCount, line)){
		wchar_t* tempLine=(*line)+1;
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


