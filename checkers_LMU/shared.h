#ifndef SHARED_H
#define SHARED_H

#define BUF_SIZE 1024
#define MAX_BOARD_SIZE 100 // Maximalgröße des Spielbretts
 
struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};

struct PlayerInfo {
    int playerNumber;
    char playerName[10];                                                           // Angenommene maximale Länge des Namens
    int isRegistered;                                                               // 0 für nicht registriert, 1 für registriert
};

struct SharedMemoryData {
        char *gameName;
        int playerNumber;
        int numPlayers;
        pid_t processIDs[2];
        struct PlayerInfo players[10];                                             // player_count erstmal 1000 sicherheitshalber
        char gameBoard[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
        int boardSize;
        int thinkerFlag;
        
};

#endif
