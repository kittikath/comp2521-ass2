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
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                        int *numReturnedMoves);
char *getPlayerMove(char *pastPlays, Player player, Round round);

struct draculaView {
	// TODO: ADD FIELDS HERE
	GameView gameView;
	char *pastPlays;
	Message *messages;
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
	// import from gameview
	new->pastPlays = pastPlays;
	new->messages = messages;
	new->gameView = GvNew(pastPlays, messages);

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
	return GvGetTrapLocations(dv->gameView, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	/*
	PlaceId *move = playerLastMoves(dv->gameView, PLAYER_DRACULA, 5, numReturnedMoves);
	int tmp = *numReturnedMoves;

	for (int i = 0; i < tmp; i++) {
		const char *movename = placeIdToName(move[i]);
		printf("placename is %s\n", movename);
		
		printf("%d\n", move[0]);
	}
	*/

	int round = DvGetRound(dv);
	// bools
	bool hasHide = false;
	bool hasDoubleBack1 = false;
	bool hasDoubleBack2 = false;
	bool hasDoubleBack3 = false;
	bool hasDoubleBack4 = false;
	bool hasDoubleBack5 = false;
	int count = 0;
	int i = 0;
	printf("round %d\n", round);


	//int numValidMoves = numReturnedMoves;
	// get current move and location
	char *currMove = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, round);
	PlaceId currLocation = GvGetPlayerLocation(dv->gameView, PLAYER_DRACULA);

	// need to find out how many legal moves is possible first //

	// run through how many places are reachable from current location	
	int numReturnedLocs = 0;
	PlaceId *reachable = GvGetReachableByType(dv->gameView, PLAYER_DRACULA, round, currLocation, true, false, true, &numReturnedLocs);

	// run through dracula's past 5 turns to collect data
	for (int i = round; i > 0; i--) {
		printf("hi\n");
		PlaceId location = GvGetPlayerLocation(dv->gameView, PLAYER_DRACULA);
		char *move = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, i);
		if (move != NULL) {
			if (location == HIDE) {
				hasHide = true;
			}
			if (location == DOUBLE_BACK_1) {
				hasDoubleBack1 = true;
			}
			if (location == DOUBLE_BACK_2) {
				hasDoubleBack2 = true;
			}
			if (location == DOUBLE_BACK_3) {
				hasDoubleBack3 = true;
			}
			if (location == DOUBLE_BACK_4) {
				hasDoubleBack4 = true;
			}
			if (location == DOUBLE_BACK_5) {
				hasDoubleBack5 = true;
			}
		}
	}

	// if 'hide' or any 'double back' move was found, increment
	if (hasHide == true) {
		count++;
	}
	if (hasDoubleBack1 == true) {
		count++;
	}
	if (hasDoubleBack2 == true) {
		count++;
	}
	if (hasDoubleBack3 == true) {
		count++;
	}
	if (hasDoubleBack4 == true) {
		count++;
	}
	if (hasDoubleBack5 == true) {
		count++;
	}

	// calculate total number of valid moves
	// 6 hide/doubleback moves + num of reachable locations - illegal moves
	int numValidMoves = 6 + numReturnedLocs - count;

	PlaceId *validMoves = malloc(numReturnedLocs * sizeof(*validMoves));

	for (int i = 0; i < numValidMoves; i++) {
		char *move = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, i);
      	char *abbrev = strndup(move + 1, 2);
		if (hasHide == false) {
			validMoves[i] = HIDE;
			break;
		}
		if (hasDoubleBack1 == false) {
			validMoves[i] = DOUBLE_BACK_1;
			break;
		}
		if (hasDoubleBack2 == false) {
			validMoves[i] = DOUBLE_BACK_2;
			break;
		}
		if (hasDoubleBack3 == false) {
			validMoves[i] = DOUBLE_BACK_3;
			break;
		}
		if (hasDoubleBack4 == false) {
			validMoves[i] = DOUBLE_BACK_4;
			break;
		}
		if (hasDoubleBack5 == false) {
			validMoves[i] = DOUBLE_BACK_5;
			break;
		}
	}


	// Vampire move
	// if the round is 0, 13, 26(divisible by 13) etc.. place a vampire
	// when he visits a city 
	// cannot place if there are already 3 encounters in city
	// cannot be used in sea

	// Trap move
	// when he visits a city, drop a trap unless he is able to drop a vampire
	// cannot place if there are already 3 encounters in city
	// cannot be used in sea

	// Location move
	// can only move to a adjacent location via road or boat
	// cannot move to location if dracula has been there in the past 5 rounds
	// can not move to hospital

	// Hide move
	// can only stay in the same city he was in
	// cannot make a hide move if he has made one in the past 5 founds
	// cannot make a hide move at sea

	// Double back move
	// can make a move to a location adjacent to his current location that he
	// has been to in his past 5 rounds
	// cannot make a double back move if he has done so in the past 5 rounds

	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: Have not tested
	int round = DvGetRound(dv);
	PlaceId curLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	char *curMove = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, round);

	int numLocs = -1;
	PlaceId *locs = GvGetReachable(dv->gameView, PLAYER_DRACULA, round, curLoc, &numLocs);
	printf("hi\n");
	// if dracula has not made a move this round yet, return null
	if (curMove == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}
	// dracula has made a move
	else {
		// if there are no locations to go to
		if (numLocs == 0) {
			// teleport to castle dracula
			*numReturnedLocs = 1;
		}
		else {
			*numReturnedLocs = numLocs;
		}
			return locs;
	}
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: Have not tested
	int round = DvGetRound(dv);
	PlaceId curLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	char *curMove = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, round);

	int numLocs = -1;
	// dracula cannot use rail
	PlaceId *locs = GvGetReachableByType(dv->gameView, PLAYER_DRACULA, round, curLoc, true, false, true, &numLocs);

	// if dracula has not made a move this round yet, return null
	if (curMove == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}
	// dracula has made a move
	else {
		// if there are no locations to go to
		if (numLocs == 0) {
			// teleport to castle dracula
			*numReturnedLocs = 1;
		}
		else {
			*numReturnedLocs = numLocs;
		}
		return locs;
	}
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

