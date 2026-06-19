/* FORTH_H */
#ifndef FORTH_H
#define FORTH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_DICTIONARY_SIZE         10*1024*1024
#define DEFAULT_WORD_NAME_MAX_LENGTH    32

#define BIT(x) (1<<(x))
#define FLAG_HIDDEN     BIT(0)
#define FLAG_IMMEDIATE  BIT(1)
#define FLAG_BUILTIN    BIT(2)
#define FLAG_HASARG     BIT(3)
#define FLAG_INLINE     BIT(4)
#define FLAG_DEFERRED   BIT(5)

#define STATE_IMMEDIATE 0
#define STATE_COMPILE   1

typedef intptr_t    cell;
typedef void**      xt;
typedef struct      word_header {
    cell                flags;
    struct word_header* next;
    char                name[DEFAULT_WORD_NAME_MAX_LENGTH];
} word_header_t;

extern word_header_t* latest;
extern void* dictionary_pointer; /* todo: dp? */
extern void* dictionary_base;
extern cell  dictionary_size;

extern int state;
extern int base;
extern void* builtin_immediatebuf[2];
extern void* word_immediatebuf[3];
extern xt call_code;
extern xt lit_code;

/* forth interpreter */
extern void forth_interpreter_handle_word(word_header_t* word);
extern int  forth_interpreter_parse_number(const char* wordbuf, int* result);
extern void forth_interpreter_handle_number(int number);
extern int  forth_interpreter_interpret(void);
/* end forth interpreter */

/* forth dictionary */
extern void             forth_dictionary_init_defaults(void);
extern word_header_t*   forth_dictionary_find_word(const char* name);
extern word_header_t*   forth_dictionary_create_word(const char* name, cell flags);
extern void             forth_dictionary_compile(cell value);
extern xt               forth_dictionary_get_cfa(word_header_t* word);
extern xt               forth_dictionary_get_cfa_by_name(const char* name);
extern void             forth_dictionary_defcode(const char* name, void* code, cell flags);
extern void             forth_dictionary_defconst(const char* name, cell value);
/* end forth dictionary */

/* forth io */
/* core forth io ops */
extern void  forth_io_init_defaults(void);
extern void  forth_io_set_input_file(const char* filename);
extern char* forth_io_get_next_word(void);
extern void  forth_io_read_string(const char* str);
/* other forth io ops */
void forth_io_emit(int ch);
void forth_io_tell(const char* str);
/* end forth io */

/* forth vm */
extern void forth_vm_init_defaults(void);
extern int  forth_vm_run(void);
extern void forth_vm_push_ds(cell value);
extern void forth_vm_push_ns(void);
extern void forth_vm_schedule_builtin(xt code);
extern void forth_vm_schedule_word(xt code);
/* end forth vm */

/* tests */
extern void forth_test_run_all(void);
extern void forth_dictionary_test(void);
extern void forth_io_test(void);
extern void forth_vm_test(void);





#endif /* FORTH_H */
