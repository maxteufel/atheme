/*
 * Copyright (C) 2005 William Pitcock, et al.
 * Copyright (C) 2018 Atheme Development Group (https://atheme.github.io/)
 *
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Command help routines.
 */

#ifndef ATHEME_INC_COMMANDHELP_H
#define ATHEME_INC_COMMANDHELP_H 1

/* help.c */
void help_display(struct sourceinfo *, const struct service *, const char *, mowgli_patricia_t *);
void help_display_as_subcmd(struct sourceinfo *, const struct service *, const char *, const char *, mowgli_patricia_t *);
void help_display_invalid(struct sourceinfo *, const struct service *, const char *);
void help_display_locations(struct sourceinfo *);
void help_display_moreinfo(struct sourceinfo *, const struct service *, const char *);
void help_display_newline(struct sourceinfo *);
void help_display_prefix(struct sourceinfo *, const struct service *);
void help_display_suffix(struct sourceinfo *);
void help_display_verblist(struct sourceinfo *, const struct service *);
void command_help(struct sourceinfo *, mowgli_patricia_t *);
void command_help_short(struct sourceinfo *, mowgli_patricia_t *, const char *);

#endif /* !ATHEME_INC_COMMANDHELP_H */
