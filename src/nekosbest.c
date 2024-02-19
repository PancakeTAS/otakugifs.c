#include "nekosbest.h"

static char API_URL[26 + 8 + 9 + 1] = "https://nekos.best/api/v2/";

typedef struct HTTPResponse {
    char *text; // not null-terminated
    size_t len;
} http_response;

static size_t http_response_write(void *ptr, size_t size, size_t nmemb, http_response *http_response) {
    // resize response text
    size_t new_len = http_response->len + size * nmemb;
    http_response->text = realloc(http_response->text, new_len + 1);
    if (http_response->text == NULL) {
        fprintf(stderr, "Failed to reallocate memory for http response message\n");
        exit(EXIT_FAILURE);
    }

    // copy new data to response text
    memcpy(http_response->text + http_response->len, ptr, size * nmemb);
    http_response->len = new_len;
    return size * nmemb;
}

static void do_request(http_response *http_response, char* url) {
    // create http response object
    http_response->text = calloc(1, 1);
    http_response->len = 0;
    if (http_response->text == NULL) {
        fprintf(stderr, "Failed to allocate memory for http response message\n");
        exit(EXIT_FAILURE);
    }

    // initialize curl
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Failed to initialize curl\n");
        exit(EXIT_FAILURE);
    }

    // configure curl request
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_response_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_response);

    // make request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Failed to perform curl request: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }

    // cleanup curl
    curl_easy_cleanup(curl);
}

void endpoints(endpoint_list* list) {
    // create endpoint url
    memcpy(API_URL + 26, "endpoints\0", 10);

    // make request
    http_response http_response;
    do_request(&http_response, API_URL);

    // parse response
    cJSON *json = cJSON_ParseWithLength(http_response.text, http_response.len);
    if (!json || !cJSON_IsObject(json)) {
        fprintf(stderr, "Failed to parse JSON\n");
        exit(EXIT_FAILURE);
    }

    // parse json
    list->len = cJSON_GetArraySize(json);
    list->endpoints = malloc(list->len * sizeof(api_endpoint*));

    // iterate through endpoints
    const cJSON *endpoint_obj;
    size_t i = 0;
    cJSON_ArrayForEach(endpoint_obj, json) {
        api_endpoint* endpoint = malloc(sizeof(api_endpoint));
        list->endpoints[i++] = endpoint;

        char* name = endpoint_obj->string;
        endpoint->name = malloc(strlen(name) + 1);
        strcpy(endpoint->name, name);

        const cJSON *format_obj = cJSON_GetObjectItemCaseSensitive(endpoint_obj, "format");
        endpoint->format = strcmp(format_obj->valuestring, "png") == 0 ? PNG : GIF;
    }
    
    // cleanup
    cJSON_Delete(json);
    free(http_response.text);
}