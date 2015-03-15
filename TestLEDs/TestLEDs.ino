
/*
 * Die Muster für die einzelnen Bilder 
 */
#define BILD_0 {0, 0, 0, 0, 0, 0, 0}
#define BILD_1 {0, 0, 0, 1, 0, 0, 0}
#define BILD_2 {0, 1, 0, 0, 1, 0, 0}
#define BILD_3 {0, 1, 0, 1, 1, 0, 0}
#define BILD_4 {0, 1, 1, 0, 1, 1, 0}
#define BILD_5 {0, 1, 1, 1, 1, 1, 0}
#define BILD_6 {1, 1, 1, 0, 1, 1, 1}
#define BILD_7 {1, 1, 1, 1, 1, 1, 1}
int BILDER[][7] = { BILD_0, BILD_1, BILD_2, BILD_3, BILD_4, BILD_5, BILD_6, BILD_7 };


const int ANZAHL_AUGEN = 7;

const int pinFuerAuge[7] = {2, 3, 4, 5, 6, 7, 8};

const int ANZAHL_SEITEN = 6;
const int pinFuerSeite[6] = {A0, A1, A2, A3, A4, A5};

/*
 * Initialisieren der Pins, die für die Ansteuerung der LEDs verwendet werden.
 */
void setup() {                
  // initialize the digital pin as an output.
  for(int i=0; i<ANZAHL_AUGEN; ++i) {
    pinMode(pinFuerAuge[i], OUTPUT);     
    digitalWrite(pinFuerAuge[i], LOW);
  }
  for(int i=0; i<ANZAHL_SEITEN; ++i) {
    pinMode(pinFuerSeite[i], OUTPUT);     
    digitalWrite(pinFuerSeite[i], LOW);
  }
}

/*
 * Anzeige des aktuellen Bildes nacheinander auf allen Seiten
 */
void loop() {
  int bild = (millis()/1000) % (ANZAHL_AUGEN+1);
  zeigeBild(bild);
  /*
   * Nacheinander werden alle Seiten jeweils einmal eingeschaltet
   * und zeigen dann das Bild an
   */
  for(int i=0; i<ANZAHL_SEITEN; ++i) {
    digitalWrite(pinFuerSeite[i], HIGH);   // Einschalten einer Seite
    delay(1);               // wait for a second
    digitalWrite(pinFuerSeite[i], LOW);    // und wieder auschalten
  }
}

/*
 * Setzt die Pins der LEDs auf HIGH, die Teil des Musters sind,
 * alle anderen Pins werden auf LOW gesetzt.
 */
void zeigeBild(int bild) {
  for(int auge=0; auge<ANZAHL_AUGEN; ++auge) {
    digitalWrite(pinFuerAuge[auge], BILDER[bild][auge] ? HIGH : LOW);
  }
}


