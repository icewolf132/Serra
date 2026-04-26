#include <Stepper.h>
#include <ESP32Servo.h>


/************ DEFINE *******************************/
//define per la gestione semplificata del motore
#define START 1
#define STOP 0

#define AVANTI 1
#define DIETRO 0

#define VELOCE 1
#define LENTO 0

#define WET 1
#define DRY 0
/********************************************************/



/************* ASSEGNAZIONI COSTANTI ******************************************************************************************/
//assegnazione pin irrigazione
const int pin_power=27, pin_dir=26, pin_vel=25, pin_acqua=33, pin_STOP=32, pin_START=35, pin_MODO=34, pin_FC1=39, pin_FC2=36;

//assegnazione pin driver motore
const int pin_IN1=19, pin_IN3=18, pin_IN2=5, pin_IN4=17;

//assegnazioni variabili per la libreria Stepper.h
const int stepsPerRevolution = 2048;  //number of steps per revolution



const int PIN_FINESTRE = 22, PIN_VENTOLE = 23, pin_Relè=21, pin_servo=16;


// Costanti per il sensore (locali nella funzione sarebbe meglio, ma così eviti di ricalcolarle)
const int ADC_ZERO_GRADI = 1070;
const float ADC_PER_GRADO = (4095/3.30)*0.01;

/******************************************************************************************************************************/



/*********** COMANDI FISSI PER FUNZIONI E LIBRERIE *****************/
// inizializzazione timer
hw_timer_t *timer = NULL;
volatile bool timer_scaduto = false;  // DEVE essere globale e volatile (usata nell'interrupt)

// inizializzazione servo per ventole
Servo MyServo;

// inizializzazione libreria Stepper.h per motore passo passo
Stepper myStepper(2048, 19, 5 , 18, 17);
/*******************************************************************/



/************ FUNZIONI SECONDARIE ************************************/
//funzione per il timer dello stop
void IRAM_ATTR onTimer()
{
  timer_scaduto = true; //quando il timer della ventola arriva a 3s imposto il flag a vero
  //quando il timer del tasto di stop scade arrivo il flag (meglio usare due timer diversi)
}

//setup per la parte della temperatura
void setupTemp()
{
  pinMode(PIN_FINESTRE, OUTPUT);
  pinMode(PIN_VENTOLE, OUTPUT);
  pinMode(pin_Relè, OUTPUT);
  
  digitalWrite(PIN_FINESTRE, LOW);
  digitalWrite(PIN_VENTOLE, LOW);
  digitalWrite(pin_Relè, LOW);
}

//setup per la parte dell'irrigazione
void setup_Irrigazione()
{
  //Output
  pinMode(pin_power,OUTPUT);
  pinMode(pin_dir,OUTPUT);
  pinMode(pin_vel,OUTPUT);

  pinMode(pin_acqua,OUTPUT);

  //Input
  pinMode(pin_FC1,INPUT);
  pinMode(pin_FC2,INPUT);

  pinMode(pin_START,INPUT);
  pinMode(pin_STOP,INPUT);
  pinMode(pin_MODO, INPUT);
}

//funzione per il sensore di temperatura per scrivere la temperatura su una variabile
int sensoreTemp() 
{
  int somma = 0;
  for (int i = 0; i < 10; i++) 
  {
    somma += analogRead(4);
    delay(10);
  }
  
  int temp = somma / 10;
  temp = temp - ADC_ZERO_GRADI;
  temp = temp / ADC_PER_GRADO;
  
  return temp;
}

//funzione dipendente da irrigazione per semplificare la gestione della barra
void barra (int power, int direzione, int velocità, int acqua)
{
  if (power == START) 
  {
    digitalWrite(pin_power, 1);

    if ( direzione == AVANTI)
    {
      digitalWrite(pin_dir, 1);
      myStepper.step(1); Serial.print("avanti ");//manda il motore verso in avanti
    }
    else 
    {
      digitalWrite(pin_dir, 0);
      myStepper.step(-1); Serial.print("dietro ");//manda il motore indietro
    }

    if (velocità == VELOCE) digitalWrite(pin_vel, 1);
    else digitalWrite(pin_vel, 0);

    if (acqua == WET) digitalWrite(pin_acqua, 1);
    else digitalWrite(pin_acqua, 0);
  }
  else
  {
    digitalWrite(pin_power, 0);
    digitalWrite(pin_vel, 0);
    digitalWrite(pin_dir, 0);
    digitalWrite(pin_acqua, 0);
  }
}
/*************************************************************************/



