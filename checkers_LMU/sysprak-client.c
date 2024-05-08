#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "config.h"
#include "shared.h"
#include "performConnection.h"
#include "runningGame.h"

#define HOSTNAME "sysprak.priv.lab.nm.ifi.lmu.de"
#define PORTNUMBER "1357"
struct SharedMemoryData* shared_Data;
int pipe_fd[2]; //0 for reading (connector/child), 1 for writing (thinker/parent)


void getStoneValue(){

}


void think(int singal){                                                           //Signal-Handler for SIGUSR1
    printf("Im Thinker\n");
    if (shared_Data->thinkerFlag == 1) {
        switch(shared_Data->playerNumber){
            case 0:
                for(int i=0; i<shared_Data->boardSize; i++){
                    for(int j=0; j<shared_Data->boardSize; j++){
                        if(shared_Data->gameBoard[i][j] == 'w'){
                            getStoneValue();
                            printf("weißer stein gefunden\n");

                        }
                    }
                }
                break;

            case 1: 
                for(int i=0; i<shared_Data->boardSize; i++){
                    for(int j=0;j<shared_Data->boardSize; j++){
                        if(shared_Data->gameBoard[i][j] == 'b'){
                            getStoneValue();
                            printf("schwarzer stein gefunden\n");
                        }
                    }
                }
                break;
        }

        const char *moveB = "PLAY A5:C3\n";                                             //Übungsblatt: white A3:B4, black B6:A5
        printf("%s\n", moveB);
        write (pipe_fd[1], moveB, strlen(moveB)+1);
        shared_Data->thinkerFlag = 0;
    }
}

int main(int argc, char *argv[]){
    char gameid[13];
    int playerid = 100;  
    char *configFileName = "client.conf";
    int option;
    pid_t pid; //child process ID oder?
    //extern int player_count;

    int socketcreation;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *rp;

    // get starting arguments
    while ((option = getopt(argc, argv, "g:p:c:")) != -1){
        switch (option){
        case 'g': // in case option was inputted as g
            if (strcmp(optarg, "") == 0 || strlen(optarg) != 13)                    { //  Abfangen für keine Eingabe oder Eingabe nicht gleich 13 Stellen
                perror("Invalid game ID. Valid game ID: 13-characters.\n");
                return 1;
            }
            strcpy(gameid, optarg); 
            break;
        case 'p': // in case option was inputted as p
            playerid = atoi(optarg);
            if (playerid != 1 && playerid != 2){
                perror("Invalid number of players. Valid player ID: 1 or 2.\n");
                return 1;
            }
            break;
        case 'c':
            configFileName = optarg;
            break;
        default:
            perror("Invalid options. Valid options: ./sysprak-client -g <GAME-ID> -p <{1,2}> -c <configFileName.conf>\n");
            abort();
        }
    }
    // Server connection...
    memset(&hints, 0, sizeof(hints)); // clear the area 
    hints.ai_family = AF_UNSPEC;      // IPv4 and IPv6
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    // CONFIG STUFF
    struct ConfigParam myConfig;  
    readConfigFile(configFileName, &myConfig);

    // printf("CONFIG: %s\n", configFileName);
    // printf("Hostname: %s\n", myConfig.hostname);
    // printf("PortNumber: %d\n", ntohs(myConfig.portNumber));
    // printf("GameKindName: %s\n", myConfig.gameKindName);
    
    
    
    char portString[6];                                                                                         // Groß genug für alle Portnummern (0 bis 65535) plus Nullterminator
    snprintf(portString, sizeof(portString), "%u", ntohs(myConfig.portNumber));

    // printf("TEST 1:%s\n", portString);
    // printf("TEST 2:%s\n", myConfig.hostname);

    if (getaddrinfo(myConfig.hostname, portString, &hints, &res) != 0){                                        // Hostname into Data || return 0 = success | Data will be in the adress of res in case of success.
        perror("Fehler bei Auswertung der Server-Adresse");
        return -1;
    }

    for (rp = res; rp != NULL; rp = rp->ai_next){   
        printf("anz for\n");
        socketcreation = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socketcreation == -1){                                                                              // create Socket
            perror("socket creation failed.");                                                                  // continue if socket creation failed
            continue;
        }

        if (connect(socketcreation, rp->ai_addr, rp->ai_addrlen) == -1){                                        // try to connect with given data
            perror("connection attempt failed.");                                                               // continue if the connection attempt failed
            close(socketcreation);
            continue;
        }

        key_t shm_key = getpid();
        int shm_ID = shmget(shm_key, sizeof(struct SharedMemoryData), 0644|IPC_CREAT);
        if (shm_ID == -1) {
            perror("Shared memory");
            return 1;
        }
        shared_Data = shmat(shm_ID, NULL, 0);
        if (shared_Data == (void *) -1) {
            perror("Shared memory attach");
            return 1;
        }

        //Unnamed pipe
        shared_Data->thinkerFlag = 0;      //thinkerFlag gets initialized as 0                //make sure it isnt 0 or 1

        if (pipe(pipe_fd) == -1){
            perror("Error when trying to create pipe\n");
            return -1;
        }

        //Fork the process
        pid = fork();                                                                                       //create Child-Process
        if (pid < 0){
            perror("Fork fehlgeschlagen");
            return -1;
        }else if(pid == 0){                                                                                 //Child-Process / Connector
            printf("Im Kind-Prozess (Connector)\n");
            close(pipe_fd[1]); //close write end

            //TODO wrap into if: depending on return value(?): do sth

            performConnection(socketcreation, gameid, playerid);                                            // the seperate performConnection() function is called uppon if socket creation and connect are successful
            printf("Prolog was successful. The Game will start now!\n");

            int runningReturn;
            char buffer[100];
            for(;;){
                printf("\n\n\nGame still running Loop:\n");
                runningReturn = runningGame(socketcreation);

                switch (runningReturn){
                case 0:                                                         //GameOver
                    printf("Game is over...\n");
                    break;
                case 1:                                                         //Wait
                    printf("\"Wait\" has been answered\n");
                    continue;
                case 2:                                                           //Move
                    printf("Spielfeld wurde in Shared gelegt\n");
                    shared_Data->thinkerFlag = 1;
                    kill(getppid(), SIGUSR1);                                   //give the call to "Thinker"                                
                    continue;
                case 4:
                    //Read from pipe
                    read(pipe_fd[0], buffer, sizeof(buffer));           //wait till the move is in the pipe
                    writeServerIngame(socketcreation, buffer);
                    continue;
                case -1: 
                    printf("Unexpected Message! Terminating Programm...\n");
                    break;
                }
                break;
            }

            shmdt((void *)shared_Data);                                                                         //clear shared memory from child-process
            exit(1);
        }else{                                                                                              //inParent-Process / Thinker                                                           
            printf("Eltern / Thinker\n");
            close(pipe_fd[0]); //close read end

            printf("Server gave ok to start thinking\nStart thinking...\n");
            signal(SIGUSR1, think);                                                                       //Signal-Handler for SIGUSR1
 
            wait(NULL);                                                                                     //wait till Child is finished.
            shmdt((void *)shared_Data);                                                                         //clear shared memory from child-process
            shmctl(shm_ID, IPC_RMID, NULL);                                                                  //delete shared emory
        }
        
        break;
    }
    printf("\n\nProgram ends herdfddder!");
    freeaddrinfo(res);
    return 0;
}
