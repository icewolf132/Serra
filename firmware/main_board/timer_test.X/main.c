/*
 * File:   main.c
 * Author: william
 *
 * Created on 17 maggio 2026, 18.39
 */

#include <xc.h>
#include "header.h"

#define BTN RD4
#define LED RC0

volatile unsigned long ms = 0;
unsigned long start = 0;
int running = 0;

void __interrupt() isr() {
    if (TMR1IF) {
        TMR1IF = 0;

        // preload per 1ms con Fosc=4MHz, prescaler 1:8
        TMR1 = 65411;

        ms++;
    }
}

void main() {
    TRISD4 = 1;
    TRISC0 = 0;

    LED = 0;

    // Timer1: interno, prescaler 1:8
    T1CON = 0b00110001;

    TMR1 = 65411;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;

    while (1) {

        if (BTN == 0) 
        { 
            if (!running) 
            {
                running = 1;
                start = ms;
            }

            if ((ms - start) >= 5000) {
                LED = 1;
            }
        } 
        else 
        {
            running = 0;
            LED = 0;
        }
    }
}