////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
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

struct gameView {
	// TODO: ADD FIELDS HERE
	int round;
	int currentPlayer;
	int score;
	int health[NUM_PLAYERS];
	char *pastPlays;
};

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

	int i;
	if (strlen(pastPlays) == 0) {
		// for hunters
		for (i = 0; i < 3; i++) {
			new->health[i] = GAME_START_HUNTER_LIFE_POINTS;
			printf("health of hunters: %d\n", new->health[i]);
		}
		// for dracula
		new->health[4] = GAME_START_BLOOD_POINTS;
		printf("health of dracula: %d\n", new->health[4]);
		// initial score
		new->score = GAME_START_SCORE;
		printf("score = %d\n", new->score);
		return new;
	}
    
	printf("pastPlays string is: %s\n", pastPlays);

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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	printf("gvgetround\n");
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	printf("gvgetplayer\n");
	int currentPlayer = ((strlen(gv->pastPlays) + 1) / 8) % NUM_PLAYERS;
	return currentPlayer;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->score;
}

int GvGetHealth(GameView gv, enum player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return gv->health[player];
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    // check if hunter has been to location
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
   move= strtok(string, delim);
   for (int i = 0; i < limit && move != NULL; i++) {
      move = strtok(NULL, delim);
   }   
   return move;
}

//static void hunterActions(GameView gv, char *pastPlays) {

//}

// calculates the score 
int calculateScore(GameView gv, char *pastPlays) {

	int totalScore = GAME_START_SCORE;
	int draculaTurn = 0, i = 0, vampCount = 0;
	char *string = strdup(pastPlays);

	// loop through all the rounds, if dracula's turn does return something then increment
	for (i = 0; i < GAME_START_SCORE; i++) {
		if (getCurrentMove(pastPlays, PLAYER_DRACULA, i) != NULL) {
			draculaTurn++;
		}
	}

	

	// for every round divisible by 13, check if vampire has matured
	for (i = 0; i < GAME_START_SCORE; i = i + 13) {
		if (getCurrentMove(pastPlays, PLAYER_DRACULA, i) != NULL) {
			// this should be "DC?T.V." .. etc
			char *move = getCurrentMove(pastPlays, PLAYER_DRACULA, i);
			// if vampire has matured, then increment
			if (move[6] == 'V') {
				vampCount++;
			}
		}
	}

	// dracula finishes his turn
	totalScore = totalScore - draculaTurn;

	// hunters life goes to 0 and moves to hospital

	// vampire matures
	vampCount = vampCount * 13;
	totalScore = totalScore - vampCount;


	return totalScore;
}

// TODO
