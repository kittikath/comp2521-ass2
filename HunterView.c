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

// add your own #includes here
// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView gameView;
	char *pastPlays;
	int pred[NUM_REAL_PLACES];
	Message *messages;
	
};

typedef struct QueueRep {
	ConnList head;  // ptr to first node
	ConnList tail;  // ptr to last node
} QueueRep, *Queue;


// queue functions
Queue newQueue(); // create new empty queue
void dropQueue(Queue); // free memory used by queue
void showQueue(Queue); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue, ConnList); // add item on queue
PlaceId QueueLeave(Queue); // remove item from queue
int QueueIsEmpty(Queue); // check for no items


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

bool checkRail(HunterView hv, Player player, bool rail);

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
	for(int i = 0; i < NUM_REAL_PLACES; i++){
		new->pred[i] = -1;
	}
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
	Map m = MapNew();
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	TransportType Tsrc = placeIdToType(src);
	ConnList srcNode = malloc(sizeof(ConnList)); 
	srcNode->p = src;
	printf("first location %s last location is %s %s\n", placeIdToName(srcNode->p), placeIdToName(dest), transportTypeToString(Tsrc));
	srcNode->type = Tsrc;
	srcNode->next = NULL;
	Queue Q = newQueue();
	ConnList curr;
	PlaceId currPlace;
	int railMoves = 0;
	int moveFlag = 0;
	int isFound = 0;
	QueueJoin(Q, srcNode);
	while(!QueueIsEmpty(Q) && !isFound){
		currPlace = QueueLeave(Q);
		//QueueLeave function not printing correct
		printf("first location %s\n", placeIdToName(currPlace));
		//check for connections of node
		for(curr =  MapGetConnections(m, currPlace); curr != NULL; curr = curr->next){
			//we found a connection :O
			printf("curr %s transport %s\n", placeIdToName(curr->p), transportTypeToString(curr->type));
			if(hv->pred[curr->p] == -1){
				if(curr->type == RAIL && railMoves != 0){
					railMoves--;
				}
				else if(curr->type == RAIL && !moveFlag){
					moveFlag = 1;
					railMoves = (hunter+HvGetRound(hv))%4;
				}
				hv->pred[curr->p] = currPlace;
				QueueJoin(Q, curr);
			}
			if(curr->p == dest){ 
				printf("--------------------------\n");
				isFound = 1; 
				break; 
			}
			//check if theres another route other than rail,
			//if there is switch the queue node for that one.
			showQueue(Q);
			// m->connection[curr->p] != 0){
		}
	}
	dropQueue(Q);
	//no path found
	if(hv->pred[dest] == -1){
		return NULL;
	}
	else{
		printf("ok\n");
		for(int v = dest; v != src; v = hv->pred[v]){
			printf("%d %s<-", v, placeIdToName(v));
			pathLength[0]++;
		}
		printf("%d\n", src);
	}
	printf("pathlength %d\n",pathLength[0]);

	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	int round = HvGetRound(hv);
	Player player = HvGetPlayer(hv);
	if (player != PLAYER_DRACULA) {
		// if player has not made a move, return null
		int returnedlocs = *numReturnedLocs;
		PlaceId *reachables = GvGetReachable(hv->gameView, player, round, GvGetPlayerLocation(hv->gameView, 
								player), &returnedlocs);
		*numReturnedLocs = returnedlocs;
		return reachables;
	}
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	Player player = HvGetPlayer(hv);
	int round = HvGetRound(hv);
	int returnedlocs = *numReturnedLocs;
	if (player != PLAYER_DRACULA) {
		PlaceId *reachables = GvGetReachableByType(hv->gameView, player, round, GvGetPlayerLocation(hv->gameView, 
									player), road, rail, boat, &returnedlocs);
		*numReturnedLocs = returnedlocs;
		return reachables;
	}
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int round = HvGetRound(hv);
	int from = GvGetPlayerLocation(hv->gameView, player);
	*numReturnedLocs = 0;
	return GvGetReachableByType(hv->gameView, player, round, from, true, true, true, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int round = HvGetRound(hv);
	int from = GvGetPlayerLocation(hv->gameView, player);
	bool railCheck = checkRail(hv, player, rail);
	*numReturnedLocs = 0;
	PlaceId *hello = GvGetReachableByType(hv->gameView, player, round, from, road, railCheck, boat, numReturnedLocs);
	return hello;
	//return GvGetReachableByType(hv->gameView, player, round, from, road, rail, boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO

// create new empty Queue
Queue newQueue()
{
	Queue q;
	q = malloc(sizeof(QueueRep));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Queue
void dropQueue(Queue Q)
{
	ConnList curr, next;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue rep
	free(Q);
}

// display as 3 > 5 > 4 > ...
void showQueue(Queue Q)
{
	ConnList curr;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		printf("%s ", placeIdToName(curr->p));
		if (curr->next != NULL)
			printf(">");
		curr = curr->next;
	}
	printf("\n");
}

// add item at end of Queue 
void QueueJoin(Queue Q, ConnList node)
{
	assert(Q != NULL);
	assert(placeIsReal(node->p));
	assert(transportTypeIsValid(node->type));
	ConnList new = malloc(sizeof(ConnList));
	assert(new != NULL);
	new->p = node->p;
	new->type = node->type;
	new->next = NULL;
	if (Q->head == NULL)
		Q->head = new;
	if (Q->tail != NULL)
		Q->tail->next = new;
	Q->tail = new;
}

// remove item from front of Queue
PlaceId QueueLeave(Queue Q)
{
	assert(Q != NULL);
	assert(Q->head != NULL);
	PlaceId it = Q->head->p;
	ConnList old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL)
		Q->tail = NULL;
	free(old);
	return it;
}

// check for no items
int QueueIsEmpty(Queue Q)
{
	return (Q->head == NULL);
}

bool checkRail(HunterView hv, Player player, bool rail)
{
	int round = HvGetRound(hv);
	if(rail && (player+round)%4 == 0){
		printf("changed to false\n");
		rail = false;
		printf("%d bool\n", rail);

	}
	return rail;
}