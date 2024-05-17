# otakugifs.c
A simple, lightweight c wrapper the [otakugifs.xyz](https://otakugifs.xyz/) API compliant with C99.

Join the official Discord server [here](https://discord.com/invite/3TQgpKK).

## Requirements
- libcurl (tested with 8.7.1-6)
- cjson (tested with 1.7.17-1)

## Installation
Copy the single header file to source include directory and include the header. Specify `OTAKUGIFS_IMPL` before including the header in the source file where you want to use the library.

```c
#define OTAKUGIFS_IMPL
#include "otakugifs.h"
```

## Documentation
Doxygen documentation is available [here](https://pancake.gay/otakugifs.c).
(Jump to [otakugifs.h](https://pancake.gay/otakugifs.c/otakugifs_8h.html))

## Example
```c
#define OTAKUGIFS_IMPL
#include <otakugifs.h>

#include <stdio.h>
#include <stdlib.h>

int main() {
    // fetch gif
    otaku_result result;
    otaku_reaction(&result, "kiss", OTAKU_GIF); // use otaku_reactions() to get all available reactions

    // download gif
    otaku_http_response http_response;
    otaku_download(&http_response, result.url);

    // save image to file
    FILE *file = fopen("kiss.gif", "wb");
    fwrite(http_response.text, 1, http_response.len, file);
    fclose(file);

    // cleanup
    otaku_free_result(&result);
    otaku_free_http_response(&http_response);
    return EXIT_SUCCESS;
}
```
This example fetches a gif from the 'kiss' endpoint saves it to a file. Please note that this example does not handle errors.

For more examples, see `tests/`.
