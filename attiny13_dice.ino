//  C _ B
//  D A D 
//  B _ C

//    D C B A
// ----------
// 1        X 
// 2      X 
// 3      X X 
// 4    X X 
// 5    X X X 
// 6  X X X 

// PB | 0 1 2 3
//    | C D A B

#define pAIn A0

#include <EEPROM.h>
//byte value;


template<typename T>
void swap(T &a, T &b) {
  T c = a;
  a = b;
  b = c;
}


#define TAB_ADR 1
byte tab[6], numbers[6];
bool clearModeOn = false;

void setup() {
  uint16_t uiAIn = 0;
  byte bMin = 0;
  uint8_t vDice = 0, i = 0, j = 0, rndId = 0;
  bool found255 = false;
  
  pinMode(pAIn, INPUT);
  DDRB |= B00001111; // PB0 - PB3 as OUTPUT
  
  
  // download table from EEPROM
  for( i = 0; i < 6; i++ ) {
    tab[i] = EEPROM.read(TAB_ADR + i);
    numbers[i] = i;
  }

  // check for min count
  bMin = tab[0];
  for( i = 1; i < 6; i++ ) {
    if( tab[i] < bMin ) bMin = tab[i];
  }

  // sort counts with numbers asc
  for( i = 0; i < 6; i++ ) {
    for(j = i + 1; j < 6; j++ ) {
      if( tab[i] > tab[j] ) {
        swap( tab[i], tab[j] );
        swap( numbers[i], numbers[j] );
      }
    }
  }

  // random get id {0, 2, 4}
  for( i = 0; i < 4; i++ ) {
    delay(2);
    uiAIn += analogRead( pAIn );
  }
  
  rndId = 2*(uint16_t)( uiAIn % 3 );
  vDice = numbers[ rndId ]; // numbers = {0, 1, 2, 3, 4, 5}
  // vDice e <0, 5>

  for( i = 0; i < 6 && !found255; i++ ) {
    if( tab[i] >= 254 ) found255 = true;
  }
  if( found255 ) {
    for( i = 0; i < 6; i++ ) {
      EEPROM[TAB_ADR + i] -= bMin;
    }
  }

  // update counts
  EEPROM[TAB_ADR + vDice]++;  // vDice e <0, 5>

  vDice++;
  // calculate and write output signals on pins
  PORTB = ( PORTB & 0xFA ) | ( ( (vDice & 0x01) << 2 ) | ( (vDice & 0x04) >> 2 ) ); // A & C
  PORTB = ( PORTB & 0xF5 ) | ( ( (vDice >    1) << 3 ) | ( (vDice >    5) << 1 ) ); // B & D
}

void loop() {

  if( millis() > 10000UL ){
    if( !clearModeOn ) {
      clearModeOn = true;
      for( uint16_t i = 0; i < 6; i++ ) {
        EEPROM.write( TAB_ADR + i, 0 );
      }
      PORTB &= 0xF0;
      delay( 300 );
      PORTB |= 0x0F;
    }
  }
}




// BOD: Brown out detection, or BOD for short lets the microcontroller sense the input voltage and shut down if the voltage goes below the brown out setting
// Clock can be changed to slower, which should reduce minimum voltage!!!
