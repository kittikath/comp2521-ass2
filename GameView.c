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

#define PLAY_LEN 8
#define MAX_TRAPS 18

struct gameView {
	// TODO: DONE!
	char *pastPlays;
	Message *messages;
};


// helper functions

//------------------------- general helper prototypes --------------------------
char *getLastMove(GameView gv, Player player);
char* getPlayerMove(char *pastPlays, Player player, Round round);

bool placeMatch(char *pastPlays, Player player, PlaceId Place,
                Round roundStart, Round roundEnd);
Round placeBeenF(char *pastPlays, Player player, PlaceId place);
Round placeBeenL(char *pastPlays, Player player, PlaceId place);

//------------------------- score helper prototypes ----------------------------

int hunterDeathCount(GameView gv);

//------------------------ health helper functions -----------------------------
int updateHunterHealth(char *move, int health, PlaceId prevLoc, 
                       PlaceId currLoc);
int updateDraculaHealth(GameView gv, char *move, int health);

// ------------------ move-location helper prototypes --------------------------

PlaceId *playerMoveHistory(GameView gv, Player player, int *numReturnedMoves);
PlaceId *playerLastMoves(GameView gv, Player player, int numMoves, 
                         int *numReturnedMoves);
void findDraculaLocation(int numMoves, PlaceId *draculaMoves);

//------------------------ reachable helper prototypes -------------------------

