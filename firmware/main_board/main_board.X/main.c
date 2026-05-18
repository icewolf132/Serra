/*
 * File:   main.c
 * Author: william
 *
 * Created on 17 maggio 2026, 18.39
 */

#include <xc.h>
#include "header.h"

void main(void) 
{
    OSCCON = 0b01100000;
    ANSEL  = 0x00; // Imposto tutti i pin digitali
    
    // PORTA come input/output
    //TRISA = 0b00000000;  // 0 = output, 1 = input
    //PORTA = 0x00;        // inizializza a 0

    // PORTB come input/output
    TRISB = 0b00000000;
    PORTB = 0x00;

    // PORTC come input/output
    TRISC = 0b00000111;
    PORTC = 0x00;

    // PORTD come input/output
    //TRISD = 0b00000000;
    //PORTD = 0x00;

    // PORTE come input/output
    //TRISE = 0b000;
    //PORTE = 0x00;

    // Configurazione ADC (se usato)
    
    //RB0=power RB1=dir RB2=vel
    
    while(1)
    {
        if(RC0==1)RB0=1;
        /*
        PORTB=0b00000000; //tutto spento
        __delay_ms(1000);
        PORTB=0b00000001; //lento indietro acceso
        __delay_ms(3000);
        PORTB=0b00000101; //veloce indietro acceso
        __delay_ms(3000);
        PORTB=0b00000011; //lento avanti acceso
        __delay_ms(3000);
        PORTB=0b00000111; //veloce avanti acceso
        __delay_ms(3000);*/
    }
}
