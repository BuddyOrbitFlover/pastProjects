#ifndef RUNNINGGAME_H_ // if schon defined
#define RUNNINGGAME_H_ // ansonsten mache...

extern int MAXIMUM;
extern int BOARD_SIZE;
//extern char gameBoard[8][8];

void writeServerIngame(int socketreturn, char buffer[MAXIMUM]);
int runningGame(int socketreturn);
void readServerIngame(int socketreturn);
void initializeGameBoard();
void processBoardLine(char* line, char gameBoard[BOARD_SIZE][BOARD_SIZE]);
void printGameBoard(int boardSize, char gameBoard[BOARD_SIZE][BOARD_SIZE]);
void initSHM(key_t shm_key);

#endif /*_RUNNINGGAME_H_ */