/************** FUNZIONI PRINCIPALI ******************************************************************************/
//funzione principale d'irrigazione
void Irrigazione()
{
  static int stato_irrigazione=0, stato_irrigazione_precedente;
  Serial.println(stato_irrigazione);
/*
  if(digitalRead(pin_STOP) == 1) 
  {
    stato_irrigazione_precedente = stato_irrigazione; //mi salvo a che punto stava l'irrigazione
    stato_irrigazione=10; //interrupt virtuale 
  }
*/

  switch (stato_irrigazione)
  {
    case 0: //tutto spendo
      barra(STOP, DIETRO, LENTO, DRY);
      if(digitalRead(pin_START) == 0) stato_irrigazione=1; //se premo start
    break;

    case 1: //mando la barra all'inizio
      barra(START, DIETRO, VELOCE, DRY);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=2; //se è arrivato all'inizio
    break;

    case 2: //mando la barra alla fine
      barra(START, AVANTI, LENTO, WET); 
      if(digitalRead(pin_FC2) == 1) stato_irrigazione=3;
    break;

    case 3: //controllo il modo di irrigazione
      if (digitalRead(pin_MODO) == 1) stato_irrigazione=4; //se Modo 0 (aperto) vai allo stato 4
      else stato_irrigazione=5; //se Modo 1 (chiuso) vai allo stato 5
    break;

    case 4: //è stato selezionato ritorno veloce senza irrigazione
      barra(START, DIETRO, VELOCE, DRY);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=0;
    break;
    

    case 5: //è stato selezionata ritorno lento con irrigazione
      barra (START, DIETRO, LENTO, WET);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=0;
    break;

    /*
    case 10:
      // Ferma tutto
      stato_irrigazione = stato_irrigazione_precedente;   //memorizza lo stato in cui stava l'irrigazione
      barra(STOP, DIETRO, LENTO, DRY);                    // blocca la barra

      // Aspetto 5 secondi
      timerWrite(timer_STOP, 0);  //Azzera il contatore a 0
      flag_STOP = false;          //reseto il flag per sicurezza

      // Se premo START il ciclo riprende da dov'era
      if(digitalRead(pin_START)==0) //se premo il tasto start
      {
        stato_irrigazione = stato_irrigazione_precedente;
      }

      // Se sono passati 5 secondi e lo stop è ancora premuto
      if(flag_STOP == true)
      {
        flag_STOP = false;    //resetta il flag del timer di stop
        stato_irrigazione=4;  //vai allo stato 4
      }
    break;
    */
  }
}

void Temperatura(void)
{
  static int stato = 0;  // static: mantiene il valore tra le chiamate,
  static bool timer_avviato = false;  // static: mantiene il valore tra le chiamate
  
  int temp = sensoreTemp();
  
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("°C | Stato: ");
  Serial.print(stato);
  
  switch(stato)
  {
    case 0: // stato iniziale dove è tutto spento
      digitalWrite(PIN_FINESTRE, LOW);
      MyServo.write(110);
      digitalWrite(PIN_VENTOLE, LOW);
      digitalWrite(pin_Relè, LOW);

      Serial.println(" | Finestre=0, Ventole=0");
      
      if(temp > 23) {
        Serial.println(">>> Passo a stato 1");
        stato = 1;
        timer_avviato = false;
        timer_scaduto = false;
      }
      break;
      
    case 1: // accende finestre e avvia timer
      digitalWrite(PIN_FINESTRE, HIGH);
      MyServo.write(0);
      digitalWrite(PIN_VENTOLE, LOW);
      digitalWrite(pin_Relè, LO2W);

      Serial.print(" | Finestre=1, Ventole=0");
      
      if (!timer_avviato) {
        // Ricrea il timer
        if (timer != NULL) {
          timerEnd(timer);
        }
        timer = timerBegin(1000000);
        timerAttachInterrupt(timer, &onTimer);
        timerAlarm(timer, 3000000, false, 0);
        
        timer_scaduto = false;
        timer_avviato = true;
        Serial.println(" | >>> TIMER AVVIATO");
      } else {
        Serial.println(" | Timer in corso...");
      }
      
      if (timer_scaduto) {
        Serial.println(">>> 3 SECONDI! Passo a stato 2");
        stato = 2;
        timer_avviato = false;
      }
      break;
      
    case 2: // accende anche ventole
      digitalWrite(PIN_FINESTRE, HIGH);
      MyServo.write(0);
      digitalWrite(PIN_VENTOLE, HIGH);
      digitalWrite(pin_Relè, HIGH);

      Serial.println(" | Finestre=1, Ventole=1");
      
      if(temp < 22) {
        Serial.println(">>> Torno a stato 0");
        stato = 0;
        if (timer != NULL) {
          timerEnd(timer);
          timer = NULL;
        }
      }
      break;
  }
}
/*************************************************************************************/



/****** FUNZIONI OBBLIGATORIE *****************/
void setup() 
{
  MyServo.attach(pin_servo);
  MyServo.write(0);

  Serial.begin(9600);
  setupTemp();
  myStepper.setSpeed(17);
  setup_Irrigazione();

  timer = timerBegin(3000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 3000000, true, 0);
}

void loop() 
{
  Irrigazione();
  Temperatura();
}
