/*
 * File:   PostLab7.c
 * Author: Jorge Cerón
 *
 * Created on 8 de abril de 2022, 08:40 AM
 */
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>             // int8_t, unit8_t
#define _tmr0_value 217         // Valor para 10 ms
#define _XTAL_FREQ 4000000

uint8_t cont1;
uint8_t cont2;
uint8_t bandera;
uint8_t display1;
uint8_t display2;
uint8_t display3;
uint8_t digito1;
uint8_t digito2;
uint8_t digito3;
uint8_t splitarray[3];
uint8_t tabla[10] = {0b00111111,       // 0
                     0b00000110,       // 1
                     0b01011011,       // 2
                     0b01001111,       // 3
                     0b01100110,       // 4
                     0b01101101,       // 5
                     0b01111101,       // 6
                     0b00000111,       // 7
                     0b01111111,       // 8
                     0b01101111};      // 9

void tmr0_setup(void){
    OPTION_REGbits.T0CS = 0;     // Temporizador
    OPTION_REGbits.PSA = 0;      // Prescaler a TMR0
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;      // Prescaler 1:256
    INTCONbits.T0IF = 0;         // Limpiar la bandera de TMR0
    TMR0 = _tmr0_value;          // Valor de retraso
    return;
}

void __interrupt() isr (void){
    if(INTCONbits.RBIF){        // Verificar si se activa la bandera de interrupcion de PORTB 
        if(!PORTBbits.RB0){     // Verificar si fue actividad por presionar RB0
            PORTA++;            // Incrementar en 1 PORTA 
        }
        else if(!PORTBbits.RB1){// Verificar si se activa la bandera de interrupcion de PORTB 
            PORTA--;            // Decrementar en 1 PORTA 
        }
        INTCONbits.RBIF = 0;    // Limpiar la bandera de interrupcion de PORTB
    }
    if(INTCONbits.T0IF){        // Verificar si se activa la bandera de interrupcion de TMR0
        PORTD = 0;
        if(bandera == 0){        
            PORTC = display1;   
            PORTD = 0b00000001;
            bandera = 1; 
        }
        else if(bandera == 1){        
            PORTC = display2;   
            PORTD = 0b00000010;
            bandera = 2; 
        }
        else if(bandera == 2){        
            PORTC = display3;   
            PORTD = 0b00000100;
            bandera = 0; 
        }
        INTCONbits.T0IF = 0;// Limpiar la bandera de TMR0
        TMR0 = _tmr0_value; // Valor de retraso para reinicio de TMR0
    }
    return;
}

void setup(void){
    ANSEL = 0;
    ANSELH = 0;                 // I/O digitales
        
    TRISA = 0x00;               // PORTA como salida
    TRISC = 0x00;               // PORTC como salida
    TRISD = 0x00;               // PORTD como salida
    PORTA = 0x00;               // Se limpia PORTA
    PORTC = 0x00;               // Se limpia PORTC
    PORTD = 0x00;               // Se limpia PORTD
    
    TRISBbits.TRISB0 = 1;       // RBO como entrada
    TRISBbits.TRISB1 = 1;       // RB1 como entrada
    
    OPTION_REGbits.nRBPU = 0;   // Habilitar Pull-ups
    WPUBbits.WPUB0 = 1;         // Pull-up en RB0
    WPUBbits.WPUB1 = 1;         // Pull-up en RB1
    // Configuración de interrupciones
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.T0IE = 1;        // Habilitar interrupciones de TMR0
    INTCONbits.RBIE = 0;        // Deshabilitar interrupciones de PORTB
    IOCBbits.IOCB0 = 1;         // Habilitar interrupciones de cambio de estado en RB0
    IOCBbits.IOCB1 = 1;         // Habilitar interrupciones de cambio de estado en RB1
    INTCONbits.RBIF = 0;        // Limpiar la bandera de interrupcion de PORTB
    INTCONbits.T0IF = 0;        // Limpiar la bandera de TMR0
    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;   // 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    return;
}

struct digitos {                 // Estructura contenedora de digitos de numero de 3 digitos de largo
    int digito1;
    int digito2;
    int digito3;
};


struct digitos function(int num){   // Función de separación de dígitos
    int splitarray[3];              // Array que almacena los digitos
    struct digitos D1;              // Estructura temporal para asignar los digitos y devolverlos

    int digit = num%10;             // Se toma el ultimo digito del numero que entra a la función
    splitarray[0] = digit;          // Se almacena el digito en la primera posición del array
    
    num = num/10;                   // Se divide el valor que entra de la función en 10 y se actualiza
    digit = num%10;                 // Se toma el ultimo digito del numero actualizado
    splitarray[1] = digit;          // Se almacena el digito en la segunda posición del array

    num = num/10;                   // Se divide el valor que entra de la función en 10 y se actualiza
    splitarray[2] = num;          // Se almacena el digito en la tercera  posición del array

    // Almacenamiento de digitos (Se hace de forma inversa debido a que "digit" extrae los digitos inversamente)
    D1.digito3 = splitarray[0];     // Unidades
    D1.digito2 = splitarray[1];     // Decenas
    D1.digito1 = splitarray[2];     // Centenas
    
    // Regresamos la structure
    return D1;
}

void main(void){
    setup();
    tmr0_setup();
    while(1){                                       // Ejecutar para siempre
        struct digitos result = function(PORTA);    // Se corre la funcion para extraer los digitos
        display3 = tabla[result.digito3];           // Mostrar el valor de Unidades en display3
        display2 = tabla[result.digito2];           // Mostrar el valor de Decenas en display2
        display1 = tabla[result.digito1];           // Mostrar el valor de Centenas en display1
    }
    return;
}
