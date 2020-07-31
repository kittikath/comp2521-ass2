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
	// TODO: DONE!
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
      // TODO: keep this commented code, might be handy/
      //if (health == 0 && GvGetPlayerLocation(gv, player) == HOSPITAL_PLACE) {
      //   health = GAME_START_HUNTER_LIFE_POINTS;
      //}
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
   // TODO: DONE! - needs testing
   
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
	// TODO: DONE! - but needs testing

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
	// TODO: still needs considering
	int numMoveHistory = GvGetRound(gv) + 1;
	
	int laidTraps = 0;
	
	int destroyedTraps = 0;
	
	*numTraps = 0;
	
	PlaceId *totalTraps = malloc(numMoveHistory * sizeof(*totalTraps));
	
	PlaceId *trapLocations = malloc(laidTraps * sizeof(*trapLocations));
	
	PlaceId *removeLocations = malloc(destroyedTraps * sizeof(*removeLocations));
	
	// Locations where Dracula laid a trap
	for (int i = 0; i < numMoveHistory - 1; i++) {
		char *move = getPlayerMove(gv->pastPlays, PLAYER_DRACULA, i);
		if (strncmp(move + 3, "T", 1) == 0) {
			char *abbrev = strndup(move + 1, 2);
			
			// Dracula hides or double-backs
			if (strcmp(abbrev, "HI") == 0 || strcmp(abbrev, "D1") == 0) {
				trapLocations[laidTraps] = trapLocations[laidTraps - 1];
			} else if (strcmp(abbrev, "D2") == 0) {
				trapLocations[laidTraps] = trapLocations[laidTraps - 2];
			} else if (strcmp(abbrev, "D3") == 0) {
				trapLocations[laidTraps] = trapLocations[laidTraps - 3];
			} else if (strcmp(abbrev, "D4") == 0) {
				trapLocations[laidTraps] = trapLocations[laidTraps - 4];
			} else if (strcmp(abbrev, "D5") == 0) {
				trapLocations[laidTraps] = trapLocations[laidTraps - 5];
			} else {
				trapLocations[laidTraps] = placeAbbrevToId(abbrev);
			}
			(laidTraps)++;
		}
	}
	
	// Locations where hunters have encounter a trap
	for (int i = 0; i < numMoveHistory; i++) {
		for (int j = 0; j < 4; j++) {
			char *hunterMove = getPlayerMove(gv->pastPlays, j, i);
			if (strncmp(hunterMove + 3, "T", 1) == 0) {
				char *abbrev = strndup(hunterMove + 1, 2);
				removeLocations[destroyedTraps] = placeAbbrevToId(abbrev);
				(destroyedTraps)++;
			}
		}
	}
	
	// Record locations where traps exist
	for (int i = 0; i < laidTraps; i++) {
		if (trapLocations[i] != removeLocations[i]) {
			totalTraps[*numTraps] = trapLocations[i];
			(*numTraps)++;
		}
	}
	
	// Free memory
	free(trapLocations);
	
	free(removeLocations);
	
	return totalTraps;
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
	// TODO: Need to include rail case. Needs testing.
	return GvGetReachableByType(gv, player, round, from, true, true, true, 
	                     numReturnedLocs);
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: done?
	
	*numReturnedLocs = 1;
	
	PlaceId *connections = malloc((*numReturnedLocs) * sizeof(connections));
	
	Map europe = MapNew();
	
	connections[0] = from;
	
	if (road) {
		
		int addRoad = 1;
		
		PlaceId insert[NUM_REAL_PLACES];
		
		nearby(europe, from, insert, &addRoad, ROAD);
		
		for (int i = 1; i < addRoad; i++) {
			if (player == PLAYER_DRACULA && insert[i] == ST_JOSEPH_AND_ST_MARY) {
				continue;
			} else {
				connections[*numReturnedLocs] = insert[i];
				(*numReturnedLocs)++;
			}
		}
	}
	
	if (boat) {
	
		nearby(europe, from, connections, numReturnedLocs, BOAT);
	}
	
	// PlaceId *nearby(Map m, PlaceId from, PlaceId *near, int *size, int type)
	
	if (rail) {
		
		int travelRail = (player + round) % 4;
		
		printf("Allowed travel distance: %d\n", travelRail);
		
		int railStations = 0;
		PlaceId connRail[NUM_REAL_PLACES];
		
		int nextStations = 0;
		PlaceId connNext[NUM_REAL_PLACES];
		
		int secStations = 0;
		PlaceId connSec[NUM_REAL_PLACES];
		
		int numStations = 0;
		PlaceId noDups[NUM_REAL_PLACES];
		
		int numStationsSec = 0;
		PlaceId noDupsSec[NUM_REAL_PLACES];
		
		// Outputs
		// int numRails = 0;
		// Placeid railConnections[numRail];
		
		if (travelRail == 0 || player == PLAYER_DRACULA) {
			return connections;
		}
		
		nearby(europe, from, connRail, &railStations, RAIL);
		if (travelRail >= 1) {
			for (int i = 0; i < railStations; i++) {
				connections[*numReturnedLocs] = connRail[i];
				(*numReturnedLocs)++;
			}
		}
		
		// int removeDups(PlaceId *arrayDups, PlaceId *arrayEmpty, int size, int size2);
		
		for (int i = 0; i < railStations; i++) {
			nearby(europe, connRail[i], connNext, &nextStations, RAIL);
		}
		
		numStations = removeDups(connNext, noDups, NUM_REAL_PLACES, NUM_REAL_PLACES);
		
		if (travelRail >= 2) {
			for (int i = 0; i < numStations; i++) {
				if (noDups[i] != from) {
					connections[*numReturnedLocs] = noDups[i];
					(*numReturnedLocs)++;
				}
			}
		}
/*			
		printf("---------------------Debugging------------------------\n");
		printf("---------------------travel = 2------------------------\n");
		printf("number of noDups: %d\n", numStations);
		printf ("locations:\n");
*/
		for (int i = 0; i < numStations; i++) {
			printf("%s\n", placeIdToName(noDups[i]));
		}
		
		// Remove duplicates of locations of 3rd stations
		for (int i = 0; i < nextStations; i++) {
			nearby(europe, connNext[i], connSec, &secStations, RAIL);			
		}
		
		numStationsSec = removeDups(connSec, noDupsSec, NUM_REAL_PLACES, NUM_REAL_PLACES);
		
		if (travelRail == 3) {
			for (int i = 0; i < numStationsSec; i++) {
				bool visited = false;
				for (int j= 0; j < railStations; j++) {
					if (noDupsSec[i] == connRail[j]) {
						visited = true;
					}
				}
				for (int k = 0; k < nextStations; k++) {
					if (noDupsSec[i] == connNext[k]) {
						visited = true;
					}
				}
				if (!visited) {
					connections[*numReturnedLocs] = noDupsSec[i];
					(*numReturnedLocs)++;
				}
			}
		}
/*		
		printf("---------------------travel = 3------------------------\n");
		printf("number of noDups: %d\n", numStations);
		printf ("locations:\n");
		for (int i = 0; i < numStations; i++) {
			printf("%s\n", placeIdToName(noDups[i]));
		}
		printf("\n");
		printf("number of secStations: %d\n", secStations);
		printf ("locations:\n");
		for (int i = 0; i < secStations; i++) {
			printf("%s\n", placeIdToName(connSec[i]));
		}
		printf("\n");
		printf("number of noDupsSec: %d\n", numStationsSec);
		printf ("locations:\n");
		for (int i = 0; i < numStationsSec; i++) {
			printf("%s\n", placeIdToName(noDupsSec[i]));
		}
		printf("-------------------End of debugging---------------------\n");
*/		
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
   // TODO: DONE! - needs a bit of testing
   
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
   // TODO: DONE! - needs a bit of testing

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
   // TODO: DONE! - needs a bit of testing

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
   // TODO: DONE! - needs a bit of testing

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
   // TODO: DONE! - needs a bit of testing

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

int removeDups(PlaceId *arrayDups, PlaceId *arrayEmpty, int size, int size_2) {
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
