#include "adcconfig.h"

//Función para inicializar el módulo ADC
void adc_init(uint8_t J, uint8_t R, uint8_t clock, uint8_t channel){
    
    
    ADFM = J;   //justificado
    VCFG0 = R;  //referencia de voltaje
    VCFG1 = R;    
    //Conversion Clock Select bits
    switch(clock){
        case 1:
            ADCON0bits.ADCS = 0b00;
            //delay = 2.0us;
            break;
        case 4:
            ADCON0bits.ADCS = 0b01;
            //delay = 2.0us;
            break;
        case 8:
            ADCON0bits.ADCS = 0b10;
            //delay = 4.0us;
            break;
        case 20:
            ADCON0bits.ADCS = 0b11;
            //delay = 6.0us;
            break;
    }    
    ADCON0bits.CHS = channel;   //seleccionar canal
    ADON = 1;           //Habilitar módulo ADC   
}

//leer ADC
uint16_t adc_value(void){    
    ADCON0bits.GO = 1;      
    while(ADCON0bits.GO);   
    return (ADRESH<<8) | ADRESL; 
}

//cambiar de canal
void adc_sel_channel(uint8_t channel){
    ADCON0bits.CHS = channel;  
}

//obtener canal 
uint8_t adc_get_channel(void){
    return ADCON0bits.CHS;
}