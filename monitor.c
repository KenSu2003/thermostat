#include "avr/io.h"
#include "util/delay.h"
#define PERSISTENCE 5
#define COUNTTIME 50            // # ms between counts
void uart_init(void);
void uart_send (unsigned char);

int main(void) {
    unsigned char ledDigits [] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    unsigned int i=0;
    unsigned char DIG1, DIG2, DIG3, DIG4;
    uart_init();
    DDRC = 0x0F;
    DDRD = 0xF0;
    DDRB = 0xFF;
    
    while (1) {
        i++;
        if(i>9999) i=0;
        
        DIG4 = i%10;
        DIG3= (i/10)%10;
        DIG2 = (i/100)%10;
        DIG1 = (i/1000);

        for (int j=0; j<COUNTTIME/PERSISTENCE/4; j++){

            //Show 1000's digit
            PORTC = ledDigits[DIG1];
            PORTD = ledDigits[DIG1];
            uart_send (DIG1+ '0');
            PORTB = ~ (1<<4) ;
            _delay_ms (PERSISTENCE);

            //Show 100's digit
            PORTC = ledDigits [DIG2];
            PORTD = ledDigits [DIG2];
            uart_send (DIG2+ '0');
            PORTB = ~ (1<<3);
            _delay_ms (PERSISTENCE);

            //Show 10's digit
            PORTC = ledDigits [DIG3];
            PORTD = ledDigits [DIG3];
            uart_send (DIG3+ '0');
            PORTB = ~ (1<<2) ;
            _delay_ms (PERSISTENCE);

            //Show 1's digit
            PORTC = ledDigits [DIG4];
            PORTD = ledDigits [DIG4];
            uart_send (DIG4+ '0');
            PORTB = ~ (1<<1);
            _delay_ms (PERSISTENCE);

            PORTB = 0xFF;
            uart_send(13); 
            uart_send(10);

        }
    }
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

