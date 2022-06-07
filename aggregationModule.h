void createAggregation();
void setPositionInAggregation();
void fillAggregation();

int isLineIncomplete();
int isThereABitLeft();

void addWordToAggregation();
int findNextWord();
int didTheWordEnd();
int finishWordCreation(int);
//TODO: skipped securing buffer on word level; find more a way to do it on line level

int isThereOverflow(int);
int isWordEmpty(int);
void handleAbnormalWord(int);

void updateAggregation(int);
char** createWord(int);//not sure about this one; will need testing
void updateWordLenght(int);
int isItUtf8(char);
