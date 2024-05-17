#define OTAKUGIFS_IMPL
#include <otakugifs.h>
#include "tests_common.h"

int main() {
    fprintf(stderr, WHITE BOLD "Fetching reactions... ");

    // get reactions
    otaku_reaction_list reactions;
    otaku_status status = otaku_reactions(&reactions);
    if (status != OTAKU_OK) {
        fprintf(stderr, RED "failed!" BOLD " Error code: %d\n", status);
        return EXIT_FAILURE;
    }
    fprintf(stderr, GREEN "success.\n");

    // print reactions
    fprintf(stderr, WHITE BOLD "->" PINK);
    for (size_t i = 0; i < reactions.len; i++)
        fprintf(stderr, " %s", reactions.reactions[i]);
    fprintf(stderr, "\n" WHITE);

    // free reactions
    otaku_free_reactions(&reactions);

    return EXIT_SUCCESS;
}
