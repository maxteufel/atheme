/*
 * Copyright (C) 2005 Atheme Project (http://atheme.org/)
 * Copyright (C) 2018 Atheme Development Group (https://atheme.github.io/)
 *
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains routines to handle the BotServ HELP command.
 */

#include "atheme.h"

static void
bs_cmd_help(struct sourceinfo *const restrict si, const int ATHEME_VATTR_UNUSED parc, char **const restrict parv)
{
	if (parv[0])
	{
		(void) help_display(si, si->service, parv[0], si->service->commands);
		return;
	}

	(void) help_display_prefix(si, si->service);

	(void) command_success_nodata(si, _("\2%s\2 allows users to request a custom bot for their channel."),
	                              si->service->nick);

	(void) help_display_newline(si);
	(void) command_help(si, si->service->commands);
	(void) help_display_moreinfo(si, si->service, NULL);
	(void) help_display_locations(si);
	(void) help_display_suffix(si);
}

static struct command bs_help = {
	.name           = "HELP",
	.desc           = N_("Displays contextual help information."),
	.access         = AC_NONE,
	.maxparc        = 1,
	.cmd            = &bs_cmd_help,
	.help           = { .path = "help" },
};

static void
mod_init(struct module ATHEME_VATTR_UNUSED *const restrict m)
{
	(void) service_named_bind_command("botserv", &bs_help);
}

static void
mod_deinit(const enum module_unload_intent ATHEME_VATTR_UNUSED intent)
{
	(void) service_named_unbind_command("botserv", &bs_help);
}

SIMPLE_DECLARE_MODULE_V1("botserv/help", MODULE_UNLOAD_CAPABILITY_OK)
