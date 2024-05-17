/// \file otakugifs.h This file contains structs and functions for interacting with the otakugifs.xyz API.

#ifndef OTAKUGIFS_H
#define OTAKUGIFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

/// Base URL for the otakugifs.xyz API.
#define OTAKU_BASE_URL "https://api.otakugifs.xyz/gif"

/// Status codes for the otakugifs.xyz c wrapper.
typedef enum {
    OTAKU_OK, ///< Indicates that the operation was successful.
    OTAKU_MEM_ERR, ///< Indicates that there was a memory allocation error.
    OTAKU_LIBCURL_ERR, ///< Indicates that there was an error with libcurl.
    OTAKU_CJSON_ERR ///< Indicates that there was an error with cJSON.
} otaku_status;

/// Enum for the format of the image.
typedef enum {
    OTAKU_GIF, ///< Indicates that the response image is a gif.
    OTAKU_WEBP, ///< Indicates that the response image is a webp.
    OTAKU_AVIF ///< Indicates that the response image is a avif.
} otaku_format;

/// Struct for a list of reactions.
typedef struct {
    char **reactions; ///< [out] Array of reactions.
    size_t len; ///< [out] Amount of reactions.
} otaku_reaction_list;

/// Struct for a result.
typedef struct {
    otaku_format format; ///< [out] Format of the result.
    char* url; ///< [out] URL to the result.
} otaku_result;

/**
 * Struct for an http response.
 */
typedef struct {
    char *text; ///< [out] Non-nullterminated text of the response.
    size_t len; ///< [out] Length of the response text.
} otaku_http_response;

#ifndef OTAKUGIFS_IMPL

/**
 * Get a list of reactions.
 *
 * This function fetches the `allreactions` endpoint of the api
 * and parses the response into a list of reactions.
 *
 * It will allocate memory for the list of reactions and the reactions themselves.
 *
 * \param [out] reactions
 *   Pointer to a \link otaku_reaction_list otaku_reaction_list \endlink to store the reactions in.
 *
 * \return
 *   ::OTAKU_OK \n
 *   ::OTAKU_MEM_ERR \n
 *   ::OTAKU_LIBCURL_ERR \n
 *   ::OTAKU_CJSON_ERR
 */
otaku_status otaku_reactions(otaku_reaction_list* reactions);

/**
 * Check if a reaction exists.
 *
 * This function searches for a specific reaction in a list of reactions by name.
 *
 * \param [in] reactions
 *   Pointer to a \link otaku_reaction_list otaku_reaction_list \endlink to search in.
 * \param [in] name
 *   Name of the reaction to search for.
 *
 * \return
 *   True if the reaction exists, false otherwise.
 */
bool otaku_is_reaction(const otaku_reaction_list* reactions, const char* name);

/**
 * Fetch a reaction.
 *
 * This function fetches the specified reaction endpoint of the api
 * and parses the response into a result reaction.
 *
 * It will allocate memory for the result.
 *
 * \param [out] result
 *   Pointer to a \link otaku_result otaku_result \endlink to store the result in.
 * \param [in] reaction
 *   Reaction to fetch. Must be a valid reaction.
 * \param [in] format
 *   Format of the result.
 *
 * \return
 *   ::OTAKU_OK \n
 *   ::OTAKU_MEM_ERR \n
 *   ::OTAKU_LIBCURL_ERR \n
 *   ::OTAKU_CJSON_ERR \n
 */
otaku_status otaku_reaction(otaku_result *result, const char *reaction, otaku_format format);

/**
 * Download an image.
 *
 * This function fetches the specified image url
 * and stores the response in a \link otaku_http_response otaku_http_response \endlink.
 *
 * It will allocate memory for the response text.
 *
 * \param [out] http_response
 *   Pointer to a \link otaku_http_response otaku_http_response \endlink to store the response in.
 * \param [in] url
 *   URL of the image to download.
 *
 * \return
 *   ::OTAKU_OK \n
 *   ::OTAKU_MEM_ERR \n
 *   ::OTAKU_LIBCURL_ERR
 */
otaku_status otaku_download(otaku_http_response *http_response, const char* url);

/**
 * Free a list of reactions.
 *
 * This function frees the memory allocated for the list of reactions and the reactions themselves.
 *
 * \param [in] reactions
 *   Pointer to a \link otaku_reaction_list otaku_reaction_list \endlink to free.
 */
void otaku_free_reactions(const otaku_reaction_list* reactions);

/**
 * Free a result.
 *
 * This function frees the memory allocated for the result.
 *
 * \param [in] result
 *   Pointer to a \link otaku_result otaku_result \endlink to free.
 */
void otaku_free_result(const otaku_result* result);

/**
 * Free an http response.
 *
 * This function frees the memory allocated for the response text.
 *
 * \param [in] http_response
 *   Pointer to a \link otaku_http_response otaku_http_response \endlink to free.
 */
