/*
 * Copyright (C) 2014 Martin Willi
 * Copyright (C) 2014 revosec AG
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "command.h"

#include <errno.h>

static int version(vici_conn_t *conn)
{
	vici_req_t *req;
	vici_res_t *res;
	char *arg;
	bool raw = FALSE, daemon = FALSE;;

	while (TRUE)
	{
		switch (command_getopt(&arg))
		{
			case 'h':
				return command_usage(NULL);
			case 'r':
				raw = TRUE;
				continue;
			case 'd':
				daemon = TRUE;
				continue;
			case EOF:
				break;
			default:
				return command_usage("invalid --terminate option");
		}
		break;
	}

	if (!daemon)
	{
		printf("strongSwan swanctl %s\n", VERSION);
		return 0;
	}

	req = vici_begin("version");
	res = vici_submit(req, conn);
	if (!res)
	{
		fprintf(stderr, "version request failed: %s\n", strerror(errno));
		return errno;
	}
	if (raw)
	{
		vici_dump(res, "version reply", stdout);
	}
	else
	{
		printf("strongSwan %s %s (%s, %s, %s)\n",
			vici_find_str(res, "", "version"),
			vici_find_str(res, "", "daemon"),
			vici_find_str(res, "", "sysname"),
			vici_find_str(res, "", "release"),
			vici_find_str(res, "", "machine"));
	}
	vici_free_res(res);
	return 0;
}

/**
 * Register the command.
 */
static void __attribute__ ((constructor))reg()
{
	command_register((command_t) {
		version, 'v', "version", "show version information",
		{"[--raw]"},
		{
			{"help",		'h', 0, "show usage information"},
			{"daemon",		'd', 0, "query daemon version"},
			{"raw",			'r', 0, "dump raw response message"},
		}
	});
}
