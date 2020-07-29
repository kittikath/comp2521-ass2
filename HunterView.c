////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
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

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
#include "Queue.h"
// add your own #includes here
// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView gameView;
	char *pastPlays;
	Message *messages;
};

// helper functions

char *getLastMove(GameView gv, Player player);
char* getPlayerMove(char *pastPlays, Player player, Round round);

bool placeMatch(char *pastPlays, Player player, PlaceId Place,
                Round roundStart, Round roundEnd);
Round placeBeenF(char *pastPlays, Player player, PlaceId place);
Round placeBeenL(char *pastPlays, Player player, PlaceId place);

int updateHunterHealth(char *move, int health, PlaceId prevLoc, PlaceId currLoc);
int updateDraculaHealth(GameView gv, char *move, int health);
int hunterDeathCount(GameView gv);

PlaceId *playerMoveHistory(GameView gv, Player player, int *numReturnedMoves);
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                         int *numReturnedMoves);
void findDraculaLocation(int numMoves, PlaceId *draculaMoves);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}

	new->pastPlays = pastPlays;
	new->messages = messages;
	new->gameView = GvNew(pastPlays, messages);
	return new;
}

void HvFree(HunterView hv)
{
	// TODO: DONE
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: DONE
	return GvGetRound(hv->gameView);
}

Player HvGetPlayer(HunterView hv)
{
	// TODO: DONE
	return GvGetPlayer(hv->gameView);
}

int HvGetScore(HunterView hv)
{
	// TODO: DONE
	return GvGetScore(hv->gameView);
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: DONE
	return GvGetHealth(hv->gameView, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: DONE
	return GvGetPlayerLocation(hv->gameView, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: DONE
	return GvGetVampireLocation(hv->gameView);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// TODO: DONE
	int numReturnedLocs = 0;
	bool canFree = true;
	PlaceId *draculaLocation = GvGetLocationHistory(hv->gameView ,PLAYER_DRACULA , &numReturnedLocs, &canFree);
	int known = numReturnedLocs-1;
	while(known > 0){
		if(placeIsReal(draculaLocation[known])){
			break;
		}
		known--;
	}
	if(!placeIsReal(draculaLocation[known])){
		return NOWHERE;
	}

	Round latestRound = placeBeenL(hv->pastPlays, PLAYER_DRACULA, draculaLocation[known]);
	*round = latestRound;
	PlaceId draculaPlace = draculaLocation[known];
    free(draculaLocation); 
	return draculaPlace;
	
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	PlaceId curr;
	Map m = MapNew();
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	int pred[m->nV];
	//set all to -1 (not found)
	for(int i = 0; i < m->nV; i++){
		pred[i] = -1;
	}
	Queue Q = newQueue();
	QueueJoin(Q, src);
	while(!QueueIsEmpty(Q)){
		curr = QueueLeave(Q);
		//check for connections of node
		for(int j = 0; j < m->nV; j++){
			//we found a connection :O
			if(m->edges[curr][j] != 0){
			//add 
			printf("value %d\n", m->edges[curr][j] != 0);
				if(pred[j] == -1){
					pred[j] = curr;
					QueueJoin(Q, j);
					showQueue(Q);
					printf("----------\n");
				}
			}
		}
	}
	dropQueue(Q);
	//no path found
	if(pred[dest] == -1){
		return NULL;
	}
	


	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
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
