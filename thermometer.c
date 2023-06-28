// Written 4/21/22 D.McLaughlin
// here are the necessary includes and function prototypes
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
void uart_init(void);
void uart_send(unsigned char letter);
void send_string(char *stringAddress);

// main code
int main(void){
    unsigned int digitalValue, voltInt;
    char buffer [6]; adc_init(); uart_init();
    double Tc; double Tf;
    while (1) {
        digitalValue = get_adc() ;
        itoa(digitalValue, buffer, 10);
        send_string (buffer);
        Tc = digitalValue * 0.4883 - 50;    //gives us the temperature in degrees C.
        Tf = Tc * 9./5.+32.;
        itoa(Tc, buffer, 10);
        send_string(" ");
        send_string(buffer);
        uart_send(13);
        uart_send(10);
        _delay_ms(100);
    }
    return 0;
}


// here are the function definitions
void adc_init (void) {
    ADMUX = 0x40;
    ADCSRA = 0x87;
}

unsigned int get_adc(){
    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADIF)) == 0);
    return ADCL | (ADCH << 8);
}

void uart_init(void){
    UCSR0B = (1 << TXEN0); //enable the UART transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //set 8 bit character size
    UBRR0L = 103; //set baud rate to 9600 for 16 MHz crystal
}

void uart_send(unsigned char ch){
    while (!(UCSR0A & (1 << UDRE0))); //wait til tx data buffer empty
    UDR0 = ch; //write the character to the USART data register
}

void send_string(char *stringAddress){
    for (unsigned char i = 0; i < strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}