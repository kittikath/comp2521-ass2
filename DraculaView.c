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
#include "Places.h"
// add your own #includes here

#define TRUE 1
#define FALSE 0

// TODO: ADD YOUR OWN STRUCTS HERE
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                        int *numReturnedMoves);
char *getPlayerMove(char *pastPlays, Player player, Round round);

int numTrailLocations(int numTrail, PlaceId *trail);
int numTrailMoves(int numTrail, PlaceId *trail);

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
    
//////////////////////////////////////////////////////////////

	int round = DvGetRound(dv);
	if (round == 0 && GvGetPlayer(dv->gameView) <= PLAYER_DRACULA) {
        *numReturnedMoves = 0;
        return NULL;
    }
    
    bool canFreeMoves = TRUE;
    int numMoves = 0;
    PlaceId *trailMoves = GvGetLastMoves(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numMoves, &canFreeMoves);
    
    bool canFreeLocations = TRUE;
    int numLocations = 0;
    PlaceId *trailLocations = GvGetLastLocations(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numLocations, &canFreeLocations);
    
    int numReachables = 0;    
    PlaceId *reachables = GvGetReachable(dv->gameView, PLAYER_DRACULA, DvGetRound(dv), DvGetPlayerLocation(dv, PLAYER_DRACULA), &numReachables);

    // looking through trail moves, finding DOUBLE_BACK and HIDE moves
    bool hide = false;
    bool doubleBack = false;

    for (int i = 0; i < numMoves; i++) {
        switch (trailMoves[i]) {
            case HIDE:
                hide = true;
                break;
             case DOUBLE_BACK_1:
                doubleBack = true;
                break;
             case DOUBLE_BACK_2:
                doubleBack = true;
                break;
             case DOUBLE_BACK_3:
                doubleBack = true;
                break;
             case DOUBLE_BACK_4:
                doubleBack = true;
                break;
             case DOUBLE_BACK_5:
                doubleBack = true;
                break;
			   default:
				   continue;
        }
    }
    
    // building a valid moves array    
    int numValidMoves = numReachables;
    
    // removing double back and hiding moves
    numValidMoves -= numTrailMoves(numMoves, trailMoves);
    
    if (!doubleBack) {
      numValidMoves += 1;
    }
    
    /*
    // if he has already hidden, one less move
    if (hide) {
        numValidMoves -= 1;
    }
    
    // if he has already double backed, trail locations are removed
    if (doubleBack) {
        numValidMoves -= numLocations;
    } else {
        numValidMoves += 1;
    }
    */
    
    if (numValidMoves == 0) {
        free(trailMoves);
        free(trailLocations);
        free(reachables);
        *numReturnedMoves = 0;
        return NULL;   
    }
        
    PlaceId *validMoves = malloc(numValidMoves * sizeof(*validMoves));
    // populating valid moves array
    
    // adding reachable locations, except for those that are already in the trail
    int j = 0;
    for (int i = 0; i < numReachables; i++) {
        bool inTrail = false;
        // checks if the location is in the trail
        for (int k = 0; k < numLocations; k++) {
            if (reachables[i] == trailLocations[k]) {
                inTrail = true;
            }
        }
        // adds into validMoves if not in trail
        if (!inTrail) {
            validMoves[j] = reachables[i];
            j++;
        }
    }
    
    // add hide move if none present in trail
    if (!hide) {
        validMoves[j] = HIDE;
        j++;
    }
    
    // add double backs if none present in trail
    if (!doubleBack) {
        for (int i = 0; j < numValidMoves; i++) {
            validMoves[j] = DOUBLE_BACK_1 + i;
            j++;
        }
    }
    
    free(trailMoves);
    free(trailLocations);
    free(reachables);
    
    *numReturnedMoves = numValidMoves;
    return validMoves;
    
