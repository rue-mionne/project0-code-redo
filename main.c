#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* String;

//zapewnienie poprawnej obsługi dla obu systemów plików
#ifdef _WIN32
	#define endline "\n"//dlaczego 3 prawie identyczne linie kodu? wcześniej było tu /r/n, ale podczas testowania na MS Windows okazało się, że C automatycznie tłumaczy \n na natywny dla środowiska znak nowej linii... a linia zostałą tu jako swoisty easteregg
#endif

#ifdef _unix_
	#define endline "\n"//teoretycznie w tej linii mogłoby się pojawić "/r/n" w celu zgodności z MS Windows... ale skoro plik tworzony na UNIX, to z założenia na UNIX używany, więc po co na siłę dostosowywać
#endif

#ifndef endline   //o ile unix zawsze złapie linuxa, dobrze mieć awaryjny define
	#define endline "\n"
#endif

#define LINE_LENGTH 80
#define BUF_SIZE 512

enum lineError{END_OF_BUFFER=-2, EMPTY_LINE=-1};
char* defaultOriginPath={"t1.txt"};
char* defaultSavePath={"j1.txt"};
char originPath[40],savePath[40];

void askForFile(char mode)
{
	switch(mode)
	{
		case 'r':
			printf("Podaj ścieżkę do pliku do konwersji: (d - wartość domyślna \"t1.txt\")%s", endline);
			scanf("%s",originPath);
			if(originPath[0]=='d')
				strcpy(originPath,defaultOriginPath);
			break;
		case 'w':
			printf("Podaj ścieżkę pliku wynikowego:(d - wartość domyślna \"j1.txt\")%s", endline);
			scanf("%s",savePath);
			if(savePath[0]=='d')
				strcpy(savePath,defaultSavePath);
			break;
	}
	fflush(stdin);//żeby mieć pewność, że program się zatrzyma w razie błędu, aż nie dostanie inputu użytkownika
}

int readToBuffer(FILE* originDescriptor, char buffer[BUF_SIZE])
{
	char tempChar;
	int characterCount=0;
	memset(buffer, 0, strlen(buffer));
	while((tempChar=fgetc(originDescriptor))!=EOF&&characterCount<(BUF_SIZE-1))
	{
		if(tempChar!='\r'&&tempChar!='\n'&&tempChar!='\t')//ignorowanie znaków nowej linii i tabulatorów
		{
			buffer[characterCount]=tempChar;
			characterCount++;
		}
		else if(tempChar=='\n')//ignorowanie zachowań nowej linii
		{
			buffer[characterCount]=' ';
			characterCount++;
		}//ostatecznie kod powyżej ignoruje całkowicie znak powrotu (pliki Windows), a znaki nowej linii zmienia w spacje unikając klejenia się słów i przeskoków między liniami
	}
	buffer[characterCount]='\0';//jako że kopiuje ręcznie znaki, to zaznaczyć koniec string też trzeba ręcznie
	ungetc(tempChar,originDescriptor);//zwrócenie nadmiarowego znaku z getc
	return characterCount;//wzorem UNIXowej funkcji read() zwrócenie ilości zaczytanych znaków
}

int findWord(String buffer)//buf musi być iteratorem!
{
	int bitCount;//zmienna pomocnicza licząca ilość bitów w słowie
	for(bitCount=0;bitCount<LINE_LENGTH*2;bitCount++)//mało prawdopodobne, żeby napotkał na ciąg znaków 2-bitowych na całą linię, ale możliwe
	{
		if(*buffer==' '||*buffer=='\0')//końcem słowa jest zawsze spacja lub koniec bufora
		{
			if(bitCount==0)
				return END_OF_BUFFER;//-2 dokładnie, nazwa enum tu trochę myląca, jako że w tym przypadku jest błędem podwójnej spacji, a nie końca bufora... ale ta wartość jest wykorzystywana później
			if(*buffer=='\0')//bez tego warunku czasem znak zostaje pominięty; dokładnie, kiedy jest zaraz po słowie; w pewnych warunkach powoduje zaczytanie śmieci z bufora do linii, więc dobrze się upewnić, że funkcja obsługująca linie na pewno trafi
				*(buffer+1)='\0';
			break;
		}
			buffer++;
	}
	buffer=buffer-bitCount;//iterator wraca na swoje miejsce
	return bitCount;//i ponownie inspiracja UNIXowym read(); tym razem jednak zwrócona jest ilość znaków dozaczytania przez kolejną funkcję
}

