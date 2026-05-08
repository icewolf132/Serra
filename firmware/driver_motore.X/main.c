/*
 * File:   main.c
 * Author: William Landolfi
 *
 * Created on 28 aprile 2026, 21.38
 */

#include "header.h"

void main(void)
{
    OSCCON = 0b01110000; // 8 MHz interno
    ANSEL = 0x00;  // Tutti i pin digitali
    CMCON = 0x07;

    TRISA = 0b00011100; // dichiaro ingressi e uscite nel registo A
    PORTA = 0b00000000; // imposto tutte le uscite a 0 logico

    TRISB = 0b00000000; 
    PORTB = 0b00000000;

    while(1) 
    {
        RA1=1;
        RA0=0;
        RB3=1;
    }
    return;
}