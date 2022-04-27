//Settins
#define SENSOR_RECHTS
#define SENSOR_VORNE
#define MOTOR_LINKS
#define MOTOR_RECHTS
#define MINDESTABSTAND
#define SIZE 5
#define NICHTS 0
#define OBEN 1
#define RECHTS 2
#define UNTEN 3
#define LINKS 4

//Vars
float sensorRechts;
float sensorVorne;
int feld[SIZE][SIZE]; //Zeile, Spalte
int richtung = OBEN; //Startrichtung
int posZeile = SIZE-1; //Momentane X-Position vom Roboter.
int posSpalte; //Momentane Y-Position vom Roboter.


void Setup() {
    //Settup für drehen und Kordinatennetz.

    //Sensoren setzen.
    SetSensorLowspeed(SENSOR_VORNE);
    SetSensorLowspeed(SENSOR_RECHTS);

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
    TextOut(1, LCD_LINE8 , "Startpunkt auswählen", false);

    while(true){
        NumOut(1, LCD_LINE6, startfeld);
        if (ButtonPressed(BTNRIGHT,false) && startfeld < 4){
            startfeld += 1;
        }else if (ButtonPressed(BTNLEFT, false) && startfeld > 0){
            startfeld -= 1;
        }else if (ButtonPressed(BTNCENTER, false)){
            break;
        }
    }

    TextOut(1, LCD_LINE8, "Vielen Dank", true);
    Wait(2000);
    ResetScreen();
    return startfeld;
}

void Drehen() {
    //90 Grad Drehung nach rechts durchführen.

    //Rechtsdrehen
    OnFwdSync(OUT_BC, 50, 100);//ACHTUNG: BC bzw 100/-100 richtig belegt?
    Wait(2000);
}

void DrehenMitFeld() {
    //Drehung ausführen und Richtung anpassen.

    Drehen();

    //Richtung anpassen.
    if (richtung < 4) {
        richtung++;
    } else {
        richtung = OBEN;
    }
}

void Fahren() {
    //Ein Tile nach vorne fahren.

    //Vorwärtsfahren
    OnFwdSync(OUT_BC, 50, 0);
    Wait(4000);
}

int FahrenMitFeld() {
    //Nach vorne Fahren, neue Position merken und in Feld eintragen.

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

    //In Feld eintragen.
    feld[posZeile][posSpalte] = richtung;
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
    
    //TODO
    /*
    float sensorRechts;
    float sensorVorne;
    */
}

void Zyklus() {
    //Zyklus für ein Tile(Logik).

    //Sensoren auslesen.
    SensorenAuslesen();

    //Fahren und speichern.
    if (sensorRechts > MINDESTABSTAND) {
        //Rechts abbiegen.
        DrehenMitFeld();
        FahrenMitFeld();
    } else if (sensorVorne > MINDESTABSTAND) {
        //Gerade aus fahren.
        FahrenMitFeld();
    } else {
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
    //Setup.
    Setup();

    //Weg finden.
    while(Exitcheck() != 1) {
        Zyklus();
    }

    //Übertragen
    ErgbenisUbertragen();
}