int putWord(String buffer, String* word, int bitCount)
{
	*word=(char*)calloc((bitCount+1),sizeof(char));
	strncpy(*word, buffer,bitCount);//zczytaj liczbę bajtów (podaną przez find_word) z bufora i zapisz w drop
	(*word)[bitCount]='\0';//dla bezpieczeństwa, żeby nie wyszło w przypadku błędu
	int currentBit=0,utf8Count=0;
	for(;currentBit<bitCount;currentBit++)
	{
		if((*word)[currentBit]&0x80)//wykryj znak specjalny
		{
			currentBit++;
			utf8Count++;
		}
	}
	word=NULL;//wskaźnika już nie potrzebujemy: agregat przejmuje obsługę, a miejsce w pamięci zostaje zwolnione w funkcji free_agregat
	bitCount-=utf8Count;
	return bitCount;//charac jest więc teraz liczbą faktycznych znaków, a nie bajtów

}

void freeAggregation(String* aggregation, int wordCount)
{
	if(wordCount>0)//ciężko zwolnić coś, czego nie ma
	{
		for(;wordCount>=0;--wordCount)
		{
			free(aggregation[wordCount]);//po kolei, zwalniany element agregatu, zaczynając od najnowszego
		}
	}
	free(aggregation);//zostało już tylko zwolnić sam agregat
	aggregation=NULL;//nie chcemy przypadkiem użyć wskaźnika i nabrać śmieci
}

int createLine(String* source, String* line)//source musi być iteratorem
{
	String* aggregation;
	int sumWithSpaces=0, wordLenghtWithSpaces=1, wordCount=0;
	if((aggregation=(char**)calloc((LINE_LENGTH+1),sizeof(char*)))==NULL)//w najgorszym przypadku linia będzie składała się z (długość linii/2) jednoznakowych słów, każdym będącym literą specjalną (2 bajty)
	{
		fprintf(stderr, "Zabrakło pamięci!%s", endline);
		exit(1);
	}
	String* positionInAggregation=aggregation;
	while(sumWithSpaces<(LINE_LENGTH))//warunek konieczny do zatrzymana pętli po zapełnieniu agregatu
	{
		if(**source=='\0')//kiedy napotka koniec buffora: zakończ funkcję i powiadomfunkcję wywołującą o końcu buffera
		{
			(*source)-=wordLenghtWithSpaces;//usunięcie nadmiaru
			freeAggregation(aggregation,wordCount);//zwolnienie miejsca
			positionInAggregation=NULL;
			return END_OF_BUFFER;
		}
		wordLenghtWithSpaces=(findWord(*source)+1);//zapisz długość słowa ze spacją
		if((sumWithSpaces+wordLenghtWithSpaces)<LINE_LENGTH&&wordLenghtWithSpaces!=-1)//-1 w drugim warunku jako zabezpieczenie przed zapisem podwójnej spacji oznaczanej jako puste słowo
		{
			wordLenghtWithSpaces=putWord(*source,positionInAggregation,wordLenghtWithSpaces-1)+1;//alokacja i zapisanie słowa tylko po upewnieniu się, że jest na nie miejsce w agregacie
			positionInAggregation++;//it_agregat i word_count sterują agregatem, więc one też są zależne od warunku
			wordCount++;
		}
		if(wordLenghtWithSpaces!=-1)//podwójna spacja ignorowana przez licznik znaków
		{
			sumWithSpaces+=wordLenghtWithSpaces;
		}
		if(sumWithSpaces<LINE_LENGTH)
			*source+=(abs(wordLenghtWithSpaces));//właśnie ze względu na tą linię, kod błędu podwójnej spacji to -2
	}
	sumWithSpaces-=wordLenghtWithSpaces;//nadmiar już nie potrzebny
	positionInAggregation=aggregation;//powrót iteratora agregatu na początek
	int spaceDifference=LINE_LENGTH-(sumWithSpaces-1);//ilość dodatkowych spacji (-1 ze względu na brak spacji po ostatnim słowie)
	int additionalSpaces=spaceDifference;
	if(wordCount>1)
		additionalSpaces=spaceDifference%(wordCount-1);
	int spacesForEveryWord=0;
	if(wordCount!=1)
		spacesForEveryWord=(spaceDifference-additionalSpaces)/(wordCount-1);
	int currentWordNr,spacesRemaining;
	memset(*line, 0, strlen(*line));//czyszczenie linii
	for(currentWordNr=0;currentWordNr<(wordCount-1);currentWordNr++)
	{
		strcat(*line, *positionInAggregation);//wpisz słowo
		strcat(*line, " ");//wpisz standardową spację
		for(spacesRemaining=0;spacesRemaining<spacesForEveryWord;spacesRemaining++)//wpisz dodatkowe spacje
		{
			strcat(*line, " ");
		}
		if(additionalSpaces>0)//jeśli została jakaś reszta z dzielenia, to daj jedną dodatkową spację
		{
			strcat(*line, " ");
			additionalSpaces--;
		}
		positionInAggregation++;
	}
	strcat(*line, *positionInAggregation);//ostatnie słowo, po którym nie ma spacji
	freeAggregation(positionInAggregation,wordCount);//wyczyść pamięć
	positionInAggregation=NULL;
	return wordCount;//zwróć ilość zaczytanych słów
}

