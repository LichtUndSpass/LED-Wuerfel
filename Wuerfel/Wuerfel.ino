/* Definitionen welche LED bei welcher Zahl leuchten soll */
#define BILD_0 {0, 0, 0, 0, 0, 0, 0}
#define BILD_1 {0, 0, 0, 1, 0, 0, 0}
#define BILD_2 {0, 1, 0, 0, 1, 0, 0}
#define BILD_3 {0, 1, 0, 1, 1, 0, 0}
#define BILD_4 {0, 1, 1, 0, 1, 1, 0}
#define BILD_5 {0, 1, 1, 1, 1, 1, 0}
#define BILD_6 {1, 1, 1, 0, 1, 1, 1}
#define BILD_7 {1, 1, 1, 1, 1, 1, 1}

/* Eine Liste der Bilder aller Würfe */
int BILDER[][7] = { BILD_0, BILD_1, BILD_2, BILD_3, BILD_4, BILD_5, BILD_6, BILD_7 };

/* Anzahl der Augen auf jeder Seite */
const int ANZAHL_AUGEN = 7;

/* Arduino Pins für jedes Auge */
const int pinFuerAuge[7] = {2, 3, 4, 5, 6, 7, 8};

/* Anzahl der Seiten auf dem Würfel */
const int ANZAHL_SEITEN = 6;

/* Arduino Pins für jede Seite */ 
const int pinFuerSeite[6] = {A0, A1, A2, A3, A4, A5};

/* aktueller Wurf, der angezeigt wird */
int aktuellerWurf = 0;

/* Die letzten Beschleunigungen werden sich gemerkt, um den gleitenden Durchschnitt berechnen zu können */
const int ANZAHL_VERGANGENE_BESCHLEUNIGUNGEN = 100;
float beschleunigungen[ANZAHL_VERGANGENE_BESCHLEUNIGUNGEN];
float gleitenderDurchschnitt = 0.0f;
int indexBeschleunigung = 0;

/* Es wird alle 10 Millisekunden eine neue Messung der Beschleunigung vorgenommen */
const long ZEIT_ZWISCHEN_MESSUNGEN = 10;

/* 
 * Die Bestimmung ob der Würfel ruht geschieht über die Anzahl von aufeinanderfolgenden Messungen, 
 * bei denen die Beschleunigung innerhalb einer maximalen Abweichung vom Durchschnitt liegt.
 */ 
const int MESSUNGEN_BIS_RUHE = 100;
const int MESSUNGEN_BIS_LEDS_AUS = 1000;
const float MAXIMALE_ZULAESSIGE_ABWEICHUNG = 20;
int anzahlMessungenInRuhe = 0;
int anzahlMessungenAbweichendVomDurchschnitt = 0;

/* Zeitpunkt der letzten Messung */
long letzteMessung = 0;

void setup() {                
  
  /* Initialisieren der Pins zur Ansteuerung der LEDs */
  for(int i=0; i<ANZAHL_AUGEN; ++i) {
    pinMode(pinFuerAuge[i], OUTPUT);     
    digitalWrite(pinFuerAuge[i], LOW);
  }
  for(int i=0; i<ANZAHL_SEITEN; ++i) {
    pinMode(pinFuerSeite[i], OUTPUT);     
    digitalWrite(pinFuerSeite[i], LOW);
  }
  /* initialisieren des Beschleunigungssensors */
  initialisiereBeschleunigungssensor();
}

/* Wird immer wieder automatisch aufgerufen */ 
void loop() {
  /* Überprüfe die Beschleunigung */
  aktualisiereBeschleunigung();

  /* */
  zeigeWurf(aktuellerWurf);
    
  /* Zeige den Wurf nacheinander auf jeder Seite jeweils für 1 ms */
  for(int i=0; i<ANZAHL_SEITEN; ++i) {
    /* Schalte die LEDs auf der Seite i an */
    digitalWrite(pinFuerSeite[i], HIGH);   
    /* warte eine ms */
    delay(1);
    /* Schalte die LEDs auf der Seite i aus */
    digitalWrite(pinFuerSeite[i], LOW);
  }
}

/* Setze die Pins für die einzelnen Augen je nachdem, welcher Wurf angezeigt werden soll */
void zeigeWurf(int wurf) {
  for(int auge=0; auge<ANZAHL_AUGEN; ++auge) {
    digitalWrite(pinFuerAuge[auge], BILDER[wurf][auge] ? HIGH : LOW);
  }
}

