#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata);

double testUpload(const char* host)
{
    const char *prefix = "http://";
    const char *suffix = "/speedtest/upload.php";
    CURL *curl;
    curl_off_t speed;

    curl = curl_easy_init();

    if(!curl)
    {
        fprintf(stderr,"curl init failed \n");
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Speedtest-Client");

    size_t urlLength = strlen(prefix) + strlen(host) + strlen(suffix) + 1;
    char url[urlLength];
    
    strcpy(url,prefix);
    strcat(url,host);
    strcat(url,suffix);
   
    curl_easy_setopt(curl, CURLOPT_URL, url);

    size_t uploadSize = 1024 * 1024 * 5;
    char *uploadData = malloc(uploadSize);
    if (!uploadData) 
    {
        perror("malloc failed");
        return -1;
    }
    memset(uploadData, 'x', uploadSize);

    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, uploadSize);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,uploadData);
    CURLcode err = curl_easy_perform(curl);
    free(uploadData);

    if(err != CURLE_OK) 
    {
        fprintf(stderr, "upload failed: %s\n", curl_easy_strerror(err));
        curl_easy_cleanup(curl);
        return -1;
    }    
    err = curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &speed);
    if(err != CURLE_OK) 
    {
        fprintf(stderr, "getinfo failed: %s\n", curl_easy_strerror(err));
        curl_easy_cleanup(curl);
        return -1;
    }
    
    curl_easy_cleanup(curl);

    double speed_mbit = (speed*8.0) / 1000000.0;

    return speed_mbit;
}

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) 
{
    (void)ptr;
    (void)userdata;
    return size * nmemb;
}