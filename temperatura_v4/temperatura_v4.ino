#include <ESP32Servo.h>

hw_timer_t * timer_Irrigazione = NULL;
volatile bool flag_Irrigazione = false;  // DEVE essere globale e volatile (usata nell'interrupt)

// Constanti PIN per l'irrigazione
static const int PIN_FINESTRE = 22, PIN_VENTOLE = 23, servoPin = 1;

// Costanti per il sensore (locali nella funzione sarebbe meglio, ma così eviti di ricalcolarle)
const int ADC_ZERO_GRADI = 1070;
const float ADC_PER_GRADO = (4095/3.30)*0.01;

Servo MyServo;

void IRAM_ATTR onTimer() 
{
  flag_Irrigazione = true;
}

void setupTemp()
{
  pinMode(PIN_FINESTRE, OUTPUT);
  pinMode(PIN_VENTOLE, OUTPUT);
  
  digitalWrite(PIN_FINESTRE, LOW);
  digitalWrite(PIN_VENTOLE, LOW);
}

int sensoreTemp() 
{
  int somma = 0;
  for (int i = 0; i < 10; i++) {
    somma += analogRead(4);
    delay(10);
  }
  
  int temp = somma / 10;
  temp = temp - ADC_ZERO_GRADI;
  temp = temp / ADC_PER_GRADO;
  
  return temp;
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
      MyServo.write(0);
      digitalWrite(PIN_VENTOLE, LOW);
      Serial.println(" | Finestre=0, Ventole=0");
      
      if(temp > 22) {
        Serial.println(">>> Passo a stato 1");
        stato = 1;
        timer_avviato = false;
        timer_scaduto = false;
      }
      break;
      
    case 1: // accende finestre e avvia timer
      digitalWrite(PIN_FINESTRE, HIGH);
      MyServo.write(180);
      digitalWrite(PIN_VENTOLE, LOW);
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
      MyServo.write(180);
      digitalWrite(PIN_VENTOLE, HIGH);
      Serial.println(" | Finestre=1, Ventole=1");
      
      if(temp < 21) {
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

void setup(void)
{
  MyServo.attach(servoPin);
  MyServo.write(0);
  Serial.begin(9600);
  setupTemp();
}

void loop(void)
{ 
  Temperatura();
  delay(1000);
}