#include <xc.h>

#define _XTAL_FREQ 8000000  // 8 MHz (oscillatore interno)

// CONFIG1
#pragma config FOSC = EXTRCCLK  // Oscillator Selection bits (EXTRC oscillator; CLKO function on RA6/OSC2/CLKO)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable bit (RB3/PGM pin has PGM function, Low-Voltage Programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


/*
    Dichiarazione bit:
    
    Direzione
    0 = indietro
    1 = avanti

    Velocità
    0 = lento
    1 = veloce
*/

TRISA = 0b11111100;
    
    power = RB2;
    dir = RB3;
    vel = RB4;

// INPUT
int power=0, dir=0, vel=0; // ingressi da ricevere dalla scheda main

// OUTPUT
int A=0, B=0;   // direzione motore
int C=0;        // enable motore

void dir_out (int input)
{
    switch (input)
    {
        case 0: // indietro
            A = 0;
            B = 1;
            break;

        case 1: // avanti
            A = 1;
            B = 0;
            break;
    }
}

void vel_out (int speed)
{
    switch (speed)
    {
        case 0: // lento
            C = 0;
            break;

        case 1: // avanti Duty cycle 50%
            
            break;
    }
}

void main(void)
{
    while(1) 
    {
        int mod;

        if (power = 1)     //controlla lo stato de
        {
        
            // 4 modalità possibili
            if(dir == 0 && vel == 0) // se indietro lento
            { 
                dir_out(0);
                vel_out(0);
            }

            if(dir == 0 && vel == 1) // se indietro veloce
            { 
                dir_out(0);
                vel_out(1);
            }

            if(dir == 1 && vel == 0) // se avanti lento
            { 
                dir_out(1);
                vel_out(0);
            }

            if(dir == 1 && vel == 1) // se avanti veloce
            { 
                dir_out(1);
                vel_out(1);
            }
        }
        else enable = 0
        }
}