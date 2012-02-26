/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>
#include <unistd.h>
#include <libubox/uloop.h>

#include "../freecwmp.h"

#include "external.h"

static struct uloop_timeout timeout;
static struct uloop_process uproc;

static void
cancel_child_timeout(struct uloop_process *uproc, int ret)
{
	FC_DEVEL_DEBUG("enter");

	FC_DEVEL_DEBUG("uloop_timeout_cancel(&timeout);");

	FC_DEVEL_DEBUG("exit");
}

void
kill_child_timeout()
{
	FC_DEVEL_DEBUG("enter");

	// TODO: think about ; some scripts make take longer to finish
	FC_DEVEL_DEBUG("killpg(uproc.pid, SIGTERM);");

	FC_DEVEL_DEBUG("exit");
}

int8_t
external_get_parameter(char *name, char **value)
{
	FC_DEVEL_DEBUG("enter");

	int8_t status;

	int pfds[2];
	if (pipe(pfds) < 0)
		return -1;

	if ((uproc.pid = fork()) == -1) {
		goto error;
	}

	if (uproc.pid == 0) {
		/* child */
		dup2(pfds[1], 0);
		dup2(pfds[1], 1);
		dup2(pfds[1], 2);

		close(pfds[0]);
		close(pfds[1]);

		const char *argv[7];
		int i = 0;
		argv[i++] = "sh";
		argv[i++] = fc_script;
		argv[i++] = "--newline";
		argv[i++] = "--value";
		argv[i++] = "get";
		argv[i++] = name;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0) {
		goto error;
	}

	/* parent code */
	close (pfds[1]);

	while (wait(&status) != uproc.pid) {
		FC_DEVEL_DEBUG("waiting for child to exit");
	}

	char buffer[2];
	ssize_t rxed;

	*value = (char *) calloc(1, sizeof(char));
	while ((rxed = read(pfds[0], buffer, sizeof(buffer))) > 0) {
		*value = (char *) realloc(*value, (strlen(*value) + rxed + 1) * sizeof(char));
		if (!(*value))
			goto error;
		bzero(*value + strlen(*value), rxed + 1);
		memcpy(*value + strlen(*value), buffer, rxed);
	}

	if (!strlen(*value)) {
		free(*value);
		*value = NULL;
		goto almost_done;
	}

	if (rxed < 0)
		goto error;
	
almost_done:
	status = FC_SUCCESS;
	goto done;

error:
	status = FC_ERROR;

done:
	FC_DEVEL_DEBUG("exit");
	return status;
}

int8_t
external_set_parameter(char *name, char *value)
{
	FC_DEVEL_DEBUG("enter");
	int8_t status;

	if ((uproc.pid = fork()) == -1) {
		goto error;
	}

	if (uproc.pid == 0) {
		/* child */

		const char *argv[6];
		int i = 0;
		argv[i++] = "sh";
		argv[i++] = fc_script;
		argv[i++] = "set";
		argv[i++] = name;
		argv[i++] = value;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0) {
		goto error;
	}

	/* parent code */

	while (wait(&status) != uproc.pid) {
		FC_DEVEL_DEBUG("waiting for child to exit");
	}

	// TODO: add some kind of checks

	status = FC_SUCCESS;
	goto done;

error:
	status = FC_ERROR;

done:
	FC_DEVEL_DEBUG("exit");
	return status;
}

int8_t
external_simple(char *arg)
{
	FC_DEVEL_DEBUG("enter");
	int8_t status;

	if ((uproc.pid = fork()) == -1) {
		goto error;
	}

	if (uproc.pid == 0) {
		/* child */

		const char *argv[4];
		int i = 0;
		argv[i++] = "sh";
		argv[i++] = fc_script;
		argv[i++] = arg;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0) {
		goto error;
	}

	/* parent code */

	while (wait(&status) != uproc.pid) {
		FC_DEVEL_DEBUG("waiting for child to exit");
	}

	// TODO: add some kind of checks

	status = FC_SUCCESS;
	goto done;

error:
	status = FC_ERROR;

done:
	FC_DEVEL_DEBUG("exit");
	return status;
}

int8_t
external_download(char *url, char *size)
{
	FC_DEVEL_DEBUG("enter");

	int8_t status;

	if ((uproc.pid = fork()) == -1) {
		goto error;
	}

	if (uproc.pid == 0) {
		/* child */

		const char *argv[8];
		int i = 0;
		argv[i++] = "sh";
		argv[i++] = fc_script;
		argv[i++] = "download";
		argv[i++] = "--url";
		argv[i++] = url;
		argv[i++] = "--size";
		argv[i++] = size;
		argv[i++] = NULL;

		execvp(argv[0], (char **) argv);
		exit(ESRCH);

	} else if (uproc.pid < 0) {
		goto error;
	}

	/* parent code */
	int child_status;

	while (wait(&child_status) != uproc.pid) {
		FC_DEVEL_DEBUG("waiting for child to exit");
	}

	if (WIFEXITED(child_status) && !WEXITSTATUS(child_status))
		status = FC_SUCCESS;
	else
		status = FC_ERROR;
	goto done;

error:
	status = FC_ERROR;

done:
	FC_DEVEL_DEBUG("exit");
	return status;
}
