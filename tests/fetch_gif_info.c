#define OTAKUGIFS_IMPL
#include <otakugifs.h>
#include "tests_common.h"

int main() {
    fprintf(stderr, WHITE BOLD "Fetching gif info from kiss reaction... ");

    // get gif
    otaku_result result;
    otaku_status status = otaku_reaction(&result, "kiss", OTAKU_GIF);
    if (status != OTAKU_OK) {
        fprintf(stderr, RED "failed!" BOLD " Error code: %d\n", status);
        return EXIT_FAILURE;
    }
    fprintf(stderr, GREEN "success.\n");

    // print results
    fprintf(stderr, WHITE BOLD "-> " PINK BOLD "%s\n", result.url);

    // free results
    otaku_free_result(&result);

    return EXIT_SUCCESS;
}
