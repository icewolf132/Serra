#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

int stato=1;

int pin_FC1=39, pin_FC2=34;

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  pinMode(39, INPUT); //FC1
  pinMode(34, INPUT); //FC2
  // set the speed at 5 rpm
  myStepper.setSpeed(10);
  // initialize the serial port
  Serial.begin(9600);
}

void loop()
{
  static int stato_irrigazione=1;
  Serial.println(stato_irrigazione);

  switch (stato_irrigazione)
  {
    case 1: //mando la barra all'inizio
      myStepper.step(-1);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=2; //se è arrivato all'inizio
    break;

    case 2: //mando la barra alla fine
      myStepper.step(1);
      if(digitalRead(pin_FC2) == 1) stato_irrigazione=4;
    break;

    case 4: //è stato selezionato ritorno veloce senza irrigazione
      myStepper.step(-1);
      if(digitalRead(pin_FC1) == 1) stato_irrigazione=0;
    break;
  }
}
