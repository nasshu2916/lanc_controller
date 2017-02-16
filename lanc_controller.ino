#include "command.h"
#include "Arduino.h"

#define cmdPin 7
#define lancPin 11
#define recButton 2
#define teleButton 3
#define wideButton 4
#define bitDuration 96 //Duration of one LANC bit in microseconds.

void setup() {
	Serial.begin(9600);
	pinMode(lancPin, INPUT); //listens to the LANC line
	pinMode(cmdPin, OUTPUT); //writes to the LANC line
	pinMode(recButton, INPUT); //start-stop recording button
	pinMode(teleButton, INPUT); //start-stop recording button
	pinMode(wideButton, INPUT); //start-stop recording button

	digitalWrite(recButton, HIGH); //turn on an internal pull up resistor
	digitalWrite(teleButton, HIGH); //turn on an internal pull up resistor
	digitalWrite(wideButton, HIGH); //turn on an internal pull up resistor
	digitalWrite(cmdPin, LOW); //set LANC line to +5V
	delay(5000); //Wait for camera to power up completly
	Serial.println("start");
}

void loop() {
	if (!digitalRead(recButton)) {
		sendCommand (AutoFocus);
	}
	if (!digitalRead(teleButton)) {
		sendCommand (TELE8); //send a command to camera with Blue Button
	}
	if (!digitalRead(wideButton)) {
		sendCommand (WIDE8); //send a command to camera with Blue Button
	}
}

void sendCommand(int cmd) {
	for (int cmdRepeatCount = 0; cmdRepeatCount < 3; cmdRepeatCount++) { //repeat 3 times to make sure the camera accepts the command

		while (pulseIn(lancPin, HIGH) < 5000) {
			//"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
			//"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
			//Loop till pulse duration is >5ms
		}

		//LOW after long pause means the START bit of Byte 0 is here
		delayMicroseconds(bitDuration);  //wait START bit duration

		for (int i = 8; i < 16; i++) {
			digitalWrite(cmdPin, (cmd & (1 << i)) ? HIGH : LOW);  //Write bit 0.
			delayMicroseconds(bitDuration);
		}

		//Byte 0 is written now put LANC line back to +5V
		digitalWrite(cmdPin, LOW);
		delayMicroseconds(10); //make sure to be in the stop bit before byte 1

		while (digitalRead(lancPin)) {
			//Loop as long as the LANC line is +5V during the stop bit
		}

		//0V after the previous stop bit means the START bit of Byte 1 is here
		delayMicroseconds(bitDuration);  //wait START bit duration

		for (int i = 0; i < 8; i++) {
			digitalWrite(cmdPin, (cmd & (1 << i)) ? HIGH : LOW);  //Write bit 0.
			delayMicroseconds(bitDuration);
		}
		//Byte 1 is written now put LANC line back to +5V
		digitalWrite(cmdPin, LOW);
	}
}

void serialEvent() {
	if (Serial.available() >= 4) {
		int cmd1 = changeCmd(Serial.read());
		int cmd2 = changeCmd(Serial.read());
		int cmd3 = changeCmd(Serial.read());
		int cmd4 = changeCmd(Serial.read());


		Serial.println((cmd1 << 12) + (cmd2 << 8) + (cmd3 << 4) + cmd4,HEX);
		Serial.println((cmd1 << 12) + (cmd2 << 8) + (cmd3 << 4) + cmd4);
		Serial.println(0x281A);
		sendCommand((cmd1 << 12) + (cmd2 << 8) + (cmd3 << 4) + cmd4);
	}
}

int changeCmd(int cmd) {
	if (cmd >= 48 && cmd <= 57) {
		cmd -= 48;
	}
	else if (cmd >= 65 && cmd <= 70) {
		cmd -= 55;
	}
	else {
		cmd = 0;
	}
	return cmd;
}
