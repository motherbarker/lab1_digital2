/* 
 * File:   lab1main.c
 * Device: PIC16F887
 * Author: Carlos Daniel Valdez Coreas - 21976
 *Compiler: XC8 (v2.41)
 * 
 * Program: 
 * Hardware:
 * 
 * Created: 18 de julio del 2023
 * Last updated:
 */

/*--------------------------------- LIBRARIES --------------------------------*/
#include <xc.h>
#include <PIC16F887.h>
#include "iocb_init.h"
#include "adcconfig.h"
#include "displayconfig.h"

/*---------------------------- CONFIGURATION BITS ----------------------------*/
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)




/*----------------------- GLOBAL VARIABLES & CONSTANTS -----------------------*/
#define _XTAL_FREQ 8000000  //Constante para delay
#define tmr0n 100   //TMR0 load value
uint8_t adc_value;    //lectura del ADC
uint16_t nibbles;   //nibbles alto y bajo
uint8_t hnibble;   //nibble alto
uint8_t lnibble;    //nibble bajo
uint8_t display1;      //valor primer display
uint8_t display2;      //valor segundo display
uint8_t displayselect;    //seleccionar display
        


        
/*-------------------------------- PROTOTYPES --------------------------------*/
void ioc_portB(void);
void setup(void);
void multiplexado(uint8_t selector);
void alarma(uint8_t ref,uint8_t var);



/*----------------------------- INTERRUPT VECTOR -----------------------------*/
void __interrupt() isr(void){
    if(T0IF){
        displayselect++;  //cambiar de display
        TMR0 = tmr0n;       //resetear el tmr0
        T0IF = 0;
    }
    if(RBIF){
        ioc_portB();
        RBIF = 0;
    }    
}

/*--------------------------- INTERRUPT SUBROUTINES --------------------------*/
void ioc_portB(void){
    if(!RB0) PORTC++;
    if(!RB1) PORTC--;
    if(!RB2) adc_sel_channel(13);
    if(!RB3) adc_sel_channel(11);
}
/*---------------------------------- TABLES ----------------------------------*/

/*----------------------------------- SETUP ----------------------------------*/

/*----------------------------------- MAIN -----------------------------------*/
int main(void) {
    setup();
    while(1){       //loop infinito
        
        //toma el valor del potenciometro y lo muestra en el display
        adc_value = adc_read()>>8;            //lee valor del pot
        nibbles = split_nibbles(adc_value);   //separa el valor por nibbles
        lnibble = nibbles;                  //toma nibble bajo
        hnibble = nibbles>>8;              //toma nibble alto
        display2 = hex_to_7seg(lnibble);       //envia el nibble bajo como valor para el display1
        display1 = hex_to_7seg(hnibble);      //envia el nibble alto como valor para el display2
        multiplexado(displayselect);                 //cambia de display
        
        alarma(PORTA,adc_value); //revisa si el valor de la lectura del pot es mayor que el valor del contador
    }
}
/*-------------------------------- SUBROUTINES -------------------------------*/
void setup(void){
   //entradas y salidas
    ANSEL = 0;  //PORTA digital
    ANSELH= 0b00101000; 
    TRISA = 0;  
    PORTC = 0x7F;  
    TRISC = 0;  
    PORTC = 0;  
    TRISD = 0;  
    PORTD = 0;  
    TRISE = 0;  
    PORTE = 0;  
    
    //OSCILLATOR CONFIG
    OSCCONbits.IRCF = 0b111;  //8MHz
    SCS = 1;
    
    iocb_init(0x0F); // IOCB
    adc_init(0, 0, 8, 0b1101); //configuracion para ADC.
    
    //Configuración del TMR0
    GIE = 1;    //interrupciones globales
    T0IE = 1;   //TMR0 interrupt
    OPTION_REGbits.PS = 0b000;  //Prescaler = 2
    T0CS = 0;   //internal clock
    TMR0 = tmr0n;
    T0IF = 0;   //limpiar bandera
}

void multiplexado(uint8_t selector){
    if(selector & 0x01 == 1){        //display1
            RE0 = 0;    //desactiva disp 1
            PORTD = display2;
            RE1 = 1;    //activa disp 2            
        }
        else{           //display 1 desactivado
            RE1 = 0;    //desactiva display 2
            PORTD = display1;
            RE0 = 1;    //activa display 1           
        }        
}

void alarma(uint8_t ref,uint8_t var){
    if(var >= ref) RE2 = 1;
    else RE2 = 0;
}