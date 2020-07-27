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

#define PLAY_LEN 8

// TODO: ADD YOUR OWN STRUCTS HERE

struct gameView {
	// TODO: ADD FIELDS HERE - added a few
	char *pastPlays;
	Message *messages;
};


// helper functions

char *getPlayerMove(char *pastPlays, Player player, Round round);
char *getCurrentMove(char *pastPlays, Player player, Round round);
bool placeMatch(char *pastPlays, Player player, PlaceId Place,
                Round roundStart, Round roundEnd);
Round placeBeenF(char *pastPlays, Player player, PlaceId place);
Round placeBeenL(char *pastPlays, Player player, PlaceId place);


PlaceId *playerMoveHistory(GameView gv, Player player, int *numReturnedMoves);
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                         int *numReturnedMoves);
void findDraculaLocation(int numMoves, PlaceId *draculaMoves);

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
   // TODO: DONE! needs testing
   
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
   // for dracula
   } else {
      int numLocs = 0;
      bool canFree = true;
      PlaceId *draculaLocation = GvGetLocationHistory(gv, player, &numLocs, &canFree);
      PlaceId draculaPlace = draculaLocation[numLocs - 1];
      if (canFree == true) {
         free(draculaLocation);
      }
      return draculaPlace;
   }
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: done but edge cases still needs to be tested.
   
   // startSearch needs a bit more thought
   int startSearch = (GvGetRound(gv) / 13) * 13;
   PlaceId vampireLocation;
	
	// if vampire is spawned
   char *draculaMove = getCurrentMove(gv->pastPlays, PLAYER_DRACULA, startSearch);
   if (draculaMove != NULL && strncmp(draculaMove + 4, "V", 1) == 0) {
      vampireLocation = GvGetPlayerLocation(gv, PLAYER_DRACULA);
   } else {
      return NOWHERE;
   }
	
	// if vampire is vanquished
	for (int i = startSearch + 1; i <= GvGetRound(gv) && i <= startSearch + 6; i++) {
	   for (int j = 0; j < 4; j++) {
	      char *hunterMove = getCurrentMove(gv->pastPlays, j, i);
	      if (hunterMove != NULL && strncmp(hunterMove + 4, "V", 1) == 0) {
	         return NOWHERE;
	      }
	   }
	}
	return vampireLocation;
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
	// TODO: done but not tested - canFree causes a segfault
   //*canFree = true;

   return playerMoveHistory(gv, player, numReturnedMoves);
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: done but not tested - canFree causes a segfault
	//*canFree = true;

	return playerLastMoves(gv, player, numMoves, numReturnedMoves);
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
   // TODO: done, needs testing - canFree causes a segfault
   //*canFree = true;

   PlaceId *moveHistory = playerMoveHistory(gv, player, numReturnedLocs);

   if (player == PLAYER_DRACULA) {
      findDraculaLocation(*numReturnedLocs, moveHistory);
   }
   return moveHistory;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
   // TODO: done, needs testing - canFree causes a segfault
   //*canFree = true;
   
   PlaceId *lastMoves = playerLastMoves(gv, player, numLocs, numReturnedLocs);

   if (player == PLAYER_DRACULA) {
      findDraculaLocation(*numReturnedLocs, lastMoves);
   }
   return lastMoves;
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

// returns the string containing a player's move in a given round. if the
// player has not made a move in the given round, it will return NULL
// returns a string formatted as such "GMN...."
char *getCurrentMove(char *pastPlays, Player player, Round round)
   {
   // TODO: some testing needed

   char *string = strdup(pastPlays);
   char delim[] = " ";
   char *move;

   // the number of times strtok tokenises to reach the requested move
   int limit = round * NUM_PLAYERS + player;

   // looks through the pastPlays string until requested move is reached
   // or end is reached
   move = strtok(string, delim);
   for (int i = 0; i < limit && move != NULL; i++) {
      move = strtok(NULL, delim);
   }   
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
                return true;
            }
        }
        move = strtok(NULL, delim);
    }
    return false;
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

// helper function to find player move history
PlaceId *playerMoveHistory(GameView gv, Player player, int *numReturnedMoves)
{
    // TODO: should be working
	int numMoveHistory = GvGetRound(gv) + 1;
	// if player has not yet made a move
	if (GvGetPlayer(gv) <= player) {
      numMoveHistory--;
    }
	
	PlaceId *moveHistory = malloc(numMoveHistory * sizeof(*moveHistory));
	
	// finds move made every round and adds to array
	for (int i = 0; i < numMoveHistory; i++) {
      char *move = getPlayerMove(gv->pastPlays, player, i);
      char *abbrev = strndup(move + 1, 2);
      moveHistory[i] = placeAbbrevToId(abbrev);
	}
	
	*numReturnedMoves = numMoveHistory;
	return moveHistory;
}


// helper function to find n last player moves
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                        int *numReturnedMoves)
{
    // TODO: should be working
	int numMoveHistory = ((strlen(gv->pastPlays) + 1) / (8 * NUM_PLAYERS)) + 1;
	// if player has not yet made a move
	if (GvGetPlayer(gv) <= player) {
      numMoveHistory--;
   }
    
    int numLastMoves;
    // setting the number of last moves available
    if (numMoveHistory > numMoves) {
        numLastMoves = numMoves;
    } else {
        numLastMoves = numMoveHistory;
    }
	
	PlaceId *lastMoves = malloc(numLastMoves * sizeof(*lastMoves));
	
	// finds last n moves made and adds to array
	for (int i = numMoveHistory - numLastMoves, j = 0; 
	     i < numMoveHistory; i++, j++) {
	    char *move = getPlayerMove(gv->pastPlays, player, i);
	    char *abbrev = strndup(move + 1, 2);
	    lastMoves[j] = placeAbbrevToId(abbrev);
	}
	
	*numReturnedMoves = numLastMoves;
	return lastMoves;
}

// helper function to find dracula location from dracula moves
void findDraculaLocation(int numMoves, PlaceId *draculaMoves)
{
   // could do without the 'refer' array
   // this array helps to refer to dracula's past moves
   int refer[numMoves];
   
   // building refer back array
   for (int i = 0; i < numMoves; i++) {
      switch (draculaMoves[i])  {
         case HIDE:
            refer[i] = i - 1;
            break;
         case DOUBLE_BACK_1:
            refer[i] = i - 1;
            break;
         case DOUBLE_BACK_2:
            refer[i] = i - 2;
            break;
         case DOUBLE_BACK_3:
            refer[i] = i - 3;
            break;
         case DOUBLE_BACK_4:
            refer[i] = i - 4;
            break;
         case DOUBLE_BACK_5:
            refer[i] = i - 5;
            break;
         case TELEPORT:
            draculaMoves[i] = CASTLE_DRACULA;
         default:
            refer[i] = i;
      }
   }

   // fixing the locations by looking through the refer list
   for (int i = 0; i < numMoves; i++) {
      draculaMoves[i] = draculaMoves[refer[i]];
   }
}
