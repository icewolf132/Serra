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

//assegno i caratteri
     int carattere_1[5] = 
    {
        0b01111110, // carattere O
        0b11100010, // carattere F
        0b10101000, // carattere n
        0b10110000, // carattere c
        0b11111000, // carattere E
    };

     int carattere_2[5] = 
    {
        0b01111110, // carattere O
        0b11100010, // carattere F
        0b10101000, // carattere n
        0b10110000, // carattere c
        0b11111000, // carattere E
    };

     int x, y;

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
// VENTILAZIONE (Controllo Rel� Termostato)
// #################
void Task_Ventilazione(void)
{
    static unsigned int filtro = 0; 
    static char prima_lettura = 1;  
    unsigned int adc;

    // 1. IMPORTANTE: Aspetta che il condensatore dell'ADC si carichi
    __delay_us(20); 

    // 2. Fai partire la conversione
    GO_nDONE = 1;
    while(GO_nDONE); // Aspetta la fine
    
    adc = ((unsigned int)ADRESH << 8) | ADRESL;

    if(prima_lettura) {
        filtro = adc;
        prima_lettura = 0;
    } else {
        filtro = (filtro * 7 + adc) / 8; 
    }

    // Controllo soglie
    if(filtro >= 143)       // >= 70�C
    {
        VENTILAZIONE = 1;   
    }
    else if(filtro <= 72)   // <= 35�C
    {
        VENTILAZIONE = 0;   
    }
}

// #################
// VISUALIZZAZIONE
// #################
void Task_Visualizzazione (void)
{
    if (FC1 && FC2)
    {
        x = 1;     // F
        y = 3;     // c
    }
    else if (stop_flag)
    {
        x = 0;     // O
        y = 1;     // F
    }
    else if (stato_mot != 0)
    {
        x = 0;     // O
        y = 2;     // n
    }
    else
    {
        x = 0;     // O
        y = 0;     // O
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
    // 1. Configurazione Oscillatore (4 MHz interno)
    OSCCON = 0b01100000;

    // 2. Configurazione I/O Direzione (TRIS)
    TRISA = 0b00000111;    // RA2 (AN2) deve essere ingresso
    TRISB = 0b00000000;
    TRISC = 0b00000010;    // Configura RC2 come uscita per il rel� (se usi RC2)
    TRISD = 0b00011100;

    // Stato iniziale delle uscite a zero
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    // 3. Configurazione ADC per AN2
    ANSEL = 0x04;          // Imposta solo AN2 come analogico (0b00000100)
    ANSELH = 0x00;         // Tutti gli altri pin digitali

    ADCON1bits.ADFM = 1;   // Giustificazione a destra (importante per i 10 bit)
    ADCON1bits.VCFG0 = 0;  // Vref+ = VDD (5V)
    ADCON1bits.VCFG1 = 0;  // Vref- = VSS (GND)

    // ADCON0: bit7-6 (Fosc/8), bit5-2 (Channel AN2: 0010), bit1 (GO/DONE=0), bit0 (ADON=1)
    ADCON0 = 0b01001001;   

    // 4. Inizializzazione Periferiche e Interrupt
    Timer1_Init();

    while(1)
    {
        
        Task_Motore();
        Task_Stop();
        Task_Ventilazione();
        Task_Visualizzazione();
    }
}