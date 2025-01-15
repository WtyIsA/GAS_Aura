/*
** $Id: ltable.h $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)	(&(t)->node[i])
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->u.next)


/*
** Clear all bits of fast-access metamethods, which means that the table
** may have any of these metamethods. (First access that fails after the
** clearing will set the bit again.)
*/
#define invalidateTMcache(t)	((t)->flags &= ~maskflags)


/* true when 't' is using 'dummynode' as its hash part */
#define isdummy(t)		((t)->lastfree == NULL)


/* allocated size for hash nodes */
#define allocsizenode(t)	(isdummy(t) ? 0 : sizenode(t))


/* returns the Node, given the value of a table entry */
#define nodefromval(v)	cast(Node *, (v))


LUA_API const TValue *luaH_getint (Table *t, lua_Integer key);
LUA_API void luaH_setint (lua_State *L, Table *t, lua_Integer key,
													TValue *value);
LUA_API const TValue *luaH_getshortstr (Table *t, TString *key);
LUA_API const TValue *luaH_getstr (Table *t, TString *key);
LUA_API const TValue *luaH_get (Table *t, const TValue *key);
LUA_API TValue *luaH_newkey (lua_State *L, Table *t, const TValue *key);
LUA_API TValue *luaH_set (lua_State *L, Table *t, const TValue *key);
LUA_API Table *luaH_new (lua_State *L);
LUA_API void luaH_resize (lua_State *L, Table *t, unsigned int nasize,
													unsigned int nhsize);
LUA_API void luaH_resizearray (lua_State *L, Table *t, unsigned int nasize);
LUA_API void luaH_free (lua_State *L, Table *t);
LUA_API int luaH_next (lua_State *L, Table *t, StkId key);
LUA_API lua_Unsigned luaH_getn (Table *t);
LUA_API unsigned int luaH_realasize (const Table *t);


#if defined(LUA_DEBUG)
LUA_API Node *luaH_mainposition (const Table *t, const TValue *key);
LUA_API int luaH_isdummy (const Table *t);
#endif


#endif
