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

void ask_for_file(char mode)
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

int find_word(String buf)//buf musi być iteratorem!
{
	int charac;//zmienna pomocnicza licząca ilość bitów w słowie
	for(charac=0;charac<LINE_LENGTH*2;charac++)//mało prawdopodobne, żeby napotkał na ciąg znaków 2-bitowych na całą linię, ale możliwe
	{
		if(*buf==' '||*buf=='\0')//końcem słowa jest zawsze spacja lub koniec bufora
		{
			if(charac==0)
				return END_OF_BUFFER;//-2 dokładnie, nazwa enum tu trochę myląca, jako że w tym przypadku jest błędem podwójnej spacji, a nie końca bufora... ale ta wartość jest wykorzystywana później
			if(*buf=='\0')//bez tego warunku czasem znak zostaje pominięty; dokładnie, kiedy jest zaraz po słowie; w pewnych warunkach powoduje zaczytanie śmieci z bufora do linii, więc dobrze się upewnić, że funkcja obsługująca linie na pewno trafi
				*(buf+1)='\0';
			break;
		}
			buf++;
	}
	buf=buf-charac;//iterator wraca na swoje miejsce
	return charac;//i ponownie inspiracja UNIXowym read(); tym razem jednak zwrócona jest ilość znaków dozaczytania przez kolejną funkcję
}

int get_word(String buf, String* drop, int charac)
{
	*drop=(char*)calloc((charac+1),sizeof(char));
	strncpy(*drop, buf, charac);//zczytaj liczbę bajtów (podaną przez find_word) z bufora i zapisz w drop
	(*drop)[charac]='\0';//dla bezpieczeństwa, żeby nie wyszło w przypadku błędu
	int i=0,temp=0;
	for(;i<charac;i++)
	{
		if((*drop)[i]&0x80)//wykryj znak specjalny
		{
			i++;
			temp++;
		}
	}
	drop=NULL;//wskaźnika już nie potrzebujemy: agregat przejmuje obsługę, a miejsce w pamięci zostaje zwolnione w funkcji free_agregat
	charac-=temp;
	return charac;//charac jest więc teraz liczbą faktycznych znaków, a nie bajtów

}

void free_agregat(String* agregat, int word_count)
{
	if(word_count>0)//ciężko zwolnić coś, czego nie ma
	{
		for(;word_count>=0;--word_count)
		{
			free(agregat[word_count]);//po kolei, zwalniany element agregatu, zaczynając od najnowszego
		}
	}
	free(agregat);//zostało już tylko zwolnić sam agregat
	agregat=NULL;//nie chcemy przypadkiem użyć wskaźnika i nabrać śmieci
}

