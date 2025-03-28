// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

const int xLength=7,yLength=6;
int array[xLength][yLength];
// setup() runs once, when the device is first turned on
void setup() {
  //reset array on boot, (0=open, 1=player 1, 2=player 2)
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      array[x][y]=0;
    }
  }
  Serial.begin(9600);
}

void loop() {
  // Log.info("Sending Hello World to the cloud");
  // Particle.publish("Hello world");
  // delay( 10 * 1000 ); // milliseconds and blocking
}