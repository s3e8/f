
#include "forth.h"

static void* dictionary_pointer; /* todo: dp? */
static void* dictionary_base;
static cell  dictionary_size;

word_header_t* latest = NULL;

word_header_t* forth_dictionary_create_word(const char* name, cell flags) {
    if(!name) name = "\0";

    printf("creating word: '%s'\n", name);

    word_header_t* new = (word_header_t*)dictionary_pointer;
    dictionary_pointer += sizeof(word_header_t); /* todo: size of type or struct or either? */

    strncpy(new->name, name, DEFAULT_WORD_NAME_MAX_LENGTH);
    new->flags = flags;
    new->next  = latest;
    latest = new;

    return new;
}

word_header_t* forth_dictionary_find_word(const char* name) {
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

/* todo: rename xt to cfa? */
/* todo: do we need this helper? */
xt forth_dictionary_get_cfa(word_header_t* word) { return (xt)(word + 1); }

xt forth_dictionary_get_cfa_by_name(const char* name) {
    word_header_t* word = forth_dictionary_find_word(name);
    return forth_dictionary_get_cfa(word);;
}

void forth_dictionary_compile(cell value) {
    *(cell*)dictionary_pointer = value;
    dictionary_pointer += sizeof(cell);
}



// Mock getcode for testing
static void* getcode(const char* name) {
    printf("Looking up: %s\n", name);
    // Return a dummy address for testing
    static int dummy;
    return &dummy;
}



// /* todo: do we need this helper? */
// static void* getcode(const char* name)
// {
//     word_header_t* word = find_word(name);
//     return *(cfa(word));
// }

void forth_dictionary_defcode(const char* name, void* code, cell flags) {
    forth_dictionary_create_word(name, flags | FLAG_BUILTIN);
    forth_dictionary_compile((cell)code);
}

void defword(const char* name, xt code, int codesize, cell flags) {
    forth_dictionary_create_word(name, flags);
    int i;
    for(i = 0; i < codesize; i++) forth_dictionary_compile((cell)code[i]);
    forth_dictionary_compile((cell)getcode("eow"));
}

void defconst(const char* name, cell value) {
    void* code[] = { getcode("lit"), (void*)value, getcode("exit") };
    defword(name, code, 3, FLAG_INLINE);
}

typedef float aliasingfloat __attribute__((__may_alias__));
void deffconst(const char* name, cell value) {
    /* todo: why can't I say xt code = {...}? */
    void* code[] = { getcode("flit"), (aliasingfloat*)value, getcode("exit") }; 
    defword(name, code, 3, FLAG_INLINE);
}

void forth_dictionary_init_defaults() {
    dictionary_base     = malloc(DEFAULT_DICTIONARY_SIZE);
    dictionary_pointer  = dictionary_base;
    dictionary_size     = DEFAULT_DICTIONARY_SIZE;
}



/* debug stuff */

/* todo: rename to find by xt? */
word_header_t* forth_dictionary_find_word_by_cfa(xt cfa) {
    if(!cfa) return NULL;

    printf("finding cfa: '%p'\n", cfa);

    word_header_t* word = latest;
    while(word) {
        if (!(word->flags & FLAG_HIDDEN) && ((xt)(word + 1) == cfa)) {
            return word;
        }
        word = word->next;
    }

    return NULL;
}
