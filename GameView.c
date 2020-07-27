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
int calculateScore(GameView gv);
static void calculateHealth(GameView gv, enum player player);
bool isHunterDead(GameView gv, enum player player);

struct gameView {
	// TODO: ADD FIELDS HERE
	int currentPlayer;
	int score;
	int location[NUM_REAL_PLACES];
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
			new->location[i] = NOWHERE;
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

	for (i = 0; i < NUM_PLAYERS; i++) {
		printf("hi\n");
		calculateHealth(new, i);
		printf("player %d's health is: %d\n", i, new->health[i]);
	}

	printf("score is: %d\n", calculateScore(new));

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
	return (strlen(gv->pastPlays) + 1) / (8 * NUM_PLAYERS);
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
int calculateScore(GameView gv) {

	int totalScore = GAME_START_SCORE;
	int round = GvGetRound(gv);
	int i, j;
	int draculaTurn = 0, vampCount = 0, huntCount = 0;

	// loop through all the rounds
	for (i = 0; i < round; i++) {
		// if dracula's turn exists, increment
		if (getCurrentMove(gv->pastPlays, PLAYER_DRACULA, i) != NULL) {
			draculaTurn += SCORE_LOSS_DRACULA_TURN;
		}
	}

	// loop through all the rounds
	for (i = 0; i < round; i++) {
		// loop through all hunters
		for (j = 0; j < 4; j++) {
			// if hunter is found to be dead, increment
			if (isHunterDead(gv, j) == true) {
				huntCount += SCORE_LOSS_HUNTER_HOSPITAL;
			}
		}
	}

	// for every round divisible by 13, check if vampire has matured
	for (i = 0; i < round; i = i + 13) {
		if (getCurrentMove(gv->pastPlays, PLAYER_DRACULA, i) != NULL) {
			// this should be "DC?T.V." .. etc
			char *move = getCurrentMove(gv->pastPlays, PLAYER_DRACULA, i);
			// if vampire has matured, then increment
			if (move[5] == 'V') {
				vampCount += SCORE_LOSS_VAMPIRE_MATURES;
			}
		}
	}

	// dracula finishes his turn
	totalScore = totalScore - draculaTurn;

	// hunters life goes to 0 and moves to hospital
	totalScore = totalScore - huntCount;

	// vampire matures
	totalScore = totalScore - vampCount;

	return totalScore;
}

// calculates the health of a player and teleports player to respawn locations
static void calculateHealth(GameView gv, enum player player) {
	
	int i;
	int round = GvGetRound(gv);
	char *curMove;
	PlaceId curLoc;
	PlaceId prevLoc;
	// hunters health
	if (player != PLAYER_DRACULA) {
		for (i = 0; i < round; i++) {
			// string of the whole player move, e.g "GMN.....""
			curMove = getCurrentMove(gv->pastPlays, player, i);
			// string of location, e.g "MN"
			curLoc = GvGetPlayerLocation(gv, player);

			// encounter Dracula, lose 4 hp and he loses 10 hp
			if (curMove[4] == 'D') {
				gv->health[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
				gv->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
			}
			if (i > 0) {
				// encounter a trap, lose 2 hp
				if (curMove[2] == 'T') {
					gv->health[player] -= LIFE_LOSS_TRAP_ENCOUNTER;
				}
				// stay at the same location as last turn, recover 3 hp
				if (curLoc == prevLoc) {
					gv->health[player] += LIFE_GAIN_REST;
					// if health exceeds max pool, revert back to max pool
					if (gv->health[player] > GAME_START_HUNTER_LIFE_POINTS) {
						gv->health[player] = GAME_START_HUNTER_LIFE_POINTS;
					}
				}
				// health goes under 0, teleport to hospital
				if (isHunterDead(gv, player) == true) {
					// if hunter is already in hospital, it means a new turn has started
					if (curLoc == ST_JOSEPH_AND_ST_MARY) {
						gv->health[player] = GAME_START_HUNTER_LIFE_POINTS;
					}
					gv->location[player] = ST_JOSEPH_AND_ST_MARY;
				}
			}
			prevLoc = GvGetPlayerLocation(gv, player);
		}
	}
	// draculas health
	/*
	else {
		for (i = 0; i < round; i++) {
			curMove = getCurrentMove(gv->pastPlays, player, i);
			curLoc = GvGetPlayerLocation(gv, player);
			if (curMove[])
		}
	}
	*/
}

bool isHunterDead(GameView gv, enum player player) {

	if (player != PLAYER_DRACULA) {
		// if hunter is ded
		if (gv->health[player] <= 0) {
			return true;
		}
		return false;
	}
	else {
		return false;
	}
}

// TODO
