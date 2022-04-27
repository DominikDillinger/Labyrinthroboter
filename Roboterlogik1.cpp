/*
    Code fur den ersten Roboter, der das Labyrinth erkundet und dann einen Weg uber Bluetooth an den zweiten Roboter schickt.

    TODO-Liste:
    -Ubertragung
    -Looperkennung(Einfach offene Nachbaren uberschreiben.)
    -Toleranzen beim Fahren
*/

//Anschlusse
#define SENSOR_RECHTS
#define SENSOR_VORNE
#define SENSOR_LINKS
#define MOTOREN
//Settings
#define MINDESTABSTAND
#define POWER 50
#define DISTANCE 360*2.2
//Defines fur Lesbarkeit.
#define SIZE 5
#define NICHTS 0
#define OBEN 1
#define RECHTS 2
#define UNTEN 3
#define LINKS 4
#define RECHTS_DREHEN 100
#define LINKS_DREHEN -100


//Vars
float sensorRechts;
float sensorVorne;
float sensorLinks;
int feld[SIZE][SIZE]; //Zeile, Spalte
int richtung = OBEN; //Startrichtung
int posZeile = SIZE-1; //Momentane X-Position vom Roboter.
int posSpalte; //Momentane Y-Position vom Roboter.

void Setup() {
    //Settup für drehen und Kordinatennetz.

    //Sensoren setzen.
    SetSensorLowspeed(SENSOR_VORNE);
    SetSensorLowspeed(SENSOR_RECHTS);
    SetSensorLowspeed(SENSOR_LINKS);

    //innit Kordinatennetz, Abfrage nach Startposition.
    for (int zeile = 0; zeile < SIZE; zeile++){
        for (int spalte = 0; spalte < SIZE; spalte++) {
            feld[zeile][spalte] = NICHTS;
        }
    }
    posSpalte = AbfrageNachStart();
}

int AbfrageNachStart() {
    //Abfrage für den Startpunkt (0-4)

    int startfeld = 0;
    TextOut(16, LCD_LINE1 , "Startpunkt", false);
    TextOut(16, LCD_LINE2 , "auswaehlen", false);

    while(true){
        NumOut(48, LCD_LINE5, startfeld);

        //INPUT-Eingabe
        if (ButtonPressed(BTNRIGHT,false) && startfeld < 4){
            startfeld += 1;
            Wait(400);
        }else if (ButtonPressed(BTNLEFT, false) && startfeld > 0){
            startfeld -= 1;
            Wait(400);
        }else if (ButtonPressed(BTNCENTER, false)){
            break;
        }

    }

    //Ausgabe Bestätigung und CountDown.
    TextOut(16, LCD_LINE2, "Vielen Dank", true);
    TextOut(16, LCD_LINE3, "Starte in..", false);
    for(int i=3; i>=0; i--){
            NumOut(48, LCD_LINE5 , i);
            PlayTone(100, 500);
            Wait(1000);
    }

    Wait(1000);
    ResetScreen();
    return startfeld;
}

void Drehen(int drehwert) {
    //90 Grad Drehung nach rechts durchführen.

    //Drehen
    RotateMotorEX(MOTOREN,  POWER, 180, grad, true, true);
}

void DrehenMitFeld(int drehwert) {
    //Drehung ausführen und Richtung anpassen.

    Drehen(drehwert);

    //Richtung anpassen.
    if (drehwert == RECHTS_DREHEN) {
        if (richtung < LINKS) {
            richtung++;
        } else {
            richtung = OBEN;
        }
    } else {
        if (richtung > OBEN) {
            richtung--;
        } else {
            richtung = LINKS;
        }
    }
}

void Fahren() {
    //Ein Tile nach vorne fahren.

    //Vorwärtsfahren
    RotateMotorEx(MOTOREN, POWER, DISTANCE, 0, true, true);
}

int FahrenMitFeld() {
    //Nach vorne Fahren, neue Position merken und in Feld eintragen.

    //In Feld eintragen.
    feld[posZeile][posSpalte] = richtung;

    Fahren();

    //Position Akualisieren.
    switch (richtung) {
        case OBEN:
            posSpalte++;
        break;
        case RECHTS:
            posZeile++;
        break;
        case LINKS:
            posZeile--;
        break;
        case UNTEN:
            posSpalte--;
        break;
        default:
        break;
    }
}

int Exitcheck() {
    //Schauen, ob der Ausgang erreicht worden ist.
    //Wenn aus dem Feld hinausgefahren wird, dann ist der Ausgang erreicht.
    if (posSpalte > SIZE || posSpalte < 0 || posZeile > SIZE || posZeile < 0) {
        return 1;
    } else {
        return 0;
    }
}

void SensorenAuslesen() {
    //Alle Sensoren Auslesen.
    
    float sensorRechts = SensorUS(SENSOR_RECHTS);
    float sensorVorne = SensorUS(SENSOR_VORNE);
    float sensorLinks = SensorUS(SENSOR_LINKS);
}

void Zyklus() {
    //Zyklus für ein Tile(Logik).

    //Sensoren auslesen.
    SensorenAuslesen();

    //Fahren und speichern.
    if (sensorRechts > MINDESTABSTAND) {
        //Rechts abbiegen.
        DrehenMitFeld(RECHTS_DREHEN);
        FahrenMitFeld();
    } else if (sensorVorne > MINDESTABSTAND) {
        //Gerade aus fahren.
        FahrenMitFeld();
    } else if (sensorLinks > MINDESTABSTAND)
    {
        //Links abbiegen.
        DrehenMitFeld(LINKS_DREHEN);
        FahrenMitFeld();
    }
    {
        //Umdrehen
        DrehenMitFeld();
        DrehenMitFeld();
        FahrenMitFeld();
    }
}

void ErgbenisUbertragen() {
    //TODO
}

task Main() {
    //Setup
    Setup();

    //Weg finden.
    while(Exitcheck() != 1) {
        Zyklus();
    }

    //Übertragen
    ErgbenisUbertragen();
}