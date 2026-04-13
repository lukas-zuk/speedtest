#ifndef BEST_SRV 
#define BEST_SRV

#include <curl/curl.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "json_utils.h"

double testLatency(const char *host);
void findBestServerByCountry(const char *country,char *result, Server *serverList, size_t serverList_count);
void findAllServersForCountry(const char* country, char ***hostList, size_t *hostList_count, const Server *serverList, size_t serverList_count);
void serverPickDialogue(char *result,char **hostList, size_t hostList_count);
int serverExists(char *server, const Server *serverList, size_t serverList_count);


#endif
