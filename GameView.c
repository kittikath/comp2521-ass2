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

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct playerinfo {
	Player player;
   	int health;
} PlayerInfo;

typedef struct vampire {
    bool exist;
    PlaceId place;
    Round roundSpawned;
} Vampire;

struct gameView {
	// TODO: ADD FIELDS HERE - added a few
	Round round;
	PlayerInfo playerInfo[NUM_PLAYERS];
	Vampire vampire;
	int score;
	char *pastPlays;
	Message messages[];
	
};


// helper functions

//char *getCurrentTurn(char *pastPlays);
char *getPlayerMove(char *pastPlays, Player player, Round round);
bool placeMatch(char *pastPlays, Player player, PlaceId Place,
                Round roundStart, Round roundEnd);
Round placeBeenF(char *pastPlays, Player player, PlaceId place);
Round placeBeenL(char *pastPlays, Player player, PlaceId place);

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
	new->round = (strlen(pastPlays)+1)/40;
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
		printf("health = %d\n", new->playerInfo[i].health);
	}
	
	// for the vampire;
	new->vampire.exist = FALSE;
	new->vampire.place = NOWHERE;
	new->vampire.roundSpawned = 0;

	return new;
}

void GvFree(GameView gv)
{
	// TODO: do we need to free pastPlays and messages?
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: DONE!
	return gv->round;
	// or
	// return strlen(gv->pastPlays) + 1) / 8;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: DONE!
	int currentPlayer = ((strlen(gv->pastPlays) + 1) / 8) % NUM_PLAYERS;
	// maybe could just return the number right away
	return gv->playerInfo[currentPlayer].player;
}

int GvGetScore(GameView gv)
{
	// TODO: DONE!
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: DONE!
	return gv->playerInfo[player].health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: should be done for hunters (by tonight); dracula still needs
	//       considering
	
	// before given player has made their play
	if (GvGetPlayer(gv) <= player) {
	    return NOWHERE;
	}

	//char *currentTurn = getCurrentTurn(gv->pastPlays);
	
	// not complete
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: DONE!
	return gv->vampire.exist == TRUE ? gv->vampire.place : NOWHERE;
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
	// TODO: done but not tested; this needs fixing for dracula
    *canFree = false;
    
	int numAllMoves = gv->round - 1;
	// if player made a move during the current round
	if (GvGetPlayer(gv) < player) {
	    numAllMoves++;
    }
	
	PlaceId *moveHistory = malloc(numAllMoves * sizeof(*moveHistory));
	
	// finding moves and adding it to the array
	for (int i = 0; i < numAllMoves; i++) {
	    char *move = getPlayerMove(gv->pastPlays, player, i);
	    char *abbrev = strndup(move + 1, 2);
	    moveHistory[i] = placeAbbrevToId(abbrev);
	}
	
	*numReturnedMoves = numAllMoves;
	return moveHistory;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: done but not tested; this needs fixing for dracula
	*canFree = false;

	int numAllMoves = gv->round - 1;
	// if player made a move during the current round
	if (GvGetPlayer(gv) < player) {
	    numAllMoves++;
    }
    
    int numLastMoves;
    // setting the number of last moves available
    if (numAllMoves > numMoves) {
        numLastMoves = numMoves;
    } else {
        numLastMoves = numAllMoves;
    }
	
	PlaceId *moveHistory = malloc(numLastMoves * sizeof(*moveHistory));
	
	// finding moves and adding it to the array
	int j = 0;
	for (int i = numAllMoves - numLastMoves; i < numAllMoves; i++) {
	    char *move = getPlayerMove(gv->pastPlays, player, i);
	    char *abbrev = strndup(move + 1, 2);
	    moveHistory[j] = placeAbbrevToId(abbrev);
	    j++;
	}
	
	*numReturnedMoves = numLastMoves;
	return moveHistory;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: this is pretty much already done as above
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: this is pretty much already done as above
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

/*
// returns the last move from pastPlays string
char *getCurrentTurn(char *pastPlays)
{
    char *currentTurn = strchr(pastPlays, '\n');    
    return currentTurn == NULL ? pastPlays : currentTurn + 1;
}
*/

// returns the string containing a player's move in a given round
char *getPlayerMove(char *pastPlays, Player player, Round round)
{
    // TODO:
    // if requested round has not been played
    // or if player has not made a move in the requested round
    // the assert could be removed it is causes an issue
    // hopefully this function doesn't need to handle invalid cases
    int currentRound = (strlen(pastPlays) + 1) / 8;    
    assert(round <= currentRound || ((strlen(pastPlays) + 1) / 8) % 5 > player);
    
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;
    
    // the number of times strtok tokenises
    int limit = round * NUM_PLAYERS + player;
    
    move = strtok(string, delim);
    for (int i = 0; i < limit && move != NULL; i++) {        
        move = strtok(NULL, delim);
    }
    return move;
}


// the following functions could be implemented with thegetPlayerMove helper,
// but would be a lot less efficient as a result

// checks if a player has been to a certain place within the specified rounds
bool placeMatch(char *pastPlays, Player player, PlaceId place,
                Round roundStart, Round roundEnd)
{
    // TODO:  preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;
    
    int start = roundStart * NUM_PLAYERS + player;
    int end = roundEnd * NUM_PLAYERS + player;
    
    move = strtok(string, delim);
    for (int i = 0; i <= end && move != NULL; i++) {
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


// checks which round the player has been to a certain place, first occurence
// returns -1 if player has not been to the place
Round placeBeenF(char *pastPlays, Player player, PlaceId place)
{
    // TODO: preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;    

    int currentRound = (strlen(pastPlays) + 1) / 8;
    int limit = currentRound * NUM_PLAYERS + player;
    
    move = strtok(string, delim);
    for (int i = 0; i <= limit && move != NULL; i++) {
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

// checks which round the player has been to a certain place, last occurence
// returns -1 if player has not been to the place
Round placeBeenL(char *pastPlays, Player player, PlaceId place)
{
    // TODO: preliminary testing suggests it works, needs commenting
    char *string = strdup(pastPlays);
    char delim[] = " ";
    char *move;    

    int currentRound = (strlen(pastPlays) + 1) / 8;
    int limit = currentRound * NUM_PLAYERS + player;
    
    Round last = -1;
    
    move = strtok(string, delim);
    for (int i = 0; i <= limit && move != NULL; i++) {
        if (i % NUM_PLAYERS == player) {
            char *abbrev = strndup(move + 1, 2);
            if (place == placeAbbrevToId(abbrev)) {
                last = i;
            }
        }
        move = strtok(NULL, delim);
    }
    return last / NUM_PLAYERS;
}
