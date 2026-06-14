
#include "forth.h"

word_header_t* forth_dictionary_find_word_by_cfa(xt cfa) {
    if(!name) return NULL;

    printf("finding word: '%s'\n", name);

    word_header_t* word = latest;
    while(word) {
        if (!(word->flags & FLAG_HIDDEN) && 
            !strncmp(word->name, name, DEFAULT_WORD_NAME_MAX_LENGTH)) {
            return word;
        }
        word = word->next;
    }

    return NULL;
}
