// Include Particle Device OS APIs
#include "Particle.h"
#include <iostream>
#include <string>

int getLowestEmptyRow(int col);
int setBoardFromString(String inputString);
int resetBoard(String inputString);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

const int xLength=7,yLength=6;
int board[yLength][xLength];

//adjust pins for final physical setup
const int pinCol[xLength]={D7,0,0,0,0,0,0};
int pinData[xLength];
const int LIGHT_SENSOR_THRESHOLD=2500;
String sendCoord="null";

void setup() {
  //reset array on boot, (0=open, 1=player 1 (cloud), 2=player 2 (photon))
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      board[x][y]=0;
    }
  }
  for(int x=0;x<xLength;x++){
    pinMode(pinCol[x], INPUT);
  }
  Particle.variable("grabChip", sendCoord);
  Particle.function("setBoard", setBoardFromString);
  Particle.function("resetBoard", resetBoard);
  Serial.begin(9600);
}

int pinPrev=-1;
void loop() {
  //check if sensor is still covered
  if(pinPrev!=-1&&pinData[pinPrev] < LIGHT_SENSOR_THRESHOLD){
    pinPrev=-1;
  }

  //goes through all sensors to see if one is covered
  if(pinPrev==-1){
    for(int x = 0; x < xLength; x++){
      pinData[x] = analogRead(pinCol[x]);
      if(pinData[x] > LIGHT_SENSOR_THRESHOLD){
        pinPrev=x;
        int row = getLowestEmptyRow(x);
        if(row != -1){
          board[row][x] = 2;
          sendCoord = row+","+x;
        }
        break;
      }
    }
  }

  //set lights on when board[row][col]==1

  // Log.info("Sending Hello World to the cloud");
  // Particle.publish("Hello world");
  // delay(1000); // milliseconds and blocking
}

//returns lowest empty row
int getLowestEmptyRow(int col) {
  for (int row = yLength-1; row >= 0; row--) {
  //for(let row=0;row<yLength;row++){
      if (board[row][col] == 0){
        Serial.println(col+","+row);
        return row;
      }
  }
  return -1;
}

//gets webpage coordinate string as row,col and sets array accordingly
//error if this cloud function too long?
int setBoardFromString(String inputString){
  Serial.println(inputString);

  //find location of comma
  int loc=0;
  for(int x=0;x<inputString.length();x++){
    if(inputString.charAt(x)==','){
      loc=x;
      break;
    }
  }
  int row=atoi(inputString.substring(0,loc));
  int col=atoi(inputString.substring(loc+1,inputString.length()));
  board[row][col]=1;
  return 1;
}

//resets board
int resetBoard(String inputString){
  return -1;
}