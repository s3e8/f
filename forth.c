/* --- */

#include "forth.h"

/* debug checklist --
    -- xt vs cfa semantics
    -- do all opcodes end with NEXT():?
    -- corrent stack checking?
*/

int state = STATE_IMMEDIATE;
int base = 10; /* todo: is this interpreter or vm? */

xt call_code;
xt lit_code;

void forth_interpreter_init_defaults() {
    state = STATE_IMMEDIATE;
    base = 10;

    forth_io_set_input_file("forth_bootstrap.f");

    printf("initializing dictionary...\n");
    forth_dictionary_init_defaults();
    printf("dictionary initialized.\n");

    printf("initializing io...\n");
    forth_io_init_defaults();
    printf("io initialized.\n");

    printf("initializing vm...\n");
    forth_vm_init_defaults();
    printf("vm initialized.\n");
}

int forth_interpreter_parse_number(const char* wordbuf, int* result) {
    char* endptr = NULL;
    cell val = (cell)strtol(wordbuf, &endptr, base);

    printf("parse_number: '%s' -> val=%ld, endptr points to '%s', base=%d\n", 
        wordbuf, (long)val, endptr, base);
    
    if(*endptr == '\0') {
        *result = val;
        return 1;
    }
    return 0;
}

int forth_interpreter_interpret(void) {
    /* todo: if !forth_initialized return 1 with err */
    char* wordbuf = forth_io_get_next_word();
    if(!wordbuf) return 1;

    word_header_t* word = forth_dictionary_find_word(wordbuf);
    if(word) {
        xt code = forth_dictionary_get_cfa(word);
        if(state == STATE_COMPILE && !(word->flags & FLAG_IMMEDIATE)) {
            printf("compile mode...\n");
            if(word->flags & FLAG_BUILTIN) {
                printf("compiling builtin...\n");
                forth_dictionary_compile((cell)*code);
            }
            else {
                forth_dictionary_compile((cell)call_code);
                forth_dictionary_compile((cell)code);
            }
        } else {
            forth_vm_push_ns();
            // *--nestingstack = current_ip;
            if(word->flags & FLAG_BUILTIN) {
                printf("interpretting builtin...\n");
                // builtin_immediatebuf[0] = *code;
                forth_vm_schedule_builtin(*code);
                // current_ip = builtin_immediatebuf;
            }
            else {
                printf("interpretting word...\n");
                // word_immediatebuf[1] = (void*)code;
                forth_vm_schedule_word((void*)code);
                // current_ip = word_immediatebuf;
            }
            /* move on to NEXT() and run ip */
            // NEXT();
            return 0;
            
        }
    }

    else {
        /* check if word is a number */
        int number; /* todo: make is_number more forth friendly for builtin? 
            * maybe not cause we define a new is_number in forth later */
        /* if number, compile or interpret (push to stack) */
        if(forth_interpreter_parse_number(wordbuf, &number)) {
            if(state == STATE_COMPILE) {
                forth_dictionary_compile((cell) lit_code);
                forth_dictionary_compile((cell) number);
            }
            else forth_vm_push_ds((cell)number);
        }
        else fprintf(stderr, "Error: no such word: %s\n", wordbuf);

        /* move on to NEXT() and run ip */
        // NEXT();
    }

    return 0;
}

// void forth_interpreter_interpret() {
//     while(1) {
//         char* wordbuf = forth_io_get_next_word();
//         if(!wordbuf) break;

//         word_header_t* word = forth_dictionary_find_word(wordbuf);
//         if(word) forth_interpreter_handle_word(word);
//         else {
//             /* check if word is a number */
//             int number; /* todo: make is_number more forth friendly for builtin? 
//              * maybe not cause we define a new is_number in forth later */
//             forth_interpreter_parse_number(wordbuf, &number);

//             /* if number, compile or interpret (push to stack) */
//             if(number) forth_interpreter_handle_number(number);
//             else fprintf(stderr, "Error: no such word: %s\n", wordbuf);
//         }
//     }

//     // void* quitcode[] = { 
//     //     WORD(INTERPRET), 
// 	// 	WORD(BRANCH), 
//     //     OFFSET(-2),
// 	// 	WORD(EOW)
//     // };
//     // ip = quitcode;

//     return;

//     /* goto labels */

// }

int main(void)
{
    forth_interpreter_init_defaults();
    // forth_test_run_all();
    // forth_interpreter_interpret();
    /* todo: take error value from set_input_file */
    /* todo: does set_input_file call set_input_stream? */
    forth_io_set_input_file("forth_bootstrap.f");
    int exit_status = forth_vm_run();
    if (exit_status) {
        fprintf(stderr, "Error: Forth VM exited with error");
        return 1;
    } else {
        printf("Forth - exiting gracefully...");
    }

    return 0;
}


// /* tests */
// void forth_test_run_all() {
//     printf("running tests...\n");
//     forth_io_set_input_file("forth_test.f");

//     // forth_io_test();
//     forth_dictionary_test();
//     forth_vm_test();
// }
