
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
    printf("Basic initialisation\n");

    char *trail = "";
    Message messages[] = {};
    printf("%d\n", PLAYER_LORD_GODALMING);
    GameView gv = GvNew(trail, messages);
    return EXIT_SUCCESS;
}