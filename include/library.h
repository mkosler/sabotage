#ifndef _SBTG_LIBRARY_H_
#define _SBTG_LIBRARY_H_

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "card.h"
#define LIBRARY_SIZE 512

struct sbtgLibrary
{
    unsigned int size;
    struct sbtgCard **cards;
    char **keys;
};

struct sbtgLibrary *sbtgLibrary_New();
struct sbtgLibrary *sbtgLibrary_FromDirectory(lua_State *L, const char *path);
void sbtgLibrary_Free(struct sbtgLibrary *lib);
void sbtgLibrary_Add(struct sbtgLibrary *lib, char *key, struct sbtgCard *card);
struct sbtgCard *sbtgLibrary_Find(struct sbtgLibrary *lib, char *key);

#endif //_SBTG_LIBRARY_H_