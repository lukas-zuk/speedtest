#ifndef JSON_UTILS
#define JSON_UTILS

#include <cjson/cJSON.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    char country[64];
    char city[64];
    char provider[64];
    char host[64];
} Server;

void copy_json_string(cJSON *source, char *dest, size_t dest_size);
int readJsonToArray(const char *filename, Server **serverList, size_t *serverList_count);
void lookUpCountryAbbreviation(char *code, size_t size);
int updateCountry(char *country,size_t size);


#endif