/*
 * File:   main.c
 * Author: william
 *
 * Created on 17 maggio 2026, 18.39
 */

#include <xc.h>
#include "header.h"

// #################
// VARIABILI GLOBALI
// #################
int stato_mot = 0;

int stop_flag = 0;

volatile unsigned long ms = 0;
unsigned long start = 0;


// #################
// DRIVER
// #################
void driver(int power, int dir, int vel, int acqua)
{
    RA3 = power;
    RA4 = dir;
    RA5 = vel;
    RC2 = acqua;
}

unsigned int ADC_Read(unsigned char channel) 
{
    ADCON0 &= 0b11000101;          // pulisce CHS
    ADCON0 |= (channel << 3);      // seleziona canale
    __delay_ms(2);                 // tempo di acquisizione

    GO_nDONE = 1;                  // avvia conversione
    while(GO_nDONE);               // attende fine conversione

    return ((ADRESH << 8) + ADRESL);
}

int leggiTemperatura(void)
{
    unsigned int adc = ADC_Read(2);       // AN2
    unsigned int temp = (adc * 500) / 1023; // temperatura �C intera
    return temp;
}


// #################
// TIMER ISR (1ms tick)
// #################
void __interrupt() isr() 
{
    if (TMR1IF) {
        TMR1IF = 0;

        // preload per 1ms con Fosc=4MHz, prescaler 1:8
        TMR1 = 65411;

        ms++;
    }
}

// #################
// TASK MOTORE (FSM)
// #################

void Task_Motore(void)
{
    if(stop_flag==0) 
    {
    switch(stato_mot)
    {
        case 0: // tutto spento
            driver(0,0,0,0);
            if(START == 0) stato_mot = 1; // se premo start
        break;

        case 1: // vedo dove sta la barra
            if(FC1 == 0) // se non sta all'inizio
                stato_mot = 2;
            else // se sta all'inizio
                stato_mot = 3;
        break;

        case 2: // torna indietro velocemente
            driver(1,0,1,0); 
            if(FC1 == 1) stato_mot = 3; // se arriva all'inizio
        break;

        case 3: // vai avanti irrigando
            driver(1,1,0,1);
            if(FC2 == 1) stato_mot = 4; // se sono arrivato alla fine
        break;

        case 4: // controllo il il modo del ritorno
            if(MODO == 1) stato_mot = 5; // se è impostato a 1 
            else stato_mot = 6; // se è impostato a 0
        break;

        case 5: // torno indietro irriganto
            driver(1,0,0,1);
            if(FC1 == 1) stato_mot = 0; // se arrivo all'inizio
        break;

        case 6: // torno indietro velocemente
            driver(1,0,1,0);
            if(FC1 == 1) 
            {   
                driver (0,0,0,0);
                stato_mot = 0; // se arrivo all'inizio
            }
        break;
        
    }
    }
}

// #################
// STOP 
// #################
void Task_Stop(void)
{
    switch(stato_stop)
    {
        case 0: //tasto non premuto aspetto che venga premuto
            if (STOP == 0) stato_stop = 1; // se il tasto viene premuto
        break;

        case 1: // il tasto è stato premuto
            stop_flag = 1; // segnalo che il tasto è stato premuto
            start = ms; // memorizzo a che ora è stato premuto il tasto
            driver(0,0,0,0); //fermo il motore
            stato_stop = 2;
        break;

        case 2:
            if (START == 0) // se dopodiche premo il tasto di start
                stato_stop = 3;
            else if (((ms - start) >= 5000) && STOP == 0) //se sono passati 5 secondi da quandoho premuto il tasto di stop ed il tasto è ancora premuto
                stato_stop = 4;
        break;

        case 3: // ho premuto il tasto di start
            stop_flag = 0; //segnalo che il programma può riprendere
            stato_stop = 0;
        break;

        case 4: // fai tornare la barra ad inizio serra
            stato_mot = 6; // dico al programma del motore da che punto deve continuare 
            stop_flag = 0; // segnalo che il motore può continuare il suo lavoro
            stato_stop = 0;
        break;
    }
}
// #################
// VENTILAZIONE (sempre attiva)
// #################
void Task_Ventilazione(void)
{
    static unsigned char adc_attivo = 0;
    static unsigned long val_medio = 0;
    static unsigned int temp = 0;
    unsigned int adc;

    if(adc_attivo == 0)
    {
        GO_nDONE = 1;      // avvia conversione
        adc_attivo = 1;
    }
    else
    {
        if(GO_nDONE == 0)  // conversione terminata
        {
            adc = ((unsigned int)ADRESH << 8) | ADRESL;
            val_medio = (val_medio * 9 + adc) / 10;   // filtro su 10 campioni
            temp = (val_medio * 500) / 1023;
            adc_attivo = 0;  // pronto per una nuova conversione
        }
    }
    
    if(temp >= 30) VENTILAZIONE = 1;
    else if(temp <= 27) VENTILAZIONE = 0;
}

// #################
// INIT TIMER
// #################
void Timer1_Init(void)
{
    T1CON = 0b00110001;
    TMR1 = 65411;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

// #################
// MAIN
// #################
void main(void)
{
    OSCCON = 0b01100000;

    TRISA = 0b00000111;
    ANSEL = 0x04;       // AN2 analogico
    ADCON0 = 0x09;      // AN2 selected, ADC ON
    ADCON1 = 0x80;      // Vref+ = Vdd, Vref- = Vss
    
    TRISB = 0b00000111;
    TRISC = 0b00000010;
    TRISD = 0b00011100;

    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    Timer1_Init();

    while(1)
    {
       
        Task_Motore();
        //Task_Stop();
        Task_Ventilazione();
    }
}