int saveToFile(FILE* saveDescriptor, String* data, String* line)//dane to iterator!
{
	int remainingBites, readBites;
	while((readBites=createLine((data), line))!=END_OF_BUFFER)//dopuki find_line nie powiadomi o końcu buffera
	{
		if(readBites>0)//jeżeli jakiekolwiek słowa zostały zapisane
			fprintf(saveDescriptor, "%s%s", *line, endline);//zapisz linię do pliku
		remainingBites=strlen(*data);//wyznacz pozostałą długość buffera
	}
	return remainingBites;//zwróć pozostałą długość buffera
}

int alignForFinalTime(String* source, String* line)//source musi być iteratorem; zmodyfikowana funkcja find_line
{

	String* aggregation;
	int sumWithSpaces=0, wordLenghtWithSpaces=1, wordCount=0;
	if((aggregation=(char**)calloc((LINE_LENGTH+1),sizeof(char*)))==NULL)//w najgorszym przypadku linia będzie składała się z (długość linii/2) jednoznakowych słów, każdym będącym literą specjalną (2 bajty)
	{
		fprintf(stderr, "Zabrakło pamięci!%s", endline);
		exit(1);
	}
	String* positionInAggregation=aggregation;
	while(**source!='\0')//tym razem mamy pewność, że ilość znaków jest mniejsza, niż długość agregatu, należy tylko sprawdzać, czy nie dotarliśmy do końcu bufora (pliku)
	{
		wordLenghtWithSpaces=(findWord(*source)+1);//zapisz długość słowa ze spacją
		if(wordLenghtWithSpaces!=-1)//-1 w drugim warunku jako zabezpieczenie przed zapisem podwójnej spacji oznaczanej jako puste słowo
		{

			wordLenghtWithSpaces=putWord(*source, positionInAggregation,wordLenghtWithSpaces-1)+1;//alokacja i zapisanie słowa tylko po upewnieniu się, że jest na nie miejsce w agregacie
			positionInAggregation++;//it_agregat i word_count sterują agregatem, więc one też są zależne od warunku
			wordCount++;

		}
		if(wordLenghtWithSpaces!=-1)//nadal ignorujemy podwójne spacje
		{
			sumWithSpaces+=wordLenghtWithSpaces;
		}
		*source+=(abs(wordLenghtWithSpaces));//właśnie ze względu na tą linię, kod błędu podwójnej spacji to -2
	}
	positionInAggregation=aggregation;//powrót iteratora agregatu na początek
	int spaceDifference=LINE_LENGTH-(sumWithSpaces-1);//ilość dodatkowych spacji (-1 ze względu na brak spacji po ostatnim słowie)
	int additionalSpaces=spaceDifference;//jak w find_line, z jedną różnicą, że nie ma nadmiaru, który wcześniej trzeba usunąć
	if(wordCount>1)
		additionalSpaces=spaceDifference%(wordCount-1);
	int spacesForEveryWord=0;
	if(wordCount!=1)
		spacesForEveryWord=(spaceDifference-additionalSpaces)/(wordCount-1);
	int currentWordNr, spacesRemaining;
	memset(*line, 0, strlen(*line));
	for(currentWordNr=0;currentWordNr<(wordCount-1);currentWordNr++)
	{
		strcat(*line, *positionInAggregation);
		strcat(*line, " ");
		for(spacesRemaining=0;spacesRemaining<spacesForEveryWord;spacesRemaining++)
		{
			strcat(*line, " ");
		}
		if(additionalSpaces>0)
		{
			strcat(*line, " ");
			additionalSpaces--;
		}
		positionInAggregation++;
	}
	strcat(*line, *positionInAggregation);
	freeAggregation(aggregation,wordCount);
	positionInAggregation=NULL;
	return wordCount;
}