int find_line(String* source, String* line)//source musi być iteratorem
{
	String* agregat;
	int sum_w_spaces=0, word_len_w_space=1, word_count=0;
	if((agregat=(char**)calloc((LINE_LENGTH+1),sizeof(char*)))==NULL)//w najgorszym przypadku linia będzie składała się z (długość linii/2) jednoznakowych słów, każdym będącym literą specjalną (2 bajty)
	{
		fprintf(stderr, "Zabrakło pamięci!%s", endline);
		exit(1);
	}
	String* it_agregat=agregat;
	while(sum_w_spaces<(LINE_LENGTH))//warunek konieczny do zatrzymana pętli po zapełnieniu agregatu
	{
		if(**source=='\0')//kiedy napotka koniec buffora: zakończ funkcję i powiadomfunkcję wywołującą o końcu buffera
		{
			(*source)-=word_len_w_space;//usunięcie nadmiaru
			free_agregat(agregat, word_count);//zwolnienie miejsca
			it_agregat=NULL;
			return END_OF_BUFFER;
		}
		word_len_w_space=(find_word(*source)+1);//zapisz długość słowa ze spacją
		if((sum_w_spaces+word_len_w_space)<LINE_LENGTH&&word_len_w_space!=-1)//-1 w drugim warunku jako zabezpieczenie przed zapisem podwójnej spacji oznaczanej jako puste słowo
		{
			word_len_w_space=get_word(*source, it_agregat, word_len_w_space-1)+1;//alokacja i zapisanie słowa tylko po upewnieniu się, że jest na nie miejsce w agregacie
			it_agregat++;//it_agregat i word_count sterują agregatem, więc one też są zależne od warunku
			word_count++;
		}
		if(word_len_w_space!=-1)//podwójna spacja ignorowana przez licznik znaków
		{
			sum_w_spaces+=word_len_w_space;
		}
		if(sum_w_spaces<LINE_LENGTH)
			*source+=(abs(word_len_w_space));//właśnie ze względu na tą linię, kod błędu podwójnej spacji to -2
	}
	sum_w_spaces-=word_len_w_space;//nadmiar już nie potrzebny
	it_agregat=agregat;//powrót iteratora agregatu na początek
	int difference=LINE_LENGTH-(sum_w_spaces-1);//ilość dodatkowych spacji (-1 ze względu na brak spacji po ostatnim słowie)
	int additional_spaces=difference;
	if(word_count>1)
		additional_spaces=difference%(word_count-1);
	int spaces_for_every=0;
	if(word_count!=1)
		spaces_for_every=(difference-additional_spaces)/(word_count-1);
	int word_nr, addit;
	memset(*line, 0, strlen(*line));//czyszczenie linii
	for(word_nr=0;word_nr<(word_count-1);word_nr++)
	{
		strcat(*line, *it_agregat);//wpisz słowo
		strcat(*line, " ");//wpisz standardową spację
		for(addit=0;addit<spaces_for_every;addit++)//wpisz dodatkowe spacje
		{
			strcat(*line, " ");
		}
		if(additional_spaces>0)//jeśli została jakaś reszta z dzielenia, to daj jedną dodatkową spację
		{
			strcat(*line, " ");
			additional_spaces--;
		}
		it_agregat++;
	}
	strcat(*line, *it_agregat);//ostatnie słowo, po którym nie ma spacji
	free_agregat(agregat, word_count);//wyczyść pamięć
	it_agregat=NULL;
	return word_count;//zwróć ilość zaczytanych słów
}

int save_to_file(FILE* desc_save, String* dane, String* line)//dane to iterator!
{
	int remaining, nread;
	while((nread=find_line((dane), line))!=END_OF_BUFFER)//dopuki find_line nie powiadomi o końcu buffera
	{
		if(nread>0)//jeżeli jakiekolwiek słowa zostały zapisane
			fprintf(desc_save, "%s%s", *line, endline);//zapisz linię do pliku
		remaining=strlen(*dane);//wyznacz pozostałą długość buffera
	}
	return remaining;//zwróć pozostałą długość buffera
}

int justuj_na_koniec(String* source, String* line)//source musi być iteratorem; zmodyfikowana funkcja find_line
{

	String* agregat;
	int sum_w_spaces=0, word_len_w_space=1, word_count=0;
	if((agregat=(char**)calloc((LINE_LENGTH+1),sizeof(char*)))==NULL)//w najgorszym przypadku linia będzie składała się z (długość linii/2) jednoznakowych słów, każdym będącym literą specjalną (2 bajty)
	{
		fprintf(stderr, "Zabrakło pamięci!%s", endline);
		exit(1);
	}
	String* it_agregat=agregat;
	while(**source!='\0')//tym razem mamy pewność, że ilość znaków jest mniejsza, niż długość agregatu, należy tylko sprawdzać, czy nie dotarliśmy do końcu bufora (pliku)
	{
		word_len_w_space=(find_word(*source)+1);//zapisz długość słowa ze spacją
		if(word_len_w_space!=-1)//-1 w drugim warunku jako zabezpieczenie przed zapisem podwójnej spacji oznaczanej jako puste słowo
		{

			word_len_w_space=get_word(*source, it_agregat, word_len_w_space-1)+1;//alokacja i zapisanie słowa tylko po upewnieniu się, że jest na nie miejsce w agregacie
			it_agregat++;//it_agregat i word_count sterują agregatem, więc one też są zależne od warunku
			word_count++;

		}
		if(word_len_w_space!=-1)//nadal ignorujemy podwójne spacje
		{
			sum_w_spaces+=word_len_w_space;
		}
		*source+=(abs(word_len_w_space));//właśnie ze względu na tą linię, kod błędu podwójnej spacji to -2
	}
	it_agregat=agregat;//powrót iteratora agregatu na początek
	int difference=LINE_LENGTH-(sum_w_spaces-1);//ilość dodatkowych spacji (-1 ze względu na brak spacji po ostatnim słowie)
	int additional_spaces=difference;//jak w find_line, z jedną różnicą, że nie ma nadmiaru, który wcześniej trzeba usunąć
	if(word_count>1)
		additional_spaces=difference%(word_count-1);
	int spaces_for_every=0;
	if(word_count!=1)
		spaces_for_every=(difference-additional_spaces)/(word_count-1);
	int word_nr, addit;
	memset(*line, 0, strlen(*line));
	for(word_nr=0;word_nr<(word_count-1);word_nr++)
	{
		strcat(*line, *it_agregat);
		strcat(*line, " ");
		for(addit=0;addit<spaces_for_every;addit++)
		{
			strcat(*line, " ");
		}
		if(additional_spaces>0)
		{
			strcat(*line, " ");
			additional_spaces--;
		}
		it_agregat++;
	}
	strcat(*line, *it_agregat);
	free_agregat(agregat, word_count);
	it_agregat=NULL;
	return word_count;
}

