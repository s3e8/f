#include "forth.h"

/* todo:
    -- input stack? for core-internal strings esp
    -- flush buffers..?
    -- flush stdin stdio...?
*/

#define DEFAULT_WORDBUF_SIZE        128
#define DEFAULT_LINEBUF_SIZE        1024

static char default_linebuf[DEFAULT_LINEBUF_SIZE]; /* todo: should these be on stack or on heap? */
static char default_wordbuf[DEFAULT_WORDBUF_SIZE];

static char*    current_wordbuf;
static char*    current_linebuf;
static int      current_wordbuf_size;
static int      current_linebuf_size;
static char*    current_linebuf_pos;
static FILE*    current_input_stream;
static FILE*    current_output_stream;
/* todo: current_err_stream? */

void forth_io_set_input_stream(FILE* input_stream) {
    current_input_stream = input_stream;
}

void forth_io_set_output_stream(FILE* output_stream) {
    current_output_stream = output_stream;
}

/* todo: don't need both of these.. can set i/o in forth */
void forth_io_open_input_file(const char* filename, const char* mode) {
    FILE* f = fopen(filename, mode);
    if(!f) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }
    current_input_stream = f;
}

void forth_io_open_output_file(const char* filename, const char* mode) {
    FILE* f = fopen(filename, mode);
    if(!f) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }
    current_output_stream = f;
}

void forth_io_close_input() {
    if(current_input_stream && current_input_stream != stdin) {
        fclose(current_input_stream);
        current_input_stream = NULL;
    }
}

FILE* forth_io_open_or_create_file(const char* filename, const char* mode)
{
    FILE* fp = fopen(filename, mode);
    if(!fp) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    return fp;
}

void forth_io_set_input_file(const char* filename)
{
    FILE* fp = forth_io_open_or_create_file(filename, "r");
    if(!fp) return; /* todo */

    current_input_stream = fp;
}

void forth_io_print_state() {
    printf("current_wordbuf: %s\n", current_wordbuf);
    printf("current_linebuf: %s\n", current_linebuf);
    printf("current_linebuf_pos: %s\n", current_linebuf_pos);
}

char* forth_io_get_next_line() {
    if(!current_linebuf) {
        fprintf(stderr, "Error: current_linebuf not set\n");
        return NULL;
    }
    if(current_input_stream == stdin) printf("> ");

    char* tmp = fgets(current_linebuf, current_linebuf_size, current_input_stream);
    if (!tmp && feof(current_input_stream)) {
        printf("End of file reached, switching to stdin...\n");
        current_input_stream = stdin;
        printf("> ");
        tmp = fgets(current_linebuf, current_linebuf_size, current_input_stream);
    }
    if(!tmp) return NULL;

    current_linebuf_pos = tmp;

    return tmp;
}

/* Parse next word from a string, updating a position pointer */
char* forth_io_get_next_word()
{
    char* tmp       = current_wordbuf;
    char* pos       = current_linebuf_pos;
    int   size      = current_wordbuf_size;
    size_t count    = 0;

    printf("getting next word...\n");

    /* Skip whitespace */
    skip_whitespace:
        while(*pos && isspace(*pos)) pos++;

    /* if line is empty, check for new line */
    if(*pos == '\0') {
        if(!forth_io_get_next_line()) return NULL;
        pos = current_linebuf_pos;
        // printf("tmp: %s\n", tmp);
        // printf("pos: %s\n", pos);
        goto skip_whitespace;
    }

    /* Copy word */
    while(*pos && !isspace(*pos) && count < size - 1) {
        *tmp++ = *pos++;
        count++;
    }
    *tmp = '\0';

    current_linebuf_pos = pos;

    printf("word retrieved.\n");

    return current_wordbuf;
}


void forth_io_read_string(const char* str) {
    const char* pos = str;
    char* tempbuf = current_linebuf;

    int i;
    /* fill linebuf */ 
    /* todo: I dont think it needs to be - 1 */
    for (i = 0; i < current_linebuf_size - 1 && *pos; i++) {
        *tempbuf++ = *pos++;
    }
    *tempbuf = '\0'; 
    current_linebuf_pos = current_linebuf;

    while(*current_linebuf_pos) {
        forth_io_get_next_word();
        printf("Got word: '%s'\n", current_wordbuf);
    }
}


void forth_io_close_all(void) {}


void forth_io_init_defaults(void) {
    current_wordbuf = default_wordbuf;
    current_linebuf = default_linebuf;
    current_wordbuf_size = DEFAULT_WORDBUF_SIZE;
    current_linebuf_size = DEFAULT_LINEBUF_SIZE;
    current_linebuf_pos = current_linebuf;
    current_input_stream    = stdin;
    current_output_stream   = stdout;
    /* todo: stderr? */
}


void forth_io_read_stream() {
    while(forth_io_get_next_word()) {
        printf("current word: '%s'\n", current_wordbuf);
    }
}


void forth_io_test(void)
{
    const char* line = "this is a test";
    forth_io_read_string(line);

    /* todo: sizeof(wordbuf) vs wordbuf_size */
    // forth_io_get_next_word(current_linebuf, current_wordbuf, sizeof(current_wordbuf));
    while(forth_io_get_next_word()) {
        printf("current word: '%s'\n", current_wordbuf);
    }

    return;

}


/* ops */
void forth_io_emit(int ch) {
    fputc(ch, current_output_stream);
}

void forth_io_tell(const char* str) {
    fputs(str, current_output_stream);
}