int main()
{
	char buffer[BUF_SIZE];//inicjacja ciągów związanych z bufferem
	buffer[0]='\0';
	String data=(char*)calloc(LINE_LENGTH*2+BUF_SIZE+1,sizeof(char));
	data[0]='\0';
	String tempData=(char*)calloc(BUF_SIZE+1, sizeof(char));
	tempData[0]='\0';

	askForFile('r');//pozwól użytkownikowi wybrać czytany plik
	FILE* originDescriptor;
	originDescriptor=fopen(originPath, "r");//otwarcie pliku źródłowego
	if(originDescriptor==NULL)
	{
		printf("Plik nie istnieje!\n");//tak łatwo o literówkę...
		getchar();
		exit(1);
	}
	int bitesInBuffer, remainingBitesInData=1;
	bitesInBuffer=readToBuffer(originDescriptor, buffer);//ilość znaków z bufora
	remainingBitesInData=bitesInBuffer;//ilość dostępnych znaków
	if(bitesInBuffer>0)
		strcpy(data, buffer);
	else
	{
		printf("Pusty plik!%s", endline);//w razie jakby plik istniał, ale był pusty
		getchar();
		exit(1);
	}
	String line=(char*)calloc((LINE_LENGTH*2+1),sizeof(char));
	char *positionInData=data;
	askForFile('w');//pozwól użytkownikowi wybrać lokalizację docelową
	FILE* saveDescriptor;
	saveDescriptor=fopen(savePath, "w");	
	while(bitesInBuffer>0&&remainingBitesInData!=0)
	{
		bitesInBuffer=saveToFile(saveDescriptor, &positionInData, &line);//zapisz bufor do pliku i zapisz liczbę pozostałych bajtów
		if(bitesInBuffer!=80&&remainingBitesInData<BUF_SIZE-1&&positionInData[0]=='\0')//zawsze może się trafić plik, który zajmie tylko jedną linijkę
		{
			positionInData=data;
			goto ending;//potraktuj jak ostatnią linijkę
		}
		positionInData=data+(strlen(data)-bitesInBuffer);//przesuń iterator na pierwsze miejsce pozostałego bufora
		if(positionInData[0]!='\0')//jeśli coś zostało
		{
			memset(tempData, 0, strlen(tempData));//wyczyść tymczasowy bufor
			strcat(tempData,positionInData);//przenieś pozostałość bufora do tymczasowego bufora
		}

		remainingBitesInData=readToBuffer(originDescriptor, buffer);//pobierz do bufora wejściowego kolejny fragment pliku
		if(data[0]!='\0')
			memset(data,0,strlen(data-1));//wyczyść główny bufor
		strcpy(data,tempData);//przenieś zawartość bufora tymczasowego
		strcat(data, buffer);//przenieś zawartość bufora wejściowego
		strcat(data, "\0");//upewnij się, że całość jest traktowana jako string
		bitesInBuffer+=remainingBitesInData;//zaktualizuj licznik bufora głównego
		positionInData=data;//iterator wraca na początek głównego bufora
		
		}
ending:
	if(alignForFinalTime(&positionInData, &line)!=EMPTY_LINE)//jeśli coś zostało 
	{
		fprintf(saveDescriptor, "%s%s", line, endline);//zapisz w pliku ostatnią linijkę
	}
	free(data);//ostateczne zwolnienie heap, wskaźników i deskryptorów
	data=NULL;
	free(tempData);
	tempData=NULL;
	free(line);
	line=NULL;
	positionInData=NULL;
	fclose(saveDescriptor);
	fclose(originDescriptor);
	exit(0);
}
