#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "config.h"
#define MAX_LINE_LENGTH 100

// struct Config {
//     char hostname[MAX_LINE_LENGTH];
//     uint16_t portNumber;
//     char gameKindName[MAX_LINE_LENGTH];
// };

int caseInsensitiveCompare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (tolower(*str1) != tolower(*str2)) {
            return 0; // Not equal
        }
        str1++;
        str2++;
    }
    return (1); 
}

char* trimWhitespace(char *str) {
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }
    if (*str == '\0') {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

void readConfigFile(const char *filename, struct ConfigParam *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        char *trimmedLine = trimWhitespace(line);
        char paramName[MAX_LINE_LENGTH];
        char paramValue[MAX_LINE_LENGTH];

        if (sscanf(trimmedLine, "%[^=] = %[^\n]", paramName, paramValue) == 2) {

            // Trim leading and trailing whitespace from paramValue
            char *trimmedParamValue = trimWhitespace(paramValue);

            // Case-insensitive comparison
            if (caseInsensitiveCompare(paramName, "Hostname")) {
                strncpy(config->hostname, trimmedParamValue, sizeof(config->hostname) - 1);
                //config->hostname[sizeof(config->hostname) - 1] = '\0';  // Ensure null-termination
            } else if (caseInsensitiveCompare(paramName, "PortNumber")) {
                config->portNumber = htons(atoi(trimmedParamValue));
                printf("DUIFHSDUI: %s", trimmedParamValue);
            } else if (caseInsensitiveCompare(paramName, "GameKindName")) {
                strncpy(config->gameKindName, trimmedParamValue, sizeof(config->gameKindName) - 1);
                config->gameKindName[sizeof(config->gameKindName) - 1] = '\0';  
            }
        } else {
            printf("Invalid line format: %s\n", trimmedLine); 
        }
    }

    fclose(file);
}

/*int main() {
    struct ConfigParam myConfig;

    readConfigFile("config.txt", &myConfig);
    printf("Hostname: %s\n", myConfig.hostname);
    printf("PortNumber: %d\n", ntohs(myConfig.portNumber));
    printf("GameKindName: %s\n", myConfig.gameKindName);

    return 0;
}*/
