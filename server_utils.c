#include <curl/curl.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "json_utils.h"

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata);

double testLatency(const char *host)
{
    const char *prefix = "http://";
    const char *suffix = "/speedtest/latency.txt";

    CURL *curl;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Speedtest-Client");

    size_t urlLength = strlen(prefix) + strlen(host) + strlen(suffix) + 1;
    char url[urlLength];
    
    strcpy(url,prefix);
    strcat(url,host);
    strcat(url,suffix);
   
    curl_easy_setopt(curl, CURLOPT_URL, url);

    double total_time = 0;
    CURLcode err = curl_easy_perform(curl);

    if(err != CURLE_OK)
    {
        fprintf(stderr,"latency get for host %s failed: %s\n",host,curl_easy_strerror(err));
        curl_easy_cleanup(curl);
        return -1;
    }

    err = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
    if(err != CURLE_OK)
    {
        fprintf(stderr,"getinfo failed: %s\n",curl_easy_strerror(err));
        curl_easy_cleanup(curl);
        return -1;
    }

    return total_time;
}

void findBestServerByCountry(const char *country,char *result, const Server *serverList, size_t serverList_count)
{
    double minlat = 999999.0;
    int minidx = -1;

    for(size_t i = 0; i < serverList_count; i++)
    {
        
        if(strcasecmp(serverList[i].country, country) == 0)
        {
            double lat = testLatency(serverList[i].host);
            if(lat < minlat && lat != -1)
            {
                minlat = lat;
                minidx = i;
            }
        }
    }
    
    if(minidx != -1)
        strcpy(result,serverList[minidx].host);
    else 
        result[0] = '\0';
}
void findAllServersForCountry(const char* country, char ***hostList, size_t *hostList_count, const Server *serverList, size_t serverList_count)
{
    int matches = 0;
    for(size_t i = 0; i < serverList_count;i++)
        if(strcasecmp(serverList[i].country,country) == 0)
            matches++;


    if(matches == 0)
        return;

    *hostList = malloc(matches * sizeof(char *));
    if (!(*hostList)) 
    {
        perror("malloc failed");
        return;
    }

    int index = 0;

    for(size_t i = 0; i < serverList_count;i++)
    {
        if(strcasecmp(serverList[i].country,country) == 0)
        {
            (*hostList)[index] = strdup(serverList[i].host);
            index++;
        }
    }
    
    *hostList_count = index;
}
void serverPickDialogue(char *result,char **hostList, size_t hostList_count)
{
    printf("Server list:\n");
    for (size_t i = 0; i < hostList_count;i++)
    {
        printf("%ld. %s\n",i+1,hostList[i]);
    }
    size_t num;
    while(1)
    {
        printf("Choose a server from the list: (1 - %ld): ",hostList_count);
        
        if (scanf("%ld", &num) != 1)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        
        if(num > 0 && num <= hostList_count)
        {
            strcpy(result,hostList[num-1]);
            printf("\nUsing server: %s\n", hostList[num-1]);
            break;
        }
    } 
}
int serverExists(char *server, const Server *serverList, size_t serverList_count)
{
    for (size_t i = 0; i < serverList_count; i++)
    {
        if(strcasecmp(serverList[i].host,server) == 0)
        {
            return 1;
        }
    }
    return 0;
}

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) 
{
    (void)ptr;
    (void)userdata;
    return size * nmemb;
}