/*
 * more.c - A simple file paging program for UNIX v6
 *
 * Usage: more [filename]
 *
 * Reads from the specified filename or standard input and displays
 * the content one screen (24 lines) at a time.
 *
 * This version sets the console to raw mode to allow single-key commands.
 * Spacebar: Next page
 * q: Quit
 */

#include "unix.h"

#define LINES_PER_SCREEN 24
#define PROMPT "--More--"
#define PROMPT_LEN 8
#define ERASE_PROMPT "\r         \r" /* Carriage return, 9 spaces, carriage return */
#define ERASE_PROMPT_LEN 11

struct sgttyb {
    char    sg_ispeed;  /* input speed */
    char    sg_ospeed;  /* output speed */
    char    sg_erase;   /* erase character */
    char    sg_kill;    /* kill character */
    int     sg_flags;   /* mode flags */
};

/*
 * tty flags
 */
#define	HUPCL   01
#define	XTABS   02
#define	LCASE   04
#define	ECHO    010
#define	CRMOD   020
#define	RAW     040
#define	ODDP    0100
#define	EVENP   0200
#define	ANYP    0300

/* Globals for terminal settings */
struct sgttyb original_term_settings;
struct sgttyb raw_settings;
int term_settings_stored = 0;
int console_fd_global = -1;

/*
 * Restores the original terminal settings before exiting.
 * This is crucial to avoid leaving the user's shell in a broken state.
 */
void restore_term_settings()
{
    if (term_settings_stored) {
        stty(console_fd_global, &original_term_settings);
    }
}

/*
 * Waits for a user to press a command key ('q' or space).
 * Returns 0 to continue, 1 to quit.
 */
int await_user_command()
{
    char cmd_char;
    write(1, PROMPT, PROMPT_LEN);

    for (;;)
    {
        if (read(console_fd_global, &cmd_char, 1) == 1) {
            if (cmd_char == 'q' || cmd_char == 'Q') {
                write(1, ERASE_PROMPT, ERASE_PROMPT_LEN);
                return 1; /* Quit */
            }
            if (cmd_char == ' ') {
                write(1, ERASE_PROMPT, ERASE_PROMPT_LEN);
                return 0; /* Continue */
            }
            /* Ignore other keys */
        } else {
            /* If read fails, quit */
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int input_fd;
    int line_count;
    char c;
    struct stat s_stdin, s_console;

    if (argc < 2) {
        /* Case 1: No file argument. Input might be a pipe or the console. */
        console_fd_global = open("/dev/console", 0);
        if (console_fd_global < 0) {
            printf("Error: Cannot open /dev/console\n");
            exit(); /* No safe_exit needed, settings not changed yet */
        }

        fstat(0, &s_stdin);
        fstat(console_fd_global, &s_console);

        if (s_stdin.s_dev == s_console.s_dev && s_stdin.s_inumber == s_console.s_inumber) {
            printf("Usage: more <filename>\nOr: some_command | more\n");
            close(console_fd_global);
            exit();
        }
        
        input_fd = 0; /* Input is from stdin (pipe) */
    } else {
        /* Case 2: File argument. Input is the file, console is stdin. */
        input_fd = open(argv[1], 0);
        if (input_fd < 0) {
            printf("Error: Cannot open file %s\n", argv[1]);
            exit();
        }
        console_fd_global = 0; /* Prompts will go to stdin */
    }

    /* Set console to raw mode */
    gtty(console_fd_global, &original_term_settings);
    term_settings_stored = 1;
    
    raw_settings = original_term_settings;
    raw_settings.sg_flags |= RAW;
    stty(console_fd_global, &raw_settings);

    line_count = 0;
    while (read(input_fd, &c, 1) == 1) {
        write(1, &c, 1);
        if (c == '\n') {
            line_count++;
            if (line_count >= LINES_PER_SCREEN) {
                if (await_user_command() == 1) {
                    break; /* User pressed 'q' */
                }
                line_count = 0;
            }
        }
    }

    /* Clean up */
    restore_term_settings();

    if (input_fd != 0) {
        close(input_fd);
    }
    if (console_fd_global != 0) {
        close(console_fd_global);
    }

    return 0;
}