/* fragt wenn nötig die aktuelle Beschleunigung ab */
void aktualisiereBeschleunigung() {
  long aktuelleZeit = millis();
  /* Schaue, ob schon die Wartezeit seit der letzten Messung vergangen ist */
  if(letzteMessung + ZEIT_ZWISCHEN_MESSUNGEN <= aktuelleZeit) {
    
    /* lies die aktuellen Beschleunigungswerte in jeder der 3 Richtungen */
    float x, y, z;
    liesBeschleunigung(x, y, z);

    /* Berechne den Betrag des aktuellen Beschleunigungsvektors */
    float aktuelleBeschleunigung = sqrt(x*x+y*y+z*z);
    
    /* hol den ältesten Beschleunigungswert und ersetz ihn mit der aktuellen Beschleunigung */
    indexBeschleunigung = (indexBeschleunigung+1) % ANZAHL_VERGANGENE_BESCHLEUNIGUNGEN;
    float aeltesteBeschleunigung = beschleunigungen[indexBeschleunigung];
    beschleunigungen[indexBeschleunigung] = aktuelleBeschleunigung;
    
    /* berechne den gleitenden Durchschnitt mit dem neuen Wert */
    gleitenderDurchschnitt = gleitenderDurchschnitt - aeltesteBeschleunigung/ANZAHL_VERGANGENE_BESCHLEUNIGUNGEN + aktuelleBeschleunigung/ANZAHL_VERGANGENE_BESCHLEUNIGUNGEN;
    
    /* speicher die Zeit der aktuellen Messung */
    letzteMessung = aktuelleZeit;
    
    /* Wenn die Abweichung vom Durchschnitt kleiner ist als der Grenzwert, ehöhe den Zähler, sonst wird er resettet */
    if(abs(aktuelleBeschleunigung - gleitenderDurchschnitt) < MAXIMALE_ZULAESSIGE_ABWEICHUNG) {
       ++anzahlMessungenInRuhe;
    } else {
      anzahlMessungenInRuhe = 0;
    }
    /* Solange noch nicht genug Messungen in Ruhe waren, wähle einen neuen Zufallswert */
    if(anzahlMessungenInRuhe < MESSUNGEN_BIS_RUHE) {
      aktuellerWurf = random(6)+1;
    } 
    
    /* Setze den angezeigten Wert auf 0, wenn keine LEDs mehr angezeigt werden sollen */
    if(anzahlMessungenInRuhe > MESSUNGEN_BIS_LEDS_AUS) {
      anzahlMessungenInRuhe = MESSUNGEN_BIS_LEDS_AUS;
      aktuellerWurf = 0;
    }
  }
}

/*
 * Der folgende Code stammt aus dem Basic Beispiel von Sparkfun für den Beschleunigungssensor 
 * https://www.sparkfun.com/tutorials/240
 */

//Add the SPI library so we can communicate with the ADXL345 sensor
#include <SPI.h>

//Assign the Chip Select signal to pin 10.
int CS=10;

//This is a list of some of the registers available on the ADXL345.
//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
char POWER_CTL = 0x2D;	//Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;	//X-Axis Data 0
char DATAX1 = 0x33;	//X-Axis Data 1
char DATAY0 = 0x34;	//Y-Axis Data 0
char DATAY1 = 0x35;	//Y-Axis Data 1
char DATAZ0 = 0x36;	//Z-Axis Data 0
char DATAZ1 = 0x37;	//Z-Axis Data 1

void initialisiereBeschleunigungssensor() {
  //Initiate an SPI communication instance.
  SPI.begin();
  //Configure the SPI connection for the ADXL345.
  SPI.setDataMode(SPI_MODE3);
  
  //Set up the Chip Select pin to be an output from the Arduino.
  pinMode(CS, OUTPUT);
  //Before communication starts, the Chip Select pin needs to be set high.
  digitalWrite(CS, HIGH);
  
  //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
  writeRegister(DATA_FORMAT, 0x01);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  
}

void liesBeschleunigung(float &x, float &y, float &z) {
  unsigned char values[10];

  //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  readRegister(DATAX0, 6, values);

  //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
  //The X value is stored in values[0] and values[1].
  x = ((int)values[1]<<8)|(int)values[0];
  //The Y value is stored in values[2] and values[3].
  y = ((int)values[3]<<8)|(int)values[2];
  //The Z value is stored in values[4] and values[5].
  z = ((int)values[5]<<8)|(int)values[4];
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.
void readRegister(char registerAddress, int numBytes, unsigned char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
}

//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.
void writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(CS, HIGH);
}

