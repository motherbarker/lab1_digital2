#include "iocb_init.h"

//Función para inicializar IOCB
void iocb_init(uint8_t pinesB){
    TRISB |= pinesB;    //Selecciona pin RBx como input    
    nRBPU = 0;          //activa pullups en puerto B
    WPUB |= pinesB;     //activa pullups en pines
    RBIE = 1;           //PORTB Interrupt on Change
    IOCB |= pinesB;     //RBx Interrupt 
    GIE  = 1;           //Global Interrupt 
}
