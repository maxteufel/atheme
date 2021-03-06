/*
 * Copyright (c) 2003-2004 E. Will et al.
 * Copyright (c) 2006-2007 Atheme Development Group
 * Rights to this code are documented in doc/LICENSE.
 *
 * This file contains routines to handle the CService SET PRIVATE command.
 */

#include "atheme.h"

static mowgli_patricia_t **cs_set_cmdtree = NULL;

static void
cs_cmd_set_private(struct sourceinfo *si, int parc, char *parv[])
{
	struct mychan *mc;

	if (!(mc = mychan_find(parv[0])))
	{
		command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), parv[0]);
		return;
	}

	if (!parv[1])
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "SET PRIVATE");
		return;
	}

	if (!chanacs_source_has_flag(mc, si, CA_SET))
	{
		command_fail(si, fault_noprivs, _("You are not authorized to perform this command."));
		return;
	}

	if (!strcasecmp("ON", parv[1]))
	{
		if (MC_PRIVATE & mc->flags)
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is already set for \2%s\2."), "PRIVATE", mc->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:PRIVATE:ON: \2%s\2", mc->name);
		verbose(mc, _("\2%s\2 enabled the PRIVATE flag"), get_source_name(si));

		mc->flags |= MC_PRIVATE;

		command_success_nodata(si, _("The \2%s\2 flag has been set for \2%s\2."), "PRIVATE", mc->name);

		return;
	}

	else if (!strcasecmp("OFF", parv[1]))
	{
		if (!(MC_PRIVATE & mc->flags))
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is not set for \2%s\2."), "PRIVATE", mc->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:PRIVATE:OFF: \2%s\2", mc->name);
		verbose(mc, _("\2%s\2 disabled the PRIVATE flag"), get_source_name(si));

		mc->flags &= ~MC_PRIVATE;

		command_success_nodata(si, _("The \2%s\2 flag has been removed for \2%s\2."), "PRIVATE", mc->name);

		return;
	}

	else
	{
		command_fail(si, fault_badparams, STR_INVALID_PARAMS, "PRIVATE");
		return;
	}
}

static struct command cs_set_private = {
	.name           = "PRIVATE",
	.desc           = N_("Hides information about a channel."),
	.access         = AC_NONE,
	.maxparc        = 2,
	.cmd            = &cs_cmd_set_private,
	.help           = { .path = "cservice/set_private" },
};

static void
mod_init(struct module *const restrict m)
{
	MODULE_TRY_REQUEST_SYMBOL(m, cs_set_cmdtree, "chanserv/set_core", "cs_set_cmdtree");

	command_add(&cs_set_private, *cs_set_cmdtree);
}

static void
mod_deinit(const enum module_unload_intent ATHEME_VATTR_UNUSED intent)
{
	command_delete(&cs_set_private, *cs_set_cmdtree);
}

SIMPLE_DECLARE_MODULE_V1("chanserv/set_private", MODULE_UNLOAD_CAPABILITY_OK)
