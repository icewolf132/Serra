/*
 * File:   main.c
 * Author: William Landolfi
 *
 * Created on 28 aprile 2026, 21.38
 */

#include "header.h"

#define POWER RA2
#define DIR RA3
#define VEL RA7
  
void dir_out (int input)
{
    switch (input)
    {
        case 0: // indietro
            RA0 = 0;
            RA1 = 1;
            break;

        case 1: // avanti
            RA0 = 1;
            RA1 = 0;
            break;
    }
}

void vel_out (int speed)
{
    switch (speed)
    {
        case 0: // lento
            RB3 = 1;
            __delay_ms(500);
            RB3 = 0;
            __delay_ms(500);
            break;

        case 1: // veloce
            RB3 = 1;
            break;
    }
}

void main(void)
{
    OSCCON = 0b01110000; // 8 MHz interno
    ANSEL = 0x00;  // Tutti i pin digitali

    TRISA = 0b00011100; // dichiaro ingressi e uscite nel registo A
    PORTA = 0b00000000; // imposto tutte le uscite a 0 logico

    TRISB = 0b00000000; 
    PORTB = 0b00000000;

    while(1) 
    {
        if (POWER == 1)     //controlla lo stato de
        {
            // 4 modalità possibili
            if(DIR == 0 && VEL == 0) // se indietro lento
            { 
                dir_out(0);
                vel_out(0);
            }

            else if(DIR == 0 && VEL == 1) // se indietro veloce
            { 
                dir_out(0);
                vel_out(1);
            }

            else if(DIR == 1 && VEL == 0) // se avanti lento
            { 
                dir_out(1);
                vel_out(0);
            }

            else if(DIR == 1 && VEL == 1) // se avanti veloce
            { 
                dir_out(1);
                vel_out(1);
            }
        }
    }
    return;
}