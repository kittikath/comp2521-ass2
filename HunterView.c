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

typedef struct QueueRep {
	ConnList head;  // ptr to first node
	ConnList tail;  // ptr to last node
} QueueRep, *Queue; 

struct hunterView {
	GameView gameView;
	char *pastPlays;
	int pred[NUM_REAL_PLACES];
	PlaceId railDest[NUM_REAL_PLACES][NUM_REAL_PLACES];
	QueueRep railPath[NUM_REAL_PLACES];
	Message *messages;
	
};

// queue functions
Queue newQueue(); // create new empty queue
void dropQueue(Queue); // free memory used by queue
void showQueue(Queue); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue, ConnList); // add item on queue
PlaceId QueueLeave(Queue); // remove item from queue
int QueueIsEmpty(Queue); // check for no items
ConnList QueueLast(Queue Q);
ConnList createNode(PlaceId place, TransportType transport);

//shortest path
void newPath(HunterView hv, int totalPath, int pathcur, PlaceId place, TransportType transport);
int travelByRail(HunterView hv, Map m, PlaceId startLoc, PlaceId src, PlaceId dest, Player player);

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
		new->railPath[i].head = NULL;
		for(int j = 0; j < NUM_REAL_PLACES; j++){
			new->railDest[i][j] = -1;
		}
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
	srcNode->type = Tsrc;
	srcNode->next = NULL;
	Queue Q = newQueue();
	ConnList curr;
	PlaceId currPlace;
	int movesByRail = 0;
	int railMoves = (hunter+HvGetRound(hv))%4;
	int isFound = 0;
	int maxSteps = 3;
	int c = 0;
	QueueJoin(Q, srcNode);
	while(!QueueIsEmpty(Q) && !isFound){
		currPlace = QueueLeave(Q);
		//check for connections of node
		for(curr =  MapGetConnections(m, currPlace); curr != NULL; curr = curr->next){
			//we found a connection :O
			// printf("curr %s transport %s\n", placeIdToName(curr->p), transportTypeToString(curr->type));
			if(hv->pred[curr->p] == -1){
				if(curr->type == RAIL && railMoves != 0 ){
					hv->pred[curr->p] = currPlace;
					printf("%d %d, %s...\n", hv->pred[curr->p], curr->p, placeIdToName(hv->pred[curr->p]));
					movesByRail = travelByRail(hv, m, curr->p, src, dest, railMoves);
					railMoves = 0;
					for(c = 0; c <= movesByRail; c++){
						PlaceId placeB4 = hv->railDest[c][0];
						for(int steps = 1; steps <= maxSteps; steps++){
							if(hv->pred[hv->railDest[c][steps]] < 0){
								hv->pred[hv->railDest[c][steps]] = placeB4;
								srcNode = createNode(hv->railDest[c][steps], RAIL);
								printf("added to queue, %s %s\n", placeIdToName(hv->pred[hv->railDest[c][steps]]), placeIdToName(placeB4));
								QueueJoin(Q, srcNode);
							}
							if(hv->railDest[c][steps] == dest){
								printf("--------------------------\n");
								isFound = 1;
								break;
							}
						}
					}
				}
				else{
					hv->pred[curr->p] = currPlace;
					QueueJoin(Q, curr);
				}
			}
			if(curr->p == dest){ 
			printf("~~~~~~~~~\n");
				isFound = 1; 
				break; 
			}
			//check if theres another route other than rail,
			//if there is switch the queue node for that one.
			// showQueue(Q);
			// m->connection[curr->p] != 0){
		}
	}
	dropQueue(Q);
	//no path found
	if(hv->pred[dest] == -1){
		return NULL;
	}
	else{
		for(int v = dest; v != src; v = hv->pred[v]){
			printf("%d %s<-", v, placeIdToName(v));
			pathLength[0]++;
		}
		printf("%d\n", src);
	}
	printf("pathlength %d\n",pathLength[0]);
	//from my lab 7 submission
	int k = 0;
	int backtrack = dest;
	int backpath[NUM_REAL_PLACES];
	backpath[0] = dest;
	k++;
	while(backtrack != src){
		backpath[k] = hv->pred[backtrack];
		backtrack = hv->pred[backtrack];
		k++;
	}
	backpath[k] = src;

	int hops = k;
	int counter = hops-1;
	PlaceId *path =  malloc(pathLength[0]*sizeof(*path));
	for(int x = 0; x < hops-1; x++){
		path[x] = backpath[counter-1];
		printf("%s ->", placeIdToName(path[x]));
		counter--;
	}
	printf("\n");
	free(srcNode);
	return path;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	int round = HvGetRound(hv);
	Player player = GvGetPlayer(hv->gameView);
	int from = GvGetPlayerLocation(hv->gameView, player);
	*numReturnedLocs = 0;
	return GvGetReachableByType(hv->gameView, player, round, from, true, true, true, numReturnedLocs);

	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	int round = HvGetRound(hv);
	Player player = GvGetPlayer(hv->gameView);
	int from = GvGetPlayerLocation(hv->gameView, player);
	bool railCheck = checkRail(hv, player, rail);
	*numReturnedLocs = 0;
	return GvGetReachableByType(hv->gameView, player, round, from, road, railCheck, boat, numReturnedLocs);
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
bool checkRail(HunterView hv, Player player, bool rail)
{
	int round = HvGetRound(hv);
	if(rail && (player+round)%4 == 0){
		printf("changed to false\n");
		rail = false;

	}
	return rail;
}

