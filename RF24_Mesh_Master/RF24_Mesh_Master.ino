/** Modification of RF24Mesh_Example_Master.ino by TMRh20 and RF24Mesh_Example_Master_Statics by TMRh20


   This example sketch shows how to send data to nodes bassed on their ID.

   The nodes can change physical or logical position in the network, and reconnect through different
   routing nodes as required. The master node manages the address assignments for the individual nodes
   in a manner similar to DHCP.

 **/


#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>


/***** Configure the chosen CE,CS pins *****/
RF24 radio(4, 5);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

struct payload_t {
  unsigned long ms;
  unsigned long counter;
  int x;
  int y;
};

uint32_t ctr = 0;

uint32_t displayTimer = 0;

//++++++++++++++++++++++++++++++++ SETUP  +++++++++++++++++++++++++++++++++++++++++//

void setup() {
  Serial.begin(115200);
  Serial.println("######################################");
  Serial.println("###### ESP32 MESH MASTER NODE ########");
  Serial.println("######################################");
  Serial.println();
  Serial.print("NODE ID ");
  
  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();

} //setup

//**********************************  LOOP  ******************************************//

void loop() {

  // Call mesh.update to keep the network updated
  mesh.update();

  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();


  // Check for incoming data from the sensors
  if (network.available()) {
    RF24NetworkHeader header;
    network.peek(header);
    
    Serial.print("Got ");
    uint32_t dat = 0;
    switch (header.type) {
      // Display the incoming millis() values from the sensor nodes
      case 'M':
        network.read(header, &dat, sizeof(dat));
        Serial.print(dat);
        Serial.print(" from RF24Network address 0");
        Serial.println(header.from_node, OCT);
        break;
      default:
        network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }
  }

if(mesh.addrListTop){
  Serial.println("Sending to NODES...");
  
  // Send each node a message every five seconds
  // Send a different message to node 1, containing another counter instead of millis()
  if (millis() - displayTimer > 8000) {
    ctr++;
    for (int i = 0; i < mesh.addrListTop; i++) {
      payload_t payload = {millis(), ctr};
      if (mesh.addrList[i].nodeID == 1) {  //Searching for node one from address list
        //payload = {ctr % 3, ctr};
        payload.ms = ctr % 3;
        payload.counter = ctr;

        
      }
      RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
      Serial.println( network.write(header, &payload, sizeof(payload)) == 1 ? F("Send OK") : F("Send Fail")); //Sending an message

    }
    displayTimer = millis();
  }//if
}//if

    if(millis() - displayTimer > 4000){
    displayTimer = millis();
    Serial.println(" ");

    if(mesh.addrListTop == 0){
      Serial.println("No NODES detected..");
      }//if
    else{  
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i<mesh.addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(mesh.addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(mesh.addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
    } //else
  }
}  //loop






//-----------------------------------------------------------------------------------------//
