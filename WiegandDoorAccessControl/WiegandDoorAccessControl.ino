//
//Memory Map - Arduino UNO version (1024 bytes)
//byte 0: number of RFID cards stored in memory
//byte 1, 2, 3: first card
//byte 4, 5, 6: second card
//byte 7, 8, 9: second card
//...
 

#include <EEPROM.h>
#include <Wiegand.h>
//#define DEBUG

WIEGAND wg;
int pushButtonOpenDoor = 7;
int pushButtonRegisterCard = 8;
int cardReaderBuzzer = 9;
int cardReaderLED = 10;
int lock = 11;
long int count;
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 400;    // the debounce time; increase if the output flickers


void checkButton();
void checkCardReader();


void setup() {
  
  //init I/Os
  pinMode(pushButtonOpenDoor,INPUT_PULLUP);
  pinMode(pushButtonRegisterCard,INPUT_PULLUP);
  pinMode(cardReaderBuzzer,OUTPUT);
  pinMode(cardReaderLED,OUTPUT);
  pinMode(lock,OUTPUT);
  digitalWrite(cardReaderBuzzer, HIGH);
  digitalWrite(cardReaderLED, HIGH);
  digitalWrite(lock, LOW);
  
  //init system
  Serial.begin(9600);  
  wg.begin();
      
}

void loop() {
  //Check Push Button from inside
  checkButton();
      
  //Check Card Reader 
  checkCardReader();
  	       
}

void checkButton()
{
  // read the state of the switch into a local variable:
  int reading = !digitalRead(pushButtonOpenDoor);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
            digitalWrite(lock,HIGH);
            delay(300);
            digitalWrite(lock,LOW);  
      }
    }
  }
  lastButtonState = reading;
}

void checkCardReader()
{
  if(wg.available())
  {
    
    unsigned int storedCards = EEPROM.read(0);
    unsigned long cardCode = wg.getCode(); 
    //Masks for 24 bits card code
    byte cardCodeByte_1  = cardCode & 0xFF;
    byte cardCodeByte_2 = (cardCode >> 8) & 0xFF;
    byte cardCodeByte_3  = (cardCode >> 16) & 0xFF;
    
  int buttonState = digitalRead(pushButtonRegisterCard);
  
  if(!buttonState){
     //Check if the card is already in memory
     unsigned int memoryOffset = 0;
     unsigned int cardInMemory = 0;
     if(storedCards > 0){
       for(int i = 0; i < storedCards; i++){
         memoryOffset = 2*i;
         byte storedCardCodeByte_1 = EEPROM.read(i+1+memoryOffset);
         byte storedCardCodeByte_2 = EEPROM.read(i+2+memoryOffset);
         byte storedCardCodeByte_3 = EEPROM.read(i+3+memoryOffset);
                           
         if(cardCodeByte_1 == storedCardCodeByte_1 &&
            cardCodeByte_2 == storedCardCodeByte_2 &&
            cardCodeByte_3 == storedCardCodeByte_3 )
         {
           cardInMemory = 1;
           digitalWrite(cardReaderBuzzer,LOW);
           delay(1000);
           digitalWrite(cardReaderBuzzer,HIGH); 
           
         } 
         #ifdef DEBUG
         Serial.print("Card in Memory, First Byte: ");
         Serial.print(storedCardCodeByte_1, HEX);
         Serial.print(" Second Byte: ");
         Serial.print(storedCardCodeByte_2, HEX);
         Serial.print(" Third Byte: ");
         Serial.print(storedCardCodeByte_3, HEX);
         Serial.print(" Stored Cards: ");
         Serial.print(storedCards);
         Serial.print("\n");
         #endif              
       }
      
     }
     if(!cardInMemory){
       EEPROM.write(0, storedCards + 1);
       int memoryWritePositionByte_1 = 1 + storedCards*3;
       int memoryWritePositionByte_2 = 2 + storedCards*3;
       int memoryWritePositionByte_3 = 3 + storedCards*3;
       EEPROM.write(memoryWritePositionByte_1, cardCodeByte_1);
       EEPROM.write(memoryWritePositionByte_2, cardCodeByte_2);
       EEPROM.write(memoryWritePositionByte_3, cardCodeByte_3);
       digitalWrite(cardReaderLED,LOW);
       delay(500);
       digitalWrite(cardReaderBuzzer,HIGH);
       digitalWrite(cardReaderLED,HIGH); 
       delay(500);
       digitalWrite(cardReaderBuzzer,LOW);
       digitalWrite(cardReaderLED,LOW);
       delay(500);
       digitalWrite(cardReaderBuzzer,HIGH);
       digitalWrite(cardReaderLED,HIGH); 
       delay(500);
       
       #ifdef DEBUG
       Serial.print("Card, First Byte: ");
       Serial.print(cardCodeByte_1, HEX);
       Serial.print(" Second Byte: ");
       Serial.print(cardCodeByte_2, HEX);
       Serial.print(" Third Byte: ");
       Serial.print(cardCodeByte_3, HEX);
       Serial.print("\n");
       Serial.print(" Written AT, First Position: ");
       Serial.print(memoryWritePositionByte_1, DEC);
       Serial.print(" Second Position: ");
       Serial.print(memoryWritePositionByte_2, DEC);
       Serial.print(" Third Position: ");
       Serial.print(memoryWritePositionByte_3, DEC);
       Serial.print(" Stored Cards: ");
       Serial.print(storedCards);
       Serial.print("\n");
       #endif 
     }
  
   }else{
     if(storedCards > 0){  
       unsigned int memoryOffset = 0;
       for(int i = 0; i < storedCards; i++){
         memoryOffset = 2*i;
         byte storedCardCodeByte_1 = EEPROM.read(i+1+memoryOffset);
         byte storedCardCodeByte_2 = EEPROM.read(i+2+memoryOffset);
         byte storedCardCodeByte_3 = EEPROM.read(i+3+memoryOffset);
                           
         if(cardCodeByte_1 == storedCardCodeByte_1 &&
            cardCodeByte_2 == storedCardCodeByte_2 &&
            cardCodeByte_3 == storedCardCodeByte_3 )
         {
           digitalWrite(lock,HIGH);
           digitalWrite(cardReaderBuzzer,LOW);
           digitalWrite(cardReaderLED,LOW);
           delay(50);
           digitalWrite(cardReaderBuzzer,HIGH);
           digitalWrite(cardReaderLED,HIGH); 
           delay(50);
           digitalWrite(cardReaderBuzzer,LOW);
           digitalWrite(cardReaderLED,LOW);
           delay(50);
           digitalWrite(cardReaderBuzzer,HIGH);
           digitalWrite(cardReaderLED,HIGH); 
           delay(50);
           digitalWrite(cardReaderBuzzer,LOW);
           digitalWrite(cardReaderLED,LOW);
           delay(50);
           digitalWrite(cardReaderBuzzer,HIGH);
           digitalWrite(cardReaderLED,HIGH); 
           delay(50);
           digitalWrite(cardReaderBuzzer,LOW);
           digitalWrite(cardReaderLED,LOW);
           delay(50);
           digitalWrite(cardReaderBuzzer,HIGH);
           digitalWrite(cardReaderLED,HIGH); 
           delay(50);
           digitalWrite(lock,LOW);
           break;
         }
       }
     }else{
       digitalWrite(cardReaderBuzzer,LOW);
       digitalWrite(cardReaderLED,LOW);
       delay(200);
       digitalWrite(cardReaderBuzzer,HIGH);
       digitalWrite(cardReaderLED,HIGH); 
       delay(200);
     }           
   }
   }
}