int main()
{
	char buffer[BUF_SIZE];//inicjacja ciągów związanych z bufferem
	buffer[0]='\0';
	String dane=(char*)calloc(LINE_LENGTH*2+BUF_SIZE+1,sizeof(char));
	dane[0]='\0';
	String danetemp=(char*)calloc(BUF_SIZE+1, sizeof(char));
	danetemp[0]='\0';

	ask_for_file('r');//pozwól użytkownikowi wybrać czytany plik
	FILE* desc;
	desc=fopen(originPath, "r");//otwarcie pliku źródłowego
	if(desc==NULL)
	{
		printf("Plik nie istnieje!\n");//tak łatwo o literówkę...
		getchar();
		exit(1);
	}
	int nread, nbuff=1, nbuffcp;
	nbuff=get_bite(desc, buffer);//ilość znaków z bufora
	nread=nbuff;//ilość dostępnych znaków
	if(nbuff>0)
		strcpy(dane, buffer);
	else
	{
		printf("Pusty plik!%s", endline);//w razie jakby plik istniał, ale był pusty
		getchar();
		exit(1);
	}
	String line=(char*)calloc((LINE_LENGTH*2+1),sizeof(char));
	char *it_dane=dane;
	ask_for_file('w');//pozwól użytkownikowi wybrać lokalizację docelową
	FILE* desc_save;
	desc_save=fopen(savePath, "w");	
	int charac=0;
	while(nread>0&&nbuff!=0)
	{
		nread=save_to_file(desc_save, &it_dane, &line);//zapisz bufor do pliku i zapisz liczbę pozostałych bajtów
		if(nread!=80&&nbuff<BUF_SIZE-1&&it_dane[0]=='\0')//zawsze może się trafić plik, który zajmie tylko jedną linijkę
		{
			it_dane=dane;
			goto ending;//potraktuj jak ostatnią linijkę
		}
		it_dane=dane+(strlen(dane)-nread);//przesuń iterator na pierwsze miejsce pozostałego bufora
		if(it_dane[0]!='\0')//jeśli coś zostało
		{
			memset(danetemp, 0, strlen(danetemp));//wyczyść tymczasowy bufor
			strcat(danetemp, it_dane);//przenieś pozostałość bufora do tymczasowego bufora
		}

		nbuff=get_bite(desc, buffer);//pobierz do bufora wejściowego kolejny fragment pliku
		if(dane[0]!='\0')
			memset(dane,0,strlen(dane-1));//wyczyść główny bufor
		strcpy(dane, danetemp);//przenieś zawartość bufora tymczasowego
		strcat(dane, buffer);//przenieś zawartość bufora wejściowego
		strcat(dane, "\0");//upewnij się, że całość jest traktowana jako string
		nread+=nbuff;//zaktualizuj licznik bufora głównego
		it_dane=dane;//iterator wraca na początek głównego bufora
		
		}
ending:
	if(justuj_na_koniec(&it_dane, &line)!=EMPTY_LINE)//jeśli coś zostało 
	{
		fprintf(desc_save, "%s%s", line, endline);//zapisz w pliku ostatnią linijkę
	}
	free(dane);//ostateczne zwolnienie heap, wskaźników i deskryptorów
	dane=NULL;
	free(danetemp);
	danetemp=NULL;
	free(line);
	line=NULL;
	it_dane=NULL;
	fclose(desc_save);
	fclose(desc);
	exit(0);
}
