/*
 * Copyright (c) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * This file contains code for the ChanServ CLEAR USERS function.
 */

#include "atheme.h"

static mowgli_patricia_t **cs_clear_cmds = NULL;

static void
cs_cmd_clear_users(struct sourceinfo *si, int parc, char *parv[])
{
	char fullreason[200];
	struct channel *c;
	char *channel = parv[0];
	struct mychan *mc = mychan_find(channel);
	struct chanuser *cu;
	mowgli_node_t *n, *tn;
	int oldlimit;
	unsigned int nmembers;

	if (parc >= 2)
		snprintf(fullreason, sizeof fullreason, "CLEAR USERS used by %s: %s", get_source_name(si), parv[1]);
	else
		snprintf(fullreason, sizeof fullreason, "CLEAR USERS used by %s", get_source_name(si));

	if (!mc)
	{
		command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), channel);
		return;
	}

	if (!(c = channel_find(channel)))
	{
		command_fail(si, fault_nosuch_target, _("\2%s\2 is currently empty."), channel);
		return;
	}

	if (!chanacs_source_has_flag(mc, si, CA_RECOVER))
	{
		command_fail(si, fault_noprivs, _("You are not authorized to perform this operation."));
		return;
	}

	if (metadata_find(mc, "private:close:closer"))
	{
		command_fail(si, fault_noprivs, _("\2%s\2 is closed."), channel);
		return;
	}

	command_add_flood(si, MOWGLI_LIST_LENGTH(&c->members) > 3 ? FLOOD_HEAVY : FLOOD_MODERATE);

	// stop a race condition where users can rejoin
	oldlimit = c->limit;
	if (oldlimit != 1)
		modestack_mode_limit(chansvs.nick, c, MTYPE_ADD, 1);
	modestack_flush_channel(c);

	MOWGLI_ITER_FOREACH_SAFE(n, tn, c->members.head)
	{
		cu = n->data;

		// don't kick the user who requested the masskick
		if (cu->user == si->su || is_internal_client(cu->user))
			continue;

		nmembers = MOWGLI_LIST_LENGTH(&c->members);
		try_kick(chansvs.me->me, c, cu->user, fullreason);

		/* If there are only two users remaining before the kick,
		 * it is possible that the last user is chanserv which will
		 * part if leave_chans is enabled. If it is a permanent
		 * channel this will leave an empty channel, otherwise the
		 * channel will have been destroyed. In either case, it is
		 * not safe to continue.
		 *
		 * Kicking the last user is safe, tn will be NULL and
		 * MOWGLI_ITER_FOREACH_SAFE will stop without touching any part
		 * of the channel structure.
		 */
		if (nmembers == 2 && ((c = channel_find(channel)) == NULL ||
					MOWGLI_LIST_LENGTH(&c->members) == 0))
			break;
	}

	// the channel may be empty now, so our pointer may be bogus!
	c = channel_find(channel);
	if (c != NULL)
	{
		if ((mc->flags & MC_GUARD) && !config_options.leave_chans
				&& c != NULL &&
				(si->su == NULL || !chanuser_find(c, si->su)))
		{
			/* Always cycle it if the requester is not on channel
			 * -- jilles */
			part(channel, chansvs.nick);
		}

		// could be permanent channel, blah
		c = channel_find(channel);
		if (c != NULL)
		{
			if (oldlimit == 0)
				modestack_mode_limit(chansvs.nick, c, MTYPE_DEL, 0);
			else if (oldlimit != 1)
				modestack_mode_limit(chansvs.nick, c, MTYPE_ADD, oldlimit);
		}
	}

	logcommand(si, CMDLOG_DO, "CLEAR:USERS: \2%s\2", mc->name);

	command_success_nodata(si, _("Cleared users from \2%s\2."), channel);
}

static struct command cs_clear_users = {
	.name           = "USERS",
	.desc           = N_("Kicks all users from a channel."),
	.access         = AC_NONE,
	.maxparc        = 2,
	.cmd            = &cs_cmd_clear_users,
	.help           = { .path = "cservice/clear_users" },
};

static void
mod_init(struct module *const restrict m)
{
	MODULE_TRY_REQUEST_SYMBOL(m, cs_clear_cmds, "chanserv/clear", "cs_clear_cmds");

	command_add(&cs_clear_users, *cs_clear_cmds);
}

static void
mod_deinit(const enum module_unload_intent ATHEME_VATTR_UNUSED intent)
{
	command_delete(&cs_clear_users, *cs_clear_cmds);
}

SIMPLE_DECLARE_MODULE_V1("chanserv/clear_users", MODULE_UNLOAD_CAPABILITY_OK)
