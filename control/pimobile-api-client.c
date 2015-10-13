/*
 * Uses ultra and compass http cgi services to get current sensor values.
 * Recommended to use if multiple processes will access sensor data to avoid
 * problems that will arise if concurrent access to sensor hardware is attempted.
 * Uses libcurl for http access.
 */

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
 
struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
 
struct MemoryStruct getDataFromUrl(const char *url) {
    CURL *curl_handle;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */ 
    curl_handle = curl_easy_init();

    /* specify URL to get */ 
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */ 
    curl_easy_perform(curl_handle);

    /* cleanup curl stuff */ 
    curl_easy_cleanup(curl_handle);

    /* we're done with libcurl, so clean it up */ 
    curl_global_cleanup();
    
    /* check for errors */ 
    return chunk;
}

float getCompassAngleFromApi(void) {
    float angle = -1.0;
    struct MemoryStruct chunk = getDataFromUrl("http://localhost/compass");

    if((long)chunk.size >= 30) {
        sscanf(chunk.memory, "{\"angle\":%f", &angle);
    }

    free(chunk.memory);
    return angle;
}

float getDistanceFromApi(void) {
    float distance = -1.0, time=0.0;
    struct MemoryStruct chunk = getDataFromUrl("http://localhost/ultra");

    if((long)chunk.size >= 30) {
        sscanf(chunk.memory, "{\"time\":%f,\"distance\":%f}\n", &time, &distance);
    }
   
    free(chunk.memory);
    return distance;
}