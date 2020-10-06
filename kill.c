/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2020, Jakob Kaivo <jkk@ung.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>

#define OFFSET_OF_EXIT_STATUS	128

static struct {
	char *name;
	int num;
} ksigs[] = {
	{ "0",		0 },
	{ "1",		SIGHUP },
	{ "2",		SIGINT },
	{ "3",		SIGQUIT },
	{ "6",		SIGABRT },
	{ "9",		SIGKILL },
	{ "14",		SIGALRM },
	{ "15",		SIGTERM },

	{ "ABRT",	SIGABRT },
	{ "ALRM",	SIGALRM },
	{ "BUS",	SIGBUS },
	{ "CHLD",	SIGCHLD },
	{ "CONT",	SIGCONT },
	{ "FPE",	SIGFPE },
	{ "HUP",	SIGHUP },
	{ "ILL",	SIGILL },
	{ "INT",	SIGINT },
	{ "KILL",	SIGKILL },
	{ "PIPE",	SIGPIPE },
	{ "QUIT",	SIGQUIT },
	{ "SEGV",	SIGSEGV },
	{ "STOP",	SIGSTOP },
	{ "TERM",	SIGTERM },
	{ "TSTP",	SIGTSTP },
	{ "TTIN",	SIGTTIN },
	{ "TTOU",	SIGTTOU },
	{ "USR1",	SIGUSR1 },
	{ "USR2",	SIGUSR2 },
	{ "POLL",	SIGPOLL },
	{ "PROF",	SIGPROF },
	{ "SYS",	SIGSYS },
	{ "TRAP",	SIGTRAP },
	{ "URG",	SIGURG },
	{ "VTALRM",	SIGVTALRM },
	{ "XCPU",	SIGXCPU },
	{ "XFSZ",	SIGXFSZ },
};
static size_t nsigs = sizeof(ksigs) / sizeof(ksigs[0]);

static int kill_list(int status)
{
	for (size_t i = 1; i < nsigs; i++) {
		if (isdigit(ksigs[i].name[0])) {
			continue;
		}

		if (status == -1 || status == ksigs[i].num) {
			printf("%s", ksigs[i].name);
			if (status == -1 && i != nsigs - 1) {
				putchar(' ');
			}
		}
	}
	putchar('\n');
	return 0;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	char *signame = "TERM";
	int list = 0;

	/* TODO: insert -s to support XSI -signal_name and -signal_number */

	int c;
	while ((c = getopt(argc, argv, "s:l")) != -1) {
		switch (c) {
		case 's':	/** send signal [signal_name] **/
			signame = optarg;
			break;

		case 'l':	/** list signal names **/
			list = 1;
			break;

		default:
			return 1;
		}
	}

	if (list) {
		int exit_status = -1;
		if (argc > optind) {
			exit_status = atoi(argv[optind]);
			exit_status -= OFFSET_OF_EXIT_STATUS;
		}
		return kill_list(exit_status);
	}

	if (argc <= optind) {
		fprintf(stderr, "kill: missing operand\n");
		return 1;
	}

	int pid = atoi(argv[optind]);

	for (size_t i = 0; i < nsigs; i++) {
		if (!strcasecmp(signame, ksigs[i].name)) {
			if (kill(pid, ksigs[i].num) != 0) {
				perror("kill");
				return 1;
			}
			return 0;
		}
	}

	fprintf(stderr, "kill: %s: %s\n", signame, strerror(errno));
	return 1;
}
