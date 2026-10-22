
//==================================================
// ADC INIT (RB7 = AN6)
//==================================================
void adc_init(void)
{
    OSCCON = 0b01110000;    // clock

    ANSEL = 0b01000000;     // RB7 come ingresso analogico

    CMCON = 0b00000111;     // disabilito i comparatori

    TRISB = 0b10000000;     // metto  RB7 come ingresso e RB3 come uscita

    ADCON0 = 0b00011001;    // bit7-6 clock RC  | bit5-3 RB7 | bit2 conversion status | bit0 ON adc
    /*
    - quale canale leggo
    - accendo ADC
    - avvio conversione
    */
    
    ADCON1 = 0b10000000;    // bit7 risultato a destra
    /*
    - formato risultato (destra/sinistra)
    - riferimento tensione (NON USATA)
    - setup generale ADC 
    */
}

//==================================================
// LETTURA ADC
//==================================================
unsigned int adc_read(void)
{
    unsigned int value; // variabile a 16bit

    __delay_us(10); // delay

    ADCON0 = ADCON0 + 0b00000100; //avvia la conversione

    while((ADCON0 & 0b00000100) != 0); // CONTINUA a ciclare finché il bit GO è diverso da 0 poi va avnati senza fare nulla

    value = (ADRESH << 8) + ADRESL; // leggo ADRESH e lo scifto nei bit15-8 poi aggiungo ADRESL nel bit7-0

    return value;
}

//==================================================
// PWM INIT (RB3)
//==================================================
void pwm_init(void)
{
    TRISB = 0b10000000;

    PR2 = 99; 

    CCP1CON = 0b00001100;

    CCPR1L = 0b00000000;

    T2CON = 0b00000101;
}

//==================================================
// PWM SET (0..399)
// senza shift e senza bitwise OR
//==================================================
void pwm_set(unsigned int duty)
{
    unsigned int temp;
    unsigned char low;

    temp = duty;

    CCPR1L = 0;

    while(temp >= 4)
    {
        CCPR1L = CCPR1L + 1;
        temp = temp - 4;
    }

    low = 0b00000000;

    if(temp == 1)
    {
        low = 0b00010000;
    }

    if(temp == 2)
    {
        low = 0b00100000;
    }

    if(temp == 3)
    {
        low = 0b00110000;
    }

    CCP1CON = 0b00001100 + low;
}

//==================================================
// MAIN
//==================================================
void main(void)
{
    unsigned int adc_value; // variabile dove memorizzo il valore in adc
    unsigned int pwm_value; // variabile dove scrivo il valore del Duty Cycle dell'uscita pwm

    adc_init(); // inizializzo i registri per l'ingresso analogico

    pwm_init(); // inizializzo i registri per l'uscita in pwm

    while(1)
    {
        adc_value = adc_read(); // assegno alla variabile "adv_value" il valore che mi restituisce la variabile "adc_read()"

        pwm_value = adc_value * 399; // assegno alla variabile "pwm_value" il valore letto in "adc_value" moltiplicato per 399 che equivale al 100% del DC
        pwm_value = pwm_value / 1023; // poi prendo l'uscita e la divido per 1023 che sarebbe il massimo valore dell'adc che è a 10bit 2^10=1024

        pwm_set(pwm_value); //do alla variabile "pwm_set()" il valore calcolato prima 
    }
}