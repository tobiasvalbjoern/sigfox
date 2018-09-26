/*
  SigFox Event Trigger tutorial

  This sketch demonstrates the usage of a MKRFox1200
  to build a battery-powered alarm sensor with email notifications

  A couple of sensors (normally open) should we wired between pins 1 and 2 and GND.

  This example code is in the public domain.
*/

#include <SigFox.h>
#include <ArduinoLowPower.h>


//volatile tells the compiler that such variables might change at any time,
//and thus the compiler must reload the variable whenever you reference it,
//rather than relying upon a copy it might have in a processor register.
volatile int alarm_source = 0;

void setup() {

  Serial1.begin(115200);
  while (!Serial1) {}


  if (!SigFox.begin()) {
    //something is really wrong, try rebooting
    reboot();
  }

  //in non debugging mode the SAMD chip is switched to sleep mode
  //waiting for Sigfox communication to terminate.
  //It never comes back from this state.
  Serial1.println("Reset...");
  SigFox.reset();
  delay(100);
  Serial1.println("End1...");
  SigFox.end();
  SigFox.debug();
  // We need to have time to program the Arduino after a reset
  // Otherwise it does not respond when in low-power mode
  Serial1.println("Booting...");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);

  // attach pin 0 and 1 to a switch and enable the interrupt on voltage falling event
  pinMode(0, INPUT);
  //turn on pull-up resistors. The input will stay high, unless the INPUT
  // is logic low.
  digitalWrite(0, HIGH);
  LowPower.attachInterruptWakeup(0, alarmEvent1, FALLING);

  pinMode(1, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(1, alarmEvent2, FALLING);
}

void loop()
{

  // Sleep until an event is recognized
  LowPower.sleep();
  send_message();
}

void alarmEvent1() {
  alarm_source = 1;
}

void alarmEvent2() {
  alarm_source = 2;
}

void reboot() {
  NVIC_SystemReset();
  while (1);
}

void send_message() {
  // if we get here it means that an event was received
  SigFox.begin();
  SigFox.debug();
  Serial1.println("Alarm event on sensor " + String(alarm_source));

  delay(100);

  // Clears all pending interrupts
  //SigFox.status();
  delay(1);

  // 3 bytes (ALM) + 4 bytes (ID) + 1 byte (source) < 12 bytes
  String to_be_sent = "ALM" + SigFox.ID() +  String(alarm_source);

  SigFox.beginPacket();
  Serial1.println("After begin packet");
  SigFox.print(to_be_sent);
  Serial1.println("After print");
  int ret = SigFox.endPacket(true);
  Serial1.println("After end packet...");
  if (ret == 0)
    Serial1.println("Transmission OK");
  else
    Serial1.println("KO");

  Serial1.println("Sigfox status");
  Serial1.println(SigFox.status(SIGFOX));
  Serial1.println("Atmel status: ");
  Serial1.println(SigFox.status(ATMEL));

  // shut down module, back to standby
  SigFox.end();
  Serial1.println("After end...");
  // Loop forever if we are testing for a single event
  //while (1) {};
}
