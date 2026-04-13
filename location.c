#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "json_utils.h"

static size_t write_cb(void *ptr, size_t size, size_t nmemb, char *userdata); 

int getLocation(char *location, size_t location_size)
{
    CURL *curl;
    curl = curl_easy_init();

    if(!curl)
    {
        fprintf(stderr,"curl init failed \n");
        return -1;
    }

    char buffer[1024] = {0};
    
    curl_easy_setopt(curl,CURLOPT_URL,"http://ip-api.com/json");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Speedtest-Client");
    
    
    CURLcode err = curl_easy_perform(curl);

    if(err != CURLE_OK) 
    {
        fprintf(stderr, "Location error: %s\n", curl_easy_strerror(err));
        return -1;
    }
    
    long http_code = 0;
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code != 200)
    {
        fprintf(stderr, "Location error: %s\n", curl_easy_strerror(err));
        return -1;
    }
    
    cJSON *json = cJSON_Parse(buffer);
    cJSON *country = cJSON_GetObjectItemCaseSensitive(json,"country");
    copy_json_string(country,location,location_size);
    curl_easy_cleanup(curl);
    return 0;
}

static size_t write_cb(void *ptr, size_t size, size_t nmemb, char *userdata) 
{
    strncat(userdata, (char *)ptr, size * nmemb);
    return size * nmemb;
}