void nearby(Map m, PlaceId from, PlaceId *near, int *size, int type);
int removeDups(PlaceId *arrayDups, PlaceId *arrayEmpty, int size, int size_2);

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
	// TODO: DONE!
	
	int totalScore = GAME_START_SCORE;
	int turnLoss = 0;
	int vampireLoss = 0;
	int hunterLoss = 0;
	int totalLoss = 0;

   // loop through all the rounds
   for (int i = 0; i <= GvGetRound(gv); i++) {
      char *move = getPlayerMove(gv->pastPlays, PLAYER_DRACULA, i);
      // if dracula's turn exists, increment score loss
      if (move != NULL) {
         turnLoss += SCORE_LOSS_DRACULA_TURN;
         // check if vampire matures
         if (i % 13 == 6 && move[5] == 'V') {
            vampireLoss += SCORE_LOSS_VAMPIRE_MATURES;
         }
      }
   }

   hunterLoss = hunterDeathCount(gv) * SCORE_LOSS_HUNTER_HOSPITAL;
   
   totalLoss = turnLoss + vampireLoss + hunterLoss;

	return totalScore - totalLoss;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: DONE!
	
   int currentRound = GvGetRound(gv);
   int health;
   
   if (player != PLAYER_DRACULA) {
      health = GAME_START_HUNTER_LIFE_POINTS;
      PlaceId prevLoc = NOWHERE;
      PlaceId currLoc;
      // loop through all player moves/encounters
      for (int i = 0; i <= currentRound; i++) {
         char *move = getPlayerMove(gv->pastPlays, player, i);
         if (move != NULL) {
            char *abbrev = strndup(move + 1, 2);
            currLoc = placeAbbrevToId(abbrev);         
            health = updateHunterHealth(move, health, prevLoc, currLoc);
            prevLoc = currLoc;      
         }
      }
   } else {
      health = GAME_START_BLOOD_POINTS;
      // loop through rounds
      for (int i = 0; i <= currentRound; i++) {
         // loop through all players, dracula needs to check all moves
         for (int j = 0; j < NUM_PLAYERS; j++) {
            char *move = getPlayerMove(gv->pastPlays, j, i);
            if (move != NULL) {
               health = updateDraculaHealth(gv, move, health);
            }
         }
      }      
   }
   return health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
   // TODO: DONE!
   
   char *move = getLastMove(gv, player);
   
   // player has not made a turn
   if (move == NULL) {
      return NOWHERE;
   }

   if (player != PLAYER_DRACULA) {
      // if hunter has died
      if (GvGetHealth(gv, player) == 0) {
         return HOSPITAL_PLACE;
      }
      char *abbrev = strndup(move + 1, 2);
      return placeAbbrevToId(abbrev);
   } else {
      int numLocs = 0;
      bool canFree = true;
      PlaceId *draculaLocation = GvGetLastLocations(gv, player, 1,
                                                   &numLocs, &canFree);
      PlaceId draculaPlace = draculaLocation[0];
      free(draculaLocation);      
      return draculaPlace;
   }
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: DONE!

   // no vampires after 6th round in each cycle or at start of cycle
   if (GvGetRound(gv) % 13 > 6 || GvGetRound(gv) % 13 == 0) {
      return NOWHERE;
   }
   
   int roundSpawn = (GvGetRound(gv) / 13) * 13;
   PlaceId vampireLocation;
   
   // if vampire is spawned
   char *spawnMove = getPlayerMove(gv->pastPlays, PLAYER_DRACULA, roundSpawn);
   if (strncmp(spawnMove + 4, "V", 1) == 0) {
      char *abbrev = strndup(spawnMove + 1, 2);
      vampireLocation = placeAbbrevToId(abbrev);   
   } else {
      return NOWHERE;
   }
	
	// if vampire is vanquished within the next 6 rounds
	int startCheck = roundSpawn + 1;
	int endCheck = roundSpawn + 6;
	
	for (int i = startCheck; i <= endCheck; i++) {
	   for (int j = 0; j < NUM_PLAYERS - 1; j++) {
	      char *hunterMove = getPlayerMove(gv->pastPlays, j, i);
	      if (hunterMove != NULL) {
	         for (int k = 3; k < 7; k++) {
	            if (hunterMove[k] == 'V') {
	               return NOWHERE;
	            }
	         }
	      }
	   }
	}
	return vampireLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: DONE!
	int numMoveHistory = GvGetRound(gv) + 1;
	
	PlaceId laidLocs[MAX_TRAPS];
	int numLaid = 0;
	
	PlaceId removedLocs[MAX_TRAPS];
	int numDestroyed = 0;
	
	for (int i = 0; i < MAX_TRAPS; i++) {
		laidLocs[i] = NOWHERE;
		removedLocs[i] = NOWHERE;
	}
	
	// Locations where Dracula laid a trap
	for (int i = 0; i < numMoveHistory - 1; i++) {
		char *move = getPlayerMove(gv->pastPlays, PLAYER_DRACULA, i);
		if (strncmp(move + 3, "T", 1) == 0) {
			char *abbrev = strndup(move + 1, 2);
			
			// Dracula hides or double-backs
			if (strcmp(abbrev, "HI") == 0 || strcmp(abbrev, "D1") == 0) {
				laidLocs[numLaid] = laidLocs[numLaid - 1];
			} else if (strcmp(abbrev, "D2") == 0) {
				laidLocs[numLaid] = laidLocs[numLaid - 2];
			} else if (strcmp(abbrev, "D3") == 0) {
				laidLocs[numLaid] = laidLocs[numLaid - 3];
			} else if (strcmp(abbrev, "D4") == 0) {
				laidLocs[numLaid] = laidLocs[numLaid - 4];
			} else if (strcmp(abbrev, "D5") == 0) {
				laidLocs[numLaid] = laidLocs[numLaid - 5];
			} else {
				laidLocs[numLaid] = placeAbbrevToId(abbrev);
			}
			numLaid++;
		}

	}
	
	// Locations where hunters have encounter a trap
	for (int i = 0; i < numMoveHistory; i++) {
		for (int j = 0; j < NUM_PLAYERS - 1; j++) {
	      char *hunterMove = getPlayerMove(gv->pastPlays, j, i);
	      if (hunterMove != NULL) {
	         for (int k = 3; k < 7; k++) {
	            if (hunterMove[k] == 'T') {
	               char *abbrev = strndup(hunterMove + 1, 2);
						removedLocs[numDestroyed] = placeAbbrevToId(abbrev);
						numDestroyed++;;
	            }
	         }
	      }
	   }
	}
	
	printf("--------------number of laid traps: %d--------------\n", numLaid);
	for (int i = 0; i < numLaid; i++) {
		printf("%s\n", placeIdToName(laidLocs[i]));
	}
	
	printf("--------------number of destroyed traps: %d--------------\n", numDestroyed);
	for (int i = 0; i < numDestroyed; i++) {
		printf("%s\n", placeIdToName(removedLocs[i]));
	}
	
	*numTraps = numLaid - numDestroyed;
	
	PlaceId *trapLocs = malloc((*numTraps) * sizeof(*trapLocs));
	
	// Record locations where traps exist
	for (int i = 0, j = 0; i < numLaid; i++) {
		bool isRemoved = false;
		for (int k = 0; k < numDestroyed; k++) {
			if (laidLocs[i] == removedLocs[k]) {
				isRemoved = true;
			}
		}
		if (!isRemoved && placeIsLand(laidLocs[i])) {
			trapLocs[j] = laidLocs[i];
			j++;
		}
	}
	
	printf("--------------number of remaining traps: %d--------------\n", *numTraps);
	for (int i = 0; i < *numTraps; i++) {
		printf("%s\n", placeIdToName(trapLocs[i]));
	}
	
	return trapLocs;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: DONE!
   *canFree = true;

   return playerMoveHistory(gv, player, numReturnedMoves);
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: DONE!
	*canFree = true;

	return playerLastMoves(gv, player, numMoves, numReturnedMoves);
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
   // TODO: DONE!
   *canFree = true;

   PlaceId *locationHistory = playerMoveHistory(gv, player, numReturnedLocs);

   if (player == PLAYER_DRACULA) {
      findDraculaLocation(*numReturnedLocs, locationHistory);
   }
   return locationHistory;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
   // TODO: DONE!
   *canFree = true;
   
   PlaceId *lastLocs;
   if (player != PLAYER_DRACULA) {
      lastLocs = playerLastMoves(gv, player, numLocs, numReturnedLocs);
   // dracula's moves need to be referenced
   } else {
      PlaceId *locationHistory = GvGetLocationHistory(gv, player,
                                                      numReturnedLocs, canFree);
      // shortening the locationHistory array
      if (numLocs < *numReturnedLocs) {
         lastLocs = malloc(numLocs * sizeof(*lastLocs));
         for (int i = 0; i < numLocs; i++) {
            lastLocs[i] = locationHistory[*numReturnedLocs - numLocs + i];         
         }
         free(locationHistory);
         *numReturnedLocs = numLocs;
      } else {
         return locationHistory;
      }
   }
   return lastLocs;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: DONE!
	return GvGetReachableByType(gv, player, round, from, true, true, true, 
	                     numReturnedLocs);
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: DONE!
	Map europe = MapNew();
	
	int numRoads = 0;
	PlaceId connRoad[NUM_REAL_PLACES];
	
	int numRails = 0;
	PlaceId connRail[NUM_REAL_PLACES];
	
	int numBoats = 0;
	PlaceId connBoat[NUM_REAL_PLACES];
	
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		connRoad[i] = NOWHERE;
		connRail[i] = NOWHERE;
		connBoat[i] = NOWHERE;
	}
	
	if (road) {
		
		// Get list of road connections from selected point
		nearby(europe, from, connRoad, &numRoads, ROAD);
		
		if (player == PLAYER_DRACULA) {
			for (int i = 0; i < numRoads; i++) {
				if (connRoad[i] == ST_JOSEPH_AND_ST_MARY) {
					connRoad[i] = NOWHERE;
				}
			}
		}
		
		connRoad[numRoads] = from;
		numRoads++;
	}
	
	int travelRail = (player + round) % 4;
	
	// Dracula can't travel by rail
	if (travelRail == 0 || player == PLAYER_DRACULA) {
		connRail[0] = from;
	}
	
	if (rail && travelRail > 0 && player != PLAYER_DRACULA) {
		
		printf("Allowed travel distance: %d\n", travelRail);
		
		int firstStations = 0;
		PlaceId connFirst[NUM_REAL_PLACES];
		
		int secStations = 0;
		PlaceId connSec[NUM_REAL_PLACES];
		
		int thirdStations = 0;
		PlaceId connThird[NUM_REAL_PLACES];
		
		int numStations = 0;
		PlaceId noDups[NUM_REAL_PLACES];
		
		int numStationsSec = 0;
		PlaceId noDupsSec[NUM_REAL_PLACES];
		
		for (int i = 0; i < NUM_REAL_PLACES; i++) {
			connFirst[i] = NOWHERE;
			connSec[i] = NOWHERE;
			connThird[i] = NOWHERE;
			noDups[i] = NOWHERE;
			noDupsSec[i] = NOWHERE;
		}
		
		// Get first set of rail connections
		nearby(europe, from, connFirst, &firstStations, RAIL);
		if (travelRail >= 1) {
			for (int i = 0; i < firstStations; i++) {
				connRail[i] = connFirst[i];
				numRails++;
			}
		}
		
		printf("---------number of rail connections 1: %d---------\n", numRoads);
		for (int i = 0; i < numRails; i++) {
			printf("%s\n", placeIdToName(connRail[i]));
		}
		
		// Get second set of rail connections
		for (int i = 0; i < firstStations; i++) {
			nearby(europe, connFirst[i], connSec, &secStations, RAIL);
		}
		
		numStations = removeDups(connSec, noDups, NUM_REAL_PLACES, 
		                         NUM_REAL_PLACES);
		
		// Appending second set of stations to reachable locations
		if (travelRail >= 2) {
			for (int i = 0; i < numStations; i++) {
				connRail[numRails] = noDups[i];
				numRails++;
			}
		}
		
		printf("---------number of rail connections 1: %d---------\n", numRoads);
		for (int i = 0; i < numRails; i++) {
			printf("%s\n", placeIdToName(connRail[i]));
		}
		
		// Get third set of rail connections
		for (int i = 0; i < secStations; i++) {
			nearby(europe, connSec[i], connThird, &thirdStations, RAIL);			
		}
		
		numStationsSec = removeDups(connThird, noDupsSec, NUM_REAL_PLACES, 
		                            NUM_REAL_PLACES);
		
		// Append third set of stations to reachable locations
		if (travelRail == 3) {
			for (int i  = 0; i < numStationsSec; i++) {
				connRail[numRails] = noDupsSec[i];
				numRails++;
			}
		}
		printf("---------number of rail connections 3: %d---------\n", numRoads);
		for (int i = 0; i < numRails; i++) {
			printf("%s\n", placeIdToName(connRail[i]));
		}
		
		connRail[numRails] = from;
		numRails++;
	}
	
	if (boat) {
		// Get list of boat connections from selected point
		nearby(europe, from, connBoat, &numBoats, BOAT);
		
		connBoat[numBoats] = from;
		numBoats++;
	}
	
	// Append road, boat and rail connections
	PlaceId connTotalDups[NUM_REAL_PLACES];
	int totalDups = 0;
	
	PlaceId connTotal[NUM_REAL_PLACES];
	int numTotal = 0;
	
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		connTotalDups[i] = NOWHERE;
		connTotal[i] = NOWHERE;
	}
	
	for (int i = 0; i < numRoads; i++) {
		connTotalDups[totalDups] = connRoad[i];
		totalDups++;
	}
	
	printf("---------number of road connections: %d---------\n", numRoads);
	for (int i = 0; i < numRoads; i++) {
		printf("%s\n", placeIdToName(connRoad[i]));
	}
	
	for (int i = 0; i < numBoats; i++) {
		connTotalDups[totalDups] = connBoat[i];
		totalDups++;
	}
	
	printf("---------number of boat connections: %d---------\n", numBoats);
	for (int i = 0; i < numBoats; i++) {
		printf("%s\n", placeIdToName(connBoat[i]));
	}
	
	for (int i = 0; i < numRails; i++) {
		connTotalDups[totalDups] = connRail[i];
		totalDups++;
	}
	
	printf("---------number of rail connections: %d---------\n", numRails);
	for (int i = 0; i < numRails; i++) {
		printf("%s\n", placeIdToName(connRail[i]));
	}
	
	printf("---------number of connTotalDups: %d---------\n", totalDups);
	for (int i = 0; i < totalDups; i++) {
		printf("%s\n", placeIdToName(connTotalDups[i]));
	}
	
	numTotal = removeDups(connTotalDups, connTotal, NUM_REAL_PLACES, 
	                      NUM_REAL_PLACES);
	
	printf("----------value of numTotal: %d---------------\n", numTotal);
	
	*numReturnedLocs = numTotal;
	
	PlaceId *connections = malloc((*numReturnedLocs) * sizeof(*connections));
	
	// Copy appended connecitons to total reachable locations
	for (int i = 0; i < numTotal; i++) {
		connections[i] = connTotal[i];
	}
	
	return connections;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO:


