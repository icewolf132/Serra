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
int stato_stop = 0;
int stato_start = 0;
int stop_flag = 0;

volatile unsigned long ms = 0;
unsigned long start = 0;


// #################
// DRIVER
// #################
void driver( int Power, int Dir, int Vel, int Acqua)
{
    static int power, dir, vel,acqua;
    power = Power;
    dir = Dir;
    vel = Vel;
    acqua =Acqua;
            
    if (power == 0 )
        RA3 = 0;
    else if (power == 1)
        RA3 = 1;
    
    if (dir == 0 )
        RA4 = 0;
    else if (dir == 1)
        RA4 = 1;
    
    if (vel == 0 )
        RA5 = 0;
    else if (vel == 1)
        RA5 = 1;
    
    if (acqua == 0 )
        RC2 = 0;
    else if (acqua == 1)
        RC2 = 1;
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
// TASK MOTORE
// #################

void Task_Motore(void)
{
    if(stop_flag==0) 
    {
    switch(stato_mot)
    {
        case 0: // tutto spento
            driver(0,0,0,0);
            stato_start = 0;
            if(START == 0) stato_mot = 1; // se premo start
        break;

        case 1: // vedo dove sta la barra
            stato_start = 1;
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
                driver (1,1,1,0);
                __delay_ms(30);
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
            if ((STOP == 0) && (stato_mot >> 1)) stato_stop = 1; // se il tasto viene premuto
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
            stato_stop = 5;
        break;
        
        case 5:
            if (stato_mot == 0 )stato_stop = 0;
        break;
    }
}

 
 
// #################
// VENTILAZIONE
// #################
void Task_Ventilazione(void)
{
    static unsigned int filtro = 0; // media mobile IIR
    unsigned int adc, temp;

    // Legge ADC AN2
    GO_nDONE = 1;
    while(GO_nDONE);
    adc = ((unsigned int)ADRESH << 8) | ADRESL;

    // Converte in �C (LM35)
    //temp = (adc * 500) / 1023;

    filtro = (filtro * 7 + adc) / 8;

    // Isteresi ventilazione
    if(filtro >= 61) 
        VENTILAZIONE = 1;   // ~30�C
    else if(filtro <= 55) 
        VENTILAZIONE = 0;   // ~27�C
}

// #################
// VISUALIZZAZIONE
// #################
void Task_Visualizzazione (void)
{
    //assegno i caratteri
    static int carattere_1[5] = 
    {
        0b01111110, // carattere O
        0b11100010, // carattere F
        0b10101000, // carattere n
        0b10110000, // carattere c
        0b11111000, // carattere E
    };

    static int carattere_2[5] = 
    {
        0b01111110, // carattere O
        0b11100010, // carattere F
        0b10101000, // carattere n
        0b10110000, // carattere c
        0b11111000, // carattere E
    };

    static int x, y;
    
    // scelgo cosa visualizzare
    if (FC1 && FC2) // se entrambi i finecorsa sono premuti
    {
            x = 0; // visualizza FC
            y = 2;
    }
    else // sennò
    {
        if (stato_stop == 1) // se la barra è ferma
        {
            x = 0; // visualizza OF
            y = 1;
        }

        if ( (stato_start == 1) && (stato_stop == 1) ) // se la barra è in movimento
        {
            x = 0; // visualizza ON
            y = 2;
        }
    }

    // Visualizzo
        // digit 1 
    RD6 = 1;    //EN D1
    RD7 = 0;    //EN D2
    PORTB = carattere_1[x];
        // digit 2
    __delay_ms (5);
    RD6 = 0;    //EN D1
    RD7 = 1;    //EN D2
    PORTB = carattere_2[y];
    __delay_ms (5);
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
    
    TRISB = 0b00000000;
    TRISC = 0b00000010;
    TRISD = 0b00011100;

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    Timer1_Init();

    while(1)
    {
        
        Task_Motore();
        Task_Stop();
        Task_Ventilazione();
        Task_Visualizzazione();
    }
}