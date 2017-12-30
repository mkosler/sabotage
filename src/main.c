#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "card.h"
#include "player-state.h"
#include "library.h"

int main()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_sbtgCard(L);
    lua_pop(L, 1);
    luaopen_sbtgPlayerState(L);
    lua_pop(L, 1);

    struct sbtgPlayerState *player = sbtgPlayerState_New();
    struct sbtgPlayerState *opponent = sbtgPlayerState_New();
    struct sbtgLibrary *lib = sbtgLibrary_FromDirectory(L, "cards/");

    opponent->deck[0] = sbtgLibrary_Find(lib, "Strike");
    opponent->deck[1] = sbtgLibrary_Find(lib, "Peek");
    opponent->deck[2] = sbtgLibrary_Find(lib, "Repair");
    opponent->deck[3] = sbtgLibrary_Find(lib, "Generate");

    opponent->plan[0] = sbtgLibrary_Find(lib, "Strike");
    opponent->plan[1] = sbtgLibrary_Find(lib, "Repair");

    sbtgCard_CallAction(L, sbtgLibrary_Find(lib, "Peek"), player, opponent);

    sbtgPlayerState_Free(player);
    sbtgPlayerState_Free(opponent);
    sbtgLibrary_Free(lib);
    lua_close(L);

    return 0;
}