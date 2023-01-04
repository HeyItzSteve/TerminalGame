#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "game.h"
#define USER_KEYS 5

int checkInput(char input,char* userKeys);
int checkUnitCollision(char nextLocation, int x, int y);
void unitInteraction(char nextLocation, char* playerIcon);

char player, playerIcon, powered; //Player Icon
int powerCheck = 0; //Powered Status
int playerX = 0; //Player Location (x-axis)
int playerY = 0; //Player Location (y-axis)
int powerDuration = 0; //Duration of Powered State

int boardHeight = 0; //Playable Area Dimensions
int boardWidth = 0;

char input; //Movement/Quit Input
int score = 0; //Player Score
int items = 0; //Item Counter

int playGame(const char* filename) {
  if(!filename)
    return 0;
  FILE* data = fopen(filename, "r");
  if(!data)
    return 0;

  char gameData[UCHAR_MAX]; //Game Data File Reading
  int lineNum = 0; //File Read Counter

  char userKeys[USER_KEYS] = "adwsq"; //moveLeft,moveRight,moveUp,moveDown,quitGame

  int totalObjects = 0; //Total objects
  int currObject = 0; //Array use (file reading)
  char object;
  int objX, objY; //Object Location

  while(fgets(gameData,sizeof(gameData),data) != NULL) { //Total Game Objects
    totalObjects++;
  }
  totalObjects -= 3;

  char objects[totalObjects]; //Stores all object types @ currObject
  int objectLoc[totalObjects][2]; //Stores object location @ currObject

  rewind(data);
  while(fgets(gameData,sizeof(gameData),data)) { //File Reading
    if(lineNum == 0) //Board Dimensions
      sscanf(gameData, "%d %d", &boardHeight, &boardWidth);
    if(lineNum == 1) //Controls (if any)
      sscanf(gameData, " %c %c %c %c %c", &userKeys[0],&userKeys[1],&userKeys[2],&userKeys[3],&userKeys[4]);
    if(lineNum == 2) //Initial Player Info
      sscanf(gameData, " %c %c %d %d", &player,&powered,&playerX,&playerY);

    if(lineNum > 2) { //Objects (Items)
      sscanf(gameData, "%c %d %d", &object,&objX,&objY);

      objects[currObject] = object;
      objectLoc[currObject][0] = objX;
      objectLoc[currObject][1] = objY;
      currObject++;
    }
    lineNum++;
  }
  fclose(data);

  for(int i = 0; i < totalObjects; i++) { //Item Counter
    if(objects[i] == 'I')
      items++;
  }

  int borderWidth = boardWidth+2; //Width & Height with Border
  int borderHeight = boardHeight+2;
  char board[borderHeight][borderWidth];

  for(int row = 0; row < borderHeight; row++) { //Board Creation
    for(int col = 0; col < borderWidth; col++) {
      board[row][col] = ' ';
    }
  }
  for(int i = 0; i < borderWidth; i++) { //Border Draw
    board[0][i] = '*';
    board[borderHeight-1][i] = '*';
  }
  for(int i = 0; i < borderHeight; i++) {
    board[i][0] = '*';
    board[i][borderWidth-1] = '*';
  }

  playerIcon = player; //Player Icon Draw
  board[playerX+=1][playerY+=1] = playerIcon;
  
  for(int i = 0; i < totalObjects; i++) { //Object Draw
    if(objects[i] == 'I')
      board[objectLoc[i][0]+=1][objectLoc[i][1]+=1] = '$';

    if(objects[i] == 'B') {
      int x = objectLoc[i][0]+=1;
      int y = objectLoc[i][1]+=1;
      for(int row = 0; row < 4; row++) {
        for(int col = 0; col < 6; col++) {
          board[x+row][y+col] = '-';
        }
      }
      for(int row = 2; row < 4; row++) {
        for(int col = 2; col < 4; col++) {
          board[x+row][y+col] = '&';
        }
      }
    }
    if(objects[i] == 'E')
      board[objectLoc[i][0]+=1][objectLoc[i][1]+=1] = 'X';

    if(objects[i] == 'P')
      board[objectLoc[i][0]+=1][objectLoc[i][1]+=1] = '*';
  }

  while(input != userKeys[4]) { //Game Start
    char locationCheck;

    printf("Score: %d\n",score);
    printf("Items remaining: %d\n", items);
    for(int row = 0; row < borderHeight; row++) { //Prints Current Board Status
      for(int col = 0; col < borderWidth; col++) {
        printf("%c",board[row][col]);
      }
      printf("\n");
    }

    if(playerIcon == '@') { //Player Death = Lose
      printf("You have died.\n");
      break;
    }

    printf("Enter input: "); //Player Input
    scanf(" %c",&input);
    while(!checkInput(input,userKeys)) {
      printf("Invalid input.\n");
      printf("Enter input: ");
      scanf(" %c",&input);
    }

    if(input == userKeys[4]) { //Player Input = Quit
      printf("You have quit.\n");
      break;
    }

    if(powerCheck) { //Powered Check if Powered (duration check)
      powerDuration+=1;
      if(powerDuration == 7) {
        powerCheck = 0;
        powerDuration = 0;
        playerIcon = player;
        board[playerX][playerY] = player;
      }
    }

    if(input == userKeys[0]) { //Movement Logic
      locationCheck = board[playerX][playerY-1];
      if(checkUnitCollision(locationCheck,playerX,playerY-1)) {
        board[playerX][playerY] = ' ';
        unitInteraction(locationCheck, &playerIcon);
        board[playerX][playerY-=1] = playerIcon;
      }
    }
    if(input == userKeys[1]) {
      locationCheck = board[playerX][playerY+1];
      if(checkUnitCollision(locationCheck,playerX,playerY+1)) {
        board[playerX][playerY] = ' ';
        unitInteraction(locationCheck, &playerIcon);
        board[playerX][playerY+=1] = playerIcon;
      }
    }
    if(input == userKeys[2]) {
      locationCheck = board[playerX-1][playerY];
      if(checkUnitCollision(locationCheck,playerX-1,playerY)) {
        board[playerX][playerY] = ' ';
        unitInteraction(locationCheck, &playerIcon);
        board[playerX-=1][playerY] = playerIcon;
      }
    }
    if(input == userKeys[3]) {
      locationCheck = board[playerX+1][playerY];
      if(checkUnitCollision(locationCheck,playerX+1,playerY)) {
        board[playerX][playerY] = ' ';
        unitInteraction(locationCheck, &playerIcon);
        board[playerX+=1][playerY] = playerIcon;
      }
    }
	
    if(items < 1) { //All Items Collected = Win
      printf("Congratulations! You have won.\n");
      break;
    }
  }
  printf("Final score: %d\n", score);
  return 1;
}

void unitInteraction(char nextLocation, char* playerIcon) { //Interaction with objects
  if(nextLocation == '$') {
    items-=1;
    score+=1;
  }
  if(nextLocation == 'X') {
    if(powerCheck)
      score+=1;
    else
      *playerIcon = '@';
  }
  if(nextLocation == '*') {
    *playerIcon = powered;
    powerCheck = 1;
    powerDuration = 0;
  }
}

int checkUnitCollision(char nextLocation, int x, int y) { //Interaction with buildings
  if(nextLocation != '-' && nextLocation != '&' && y <= boardWidth && x <= boardHeight && x >= 1 && y >= 1)
    return 1;
  return 0;
}

int checkInput(char input,char* userKeys) { //Input checker (for invalid inputs)
  for(int i = 0; i < USER_KEYS; i++) {
    if(input == userKeys[i])
      return 1;
  }
  return 0;
}