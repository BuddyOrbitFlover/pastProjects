#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>



#include "shared.h"

#define MAXIMUM 80
char buffer[MAXIMUM];
struct SharedMemoryData* shared_Data;

int getBoardSize(char buffer[]){                                                            
    char* boardPtr;
    int boardSize;
    boardPtr = strstr(buffer, "BOARD ");
    if (boardPtr != NULL) {
        // Verschiebe den Zeiger, um auf die erste Zahl zu zeigen
        boardPtr += strlen("BOARD ");
        // Lese die Zahl in einen Integer
        sscanf(boardPtr, "%d", &boardSize);
    }
    return boardSize;
}

void clearBoard(int boardSize, char gameBoard[][boardSize]) { //BOARD leeren
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            gameBoard[i][j] = '*'; // '*' könnte für ein leeres Feld stehen
        }
    }
}


void fillBoard(char buffer[], int lineNr) {
    char *token;
    int column = 0;
    // Zerlegen der Zeile in Tokens
    token = strtok(buffer, " ");
    while (token != NULL) {
        // Überprüfen, ob das Token ein Schachfigur-Symbol ist
        if (strcmp(token, "*") == 0 || strcmp(token, "b") == 0 || strcmp(token, "w") == 0) {
            shared_Data->gameBoard[lineNr][column] = token[0];
            column++;
        }
        token = strtok(NULL, " ");
    }
}

void printGameBoard() {
    printf("   A B C D E F G H\n");
    printf(" +-----------------+\n");
    for (int i = 0; i < shared_Data->boardSize; i++) {
        for (int j = 0; j < shared_Data->boardSize; j++) {
            printf("%c ", shared_Data->gameBoard[i][j]);
        }
        printf("\n");
    }
    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n");
}


char* readServerIngame(int socketreturn) {                                                                  //read Server, return String (char-Array)
    char receivedBuffer;
    memset(buffer, 0, sizeof(buffer));                                      // Speicherbereich von buffer auf Null setzen
    while (read(socketreturn, &receivedBuffer, 1) >= 0){                    // wie normales read(), nur wird hier Zeichenweise gelesen und zum Endstring angefügt -> hält die Ausgabenreihenfolge stabil (nicht mixed)
        if (receivedBuffer == '\n')
            break;
        strncat(buffer, &receivedBuffer, 1); 
    } 
    printf("S: %s\n", buffer); 
    return buffer;
    
}

void writeServerIngame(int socketreturn, char buffer[MAXIMUM]){
    printf("Buffer:%s\n", buffer);                                             // write to Socket
    ssize_t bytes_written = write(socketreturn, buffer, strlen(buffer));
    if (bytes_written == -1){                                               // Fehlerbehandlung für write to socket
        perror("Error writing to socket\n");
        memset(buffer, 0, MAXIMUM);
        exit(0);
    }
}

int runningGame(int socketreturn){
    strcpy(buffer, readServerIngame(socketreturn));                                                         //read stoppt das Programm bis es etwas returnen kann

    if (strstr(buffer, "GAMEOVER") != NULL) {                                                               //Server: GAMEOVER
        printf("Server sendet GAMEOVER\n");
        readServerIngame(socketreturn);
        for (int i=0; i<shared_Data->boardSize; i++){                                                                    //board wird befüllt.           
            strcpy(buffer, readServerIngame(socketreturn));                                                    //i = LineNumber                                             
            fillBoard(buffer, i);         
        }
        printGameBoard();
        for(int i=0;i<3;i++){
            readServerIngame(socketreturn);
        }
        return 0;
    } else if (strstr(buffer, "WAIT") != NULL) {      
        printf("Server sendet WAIT\n");                                                                     //Server: WAIT
        sleep(2);
        sprintf(buffer, "OKWAIT\n");
        writeServerIngame(socketreturn, buffer);
        return 1;
    } else if (strstr(buffer, "MOVE") != NULL && strstr(buffer, "OK") == NULL) {                                                            //Server: BOARD
        shared_Data->boardSize = getBoardSize(readServerIngame(socketreturn));               
        for (int i=0; i<shared_Data->boardSize; i++){                                                       //board wird befüllt.           
            strcpy(buffer, readServerIngame(socketreturn));                                                 
            fillBoard(buffer, i);                             //i = Linenumber
        }     
        shared_Data->boardSize = shared_Data->boardSize;
        printGameBoard();
        return 2; 
    } else if (strstr(buffer, "ENDBOARD") != NULL) {                                                         //Server: ENDBOARD
        printf("Server sendet ENDBOARD\n");
        sprintf(buffer, "THINKING\n");
        writeServerIngame(socketreturn, buffer);
        return 1;
    } else if (strstr(buffer, "OKTHINK") != NULL) {                                                         //Server: OKTHINK
        printf("Server sendet OKTHINK\n");
        return 4;
    } else if (strstr(buffer, "MOVEOK") != NULL) {                                                         //Server: MOVEOK
        printf("Server sendet MOVEOK\n");
        return 1;
    }
    return -1;
}