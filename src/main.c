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
    luaL_requiref(L, "card", luaopen_sbtgCard, 1);
    lua_pop(L, 1);
    luaL_requiref(L, "playerState", luaopen_sbtgPlayerState, 1);
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

    for (int i = 0; i < DECK_SIZE; i++) {
        if (opponent->plan[i] != NULL) {
            puts(opponent->plan[i]->name);
        }
    }

    sbtgCard_CallAction(L, sbtgLibrary_Find(lib, "Sabotage"), player, opponent);

    for (int i = 0; i < DECK_SIZE; i++) {
        if (opponent->plan[i] != NULL) {
            puts(opponent->plan[i]->name);
        }
    }

    sbtgPlayerState_Free(player);
    sbtgPlayerState_Free(opponent);
    sbtgLibrary_Free(lib);
    lua_close(L);

    return 0;
}