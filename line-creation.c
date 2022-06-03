#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line-creation.h"

char** aggregation;
char** positionInAggregation;
int sumWithSpaces, wordLenghtWithSpaces, wordCount;


void createLine()
{
	createAggregation();
	setPositionInAggregation();
}

void createAggregation()
{
	if((aggregation=(char**)calloc((lineLenght+1), sizeof(char*)))==NULL)
		fprintf(stderr, "Zabrakło pamięci!\n");
}

void setPositionInAggregation()
{
	positionInAggregation=aggregation;
}

void addWordToAggregate()
{
	while(isLineIncomplete())
	{
		
	}
}

int isLineIncomplete()
{
	if(sumWithSpaces<lineLenght)
		return 1;
	else
		return 0;
}
