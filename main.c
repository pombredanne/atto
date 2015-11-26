/*
 * main.c              
 *
 * AttoEmacs, Hugh Barney, November 2015, A single buffer, single screen Emacs
 * Derived from: Anthony's Editor January 93, (Public Domain 1991, 1993 by Anthony Howe)
 */

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "header.h"
#include <stdarg.h>

int main(int argc, char **argv)
{
	int i;
		
	/* Find basename. */
	prog_name = *argv;
	i = strlen(prog_name);
	while (0 <= i && prog_name[i] != '\\' && prog_name[i] != '/')
		--i;
	prog_name += i+1;

	if (initscr() == NULL)
		fatal(f_initscr);

	raw();
	noecho();
	idlok(stdscr, TRUE);
	filename[0] = '\0';
		
	if (1 < argc) {
		(void) insert_file(argv[1], FALSE);
		/* Save filename irregardless of load() success. */
		strcpy(filename, argv[1]);
		modified = FALSE;
	}

	if (!growgap(CHUNK))
		fatal(f_alloc);

	top();
	key_map = keymap;

	while (!done) {
		display();
		input = getkey(key_map, &key_return);

		if (key_return != NULL)
		  (key_return->func)();
		else
		  insert();
		//debug_stats("main loop:");
	}
	if (scrap != NULL)
		free(scrap);

	move(LINES-1, 0);
	refresh();
	noraw();
	endwin();

	return (EXIT_OK);
}

void fatal(msg_t m)
{
	if (curscr != NULL) {
		move(LINES-1, 0);
		refresh();
		endwin();
		putchar('\n');
	}
	fprintf(stderr, m, prog_name);
	if (m == f_ok)
		exit(EXIT_OK);
	if (m == f_error)
		exit(EXIT_ERROR);
	if (m == f_usage)
		exit(EXIT_USAGE);
	exit(EXIT_FAIL);
}

void msg(msg_t m, ...)
{
	va_list args;
	va_start(args, m);
	(void) vsprintf(msgline, m, args);
	va_end(args);
	msgflag = TRUE;
}

void debug(char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);

    static FILE *debug_fp = NULL;

    if (debug_fp == NULL) {
        debug_fp = fopen("debug.out","w");
    }

    vsprintf (buffer, format, args);
    va_end(args);

    fprintf(debug_fp,"%s", buffer);
    fflush(debug_fp);
}

void debug_stats(char *s) {
	debug("%s bsz=%d gap=%d egap=%d\n", s, ebuf - buf, gap - buf, egap - buf);
}