/*

//////////////////////////////////////////////////////////////
    
	*numReturnedMoves = 0;
	PlaceId *move = playerLastMoves(dv->gameView, PLAYER_DRACULA, 5, numReturnedMoves);
	int tmp = *numReturnedMoves;

	for (int i = 0; i < tmp; i++) {
		const char *movename = placeIdToName(move[i]);
		printf("placename is %s\n", movename);
		
		printf("%d\n", move[0]);
	}
    
	int round = DvGetRound(dv);
	// bools
	bool alreadyVisited = false;
	bool hasHide = false;
	bool hasDoubleBack1 = false;
	bool hasDoubleBack2 = false;
	bool hasDoubleBack3 = false;
	bool hasDoubleBack4 = false;
	bool hasDoubleBack5 = false;
	int count = 0;


	//int numValidMoves = numReturnedMoves;
	// get current move and location
	//char *currMove = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, round);
	PlaceId currLocation = GvGetPlayerLocation(dv->gameView, PLAYER_DRACULA);

	// need to find out how many legal moves is possible first //

	// run through how many places are reachable from current location	
	int numReturnedLocs = 0;
	int numTrailLocs = 0;
	// [GALATZ, KLAUSENBURG]
	PlaceId *reachable = GvGetReachableByType(dv->gameView, PLAYER_DRACULA, round, currLocation, 
						 true, false, true, &numReturnedLocs);
	PlaceId *locationTrail = playerLastMoves(dv->gameView, PLAYER_DRACULA, 5, &numTrailLocs);
	printf("//reachable location is %d//\n", reachable[1]);
	printf("reachable locs is %d\n", numReturnedLocs);

	// run through dracula's past 5 turns to collect data
	for (int i = round; i > 0; i--) {
		printf("hi\n");
		PlaceId location = GvGetPlayerLocation(dv->gameView, PLAYER_DRACULA);
		char *move = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, i);
		if (move != NULL) {
			// if dracula has been to these location in past 5 turns
			for (int j = 0; j < numReturnedLocs; j++) {
				if (location == locationTrail[j]) {
					alreadyVisited = true;
				}
			}
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
			else {
				// teleport to castle dracula
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
	int numValidMoves = numReturnedLocs + count;

	PlaceId *validMoves = malloc(numReturnedLocs * sizeof(*validMoves));

	for (int i = 0; i < numValidMoves; i++) {
		if (alreadyVisited == false) {
			validMoves[i] = reachable[i];
			break;
		}
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
	*numReturnedMoves = numValidMoves;
	printf("num valid moves is %d\n", numValidMoves);
	return validMoves;
	*/
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
   // TODO:

	int round = DvGetRound(dv);
	if (round == 0 && GvGetPlayer(dv->gameView) <= PLAYER_DRACULA) {
        *numReturnedLocs = 0;
        return NULL;
    }
    
    bool canFreeMoves = TRUE;
    int numMoves = 0;
    PlaceId *trailMoves = GvGetLastMoves(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numMoves, &canFreeMoves);
    
    bool canFreeLocations = TRUE;
    int numLocations = 0;
    PlaceId *trailLocations = GvGetLastLocations(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numLocations, &canFreeLocations);
    
    int numReachables = 0;    
    PlaceId *reachables = GvGetReachable(dv->gameView, PLAYER_DRACULA, DvGetRound(dv), DvGetPlayerLocation(dv, PLAYER_DRACULA), &numReachables);

    // looking through trail moves, finding DOUBLE_BACK and HIDE moves
    bool hide = false;
    bool doubleBack = false;

    for (int i = 0; i < numMoves; i++) {
        switch (trailMoves[i]) {
            case HIDE:
                hide = true;
                break;
             case DOUBLE_BACK_1:
                doubleBack = true;
                break;
             case DOUBLE_BACK_2:
                doubleBack = true;
                break;
             case DOUBLE_BACK_3:
                doubleBack = true;
                break;
             case DOUBLE_BACK_4:
                doubleBack = true;
                break;
             case DOUBLE_BACK_5:
                doubleBack = true;
                break;
			   default:
				   continue;
        }
    }
    
    // building a valid moves array    
    int numValidLocations = numReachables;
    
    // removing double back and hiding locations
    numValidLocations -= numTrailLocations(numMoves, trailMoves);
    
    if (numValidLocations == 0) {
        free(trailMoves);
        free(trailLocations);
        free(reachables);
        *numReturnedLocs = 0;
        return NULL;   
    }
        
    PlaceId *validLocations = malloc(numValidLocations * sizeof(*validLocations));
    // populating valid moves array
    
    // adding reachable locations, except for those that are already in the trail
    int j = 0;
    for (int i = 0; i < numReachables; i++) {
        bool inTrail = false;
        // checks if the location is in the trail
        for (int k = 0; k < numLocations; k++) {
            if (reachables[i] == trailLocations[k]) {
                inTrail = true;
            }
        }
        // adds into validMoves if not in trail
        if (!inTrail) {
            validLocations[j] = reachables[i];
            j++;
        }
    }

    // add double locations if none present in trail
    if (!doubleBack) {
        for (int i = 1; j < numValidLocations; i++) {
            validLocations[j] = trailLocations[numLocations - i];
            j++;
        }
    }
    
     // add hide location if not already present
    if (!hide && doubleBack) {
        validLocations[j] = trailLocations[numLocations - 1];
        j++;
    }
    
    free(trailMoves);
    free(trailLocations);
    free(reachables);
    
    *numReturnedLocs = numValidLocations;
    return validLocations;

/*
	// TODO: Have not tested
	int round = DvGetRound(dv);
	PlaceId curLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	char *curMove = getPlayerMove(dv->pastPlays, PLAYER_DRACULA, round);

	int numLocs = 0;
	PlaceId *locs = GvGetReachable(dv->gameView, PLAYER_DRACULA, round, curLoc, &numLocs);
	//printf("hi\n");

	PlaceId *moveArray = malloc(numLocs * sizeof(*moveArray));

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
			moveArray[0] = TELEPORT;
		}
		else {
			for (int i = 0; i < numLocs; i++) {
				moveArray[i] = locs[i];
			}
			*numReturnedLocs = numLocs;
		}
		return moveArray;
	}
	*/
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

// counts the number of locations in the trail
int numTrailLocations(int numTrail, PlaceId *trail)
{
   // TODO:

   int count = 0;   
   for (int i = 0; i < numTrail; i++) {
      if (placeIsReal(trail[i])) {
         count++;
      }   
   }
   return count;
}

// counts the number of moves in the trail
int numTrailMoves(int numTrail, PlaceId *trail)
{
   // TODO:

   int count = 0;   
   for (int i = 0; i < numTrail; i++) {
      if (!placeIsReal(trail[i])) {
         count++;
      }   
   }
   return count;
}







