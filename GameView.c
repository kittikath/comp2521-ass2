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

#define TRUE 1
#define FALSE 0
#define PLAY_LEN 8

// TODO: ADD YOUR OWN STRUCTS HERE

struct gameView {
	// TODO: ADD FIELDS HERE - added a few
	char *pastPlays;
	Message *messages;
};


// helper functions

char *getPlayerMove(char *pastPlays, Player player, Round round);
bool placeMatch(char *pastPlays, Player player, PlaceId Place,
                Round roundStart, Round roundEnd);
Round placeBeenF(char *pastPlays, Player player, PlaceId place);
Round placeBeenL(char *pastPlays, Player player, PlaceId place);


PlaceId *HunterMoveLocationHistory(GameView gv, Player player,
                                   int *numReturned);
PlaceId *HunterLastMoveLocation(GameView gv, Player player, int num,
                                int *numReturned);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: DONE!
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	
	// adding necessary fields to GameView struct
	new->pastPlays = pastPlays;
	new->messages = messages;
	
	return new;
}

void GvFree(GameView gv)
{
	// TODO: DONE!
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: DONE!
	return (strlen(gv->pastPlays) + 1) / (8 * NUM_PLAYERS);
}

Player GvGetPlayer(GameView gv)
{
	// TODO: DONE!
	int currentPlayer = ((strlen(gv->pastPlays) + 1) / 8) % NUM_PLAYERS;
	return currentPlayer;
}

int GvGetScore(GameView gv)
{
	// TODO: needs to be fixed
	if (GvGetRound(gv) == 0) {
	   return GAME_START_SCORE;
   } else {
      return GAME_START_SCORE - (SCORE_LOSS_DRACULA_TURN * GvGetRound(gv));
   }
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: needs to be fixed
	if (player != PLAYER_DRACULA) {
	   return GAME_START_HUNTER_LIFE_POINTS;
   } else {
      return GAME_START_BLOOD_POINTS;
   }
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
   // TODO: dracula location still need considering
   
   int currentRound = GvGetRound(gv);
   
   // player has not made a turn
   if (currentRound == 0 && GvGetPlayer(gv) <= player) {
      return NOWHERE;
   }

   // for hunters
   if (player != PLAYER_DRACULA) {
      char *move = getPlayerMove(gv->pastPlays, player, currentRound);
      char *abbrev = strndup(move + 1, 2);
      return placeAbbrevToId(abbrev);
   }
   
   // for dracula
   return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: still needs considering
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: still needs considering
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: done but not tested; this needs fixing for dracula
	//       going to make this a wrapper function
    *canFree = false;
    
    if (player != PLAYER_DRACULA) {
        // numReturnedMoves might need to get passed as a pointer to the pointer
        return HunterMoveLocationHistory(gv, player, numReturnedMoves);
    } else {
        // TODO: FOR DRACULA!!
        return NULL;
    }
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: done but not tested; this needs fixing for dracula
	//       going to make this a wrapper function
	*canFree = false;
    
    if (player != PLAYER_DRACULA) {
        // numReturnedMoves might need to get passed as a pointer to the pointer
        return HunterLastMoveLocation(gv, player, numMoves, numReturnedMoves);
    } else {
        // TODO: FOR DRACULA!!
        return NULL;
    }
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: this is pretty much already done as above
	//       this is a wrapper function
	*canFree = false;
	
    if (player != PLAYER_DRACULA) {
        // numReturnedLocs might need to get passed as a pointer to the pointer
        return HunterMoveLocationHistory(gv, player, numReturnedLocs);
    } else {
        // TODO: FOR DRACULA!!
        return NULL;
    }

}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: this is pretty much already done as above
	//       this is a wrapper function
	*canFree = false;
	
    if (player != PLAYER_DRACULA) {
        // numReturnedLocs might need to get passed as a pointer to the pointer
        return HunterLastMoveLocation(gv, player, numLocs, numReturnedLocs);
    } else {
        // TODO: FOR DRACULA!!
        return NULL;
    }
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

//------------------------- general helper functions ---------------------------

// returns the string containing a player's move in a given round. if the input
// round exceeds current round, last move made by player is returned
// returns a string formatted as such "GMN...."
char *getPlayerMove(char *pastPlays, Player player, Round round)
   {
   // TODO: some testing needed

   char *string = strdup(pastPlays);
   char delim[] = " ";
   char *token;
   char *move;

   // the number of times strtok tokenises to reach the requested move
   int limit = round * NUM_PLAYERS + player;

   // looks through the pastPlays string until requested move is reached
   // or end is reached
   token = strtok(string, delim);
   for (int i = 0; i <= limit && token != NULL; i++) {
      // if move is made by player, it is updated
      if (i % NUM_PLAYERS == player) {
         move = token;
      }
      token = strtok(NULL, delim);
   }
   
   // check to make sure function is used correctly
   assert(move != NULL);
   return move;
}


