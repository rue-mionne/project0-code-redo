extern char* line;
extern int lineLenght;

typedef enum {LINE_INCOMPLETE}lineStatus;

void createLine(char**);
void initiateLineCreation(char**);
void createAggregation();
void setPositionInAggregation();
void fillAggregation();
int isLineIncomplete();
int isThereABitLeft();
void addWordToAggregation();
void finishLineCreation(lineStatus);
void freeAggregation();
int areWordsInAggregation();
void freeWords();
lineStatus getLineStatus();
