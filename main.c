#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "download.h"
#include "upload.h"
#include "json_utils.h"
#include "location.h"
#include "server_utils.h"

void printUsage(char *prog);

int main( int argc, char * argv[] )
{
    int opt;
    char *country_arg = NULL;
    char *server_arg = NULL;
    int do_upload = 0;
    int do_download = 0;
    int do_best_server = 0;
    int do_location = 0;
    int do_automated = 0;

    while ((opt = getopt(argc, argv, "c:s:dublah")) != -1) 
    {
        switch (opt) 
        {
            case 'c': // country
                country_arg = optarg;
                break;
            case 's': // server
                server_arg = optarg;
                break;
            case 'd': // download
                do_download = 1;
                break;
            case 'u': // upload
                do_upload = 1;
                break;
            case 'b': // best server
                do_best_server = 1;
                break;
            case 'l': // get location
                do_location = 1;
                break;
            case 'a': // automated test
                do_automated = 1;
                break;
            case 'h': // help message
                printUsage(argv[0]);
                return 0;
                break;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    if(do_best_server && country_arg == NULL)
    {
        printf("Country argument required ( -c <country> )\n");
        return 1;
    }
    if((do_download || do_upload) && (country_arg == NULL && server_arg == NULL))
    {
        printf("Country or server argument required ( -c <country> or -s <server> )\n");
        return 1;
    }
    
    if((country_arg != NULL && country_arg[0] == '\0') || (server_arg != NULL && server_arg[0] == '\0'))
    {
        printf("Invalid country or server argument.\n");
        return 1;
    }


    Server* serverList = NULL;
    size_t serverList_count = 0;
    if(readJsonToArray("speedtest_server_list.json",&serverList,&serverList_count) == -1)
    {
        printf("Failed to read JSON");
        return 1;
    }
        
    CURLcode err = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(err != CURLE_OK)
    {
        printf("curl global init failed");
        return 1;
    }

    if(!do_automated)
    {
        if(do_download || do_upload)
        {
            char server[64]="";
            int has_server = 0;

            if(server_arg != NULL && server_arg[0] != '\0')
            {
                if(serverExists(server_arg,serverList,serverList_count))
                {
                    printf("Using server %s\n",server_arg);
                    strncpy(server, server_arg, sizeof(server) - 1);
                    server[sizeof(server) - 1] = '\0';
                    has_server = 1;
                }
                else
                {
                    printf("Invalid Server\n");
                }
            }
            else
            {
                char** hostList = NULL;
                size_t hostList_count = 0;
                findAllServersForCountry(country_arg,&hostList,&hostList_count,serverList,serverList_count);
                if(hostList_count == 0)
                    printf("Servers in %s not found.\n",country_arg);
                else
                {
                    serverPickDialogue(server,hostList,hostList_count);
                    has_server = 1;
                }
                
                for (size_t i = 0; i < hostList_count; i++)
                    free(hostList[i]);
                free(hostList);
            }
            if(has_server)
            {
                if(do_download)
                {
                    printf("Testing download speed...\n");
                    double downloadSpeed = testDownload(server);
                    if (downloadSpeed == -1)
                        printf("Unable to test download speed.\n");
                    else
                         printf("Download Speed: %.2f Mb/s\n",downloadSpeed);
                }
                if(do_upload)
                {
                    printf("Testing upload speed...\n");
                    double uploadSpeed = testUpload(server);
                    if (uploadSpeed == -1)
                        printf("Unable to test upload speed.\n");
                    else
                         printf("Upload Speed: %.2f Mb/s\n",uploadSpeed);
                }
            }
        }

        if(do_best_server)
        {
            printf("\nFinding best server for %s...\n",country_arg);
            char bestserver[64];
            findBestServerByCountry(country_arg,bestserver,serverList,serverList_count);
            if(strlen(bestserver) != 0)
                printf("\nBest server for %s: %s\n",country_arg,bestserver);
            else
                printf("\nServer not found.\n");
        }
        if(do_location)
        {
            printf("\nFinding location...\n\n");
            char loc[64];
            int errcode = getLocation(loc,sizeof(loc));
            if(errcode == -1)
                printf("Unable to find location.\n");
            else
                printf("User location: %s\n",loc);
        }
    }
    else
    {
        printf("Performing full test...\n");
        char loc[64];
        printf("\nFinding location...\n");
        int errcode = getLocation(loc,sizeof(loc));
        if(errcode == -1)
        {
            printf("Unable to find location.\n");
            return 1;
        }

        char bestserver[64];
        printf("Finding best server for location...\n");
        findBestServerByCountry(loc,bestserver,serverList,serverList_count);
        
        if(strlen(bestserver) == 0)
        {
            printf("\nServer not found.\n");
            return 1;
        }

        printf("Testing download speed...\n");
        double downloadSpeed = testDownload(bestserver);
        if (downloadSpeed == -1)
        {
            printf("Unable to test download speed.\n");
        }
        
        printf("Testing upload speed...\n");
        double uploadSpeed = testUpload(bestserver);
        if (uploadSpeed == -1)
        {
            printf("Unable to test upload speed.\n");
        }

        printf("\nTest results:\n\n");

        if(downloadSpeed != -1)
            printf("Download Speed: %.2f Mb/s\n",downloadSpeed);
        
        if(uploadSpeed != -1)
            printf("Upload speed: %.2f Mb/s\n",uploadSpeed);

        printf("Test completed on: %s\n",bestserver);
        printf("User location: %s\n",loc);
    }
 
    free(serverList);
    curl_global_cleanup();
    return 0;
}

void printUsage(char *prog)
{
    printf("\nUsage: %s [options]\n\n", prog);
    printf("-c <country>    Search servers by country\n");
    printf("-s <server>     Specify server directly\n");
    printf("-d              Run download test (-c or -s)\n");
    printf("-u              Run upload test (requires -c or -s)\n");
    printf("-b              Find best server (requires -c)\n");
    printf("-l              Get user location\n");
    printf("-a              Run full automated test (overrides all other options)\n");
    printf("-h              Show help message\n\n");
}