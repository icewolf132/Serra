// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  


// CONFIG1
#pragma config FOSC = INTOSCIO // Selezione oscillatore (oscillatore EXTRC; funzione CLKO su RA6/OSC2/CLKO)
#pragma config WDTE = ON        // Abilitazione Watchdog Timer (WDT attivo)
#pragma config PWRTE = OFF      // Abilitazione Power-up Timer (PWRT disattivato)
#pragma config MCLRE = ON       // Selezione funzione pin RA5/MCLR/VPP (il pin funziona come MCLR)
#pragma config BOREN = ON       // Abilitazione Brown-out Reset (BOR attivo)
#pragma config LVP = OFF         // Abilitazione programmazione a bassa tensione (RB3/PGM attivo)
#pragma config CPD = OFF        // Protezione codice memoria EEPROM dati (disattivata)
#pragma config WRT = OFF        // Protezione scrittura memoria programma Flash (disattivata)
#pragma config CCPMX = RB0      // Selezione pin CCP1 (funzione CCP1 su RB0)
#pragma config CP = OFF         // Protezione codice memoria programma Flash (disattivata)

// CONFIG2
#pragma config FCMEN = ON       // Abilitazione monitor Fail-Safe del clock (attivo)
#pragma config IESO = ON        // Abilitazione commutazione interna/esterna oscillatore (attiva)

// Le direttive #pragma config devono precedere gli include del progetto.
// Usa gli enum del progetto invece di #define per ON e OFF.

// DEFINIZIONE CLOCK
#define _XTAL_FREQ 8000000  // 8 MHz

// MACRO UTILI
//#define ON  1
//#define OFF 0

// PROTOTIPI FUNZIONI
//void initMCU(void);
//void delay_ms(unsigned int ms);


// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