int travelByRail(HunterView hv, Map m, PlaceId startLoc, PlaceId src, PlaceId dest, Player player){
	PlaceId currLoc;
	int predCopy[NUM_REAL_PLACES];
	int movesPerRound[NUM_REAL_PLACES] = {0};
	int round = HvGetRound(hv);
	int pathcur = 0;
	int totalpath = 0;
	int railConnections = 0;
	int moves;
	int counter;
	for(int p = 0; p < NUM_REAL_PLACES; p++){
		predCopy[p] = hv->pred[p];
	}
	//set src location to -2 so it doesn't return 
	predCopy[src] = -2; 
	
	for(int i = 0; i < NUM_REAL_PLACES; i++){
		hv->railDest[i][0] = startLoc; 
	}

	ConnList node = createNode(startLoc, RAIL);
	QueueJoin(&(hv->railPath[pathcur]), node);
	ConnList curr = hv->railPath[pathcur].head;
	while(pathcur <= totalpath){
		currLoc = hv->railPath[pathcur].tail->p;
		moves = (round+player)%4;
		movesPerRound[round] = moves;
		printf("start %s moves this round %d\n", placeIdToName(currLoc), moves);
		counter = moves;
		while(counter != 0){
			//find rail connections
			railConnections = 0;
			printf("---- %d round %d move ----\n", round, counter);
			for (curr = MapGetConnections(m, currLoc); curr != NULL; curr = curr->next){
				printf("pred %d %d  %s\n", curr->p, predCopy[curr->p], placeIdToName(curr->p));
				if(curr->type == RAIL && predCopy[curr->p] == -1){
					//save onto current list
					if(railConnections == 0){
						//add to list
						printf("%s added edge  ", placeIdToName(curr->p));
						QueueJoin(&(hv->railPath[pathcur]), curr);
					}
					else{ //create copy of current list
						totalpath++;
						printf("%d %s total path\n", totalpath, placeIdToName(curr->p));
						newPath(hv, totalpath, pathcur, curr->p, curr->type);
					}
					//tagged that this has been visited by rail
					// printf("pred values %d\n", predCopy[curr->p]);
					predCopy[curr->p] = currLoc;
					railConnections++;
				}
				printf("\n");
			}
			if(currLoc == hv->railPath[pathcur].tail->p && currLoc < totalpath){
				pathcur++;
			}
			currLoc = hv->railPath[pathcur].tail->p;
			counter--;
		}
		if(moves > 0 && railConnections != 0){
			for(int i = 0; i < totalpath+1; i++){
				hv->railDest[i][round] = hv->railPath[i].tail->p; 
				printf("values saved %d %d %s \n", i, round, placeIdToName(hv->railDest[i][round]));

			}
		}
		round++;
		// printf("-------------------- %d ROUND--------\n", round);
		pathcur++;
	}

	return totalpath;
}

void newPath(HunterView hv, int totalPath, int pathcur, PlaceId place, TransportType transport){
	ConnList it = hv->railPath[pathcur].head;
	hv->railPath[totalPath].head = createNode(it->p, it->type);
	hv->railPath[totalPath].tail = hv->railPath[totalPath].head;
	// printf("%s last value \n", placeIdToName(hv->railPath[totalPath].tail->p));
	it = it->next;
	while(it->next != NULL){
		QueueJoin(&(hv->railPath[totalPath]), it);
		it = it->next;
	}

	ConnList new = createNode(place, transport);
	hv->railPath[totalPath].tail->next = new;
	hv->railPath[totalPath].tail = new;
}

ConnList createNode(PlaceId place, TransportType transport)
{
	ConnList node = malloc(sizeof(ConnList)); 
	node->p = place;
	node->type = transport;
	node->next = NULL;
	return node;
}

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
	// printf("Queueue Join function last value %s\n", placeIdToName(Q->tail->p));
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

ConnList QueueLast(Queue Q){
	assert(Q != NULL);
	ConnList curr = Q->head;
	while(curr != NULL){
		curr = curr->next;
	}
	return curr;
}