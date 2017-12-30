#include "player-state.h"
#include <stdlib.h>

struct sbtgPlayerState *sbtgPlayerState_New()
{
    int i;
    struct sbtgPlayerState *ps = (struct sbtgPlayerState*)malloc(sizeof(struct sbtgPlayerState));

    ps->life = 10;
    ps->reserve = 0;
    ps->activeCard = NULL;

    for (i = 0; i < DECK_SIZE; i++) {
        ps->deck[i] = NULL;
        ps->scrapHeap[i] = NULL;
        ps->plan[i] = NULL;
    }

    return ps;
}

void sbtgPlayerState_Free(struct sbtgPlayerState *ps)
{
    free(ps);
}

// LUA BINDINGS
static const char *PLAYER_STATE_PILE_TYPES[] = {
    "Deck",
    "ScrapHeap",
    "Plan",
    NULL
};

static int getLife(lua_State *L)
{
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    lua_pushinteger(L, ps->life);
    return 1;
}

static int getReserve(lua_State *L)
{
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    lua_pushinteger(L, ps->reserve);
    return 1;
}

static int setLife(lua_State *L)
{
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    ps->life = luaL_checkinteger(L, 2);
    return 0;
}

static int setReserve(lua_State *L)
{
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    ps->reserve = luaL_checkinteger(L, 2);
    return 0;
}

static int getPile(lua_State *L)
{
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    int i, j, pt = luaL_checkoption(L, 2, NULL, PLAYER_STATE_PILE_TYPES);
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");

    deck_t *pile = NULL;
    switch (pt) {
        case Deck:
            pile = &ps->deck;
            break;
        case ScrapHeap:
            pile = &ps->scrapHeap;
            break;
        case Plan:
            pile = &ps->plan;
            break;
    }

    lua_newtable(L);
    for (i = 0, j = 1; i < DECK_SIZE; i++) {
        if ((*pile)[i] != NULL) {
            struct sbtgCard **cudata = (struct sbtgCard**)lua_newuserdata(L, sizeof(struct sbtgCard*));
            *cudata = (*pile)[i];
            luaL_setmetatable(L, "sbtg.Card");
            lua_seti(L, -2, j++);
        }
    }
    return 1;
}

// static int getDeck(lua_State *L)
// {
//     int i, j = 1;
//     struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
//     luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");

//     lua_newtable(L);
//     for (i = 0; i < DECK_SIZE; i++) {
//         if (ps->deck[i] != NULL) {
//             struct sbtgCard **cudata = (struct sbtgCard**)lua_newuserdata(L, sizeof(struct sbtgCard*));
//             *cudata = ps->deck[i];
//             luaL_setmetatable(L, "sbtg.Card");
//             lua_seti(L, -2, j++);
//         }
//     }
//     return 1;
// }

// static int getScrapHeap(lua_State *L)
// {
//     int i, j = 1;
//     struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
//     luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");

//     lua_newtable(L);
//     for (i = 0; i < DECK_SIZE; i++) {
//         if (ps->scrapHeap[i] != NULL) {
//             struct sbtgCard **cudata = (struct sbtgCard**)lua_newuserdata(L, sizeof(struct sbtgCard*));
//             *cudata = ps->deck[i];
//             luaL_setmetatable(L, "sbtg.Card");
//             lua_seti(L, -2, j++);
//         }
//     }
//     return 1;
// }

// static int getPlan(lua_State *L)
// {
//     int i, j = 1;
//     struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
//     luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");

//     lua_newtable(L);
//     for (i = 0; i < DECK_SIZE; i++) {
//         if (ps->plan[i] != NULL) {
//             struct sbtgCard **cudata = (struct sbtgCard**)lua_newuserdata(L, sizeof(struct sbtgCard*));
//             *cudata = ps->deck[i];
//             luaL_setmetatable(L, "sbtg.Card");
//             lua_seti(L, -2, j++);
//         }
//     }
//     return 1;
// }

static int setDeck(lua_State *L)
{
    int i, len = luaL_len(L, 2);
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    for (i = 1; i <= len; i++) {
        lua_geti(L, 2, i);
        ps->deck[i - 1] = (struct sbtgCard*)lua_touserdata(L, -1);
    }
    return 0;
}

static int setScrapHeap(lua_State *L)
{
    int i, len = luaL_len(L, 2);
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    for (i = 1; i <= len; i++) {
        lua_geti(L, 2, i);
        ps->scrapHeap[i - 1] = (struct sbtgCard*)lua_touserdata(L, -1);
    }
    return 0;
}

static int setPlan(lua_State *L)
{
    int i, len = luaL_len(L, 2);
    struct sbtgPlayerState *ps = *(struct sbtgPlayerState**)luaL_checkudata(L, 1, "sbtg.PlayerState");
    luaL_argcheck(L, ps != NULL, 1, "'PlayerState' expected");
    for (i = 1; i <= len; i++) {
        lua_geti(L, 2, i);
        ps->plan[i - 1] = (struct sbtgCard*)lua_touserdata(L, -1);
    }
    return 0;
}

static const struct luaL_Reg sbtgPlayerStatelib[] = {
    {"getLife", getLife},
    {"getReserve", getReserve},
    {"getPile", getPile},
    {"setLife", setLife},
    {"setReserve", setReserve},
    {"setDeck", setDeck},
    {"setScrapHeap", setScrapHeap},
    {"setPlan", setPlan},
    {NULL, NULL}
};

int luaopen_sbtgPlayerState(lua_State *L)
{
    luaL_newmetatable(L, "sbtg.PlayerState");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, sbtgPlayerStatelib, 0);
    return 1;
}