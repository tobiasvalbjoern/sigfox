#include <SigFox.h>

//pin for trigger
int trig_pin = 0;

//assume that power is on as initial condition
int state_trig = HIGH;
int state_trig_old = HIGH;

int alarm_source = 1;

void setup() {

  // open the serial port at 9600 bps
  Serial.begin(9600);

  // A special way to handle serial communication with MKRFox1200.
  // Since the MKRFox1200 doesn't have an autoreset function
  // associated to the Serial monitor opening, a Serial while is present
  while (!Serial) {};

  if (!SigFox.begin()) {
    //something is really wrong, try rebooting
    Serial.println("Rebooting...");
    reboot();
  }

  //in non debugging mode the SAMD chip is switched to sleep mode
  //waiting for Sigfox communication to terminate.
  //It never comes back from this state.
  SigFox.reset();
  delay(100);

  //debug needs to be there if using serial.
  SigFox.debug();

  //Send module to standby until we need to send a message
  SigFox.end();


  // We need to have time to program the Arduino after a reset
  // Otherwise it does not respond when in low-power mode
  Serial.println("Booting...");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);

  //interrupt pin
  pinMode(trig_pin, INPUT);
  
  //turn on pull-up resistors. The input will stay high, unless the INPUT
  // is logic low.
  digitalWrite(trig_pin, HIGH);

}

void loop()
{

  power_off();
  power_on();

}


void reboot() {
  NVIC_SystemReset();
  while (1);
}


//Function for handling, when power goes down.
int power_off() {
  // read the input pin
  state_trig = digitalRead(trig_pin);

  if (state_trig == LOW && state_trig != state_trig_old) {

    //Initializes the Sigfox library and module
    SigFox.begin();

    // Wait at least 30mS after first configuration (100mS before)
    delay(100);

    // Clears all pending interrupts
    SigFox.status();
    delay(1);

    // 3 bytes (ALM) + 4 bytes (ID) + 1 byte (source) < 12 bytes
    String to_be_sent = "ALM" + SigFox.ID() +  String(alarm_source);
    
    Serial.println("Sending " + to_be_sent);

    //Begins the process of sending a packet
    SigFox.beginPacket();

    //Sends characters data to the SigFox's backend. 
    //This data is sent as a character or series of characters;
    SigFox.print(to_be_sent);

    //Called after writing SigFox data to the remote connection, 
    //completes the process of sending a packet started by beginPacket.
    int ret = SigFox.endPacket();
    
    if (ret > 0) {
      Serial.println("No transmission");
      return 0;
    } else {
      Serial.println("Transmission ok");
      state_trig_old = state_trig;
      delay(100);
    }
    Serial.println("Sigfox status");
    Serial.println(SigFox.status(SIGFOX));
    Serial.println("Atmel status: ");
    Serial.println(SigFox.status(ATMEL));

    // shut down module, back to standby
    SigFox.end();
  }
}

//Function for handling, when power comes back on
int power_on() {
  // read the input pin
  state_trig = digitalRead(trig_pin);
  if (state_trig == HIGH && state_trig != state_trig_old) {
    delay(100);
    state_trig_old = state_trig;
  }
}
