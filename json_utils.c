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

typedef struct {
    const char *code;
    const char *name;
} country_abr;

country_abr country_lookup[] = {
    {"AO", "Angola"},
    {"AR", "Argentina"},
    {"AU", "Australia"},
    {"BG", "Bulgaria"},
    {"BR", "Brazil"},
    {"CH", "Switzerland"},
    {"CL", "Chile"},
    {"CM", "Cameroon"},
    {"CN", "China"},
    {"CR", "Costa Rica"},
    {"CZ", "Czech Republic"},
    {"DE", "Germany"},
    {"DK", "Denmark"},
    {"EG", "Egypt"},
    {"ES", "Spain"},
    {"FI", "Finland"},
    {"FJ", "Fiji"},
    {"GB", "United Kingdom"},
    {"GH", "Ghana"},
    {"GR", "Greece"},
    {"ID", "Indonesia"},
    {"IL", "Israel"},
    {"IN", "India"},
    {"IQ", "Iraq"},
    {"IT", "Italy"},
    {"JP", "Japan"},
    {"KZ", "Kazakhstan"},
    {"LB", "Lebanon"},
    {"LS", "Lesotho"},
    {"LU", "Luxembourg"},
    {"LY", "Libya"},
    {"MA", "Morocco"},
    {"MD", "Moldova"},
    {"MG", "Madagascar"},
    {"MM", "Myanmar"},
    {"MX", "Mexico"},
    {"NI", "Nicaragua"},
    {"PH", "Philippines"},
    {"PK", "Pakistan"},
    {"PL", "Poland"},
    {"PY", "Paraguay"},
    {"RO", "Romania"},
    {"RS", "Serbia"},
    {"RU", "Russia"},
    {"SA", "Saudi Arabia"},
    {"SD", "Sudan"},
    {"SI", "Slovenia"},
    {"SK", "Slovakia"},
    {"TG", "Togo"},
    {"TH", "Thailand"},
    {"TW", "Taiwan"},
    {"UA", "Ukraine"},
    {"US", "United States"},
    {"VN", "Vietnam"},
    {"ZA", "South Africa"}
};

const size_t country_lookup_size = sizeof(country_lookup) / sizeof(country_lookup[0]);

void lookUpCountryAbbreviation(char *code, size_t size);
int updateCountry(char *country,size_t size);

void copy_json_string(cJSON *source, char *dest, size_t dest_size)
{
    if(cJSON_IsString(source) && (source->valuestring != NULL))
    {
        strncpy(dest, source->valuestring, dest_size - 1);
        dest[dest_size - 1] = '\0';
    }
}
int readJsonToArray(const char *filename, Server **serverList, size_t *serverList_count)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    fseek(file,0,SEEK_END);
    long fileSize = ftell(file);
    fseek(file,0,SEEK_SET);

    char *buffer = malloc(fileSize + 1);
    
    if (!buffer) {
        perror("malloc failed");
        fclose(file);
        return -1;
    }

    size_t length = fread(buffer, 1, fileSize, file);
    buffer[length] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    if(!json)
    {
        perror("Json parse failed");
        free(buffer);
        return -1;
    }

    int count = cJSON_GetArraySize(json);
    if(count == 0)
    {
        cJSON_Delete(json);
        free(buffer);
        return 0;
    }

    *serverList = malloc(count * sizeof(Server));
    if (!(*serverList)) 
    {
        perror("malloc failed");
        cJSON_Delete(json);
        free(buffer);
        return -1;
    }

    cJSON *server = NULL;
    int index = 0;

    cJSON_ArrayForEach(server, json) 
    {    
        cJSON *country = cJSON_GetObjectItemCaseSensitive(server, "country");
        cJSON *city = cJSON_GetObjectItemCaseSensitive(server, "city");
        cJSON *provider = cJSON_GetObjectItemCaseSensitive(server, "provider"); 
        cJSON *host = cJSON_GetObjectItemCaseSensitive(server, "host");
        
        char country_str[64];
        strncpy(country_str,country->valuestring,sizeof(country_str)-1);
        country_str[sizeof(country_str)-1] = '\0';

        if(updateCountry(country_str,sizeof(country_str)) == -1)
            continue;

        Server *current = &((*serverList)[index]);

        strncpy(current->country,country_str,sizeof(current->country)-1);
        current->country[sizeof(current->country) - 1] = '\0';
        copy_json_string(city,current->city,sizeof(current->city));
        copy_json_string(provider,current->provider,sizeof(current->provider));
        copy_json_string(host,current->host,sizeof(current->host));

        index++;
    }
    *serverList_count = index;
    cJSON_Delete(json);
    free(buffer);
    return 0;
}

int updateCountry(char *country,size_t size)
{
    if(strcmp(country,"DELETE ME") == 0)
        return -1;
    
    if(strlen(country) < 3)
    {
        lookUpCountryAbbreviation(country,size);
        return 0;
    }
       
    char *comma = strchr(country,',');
    if(comma != NULL)
        *comma = '\0';
    return 0;
}

void lookUpCountryAbbreviation(char *code, size_t size)
{
    for (size_t i = 0; i < country_lookup_size; i++) {
        if (strcmp(country_lookup[i].code, code) == 0) 
        {
            strncpy(code,country_lookup[i].name,size-1);
            code[size-1] = '\0';
            break;
        }
    }
}
