////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10  v3.0  Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct playerInfo {
	Player player;
   	int health;
   	PlaceId current;
   	PlaceId next;
};


struct gameView {
	// TODO: ADD FIELDS HERE - added a few
	Round round;
	PlayerInfo playerInfo[NUM_PLAYERS];
	int score;
	char *pastPlays;
	Message messages[];
	int currentPlayer; // every time a player has made a move, this should be incremented
};


// helper functions

char *getCurrentTurn(char *pastPlays)


////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	new->score = GAME_START_SCORE;
	//new->round = (strlen(pastPlays)+1)/40;
	new->round = strlen(pastPlays)/40; // don't think the +1 is needed
	printf("round %d\n", new->round);
	new->pastPlays = pastPlays;
	printf("plays %s\n", new->pastPlays);

	//     ------ MESSAGES IS NOT WORKING -------
	// error: assignment to expression with array type
	// for(int j = 0; j < new->round*5; j++){
	// 	new->messages[j] = messages[j];

	// 	printf("%s messages\n", new->messages[j]);
	// }
	
	// initialise health and other stats
	// only has health right now
	for (int i = 0; i < NUM_PLAYERS; i++) {
		// sets player
		new->playerInfo[i].player = i;
		// for dracula
		if (i == PLAYER_DRACULA) {
			new->playerInfo[i].health = GAME_START_BLOOD_POINTS;
		}
		// for hunters
		else {
			new->playerInfo[i].health = GAME_START_HUNTER_LIFE_POINTS;
		}
		printf("health = %d\n", new->PlayerInfo[i].health);
	}

	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: DONE!
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: DONE! could fix so there isn't an extra field in GameView
	return gv->playerInfo[currentPlayer % NUM_PLAYERS].player;
}

int GvGetScore(GameView gv)
{
	// TODO: DONE!
	return gv->score ;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: DONE!
	return gv->playerInfo[player].health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	
	
	
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO:


// helper function definitions:

// returns the last line of the pastPlays string
char *getCurrentTurn(char *pastPlays)
{
    char *currentTurn = strchr(pastPlays, '\n');
    
    return currentTurn == NULL ? pastPlays : currentTurn + 1;
}
