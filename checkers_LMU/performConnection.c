#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

#define MAXIMUM 80
#define CLIENTVERSION '3'
#define GAMEKINDNAME "Checkers"
#define BOARD_SIZE '8'


char Sessionname[MAXIMUM] = "";
int readServer_count = 0;
char buffer[MAXIMUM];
int player_count = 1000;

void readServer(int socketreturn) {                                          // is used to read and process information written by the server. 
    char *hit;
    char receivedBuffer;
    char StringArray[4][20];                                                // String array [array length][word length]
    int j = 0;
    readServer_count++;                                                     // readServer_count increments by 1 every time readServer() is called. This counts how many times the server is read.
    memset(buffer, 0, sizeof(buffer));                                      // Speicherbereich von buffer auf Null setzen
    while (read(socketreturn, &receivedBuffer, 1) >= 0){                    // wie normales read(), nur wird hier Zeichenweise gelesen und zum Endstring angefügt -> hält die Ausgabenreihenfolge stabil (nicht mixed)
        if (receivedBuffer == '\n')
            break;
        strncat(buffer, &receivedBuffer, 1); 
    } 
    printf("S: %s\n", buffer);                                            //Server Output print


    switch (readServer_count){                                             // depending on exactly how often readServer() is called uppon, this switch goes into different cases. If it has not been called exactly 3, 4, or 7 times, the default case is used.
    case 4:                                                                 // checks if the correct game kind (GAMEKINDNAME) is chosen
        if (strstr(buffer, "PLAYING") == NULL){ 
            perror("Server weist GameID zurück");
            exit(0);
        }
        if (strstr(buffer, GAMEKINDNAME) == NULL){ 
            perror("Wrong game is chosen!");
            exit(0);
        }
        break;

    case 5:                                                                 // Sessionname gets saved and printed out.
       hit = strtok(buffer, " ");
        while (hit != NULL){                                                // split the buffer where spaces(" ") are found. 
            strcpy(StringArray[j], hit);
            hit = strtok(NULL, " ");
            j++;
        }
        strcpy(Sessionname, StringArray[1]);
        printf("Game \"%s\" found. Joining initialized...\n\n", Sessionname);
        break; 

    case 6:
        if (strstr(buffer, "No free player") != NULL){
            perror("Game is full, or the desired seat is already occupied! Try to join other games.");
            exit(0);
        }                                                                   // output players own name and number
        hit = strtok(buffer, " ");
        j = 0;
        while (hit != NULL){                                                // split the buffer where spaces(" ") are found. 
            strcpy(StringArray[j], hit);
            hit = strtok(NULL, " ");
            j++;
        }
        printf("You      Name: %s     Player-Number:%s\n", StringArray[3], StringArray[2]);
        break;

    case 7:
        hit = strtok(buffer, " ");
        j = 0;
        while (hit != NULL){                                                // split the buffer where spaces(" ") are found. 
            strcpy(StringArray[j], hit);
            hit = strtok(NULL, " ");
            j++;
        }
        player_count = atoi(StringArray[2]);
        printf("Spieleranzahl: %i \n", atoi(StringArray[2]));
        break;

    case 8:                                                                 // Output opponent. For example: "Player 1 (Uli) is not ready" 
        hit = strtok(buffer, " ");
        j = 0;
        while (hit != NULL){                                                // split the buffer where spaces(" ") are found. 
            strcpy(StringArray[j], hit);
            hit = strtok(NULL, " ");
            j++;
        }

        if (atoi(StringArray[3]) == 0){                                     // Ausgabe der Angaben über den anderen Spieler
            printf("Opponent Name: %s     Player-Number:%s          Status: not ready yet\n",  StringArray[2], StringArray[1]);
        }
        else{
            printf("Opponent Name: %s     Player-Number:%s          Status: ready\n",  StringArray[2], StringArray[1]);
        }
        break;

    default:
        break;
    }

    memset(buffer, 0, sizeof(buffer)); // Speicherbereich von buffer auf Null setzen
}

void writeServer(int socketreturn, char buffer[MAXIMUM]){                   // write to Socket
    ssize_t bytes_written = write(socketreturn, buffer, strlen(buffer));
    if (bytes_written == -1){                                               // Fehlerbehandlung für write to socket
        perror("Error writing to socket\n");
        memset(buffer, 0, MAXIMUM);
        exit(0);
    }
}

void performConnection(int socketreturn, char gameid[], int playerid){
    readServer(socketreturn);                                                                      //read GameServer version

    readServer(socketreturn);                                                                      // read Happy New Year
    // memset(buffer, 0, sizeof(buffer));                                                          // Client Version schreiben
    sprintf(buffer, "VERSION %c\n", CLIENTVERSION);                                                // writes client version into buffer
    writeServer(socketreturn, buffer);                                                             // calls writeServer() in order to write the client version to the server


    readServer(socketreturn);  // read the server for Game-ID ds

    // memset(buffer, 0, sizeof(buffer));                                                          // Game-ID schreiben
    sprintf(buffer, "ID %s\n", gameid);                                                            // same concept as in lines 132-135
    writeServer(socketreturn, buffer);

    readServer(socketreturn);                                                                       // read for GameKindName from Server ---> readServer_count = 3
    readServer(socketreturn);                                                                       // read for GameName (SessionName) ---> readServer_count = 4


    // memset(buffer, 0, sizeof(buffer));                                                          // Player-ID
    if(playerid == 1 || playerid == 2){                                                            // use given Player-ID
        sprintf(buffer, "PLAYER %i\n", playerid-1);
    }else{
        strcpy(buffer, "PLAYER\n");                                                                 //take a free seat
        printf("Playernumber undefined, selecting free seat:\n\n");
    }                                                                  
    writeServer(socketreturn, buffer);


    for (int i = 0; i < 4; i++){
        readServer(socketreturn);
    } 
    return;  
    }                                                                                            //Prolog-Phase endet hier!!