// the following functions could be implemented with the getPlayerMove helper,
// but would be a lot less efficient as a result

// checks if a player has been to a certain place within the specified rounds
bool placeMatch(char *pastPlays, Player player, PlaceId place,
                Round roundStart, Round roundEnd)
{
    // TODO:  preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;
    
    // when to start matching the location
    int start = roundStart * NUM_PLAYERS + player;
    // when to stop matching the locations
    int end = roundEnd * NUM_PLAYERS + player;
    
    // looks through the pastPlays string
    move = strtok(string, delim);
    for (int i = 0; i <= end && move != NULL; i++) {
        // if the move made a player is considered
        if (i >= start && i % NUM_PLAYERS == player) {
            char *abbrev = strndup(move + 1, 2);
            if (place == placeAbbrevToId(abbrev)) {
                return TRUE;
            }
        }
        move = strtok(NULL, delim);
    }
    return FALSE;
}


// returns which round the player has been to a certain place, first occurence
// returns -1 if player has not been to the place
Round placeBeenF(char *pastPlays, Player player, PlaceId place)
{
    // TODO: preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;
    
    // looks through the pastPlays string
    move = strtok(string, delim);
    for (int i = 0; move != NULL; i++) {
        // if the move is made by the player 
        if (i % NUM_PLAYERS == player) {
            char *abbrev = strndup(move + 1, 2);
            if (place == placeAbbrevToId(abbrev)) {
                return i / NUM_PLAYERS;
            }
        }
        move = strtok(NULL, delim);
    }
    return -1;
}

// returns which round the player has been to a certain place, last occurence
// returns -1 if player has not been to the place
Round placeBeenL(char *pastPlays, Player player, PlaceId place)
{
    // TODO: preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;
    
    Round last = -1;
    
    // looks through the pastPlays string
    move = strtok(string, delim);
    for (int i = 0; move != NULL; i++) {
        // if the move is made by the player 
        if (i % NUM_PLAYERS == player) {
            char *abbrev = strndup(move + 1, 2);
            // last occurence keeps updating
            if (place == placeAbbrevToId(abbrev)) {
                last = i;
            }
        }
        move = strtok(NULL, delim);
    }
    return last / NUM_PLAYERS;
}


// ------------------ move-location helper functions ---------------------------

// helper function to find hunter move and location history
PlaceId *HunterMoveLocationHistory(GameView gv, Player player,
                                   int *numReturned)
{
    // TODO: should be working
	int numHistory = ((strlen(gv->pastPlays) + 1) / (8 * NUM_PLAYERS)) + 1;
	// if player has not yet made a move
	if (GvGetPlayer(gv) <= player) {
      numHistory--;
    }
	
	PlaceId *history = malloc(numHistory * sizeof(*history));
	
	// finds move made every round and adds to array
	for (int i = 0; i < numHistory; i++) {
      char *move = getPlayerMove(gv->pastPlays, player, i);
      char *abbrev = strndup(move + 1, 2);
      history[i] = placeAbbrevToId(abbrev);
	}
	
	*numReturned = numHistory;
	return history;
}


// helper function to find n last hunter moves and locations
PlaceId *HunterLastMoveLocation(GameView gv, Player player, int num,
                                int *numReturned)
{
    // TODO: should be working
	int numHistory = ((strlen(gv->pastPlays) + 1) / (8 * NUM_PLAYERS)) + 1;
	// if player has not yet made a move
	if (GvGetPlayer(gv) <= player) {
      numHistory--;
   }
    
    int numLast;
    // setting the number of last moves available
    if (numHistory > num) {
        numLast = num;
    } else {
        numLast = numHistory;
    }
	
	PlaceId *last = malloc(numLast * sizeof(*last));
	
	// finds last n moves made and adds to array
	for (int i = numHistory - numLast, j = 0; i < numHistory; i++, j++) {
	    char *move = getPlayerMove(gv->pastPlays, player, i);
	    char *abbrev = strndup(move + 1, 2);
	    last[j] = placeAbbrevToId(abbrev);
	}
	
	*numReturned = numLast;
	return last;
}
