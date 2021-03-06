/*
 * Copyright (C) 2005 William Pitcock, et al.
 * Rights to this code are as documented in doc/LICENSE.
 *
 * Data structures for connected clients.
 */

#ifndef ATHEME_INC_USERS_H
#define ATHEME_INC_USERS_H 1

struct user
{
	struct atheme_object    parent;
	stringref               nick;
	stringref               user;
	stringref               host;           // Real host
	stringref               gecos;
	stringref               chost;          // Cloaked host
	stringref               vhost;          // Visible host
	stringref               uid;            // Used for TS6, P10, IRCNet ircd
	stringref               ip;
	mowgli_list_t           channels;
	struct server *         server;
	struct myuser *         myuser;
	unsigned int            offenses;
	unsigned int            msgs;           // times FLOOD_MSGS_FACTOR
	time_t                  lastmsg;
	unsigned int            flags;
	time_t                  ts;
	mowgli_node_t           snode;          // for struct server -> userlist
	char *                  certfp;         // client certificate fingerprint
};

#define FLOOD_MSGS_FACTOR 256

#define UF_AWAY        0x00000002
#define UF_INVIS       0x00000004
#define UF_IRCOP       0x00000010
#define UF_ADMIN       0x00000020
#define UF_SEENINFO    0x00000080
#define UF_IMMUNE      0x00000100 /* user is immune from kickban, don't bother enforcing akicks */
#define UF_HIDEHOSTREQ 0x00000200 /* host hiding requested */
#define UF_SOPER_PASS  0x00000400 /* services oper pass entered */
#define UF_DOENFORCE   0x00000800 /* introduce enforcer when nick changes */
#define UF_ENFORCER    0x00001000 /* this is an enforcer client */
#define UF_WASENFORCED 0x00002000 /* this user was FNCed once already */
#define UF_DEAF        0x00004000 /* user does not receive channel msgs */
#define UF_SERVICE     0x00008000 /* user is a service (e.g. +S on charybdis) */
#define UF_KLINESENT   0x00010000 /* we've sent a kline for this user */
#define UF_CUSTOM1     0x00020000 /* for internal use by protocol modules */
#define UF_CUSTOM2     0x00040000
#define UF_CUSTOM3     0x00080000
#define UF_CUSTOM4     0x00100000

#define CLIENT_NAME(user)	((user)->uid != NULL ? (user)->uid : (user)->nick)

typedef struct {
	struct user *    u;             // User in question. Write NULL here if you delete the user
	const char *     oldnick;       // Previous nick for nick changes. u->nick is the new nick
} hook_user_nick_t;

typedef struct {
	struct user * const     u;
	const char *            comment;
} hook_user_delete_t;

/* function.c */
bool is_ircop(struct user *user);
bool is_admin(struct user *user);
bool is_internal_client(struct user *user);
bool is_autokline_exempt(struct user *user);
bool is_service(struct user *user);

/* users.c */
extern mowgli_patricia_t *userlist;
extern mowgli_patricia_t *uidlist;

void init_users(void);

struct user *user_add(const char *nick, const char *user, const char *host, const char *vhost, const char *ip, const char *uid, const char *gecos, struct server *server, time_t ts);
void user_delete(struct user *u, const char *comment);
struct user *user_find(const char *nick);
struct user *user_find_named(const char *nick);
void user_changeuid(struct user *u, const char *uid);
bool user_changenick(struct user *u, const char *nick, time_t ts);
void user_mode(struct user *user, const char *modes);
void user_sethost(struct user *source, struct user *target, const char *host);
const char *user_get_umodestr(struct user *u);
bool user_is_channel_banned(struct user *u, char ban_type);

/* uid.c */
void init_uid(void);
const char *uid_get(void);

#endif /* !ATHEME_INC_USERS_H */
