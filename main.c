

/*****************************************************************************************
Autor: José David Ponce
Carné: 18187
Laboratorio 10 Tiva C
Algunas funciones se basaron en los ejemplos subidos al repositorio de la clase por
el Ing. Pablo Mazariegos.
 *****************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/TM4C123GH6PM.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#define tiempo 10000000

void sendString(char *a);
void returnCartnl(void);
void enviarColorCorrespondiente(void);
void esp3286_connect(void);

bool pulsado = 0;
bool pulsado2 = 0;
uint8_t color = 0;
uint8_t leds = 0;

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    //Se configura el reloj para que tenga una frecuencia de 40MHz
    //Es necesario desbloquear el botón SW2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //Se activa el periférico del puerto F
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){
        //Este ciclo espera que se termine de iniciar el periférico
    }
    //Luego se activan como outputs los pines 1 2 y 3 (LEDS del RGB)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    //Es necesario configurar el pin del botón SW1 que es el pin 4 del puerto F como input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
    //Acá se configura la resistencia weak pull up para el pi   n del botón
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    //Se configura el UART1 para comunicarse con el esp3286
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    //Se usan los pines PC4 y PC5 como RX y TX respectivamente
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    //Se les asigna el módulo UART1 a los pines
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
    //Se configura el Baud Rate de 115200
    UARTConfigSetExpClk(UART1_BASE, 16000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTEnable(UART1_BASE);
    //Función para verificar con la consola que esté funcionando el esp3286
    sendString("AT");
	while(1){
	    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){     //Antirrebote del botón SW2
	        pulsado2 = 1;
	    }
	    if (!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){     //Antirrebote del botón SW1
            pulsado = 1;
	    }

	    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)){
	        SysCtlDelay(100);
	        if (pulsado == 1){
	            color++;                                    //Aumentar el color a mostrar
	            color = color%8;
	            pulsado = 0;
	        }
	    }
	    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)){
	        SysCtlDelay(100);
	        if (pulsado2 == 1){
	            enviarColorCorrespondiente();               //Ir a rutina para enviar el color actual
	            pulsado2 = 0;
	        }

	    }
	    leds= color<<1;                                 //El shift coloca el número del color en los pines del
    GPIOPinWrite(GPIO_PORTF_BASE, 0x0E, leds);          //led RGB para mostrarlo

	}
}

void enviarColorCorrespondiente(void){
    esp3286_connect();                  //Se conecta al servidor antes de enviar un color
    SysCtlDelay(tiempo);                //Espera tiempo suficiente para que se haga la conexión
    switch(color){
    //En las siguientes funciones se envía el POST REQUEST de cada color, con su longitud de caracteres respectiva
    case 0:     //Color negro
        sendString("AT+CIPSEND=165");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 35\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=0&color=Negro\r\n");
        break;
    case 1:     //Color rojo
        sendString("AT+CIPSEND=164");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 34\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=1&color=Rojo\r\n");
        break;
    case 2:     //Color Azul
        sendString("AT+CIPSEND=164");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 34\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=2&color=Azul\r\n");
        break;
    case 3:     //Color Violeta
        sendString("AT+CIPSEND=167");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 37\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=3&color=Violeta\r\n");
        break;
    case 4:     //Color Verde
        sendString("AT+CIPSEND=165");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 35\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=4&color=Verde\r\n");
        break;
    case 5:     //Color Amarillo
        sendString("AT+CIPSEND=168");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 38\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=5&color=Amarillo\r\n");
        break;
    case 6:     //Color Magenta
        sendString("AT+CIPSEND=168");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 38\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=6&color=Turquesa\r\n");
        break;
    case 7:     //Color Blanco
        sendString("AT+CIPSEND=166");
        SysCtlDelay(tiempo);
        sendString("POST /index.php HTTP/1.0\r\nHost: 192.168.0.15\r\nAccept: /\r\nContent-Length: 36\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\ncarnet=18187&id_color=7&color=Blanco\r\n");
        break;
    }
}

void sendString(char *a){                   //Función para enviar caracteres uno por uno de una string
    while (UARTBusy(UART1_BASE)){
                                            //El programa se detiene mientras UART esté ocupado
    }
    while(*a != '\0'){
        UARTCharPut(UART1_BASE, *a++);         //Se va avanzando en el siguiente caracter
    }
    returnCartnl();                          //Se imprime el retorno del carro al final de cada linea
}

void esp3286_connect(void){     //La conexión que se hace al servidor siempre antes de enviar datos
    sendString("AT+CIPSTART=\"TCP\",\"192.168.0.15\",80");
    returnCartnl();
}

void returnCartnl(void){
    UARTCharPut(UART1_BASE, '\r');  //Se retorna el carro y se imprime nueva línea en la consola
    UARTCharPut(UART1_BASE, '\n');
}