void otaku_free_http_response(const otaku_http_response* http_response);

#else // OTAKUGIFS_IMPL

static size_t otaku_write_callback(const void *ptr, size_t count, size_t nmemb, otaku_http_response *http_response) {
    size_t size = count * nmemb;
    size_t new_len = http_response->len + size;

    // resize response text
    char* new_text = (char*) realloc(http_response->text, new_len + 1);
    if (!new_text) {
        free(http_response->text);
        return CURLE_ABORTED_BY_CALLBACK;
    }

    // copy new data to response text
    memcpy(new_text + http_response->len, ptr, size);
    http_response->text = new_text;
    http_response->len = new_len;
    return size;
}

static otaku_status otaku_do_request(otaku_http_response *http_response, const char* url) {
    // initialize http response object
    http_response->len = 0;
    http_response->text = (char*) malloc(1);
    if (!http_response->text)
        return OTAKU_MEM_ERR;

    // initialize curl
    CURL *curl = curl_easy_init();
    if (!curl)
        return OTAKU_LIBCURL_ERR;

    // configure curl request
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, otaku_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_response);

    // make request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return OTAKU_LIBCURL_ERR;

    // cleanup curl
    curl_easy_cleanup(curl);
    return OTAKU_OK;
}

otaku_status otaku_reactions(otaku_reaction_list* reactions) {
    // make request
    otaku_http_response http_response;
    otaku_status http_status = otaku_do_request(&http_response, OTAKU_BASE_URL "/allreactions");
    if (http_status != OTAKU_OK)
        return http_status;

    // parse response
    cJSON *json = cJSON_ParseWithLength(http_response.text, http_response.len);
    if (!json || !cJSON_IsObject(json) || !cJSON_HasObjectItem(json, "reactions"))
        return OTAKU_CJSON_ERR;

    // parse json
    cJSON *reactions_obj = cJSON_GetObjectItemCaseSensitive(json, "reactions");
    reactions->len = cJSON_GetArraySize(reactions_obj);
    reactions->reactions = (char**) malloc(reactions->len * sizeof(char*));

    // iterate through reactions
    const cJSON *reactions_item;
    size_t i = 0;
    cJSON_ArrayForEach(reactions_item, reactions_obj) {
        reactions->reactions[i] = (char*) malloc(strlen(reactions_item->valuestring) + 1);
        if (!reactions->reactions[i])
            return OTAKU_MEM_ERR;

        strcpy(reactions->reactions[i], reactions_item->valuestring);
        i++;
    }

    // cleanup
    cJSON_Delete(json);
    free(http_response.text);
    return OTAKU_OK;
}

bool otaku_is_reaction(const otaku_reaction_list* reactions, const char* name) {
    for (size_t i = 0; i < reactions->len; i++) {
        if (strcmp(reactions->reactions[i], name) == 0)
            return true;
    }

    return false;
}

otaku_status otaku_reaction(otaku_result *result, const char *reaction, otaku_format format) {
    // create reaction url
    char url[96];
    snprintf(url, 96, OTAKU_BASE_URL "?reaction=%s&format=%s", reaction, format == OTAKU_GIF ? (format == OTAKU_AVIF ? "avif" : "gif") : "webp");

    // make request
    otaku_http_response http_response;
    otaku_status http_status = otaku_do_request(&http_response, url);
    if (http_status != OTAKU_OK)
        return http_status;

    // parse response
    cJSON *json = cJSON_ParseWithLength(http_response.text, http_response.len);
    if (!json || !cJSON_IsObject(json) || !cJSON_HasObjectItem(json, "url"))
        return OTAKU_CJSON_ERR;

    // parse json
    cJSON *url_obj = cJSON_GetObjectItemCaseSensitive(json, "url");
    result->url = (char*) malloc(strlen(url_obj->valuestring) + 1);
    if (!result->url)
        return OTAKU_MEM_ERR;
    result->url = strcpy(result->url, url_obj->valuestring);
    result->format = format;

    // cleanup
    cJSON_Delete(json);
    free(http_response.text);
    return OTAKU_OK;
}

otaku_status otaku_download(otaku_http_response *http_response, const char* url) {
    return otaku_do_request(http_response, url);
}

void otaku_free_reactions(const otaku_reaction_list* reactions) {
    for (size_t i = 0; i < reactions->len; i++)
        free(reactions->reactions[i]);

    free(reactions->reactions);
}

void otaku_free_result(const otaku_result* result) {
    free(result->url);
}

void otaku_free_http_response(const otaku_http_response* http_response) {
    free(http_response->text);
}

#endif // OTAKUGIFS_IMPL

#ifdef __cplusplus
}
#endif

#endif // OTAKUGIFS_H
