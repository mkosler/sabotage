#ifndef _SBTG_CARD_H_
#define _SBTG_CARD_H_

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "player-state.h"

enum sbtgCardType { Energy, Modification, Process };

struct sbtgCard
{
    const char *name;
    const char *actionText;
    char *path;
    unsigned int energy;
    unsigned int lifecycle;
    int priority;
    enum sbtgCardType type;
};

struct sbtgCard *sbtgCard_New(const char *name, const char *actionText, unsigned int energy, unsigned int lifecycle, int priority, enum sbtgCardType type);
struct sbtgCard *sbtgCard_LoadFromFile(lua_State *L, const char *path);
void sbtgCard_Free(struct sbtgCard *card);
void sbtgCard_Print(struct sbtgCard *card);
void sbtgCard_CallAction(lua_State *L, struct sbtgCard *card, struct sbtgPlayerState *player, struct sbtgPlayerState *opponent);
int luaopen_sbtgCard(lua_State *L);

#endif //_SBTG_CARD_H_