// helper function definitions:

//------------------------- general helper functions ---------------------------

// returns a 7-letter string containing a player's last move
// if player has not made a turn at all, returns NULL
char *getLastMove(GameView gv, Player player)
{
   // TODO: DONE!
   
   // player has not made a turn at all
   if (GvGetRound(gv) == 0 && GvGetPlayer(gv) <= player) {
      return NULL;
   }



   char *string = strdup(gv->pastPlays);

   char delim[] = " ";
   char *token;
   char *move;

   // looks through the pastPlays string until the end
   token = strtok(string, delim);
   for (int i = 0; token != NULL; i++) {
      // if move is made by player, it is updated
      if (i % NUM_PLAYERS == player) {
         move = token;
      }
      token = strtok(NULL, delim);
   }
   return move;
}

// returns a 7-letter string containing a player's move in a given round
// if the player has not made a move in the given round, it will return NULL
char *getPlayerMove(char *pastPlays, Player player, Round round)
{
   // TODO: DONE!

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

// checks if a player has been to a certain place within the specified rounds
bool placeMatch(char *pastPlays, Player player, PlaceId place,
                Round roundStart, Round roundEnd)
{
   // TODO: DONE!

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
   // TODO: DONE!

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
   // TODO: DONE!

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

// returns a bunch of strings


// ------------------ move-location helper functions ---------------------------

// helper function to find player move history
PlaceId *playerMoveHistory(GameView gv, Player player, int *numReturnedMoves)
{
    // TODO: DONE!
    
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
    // TODO: DONE!
    
	int numMoveHistory = GvGetRound(gv) + 1;
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
   // TODO: DONE!
   
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

//------------------------- score helper function ------------------------------

// counts the total number of hunter deaths during the game
int hunterDeathCount(GameView gv)
{
   // TODO: DONE!
      
   int deathCount = 0;
   
   // loops through hunter
   for (int j = 0; j < NUM_PLAYERS - 1; j++) {
      int health = GAME_START_HUNTER_LIFE_POINTS;
      PlaceId prevLoc = NOWHERE;
      PlaceId currLoc;
      // checks when the health goes to zero
      for (int i = 0; i <= GvGetRound(gv); i++) {
         char *move = getPlayerMove(gv->pastPlays, j, i);
         if (move != NULL) {
            char *abbrev = strndup(move + 1, 2);
            currLoc = placeAbbrevToId(abbrev);         
            health = updateHunterHealth(move, health, prevLoc, currLoc);
            prevLoc = currLoc;
            // increment count if hunter dies
            if (health == 0) {
               deathCount++;
            }
         }
      }
   }
   return deathCount;
}

//------------------------ health helper functions -----------------------------

// updates the hunter's health according to the encounters in the turn
int updateHunterHealth(char *move, int health, PlaceId prevLoc, PlaceId currLoc)
{
   // TODO: DONE!
   // reset health (respawn at hospital)
   if (health == 0) {
      health = GAME_START_HUNTER_LIFE_POINTS;
   }
   
   // reading relevant characters for encounter
   for (int i = 3; i < 7; i++) {
      if (move[i] == 'T') {
         health -= LIFE_LOSS_TRAP_ENCOUNTER;
      } else if (move[i] == 'D') {
         health -= LIFE_LOSS_DRACULA_ENCOUNTER;
      }
   }
   
   // hunter at rest
   if (prevLoc == currLoc) {
      health += LIFE_GAIN_REST;   
   }
   
   // capping hunter hp
   if (health > GAME_START_HUNTER_LIFE_POINTS) {
      health = GAME_START_HUNTER_LIFE_POINTS;
   } else if (health < 0) {
      health = 0;
   }
   return health;
}


// updates the dracula's health according to the encounters during the turn
int updateDraculaHealth(GameView gv, char *move, int health)
{
   // TODO: DONE!
   // checking hunter encounters
   if (move[0] != 'D') {
      for (int i = 3; i < 7; i++) {
         if (move[i] == 'D') {
            health -= LIFE_LOSS_HUNTER_ENCOUNTER;
         }
      }
   // checks for castle dracula and if dracula is at sea
   } else {
      int numReturnedLocs = 0;
      bool canFree = true;
      PlaceId *location = GvGetLastLocations(gv, PLAYER_DRACULA, 1,
                                             &numReturnedLocs, &canFree);
      if (placeIdToType(location[0]) == SEA) {
         health -= LIFE_LOSS_SEA;
      } else if (location[0] == CASTLE_DRACULA) {
         health += LIFE_GAIN_CASTLE_DRACULA;
      }
      free(location);
   }
   return health;
}

//------------------------ reachable helper functions --------------------------

// Store connections from a selected point into an array
void nearby(Map m, PlaceId from, PlaceId *near, int *size, int type)
{
	ConnList curr;
	
	for (curr = MapGetConnections(m, from); curr !=NULL; curr = curr->next) {
		if (curr->type == type || curr->type == ANY) {
			near[*size] = curr->p;
			(*size)++;
		}
	}
}

// Remove duplicates from an array and store in empty array
int removeDups(PlaceId *arrayDups, PlaceId *arrayEmpty, int size, int size_2)
{
	int j = 0;
   for (int i = 0; i < size; i++) {
     bool inArray = false;
     
     for (int k = 0; k < size_2; k++) {
         if (arrayDups[i] == arrayEmpty[k]) {
             inArray = true;
         }
     }
     
     if (!inArray && placeIsReal(arrayDups[i])) {
         arrayEmpty[j] = arrayDups[i];
         j++;
     }
   }
   return j;
}
