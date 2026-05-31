/*
 * File:   main.c
 * Author: william
 *
 * Created on 17 maggio 2026, 18.39
 */

#include <xc.h>
#include "header.h"

// =====================
// VARIABILI GLOBALI
// =====================
int stato_mot = 0;

int stop_flag = 0;
int stop_timer_attivo = 0;

volatile unsigned int ms = 0;
volatile unsigned char stop_done = 0;

// timer 5s
unsigned int stop_start_time = 0;

// =====================
// DRIVER
// =====================
void driver(int power, int dir, int vel, int acqua)
{
    RA3 = power;
    RA4 = dir;
    RA5 = vel;
    RC2 = acqua;
}

// =====================
// TIMER ISR (1ms tick)
// =====================
void __interrupt() ISR(void)
{
    if(TMR1IF)
    {
        TMR1IF = 0;
        TMR1H = 0;
        TMR1L = 0;

        ms++;   // tick globale
    }
}

// =====================
// TASK MOTORE (FSM)
// =====================
void Task_Motore(void)
{
    if(stop_flag) return;

    switch(stato_mot)
    {
        case 0:
            driver(0,0,0,0);
            if(START == 0) stato_mot = 1;
        break;

        case 1:
            if(FC1 == 0)
                stato_mot = 2;
            else
                stato_mot = 3;
        break;

        case 2:
            driver(1,0,1,0);
            if(FC1 == 1) stato_mot = 3;
        break;

        case 3:
            driver(1,1,0,1);
            if(FC2 == 1) stato_mot = 4;
        break;

        case 4:
            if(MODO == 1) stato_mot = 5;
            else stato_mot = 6;
        break;

        case 5:
            driver(1,0,0,1);
            if(FC1 == 1) stato_mot = 0;
        break;

        case 6:
            driver(1,0,1,0);
            if(FC1 == 1) stato_mot = 0;
        break;
    }
}

// =====================
// STOP NON BLOCCANTE
// =====================
void Task_Stop(void)
{
    if(STOP == 0 && stop_flag == 0)
    {
        stop_flag = 1;
        stop_start_time = ms;
    }

    if(stop_flag)
    {
        driver(0,0,0,0);

        // dopo 5 secondi
        if((ms - stop_start_time) >= 5000)
        {
            stop_done = 1;
        }

        // se premi START riparte subito
        if(START == 0)
        {
            stop_flag = 0;
            stop_done = 0;
        }

        // dopo stop completo torna a stato iniziale
        if(stop_done)
        {
            stato_mot = 0;
            stop_flag = 0;
            stop_done = 0;
        }
    }
}

// =====================
// VENTILAZIONE (sempre attiva)
// =====================
void Task_Ventilazione(void)
{
    int temp = leggiTemperatura();   // tua funzione ADC

    if(temp > 30)
        RC1 = 1;
    else
        RC1 = 0;
}

// =====================
// INIT TIMER
// =====================
void Timer1_Init(void)
{
    T1CON = 0x31; // prescaler 1:8
    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

// =====================
// MAIN
// =====================
void main(void)
{
    OSCCON = 0b01100000;
    ANSEL = 0x00;

    TRISA = 0b00000011;
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
        Task_Stop();
        Task_Ventilazione();
    }
}