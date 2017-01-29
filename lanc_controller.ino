#define cmdPin 7
#define lancPin 11
#define recButton 2
#define teleButton 3
#define wideButton 4
int cmdRepeatCount;
int bitDuration = 96; //Duration of one LANC bit in microseconds.

void setup() {

  pinMode(lancPin, INPUT); //listens to the LANC line
  pinMode(cmdPin, OUTPUT); //writes to the LANC line
  pinMode(recButton, INPUT); //start-stop recording button
  digitalWrite(recButton, HIGH); //turn on an internal pull up resistor
  pinMode(teleButton, INPUT); //start-stop recording button
  digitalWrite(teleButton, HIGH); //turn on an internal pull up resistor
    pinMode(wideButton, INPUT); //start-stop recording button
  digitalWrite(wideButton, HIGH); //turn on an internal pull up resistor
  digitalWrite(cmdPin, LOW); //set LANC line to +5V
  delay(5000); //Wait for camera to power up completly
}

void loop() {
  if (!digitalRead(recButton)) {
    REC(); //send REC command to camera
    delay(1000); //debounce button
  }
  if (!digitalRead(teleButton)) {
    send(B00101000 , B00011010); //send a command to camera with Blue Button
  }
  if (!digitalRead(wideButton)) {
    send(B00101000 , B00001110); //send a command to camera with Blue Button
  }

}

void REC() {

  cmdRepeatCount = 0;

  while (cmdRepeatCount < 5) {  //repeat 5 times to make sure the camera accepts the command

    while (pulseIn(lancPin, HIGH) < 5000) {
      //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
      //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
      //Loop till pulse duration is >5ms
    }

    //LOW after long pause means the START bit of Byte 0 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of byte 0
    //"18hex" or “00011000”  tells the camera that there will be a normal command to camera in the next byte
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, LOW);  //Write bit 0.
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 1
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 2
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 3
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 4
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 5
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 6
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 7
    delayMicroseconds(bitDuration);
    //Byte 0 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);
    delayMicroseconds(10); //make sure to be in the stop bit before byte 1

    while (digitalRead(lancPin)) {
      //Loop as long as the LANC line is +5V during the stop bit
    }

    //0V after the previous stop bit means the START bit of Byte 1 is here
    delayMicroseconds(bitDuration);  //wait START bit duration

    //Write the 8 bits of Byte 1
    //"33hex" or “00110011” sends the  Record Start/Stop command
    //Note that the command bits have to be put out in reverse order with the least significant, right-most bit (bit 0) first
    digitalWrite(cmdPin, HIGH);  //Write bit 0
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 1
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 2
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 3
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 4
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, HIGH);  //Write bit 5
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 6
    delayMicroseconds(bitDuration);
    digitalWrite(cmdPin, LOW);  //Write bit 7
    delayMicroseconds(bitDuration);
    //Byte 1 is written now put LANC line back to +5V
    digitalWrite(cmdPin, LOW);

    cmdRepeatCount++;  //increase repeat count by 1

    /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
      and just wait for the next start bit after a long pause to send the first two command bytes again.*/


  }//While cmdRepeatCount < 5
}



void send(unsigned char cmd1, unsigned char cmd2) 
{
    for (int     cmdRepeatCount = 0; cmdRepeatCount < 1; cmdRepeatCount++) {  //repeat 5 times to make sure the camera accepts the command

        while (pulseIn(lancPin, HIGH) < 5000) {
            //"pulseIn, HIGH" catches any 0V TO +5V TRANSITION and waits until the LANC line goes back to 0V
            //"pulseIn" also returns the pulse duration so we can check if the previous +5V duration was long enough (>5ms) to be the pause before a new 8 byte data packet
            //Loop till pulse duration is >5ms
        }

        //LOW after long pause means the START bit of Byte 0 is here
        delayMicroseconds(bitDuration);  //wait START bit duration

        for( int i=0; i<8; i++){
            digitalWrite(cmdPin, (cmd1 & (1<<i) ) ? HIGH : LOW);  //Write bit 0.
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

        for( int i=0; i<8; i++){
            digitalWrite(cmdPin, (cmd2 & (1<<i) ) ? HIGH : LOW);  //Write bit 0.
            delayMicroseconds(bitDuration);
        }
        //Byte 1 is written now put LANC line back to +5V
        digitalWrite(cmdPin, LOW);

        /*Control bytes 0 and 1 are written, now don’t care what happens in Bytes 2 to 7
        and just wait for the next start bit after a long pause to send the first two command bytes again.*/


    }//While cmdRepeatCount < 5
}

