#include <SigFox.h>

#include <RTCZero.h>
#include <ArduinoLowPower.h>

void setup() {

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  
  // open the serial port at 9600 bps 
  Serial.begin(9600);           
  
  // A special way to handle serial communication with MKRFox1200. 
  // Since the MKRFox1200 doesn't have an autoreset function 
  // associated to the Serial monitor opening, a Serial while is present
  while(!Serial) {};
  //We also check that the SigFox module is present and can be used.
  if (!SigFox.begin()) {   
     Serial.println("Shield error or not present. Rebooting!");
     NVIC_SystemReset();
     while(1);
}
  //in non debugging mode the SAMD chip is switched to sleep mode
  //waiting for Sigfox communication to terminate. 
  //It never comes back from this state. 
  SigFox.reset();
  delay(100);
  SigFox.debug();
  SigFox.end();

   // We need to have time to program the Arduino after a reset
   // Otherwise it does not respond when in low-power mode
   Serial.println("Booting...");
   digitalWrite(LED_BUILTIN,HIGH);
   delay(5000);
   digitalWrite(LED_BUILTIN,LOW);
}

// A container for the sigfox message
typedef struct __attribute__ ((packed)) sigfox_message {
String packet;
} SigfoxMessage;

void loop() {
   SigFox.begin();
   SigFox.status();

   // Initialize an instance of the container for the msg
   SigfoxMessage msg;
   
   msg.packet = "hello world";
   
  // Every SigFox packet cannot exceed 12 bytes
  // If the string is longer, only the first 12 bytes will be sent
  if (msg.packet.length() > 12) {
    Serial.println("Message too long, only first 12 bytes will be sent");
  }
  Serial.println("Sending " + msg.packet);

  // Remove EOL
  msg.packet.trim();


  Serial.println("Getting the response will take up to 50 seconds");
  Serial.println("The LED will blink while the operation is ongoing");
  
  //sendString(msg.packet);

  //send and read response
  sendStringAndGetResponse(msg.packet);

  // Wait for 10 minutes.
  // Low Power version - be carefull of bug
  // LowPower.sleep(10*60*1000);
  // Normal version
  delay(10*60*1000);
}

void sendString(String str) {
  // Start the module
  SigFox.begin();
  
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.print(str);

  // send buffer to SIGFOX network
  int ret = SigFox.endPacket();  
  if (ret > 0) {
    Serial.println("No transmission");
  } else {
    Serial.println("Transmission ok");
  }

  Serial.println(SigFox.status(SIGFOX));
  Serial.println(SigFox.status(ATMEL));
  SigFox.end();
}

void sendStringAndGetResponse(String str) {
  // Start the module
  SigFox.begin();
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.print(str);

  // send buffer to SIGFOX network and wait for a response
  int ret = SigFox.endPacket(true);  
  if (ret > 0) {
    Serial.println("No transmission");
  } else {
    Serial.println("Transmission ok");
  }

  Serial.println(SigFox.status(SIGFOX));
  Serial.println(SigFox.status(ATMEL));

  if (SigFox.parsePacket()) {
    Serial.println("Response from server:");
    while (SigFox.available()) {
      Serial.print("0x");
      Serial.println(SigFox.read(), HEX);
    }
  } else {
    Serial.println("Could not get any response from the server");
    Serial.println("Check the SigFox coverage in your area");
    Serial.println("If you are indoor, check the 20dB coverage or move near a window");
  }
  Serial.println();

  SigFox.end();
}
