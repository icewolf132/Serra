/*
 * File:   main.c
 * Author: William Landolfi
 *
 * Created on 28 aprile 2026, 21.38
 */

#include "header.h"

#define POWER RA2
#define DIR RA3
#define VEL RA4
  

void adc_init(void)
{
    CMCON = 0x07;           // disabilito i comparatori
    ADCON0 = 0b00011001;    // seleziono RB7 come ingresso
    ADCON1 = 0b10000000;    // risultato giustificato a destra, clock adc corretto
}

void pwm_init(void)
{
    // Timer2
    PR2 = 124; // set the PWM period

    // PWM mode
    CCP1CON = 0b00001100; 

    // Duty cycle 50%
    CCPR1L = 62; // Set the PWM duty cycle

    TRISB = 0x00; // Make the CCP1 pin an output

    // Timer2 ON prescaler 16 
    T2CON = 0b00000111;
}

void pwm_lento(void)
{
    CCP1CON = 0b00001100;
    CCPR1L = 62;
}

void pwm_veloce(void)
{
    CCP1CON = 0x00;;
    RB3=1
    
    //CCP1CON = 0b00001100;
    //CCPR1L = 124;
}

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

void vel_out(int speed)
{
    switch(speed)
    {
        case 0: // lento
            pwm_lento(); // duty basso
            break;

        case 1: // veloce
            pwm_veloce(); // duty alto
            break;
    }
}

void main(void)
{
    OSCCON = 0b01110000; // 8 MHz interno
    ANSEL =  0b01000000;  // Tutti i pin digitali tranne RB7

    TRISA = 0b00011100; // dichiaro ingressi e uscite nel registo A
    PORTA = 0b00000000; // imposto tutte le uscite a 0 logico

    TRISB = 0b10000000; 
    PORTB = 0b00000000;
    
 
    
    pwm_init();

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
        else // uscite basse;
        {
            CCP1CON = 0x00;
            RB3 = 0;
        }
    }
    return;
}