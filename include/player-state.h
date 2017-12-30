#ifndef _SBTG_PLAYER_STATE_H_
#define _SBTG_PLAYER_STATE_H_

#include "card.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#define DECK_SIZE 30

typedef struct sbtgCard *deck_t[DECK_SIZE];
enum sbtgPlayerStatePileType { Deck, ScrapHeap, Plan };

struct sbtgPlayerState
{
    int life;
    int reserve;
    deck_t deck;
    deck_t scrapHeap;
    deck_t plan;
    struct sbtgCard *activeCard;
};

struct sbtgPlayerState *sbtgPlayerState_New();
void sbtgPlayerState_Free(struct sbtgPlayerState *ps);
int luaopen_sbtgPlayerState(lua_State *L);

#endif //_SBTG_PLAYER_STATE_H_