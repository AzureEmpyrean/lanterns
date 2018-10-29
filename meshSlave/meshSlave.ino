
/** RF24Mesh_Example.ino by TMRh20
 *
 * This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
 * master node.
 * The nodes will refresh their network address as soon as a single write fails. This allows the
 * nodes to change position in relation to each other and the master node.
 */


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(10, 9);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 *
 * In this example, configuration takes place below, prior to uploading the sketch to the device
 * A unique value from 1-255 must be configured for each node.
 * This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 *
 **/
#define nodeID 1


uint32_t displayTimer = 0;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};


#define BTN_TIMEOUT 800     // button hold delay, ms
#define BTN              13
boolean btnState, btn_flag, hold_flag;
byte btn_counter;
unsigned long btn_timer;



void setup() {

  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();

    pinMode(BTN, INPUT_PULLUP);
}



void loop() {
  //Serial.println("green");
  mesh.update();
  //btnTick();
  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
      String message = "bloo kat";
      if(mesh.write(&message,'S',sizeof(message)) ) Serial.println(message);
//     if(!mesh.write(&message,'M',sizeof(message))){
//            if( ! mesh.checkConnection() ){
//              Serial.println("Renewing Address");
//              mesh.renewAddress(); 
//            }else{
//              Serial.println("Send fail, Test OK"); 
//            }
//    }
      
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer))) {

      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  }

  while (network.available()) {
    Serial.println("blue");
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
  }
}



void btnTick() {
  btnState = !digitalRead(BTN);
  if (btnState && !btn_flag) {
    btn_flag = 1;
    btn_counter++;
    btn_timer = millis();
    delay(250);
  }
  if (!btnState && btn_flag) {
    btn_flag = 0;
    hold_flag = 0;
  }

  if (btn_flag && btnState && (millis() - btn_timer > BTN_TIMEOUT) && !hold_flag) {
  //  ls_chg_state = 1;                     // flag to change saber state (on/off)
    hold_flag = 1;
    btn_counter = 0;
    //tx
    Serial.println("btnHold");
  }
  
  if ((millis() - btn_timer > BTN_TIMEOUT) && (btn_counter != 0)) {
    if (btn_counter == 1) {               // single press count
            //tx
            Serial.println("btnx1");
          }
    if (btn_counter == 3) {               // 3 press count
        //mode++;                         // change mode
        //if (mode > modeCt) mode = 0;
        //serial.print();
        //tx("btnx3");
       // //tx("m" + (String)mode );
      }
      
      if (btn_counter == 5) {               // 5 press count         
      //tx
      Serial.println("btnx5");
    }
    btn_counter = 0;
  }
}





