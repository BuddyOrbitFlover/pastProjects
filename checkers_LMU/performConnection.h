#ifndef PERFORMCONNECTION_H_ // if schon defined
#define PERFORMCONNECTION_H_ // ansonsten mache...

extern int MAXIMUM;

void readServer(int socketreturn);
void performConnection(int socketreturn, char gameid[], int playerid);
void writeServer(int socketreturn, char buffer[MAXIMUM]);
void gameInformationExchange(int socketreturn);

#endif /*_PERFORMCONNECTION_H_ */