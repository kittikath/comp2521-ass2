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
//int numTrailMoves(int numTrail, PlaceId *trail);

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
	// TODO: DONE!
    

	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE) {
        *numReturnedMoves = 0;
        return NULL;
    }
    
    bool canFreeMoves = TRUE;
    int numMoves = 0;
    PlaceId *trailMoves = GvGetLastMoves(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numMoves, &canFreeMoves);
    
    bool canFreeLocations = TRUE;
    int numLocations = 0;
    PlaceId *trailLocations = GvGetLastLocations(dv->gameView, PLAYER_DRACULA, TRAIL_SIZE, &numLocations, 
                                                 &canFreeLocations);
    
    int numReachables = 0;    
    PlaceId *reachables = GvGetReachable(dv->gameView, PLAYER_DRACULA, DvGetRound(dv), DvGetPlayerLocation(dv, 
                                         PLAYER_DRACULA), &numReachables);

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
    
    // if no double backs are present in the trail
    // build array to store valid double back locations
    int numValidDoubleBackLocations = 0;
    PlaceId doubleBackLocs[numLocations];
    if (!doubleBack) {      
      for (int i = 0; i < numLocations; i++) {
         bool isReachable = false;
         for (int k = 0; k < numReachables; k++) {
            // check if double back is reachable
            if (trailLocations[i] == reachables[k]) {
               isReachable = true;
            }
         }
         // add location
         if (isReachable) {
            doubleBackLocs[i] = trailLocations[i];
            numValidDoubleBackLocations++;
         } else {
            doubleBackLocs[i] = NOWHERE;
         }
      }
   }
   
   // building a valid moves array
   
   // number of moves outside the trail
   int numValidMoves = numReachables - numTrailLocations(numMoves, trailMoves);
   
   // add hide move if none in trail   
   if (!hide) {
      numValidMoves += 1;
   }
   
   // add back number of valid double backs
   numValidMoves += numValidDoubleBackLocations;
    
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
      for (int i = 0; i < numLocations; i++) {
         if (doubleBackLocs[i] != NOWHERE) {
            validMoves[j] = DOUBLE_BACK_1 + i;
            j++;
         }
      }
   }
   
   free(trailMoves);
   free(trailLocations);
   free(reachables);

   *numReturnedMoves = numValidMoves;
   return validMoves;
}
   

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
   // TODO:

   if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE) {
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
    
   // if no double backs are present in the trail
   // build array to store valid double back locations
   int numValidDoubleBackLocations = 0;
   PlaceId doubleBackLocs[numLocations];
   if (!doubleBack) {      
   for (int i = 0; i < numLocations; i++) {
      bool isReachable = false;
      for (int k = 0; k < numReachables; k++) {
         // check if double back is reachable
         if (trailLocations[i] == reachables[k]) {
            isReachable = true;
         }
      }
      // add location
         if (isReachable) {
            doubleBackLocs[i] = trailLocations[i];
            numValidDoubleBackLocations++;
         } else {
            doubleBackLocs[i] = NOWHERE;
         }
      }
   }
   
   
   // building a valid moves array
   
   // number of moves outside the trail
   int numValidLocations = numReachables - numTrailLocations(numMoves, trailMoves);

   // add back number of valid double backs
   numValidLocations += numValidDoubleBackLocations;
    
   // add hide location not covered by double back
   if (!hide && doubleBack) {
      numValidLocations += 1;
   }    
    
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
    
   // add double backs if none present in trail
   if (!doubleBack) {
      for (int i = 0; i < numLocations; i++) {
         if (doubleBackLocs[i] != NOWHERE) {
            validLocations[j] = doubleBackLocs[i];
            j++;
         }
      }
   }
    
   // add hide location if not already present
   if (!hide && doubleBack) {
     validLocations[j] = GvGetPlayerLocation(dv->gameView, PLAYER_DRACULA);
     j++;
   }

   printf("numValidLocations: %d\n", numValidLocations);

   free(trailMoves);
   free(trailLocations);
   free(reachables);

   *numReturnedLocs = numValidLocations;
   return validLocations;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: Have not tested
	int round = DvGetRound(dv);
	PlaceId curLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	int numLocs = -1;
	// dracula cannot use rail
	PlaceId *locs = GvGetReachableByType(dv->gameView, PLAYER_DRACULA, round, curLoc, true, false, true, &numLocs);

	// if dracula has not made a move this round yet, return null
	if (DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE) {
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


/*
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
*/
