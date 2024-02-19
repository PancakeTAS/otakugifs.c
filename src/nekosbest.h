#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef enum ReponseFormat {
    PNG,
    GIF
} response_format;

typedef struct ApiEndpoint {
    char *name;
    response_format format;
} api_endpoint;

typedef struct EndpointList {
    api_endpoint **endpoints;
    size_t len;
} endpoint_list;


typedef struct GIFSource {
    char *anime_name;
} gif_source;

typedef struct PNGSource {
    char *artist_name;
    char *artist_href;
    char *source_url;
} png_source;

typedef struct ApiResponse {
    union {
        gif_source *gif;
        png_source *png;
    } source;
    char* url;
} api_response;

typedef struct ResponseList {
    api_response **responses;
    size_t len;
} response_list;

void endpoints(endpoint_list *list);
void category(response_list *list, api_endpoint *endpoint, int amount);
void search(response_list *list, char* query, int amount, response_format type, api_endpoint *category);
