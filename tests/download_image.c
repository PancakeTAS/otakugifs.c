#define OTAKUGIFS_IMPL
#include <otakugifs.h>
#include "tests_common.h"

#define URL "https://cdn.otakugifs.xyz/gifs/kiss/NGLVWgfzrI.gif"
#define SIZE 608706

int main() {
    fprintf(stderr, WHITE BOLD "Downloading image... ");

    // download image
    otaku_http_response http_response;
    otaku_status status = otaku_download(&http_response, URL);
    if (status != OTAKU_OK) {
        fprintf(stderr, RED "failed!" BOLD " Error code: %d\n", status);
        return EXIT_FAILURE;
    }

    // check size
    if (http_response.len != SIZE) {
        fprintf(stderr, RED "failed!" BOLD " Size mismatch: %ld != %d\n", http_response.len, SIZE);
        return EXIT_FAILURE;
    }
    fprintf(stderr, GREEN "success.\n");
    fprintf(stderr, WHITE BOLD "-> filesize matches\n");

    // free response
    otaku_free_http_response(&http_response);

    return EXIT_SUCCESS;
}
