#include <SigFox.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

// the setup function runs once when you press reset or power the board
void setup() {
  // open the serial port at 9600 bps
  Serial.begin(9600);       

  // A special way to handle serial communication with MKRFox1200. 
  // Since the MKRFox1200 doesn't have an autoreset function 
  // associated to the Serial monitor opening, a Serial while is present
  while(!Serial) {};
  if (!SigFox.begin()) {
    // We also check that the SigFox module is present and can be used.
    Serial.println("Shield error or not present!");
    return;
}
  //Get the necessary device information for activation on Sigfox. 
  String version = SigFox.SigVersion();
  String ID = SigFox.ID();
  String PAC = SigFox.PAC();

  // Display module informations on serial monitor
  Serial.println("MKRFox1200 Sigfox first configuration");
  Serial.println("SigFox FW version " + version);
  Serial.println("ID  = " + ID);
  Serial.println("PAC = " + PAC);
  Serial.println("");
  Serial.println("Register your board on https://backend.sigfox.com/activate with provided ID and PAC");
  delay(100);
  // Send the module to the deepest sleep
  SigFox.end();
}
// the loop function runs over and over again forever
void loop() {                     
}
