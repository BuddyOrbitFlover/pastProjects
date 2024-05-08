// config.h
#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>

struct ConfigParam {
    char hostname[255];
    uint16_t portNumber;
    char gameKindName[255];
};

void readConfigFile(const char *filename, struct ConfigParam *config);
#endif // CONFIG_H