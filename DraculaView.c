////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

#define TRUE 1
#define FALSE 0

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct vampire {
    bool exist;
    PlaceId place;
    Round roundSpawned;
} Vampire;

typedef struct trap {
    int count;
} Trap;

struct draculaView {
	// TODO: ADD FIELDS HERE
	GameView gameView;
	Vampire vampire;
	Trap trap;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

// initialise traps and vampires
DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	int i;
	// initialise gameview in drac view
	new->gameView = GvNew(pastPlays, messages);

	// if game is in initial state
	if (strlen(pastPlays) == 0) {
		new->vampire.exist = FALSE;
		new->vampire.place = NOWHERE;
		new->vampire.roundSpawned = 0;

		new->trap.count = 0;

		return new;
	}
	// dracula turn NOT FINISHED
	else {
		int turn = 1;
		if (pastPlays[turn] == )
	}


	return new;
}

void DvFree(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: DONE
	return GvGetRound(dv->gameView);
}

int DvGetScore(DraculaView dv)
{
	// TODO: DONE
	return GvGetScore(dv->gameView);
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: DONE
	return GvGetHealth(dv->gameView, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: DONE
	return GvGetPlayerLocation(dv->gameView, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: DONE
	return GvGetVampireLocation(dv->gameView);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: DONE

	*numTraps = 0;
	return;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
