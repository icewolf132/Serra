//define per la gestione semplificata del motore
#define START 1
#define STOP 0

#define AVANTI 1
#define DIETRO 0

#define VELOCE 1
#define LENTO 0

#define WET 1
#define DRY 0

/*
hw_timer_t * timer_STOP = NULL; //timer dello stop
volatile bool flag_STOP=false; //flag per il timer dello stop
*/

//assegnazione pin irrigazione
const int pin_power=25, pin_dir=26, pin_vel=27, pin_acqua=33, pin_FC1=39, pin_FC2=34, pin_START=35, pin_STOP=32, pin_MODO=19;

/*
void IRAM_ATTR onTimer_STOP() //funzione per il timer dello stop
{
  flag_STOP = true;
}
*/

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

//funzione dipendente da irrigazione per semplificare la gestione della barra
void barra (int power, int dir, int vel, int acqua)
{
  if (power == 1) digitalWrite(pin_power, 1);
  else digitalWrite(pin_power, 0);

  if (dir == 1) digitalWrite(pin_dir, 1);
  else digitalWrite(pin_dir, 0);

  if (vel == 1) digitalWrite(pin_vel, 1);
  else digitalWrite(pin_vel, 0);

  if (acqua == 1) digitalWrite(pin_acqua, 1);
  else digitalWrite(pin_acqua, 0);
}

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
      barra(STOP,LENTO, DIETRO, DRY);
      if(digitalRead(pin_START) == 0) stato_irrigazione=1; //se premo start
    break;

    case 1: //mando la barra all'inizio
      barra(START, VELOCE, DIETRO, DRY);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=2; //se è arrivato all'inizio
    break;

    case 2: //mando la barra alla fine
      barra(START, LENTO, AVANTI, WET);
      if(digitalRead(pin_FC2) == 1) stato_irrigazione=3;
    break;

    case 3: //controllo il modo di irrigazione
      if (digitalRead(pin_MODO) == 1) stato_irrigazione=4; //se Modo 0 (aperto) vai allo stato 4
      else stato_irrigazione=5; //se Modo 1 (chiuso) vai allo stato 5
    break;

    case 4: //è stato selezionato ritorno veloce senza irrigazione
      barra (START, VELOCE, DIETRO, DRY);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=0;
    break;
    

    case 5: //è stato selezionata ritorno lento con irrigazione
      barra (START, LENTO, DIETRO, WET);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=0;
    break;

       /*timerWrite(timer_STOP, 0); //Azzera il contatore a 0
        flag_STOP = false; //reseto il flag per sicurezza
        stato_irrigazione = stato_irrigazione_precedente; //se premo start il ciclo riprende da dov'era
      }

      if(flag_STOP == true)//se sono passati 5 secondi e lo stop è ancora premuto
      {
        flag_STOP = false;    //resetta il flag del timer di stop
        stato_irrigazione=4;  //vai allo stato 4
      }
    break;*/
  }
}

void setup() 
{
  Serial.begin(9600);
  setup_Irrigazione();
}

void loop() 
{
  Irrigazione();
  delay(1);
}
