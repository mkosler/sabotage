#include "card.h"
#include "lua-utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#elif
#include <unistd.h>
#endif //_WIN32

static const char *CARD_TYPE_STRINGS[] = { "Energy", "Modification", "Process", NULL };

void pushCardToLuaStack(lua_State *L, const char *path)
{
    long fSz = 0;
    char cwd[4096];
    char fullPath[4096];
    char *buffer = NULL, *returnBuffer = NULL;
    FILE *f = NULL;

    getcwd(cwd, 4096);
    sprintf_s(fullPath, 4096, "%s\\%s", cwd, path);

    if (fopen_s(&f, path, "rb") != 0) {
        fprintf(stderr, "problem opening file (%s)\n", path);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    fSz = ftell(f);
    rewind(f);

    buffer = (char*)malloc(fSz + 1);
    returnBuffer = (char*)malloc(fSz + 7 + 1);

    if (fread(buffer, 1, fSz, f) != fSz) {
        free(buffer);
        fclose(f);
        fprintf(stderr, "problem reading file (%s)", path);
        exit(1);
    }

    buffer[fSz] = '\0';

    fclose(f);

    sprintf_s(returnBuffer, fSz + 7 + 1, "return %s", buffer);

    if (luaL_dostring(L, returnBuffer)) {
        fprintf(stderr, "could not load as lua (%s): %s", path, lua_tostring(L, -1));
        free(returnBuffer);
        free(buffer);
        exit(1);
    }

    free(returnBuffer);
    free(buffer);
}

struct sbtgCard *sbtgCard_New(const char *name, const char *actionText, unsigned int energy, unsigned int lifecycle, int priority, enum sbtgCardType type)
{
    struct sbtgCard *c = (struct sbtgCard*)malloc(sizeof(struct sbtgCard));
    c->name = name;
    c->actionText = actionText;
    c->energy = energy;
    c->lifecycle = lifecycle;
    c->priority = priority;
    c->type = type;
    c->path = NULL;

    return c;
}

struct sbtgCard *sbtgCard_LoadFromFile(lua_State *L, const char *path)
{
    struct sbtgCard *c = NULL;

    pushCardToLuaStack(L, path);

    lua_getfield(L, 1, "name");
    lua_getfield(L, 1, "actionText");
    lua_getfield(L, 1, "energy");
    lua_getfield(L, 1, "lifecycle");
    lua_getfield(L, 1, "priority");
    lua_getfield(L, 1, "type");

    c = (struct sbtgCard*)malloc(sizeof(struct sbtgCard));
    c->name = luaL_checkstring(L, -6);
    c->actionText = luaL_checkstring(L, -5);
    c->energy = luaL_checkinteger(L, -4);
    c->lifecycle = luaL_checkinteger(L, -3);
    c->priority = luaL_checkinteger(L, -2);
    c->type = luaL_checkoption(L, -1, NULL, CARD_TYPE_STRINGS);
    c->path = (char*)calloc((strlen(path) + 1), sizeof(char));
    strncpy_s(c->path, (strlen(path) + 1) * sizeof(char), path, strlen(path));

    lua_pop(L, 7);

    return c;
}

void sbtgCard_Free(struct sbtgCard *card)
{
    free(card->path);
    free(card);
}

void sbtgCard_Print(struct sbtgCard *card)
{
    printf("%s:\n", card->name);
    printf("\t\"%s\"\n", card->actionText);
    printf("\tEnergy: %d\n", card->energy);
    printf("\tLifecycle: %d\n", card->lifecycle);
    printf("\tPriority: %d\n", card->priority);
}

// LUA_BINDINGS
void sbtgCard_CallAction(lua_State *L, struct sbtgCard *card, struct sbtgPlayerState *player, struct sbtgPlayerState *opponent)
{
    struct sbtgPlayerState **pudata = (struct sbtgPlayerState**)lua_newuserdata(L, sizeof(struct sbtgPlayerState*)); // stack: userdata
    struct sbtgPlayerState **oudata = (struct sbtgPlayerState**)lua_newuserdata(L, sizeof(struct sbtgPlayerState*)); // stack: userdata userdata
    *pudata = player;
    *oudata = opponent;

    luaL_getmetatable(L, "sbtg.PlayerState");
    lua_pushvalue(L, -1);
    lua_setmetatable(L, 1);
    lua_setmetatable(L, 2);

    pushCardToLuaStack(L, card->path);

    lua_getfield(L, -1, "action");
    lua_rotate(L, 1, -2);

    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        fprintf(stderr, "problem calling action callback: %s\n", lua_tostring(L, -1));
        exit(1);
    }
    
    lua_pop(L, 1);
}

static int getName(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushstring(L, c->name);
    return 1;
}

static int getActionText(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushstring(L, c->actionText);
    return 1;
}

static int getEnergy(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushinteger(L, c->energy);
    return 1;
}

static int getLifecycle(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushinteger(L, c->lifecycle);
    return 1;
}

static int getPriority(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushinteger(L, c->priority);
    return 1;
}

static int getType(lua_State *L)
{
    struct sbtgCard *c = *(struct sbtgCard**)luaL_checkudata(L, 1, "sbtg.Card");
    luaL_argcheck(L, c != NULL, 1, "'sbtgCard' expected");
    lua_pushstring(L, CARD_TYPE_STRINGS[c->type]);
    return 1;
}

static const struct luaL_Reg sbtgCardlib[] = {
    {"getName", getName},
    {"getActionText", getActionText},
    {"getEnergy", getEnergy},
    {"getLifecycle", getLifecycle},
    {"getPriority", getPriority},
    {"getType", getType},
    {NULL, NULL}
};

int luaopen_sbtgCard(lua_State *L)
{
    luaL_newmetatable(L, "sbtg.Card");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, sbtgCardlib, 0);
    return 1;
}