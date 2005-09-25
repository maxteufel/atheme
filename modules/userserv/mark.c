/*
 * Copyright (c) 2005 William Pitcock
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Marking for accounts.
 *
 * $Id: mark.c 2361 2005-09-25 03:05:34Z nenolod $
 */

#include "atheme.h"

DECLARE_MODULE_V1
(
	"userserv/mark", FALSE, _modinit, _moddeinit,
	"$Id: mark.c 2361 2005-09-25 03:05:34Z nenolod $",
	"Atheme Development Group <http://www.atheme.org>"
);

static void us_cmd_mark(char *origin);

command_t us_mark = { "MARK", "Adds a note to a user.",
			AC_IRCOP, us_cmd_mark };

list_t *us_cmdtree;

void _modinit(module_t *m)
{
	us_cmdtree = module_locate_symbol("userserv/main", "us_cmdtree");
	command_add(&us_mark, us_cmdtree);
}

void _moddeinit()
{
	command_delete(&us_mark, us_cmdtree);
}

static void us_cmd_mark(char *origin)
{
	char *target = strtok(NULL, " ");
	char *action = strtok(NULL, " ");
	char *info = strtok(NULL, "");
	myuser_t *mu;

	if (!target || !action)
	{
		notice(usersvs.nick, origin, "Insufficient parameters for \2MARK\2.");
		notice(usersvs.nick, origin, "Usage: MARK <target> <ON|OFF> [note]");
		return;
	}

	if (!(mu = myuser_find(target)))
	{
		notice(usersvs.nick, origin, "\2%s\2 is not registered.", target);
		return;
	}

	if (!strcasecmp(action, "ON"))
	{
		if (!info)
		{
			notice(usersvs.nick, origin, "Insufficient parameters for \2MARK\2.");
			notice(usersvs.nick, origin, "Usage: MARK <target> ON <note>");
			return;
		}

		if (metadata_find(mu, METADATA_USER, "private:mark:setter"))
		{
			notice(usersvs.nick, origin, "\2%s\2 is already marked.", target);
			return;
		}

		metadata_add(mu, METADATA_USER, "private:mark:setter", origin);
		metadata_add(mu, METADATA_USER, "private:mark:reason", info);
		metadata_add(mu, METADATA_USER, "private:mark:timestamp", itoa(time(NULL)));

		wallops("%s marked the account \2%s\2.", origin, target);
		notice(usersvs.nick, origin, "\2%s\2 is now marked.", target);
	}
	else if (!strcasecmp(action, "OFF"))
	{
		if (!metadata_find(mu, METADATA_USER, "private:mark:setter"))
		{
			notice(usersvs.nick, origin, "\2%s\2 is not marked.", target);
			return;
		}

		metadata_delete(mu, METADATA_USER, "private:mark:setter");
		metadata_delete(mu, METADATA_USER, "private:mark:reason");
		metadata_delete(mu, METADATA_USER, "private:mark:timestamp");

		wallops("%s unmarked the account \2%s\2.", origin, target);
		notice(usersvs.nick, origin, "\2%s\2 is now unmarked.", target);
	}
	else
	{
		notice(usersvs.nick, origin, "Invalid parameters for \2MARK\2.");
		notice(usersvs.nick, origin, "Usage: MARK <target> <ON|OFF> [note]");
	}
}
