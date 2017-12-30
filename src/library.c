#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

struct sbtgLibrary *sbtgLibrary_New()
{
    struct sbtgLibrary *lib = (struct sbtgLibrary*)malloc(sizeof(struct sbtgLibrary));
    lib->size = LIBRARY_SIZE;
    lib->cards = (struct sbtgCard**)calloc(lib->size, sizeof(struct sbtgCard*));
    lib->keys = (char **)calloc(lib->size, sizeof(char*));
    return lib;
}

struct sbtgLibrary *sbtgLibrary_FromDirectory(lua_State *L, const char *path)
{
    char fullPath[4096];
    struct sbtgCard *card = NULL;
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    struct sbtgLibrary *lib = sbtgLibrary_New();

    dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "cannot open directory (%s)\n", path);
        exit(1);
    }

    while ((ent = readdir(dir)) != NULL) {
        memset(fullPath, '\0', sizeof(fullPath));
        strncpy_s(fullPath, sizeof(fullPath), path, strlen(path));
        switch (ent->d_type) {
            case DT_REG:
                strcat_s(fullPath, sizeof(fullPath), ent->d_name);
                card = sbtgCard_LoadFromFile(L, fullPath);
                sbtgLibrary_Add(lib, (char*)card->name, card);
                break;
            default:
                break;
        }
    }

    closedir(dir);

    return lib;
}

void sbtgLibrary_Free(struct sbtgLibrary *lib)
{
    int i;
    for (i = 0; i < lib->size; i++) {
        sbtgCard_Free(lib->cards[i]);
        free(lib->keys[i]);
    }
    free(lib->cards);
    free(lib->keys);
    free(lib);
}

unsigned int hash(char *key)
{
    unsigned int h = 5381;
    int c;
    while ((c = *key++)) h = ((h << 5) + h) + c;
    return h;
}

void sbtgLibrary_Add(struct sbtgLibrary *lib, char *key, struct sbtgCard *card)
{
    unsigned int i = hash(key) % lib->size;
    while (lib->keys[i] != NULL) {
        if (strcmp(lib->keys[i], key) == 0) break;
        i = (i + 1) % lib->size;
    }
    lib->keys[i] = key;
    lib->cards[i] = card;
}

struct sbtgCard *sbtgLibrary_Find(struct sbtgLibrary *lib, char *key)
{
    unsigned int i = hash(key) % lib->size;
    while (lib->keys[i] != NULL) {
        if (strcmp(lib->keys[i], key) == 0) break;
        i = (i + 1) % lib->size;
    }
    return lib->cards[i];
}