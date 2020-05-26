

/*****************************************************************************************
Autor: José David Ponce
Carné: 18187
Laboratorio 10 Tiva C
Algunas funciones se basaron en los ejemplos subidos al repositorio de la clase por
el Ing. Pablo Mazariegos.
 *****************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
//#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
//#include "driverlib/timer.h"
#include "driverlib/uart.h"




int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //Se configura el reloj para que tenga una frecuencia de 40MHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //Se activa el periférico del puerto F
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){
        //Este ciclo espera que se termine de iniciar el periférico
    }
    //Luego se activan como outputs los pines 1 2 y 3 (LEDS del RGB)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //Es necesario configurar el pin del botón SW1 que es el pin 4 del puerto F como input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    //Acá se configura la resistencia weak pull up para el pin del botón
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
	while(1){


	}
}
