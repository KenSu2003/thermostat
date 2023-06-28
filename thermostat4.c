#include "avr/io.h"
#include "util/delay.h"
#include <string.h>
#include <stdlib.h>
#define PERSISTENCE 2
#define COUNTTIME 50
void uart_init(void);
void uart_send(unsigned char);
void send_string(char *stringAddress);
void adc_init(void);
unsigned int get_adc(void);

int main(void){
    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,
     0x07, 0x7F, 0x67};
    unsigned char DIG1, DIG2, DIG3, DIG4;
    // uart_init();  // initialize the UART
    // DDRC = 0x0F;  // segments a-d use PC0-PC3
    DDRD = 0xFF;  // segments e-g & dp use PD4-PD7
    DDRB = 0xFF;  // Digit enable pins
    PORTC = 1<<PORTC2;


    unsigned int digitalValue;
    int Tc; int Tf;
    char buffer[6];
    adc_init();       // Initalize the ADC

    while(1) {

        digitalValue = get_adc();
        itoa(digitalValue, buffer, 10);       // Convert to charcter string     
        // send_string(buffer);                  // Tx string
        Tc = 10*(digitalValue * 5/10.24 - 50);       // Temp in celcius
        Tf = (int)(10*(Tc * 9./50. + 32.));                   // Temp in F

        DIG4 =  0x71;          // 1's digit (least significant digit)
        DIG3 = (Tf/1)%10;      // 10's digit
        DIG2 = (Tf/10)%10;     // 100's digit
        DIG1 = (Tf/100)%10;    // 1000's digit (most significant digit)


        if((PINC & (1<<PINC2)) == 0){
            DIG4 =  0x39;          // 1's digit (least significant digit)
            DIG3 = (Tc/1)%10;      // 10's digit
            DIG2 = (Tc/10)%10;     // 100's digit
            DIG1 = (Tc/100)%10;    // 1000's digit (most significant digit)
            while((PINC & (1<<PINC2))){
                int j;
                for (j=0; j<COUNTTIME/PERSISTENCE/4; j++) {

                    // Show 1000's digit
                    // PORTC = (PORTC&0xF0)|(ledDigits[DIG1]&0x0F);
                    PORTD = (ledDigits[DIG1]);
                    // uart_send(DIG1+'0');      // Tx 1000's digit
                    PORTB = ~ (1<<4);         // Enable DIG1
                    _delay_ms(PERSISTENCE);

                    // Show 100's digit
                    // PORTC = (PORTC&0xF0)|(ledDigits[DIG2]&0x0F);
                    PORTD = ((ledDigits[DIG2])|(1<<7));
                    // uart_send(DIG2+'0');     // Tx 100's digit
                    PORTB = ~ (1<<3);        // Enable DIG2
                    _delay_ms(PERSISTENCE);

                    // Show 10's digit
                    // PORTC = (PORTC&0xF0)|(ledDigits[DIG3]&0x0F);
                    PORTD = (ledDigits[DIG3]);
                    // uart_send(DIG3+'0');     // Tx 10's digit
                    PORTB = ~ (1<<2);        // Enable DIG3
                    _delay_ms(PERSISTENCE);

                    // Show 1's digit
                    // PORTC = DIG4;
                    PORTD = DIG4;
                    // uart_send(DIG4+'0');     // Tx 1's digit
                    PORTB = ~ (1<<1);        // Enable DIG4
                    _delay_ms(PERSISTENCE);

                    PORTB = 0xFF;            // Disable all digits
                    // uart_send(13);           // Tx carriage return
                    // uart_send(10);           // Tx line feed
                }
            }
        }

        int j;
        for (j=0; j<COUNTTIME/PERSISTENCE/4; j++) {

            // Show 1000's digit
            // PORTC = (PORTC&0xF0)|(ledDigits[DIG1]&0x0F);
            PORTD = (ledDigits[DIG1]);
            // uart_send(DIG1+'0');      // Tx 1000's digit
            PORTB = ~ (1<<4);         // Enable DIG1
            _delay_ms(PERSISTENCE);

            // Show 100's digit
            // PORTC = (PORTC&0xF0)|(ledDigits[DIG2]&0x0F);
            PORTD = ((ledDigits[DIG2])|(1<<7));
            // uart_send(DIG2+'0');     // Tx 100's digit
            PORTB = ~ (1<<3);        // Enable DIG2
            _delay_ms(PERSISTENCE);

            // Show 10's digit
            // PORTC = (PORTC&0xF0)|(ledDigits[DIG3]&0x0F);
            PORTD = (ledDigits[DIG3]);
            // uart_send(DIG3+'0');     // Tx 10's digit
            PORTB = ~ (1<<2);        // Enable DIG3
            _delay_ms(PERSISTENCE);

            // Show 1's digit
            // PORTC = DIG4;
            PORTD = DIG4;
            // uart_send(DIG4+'0');     // Tx 1's digit
            PORTB = ~ (1<<1);        // Enable DIG4
            _delay_ms(PERSISTENCE);

            PORTB = 0xFF;            // Disable all digits
            // uart_send(13);           // Tx carriage return
            // uart_send(10);           // Tx line feed
        }
    }
}

// here are the function definitions
void send_string(char *stringAddress){
    unsigned char i;
    for (i = 0; i < strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}

void adc_init(void){
    ADMUX = 0x45;     // Select ADC5; Vref=AVcc=1.1V
    ADCSRA = 0x87;    // Enable ADC; set speed 125 KHz for 16 MHz
}

unsigned int get_adc(){
    ADCSRA |= (1 << ADSC);   // Start ADC conversion
    while((ADCSRA & (1 << ADIF)) == 0);   // Wait until ADC finishes
    return ADCL | (ADCH << 8);   // Read ADCL first !
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