#define OTAKUGIFS_IMPL
#include <otakugifs.h>
#include "tests_common.h"

int main() {
    fprintf(stderr, WHITE BOLD "Finding a specific reaction... ");

    // get reactions
    otaku_reaction_list reactions;
    otaku_status status = otaku_reactions(&reactions);
    if (status != OTAKU_OK) {
        fprintf(stderr, RED "failed!" BOLD " Error code: %d\n", status);
        return EXIT_FAILURE;
    }

    // is reaction
    bool reaction = otaku_is_reaction(&reactions, "kiss");
    if (!reaction) {
        fprintf(stderr, RED "failed!" BOLD " Endpoint not found.\n");
        return EXIT_FAILURE;
    }
    fprintf(stderr, GREEN "success.\n" WHITE BOLD "-> reaction found\n");

    // free reactions
    otaku_free_reactions(&reactions);

    return EXIT_SUCCESS;
}
