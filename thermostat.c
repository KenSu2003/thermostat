#include "avr/io.h"
#include "util/delay.h"
#include <string.h>
#include <stdlib.h>
#define PERSISTENCE 2
#define COUNTTIME 50                                                    // This is the # of ms between counts
void uart_init(void);
void uart_send(unsigned char);
void send_string(char *stringAddress);
void adc_init(void);
unsigned int get_adc(void);


int main(void){

    /* setup */
    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    unsigned char DIG1, DIG2, DIG3, DIG4;
    unsigned int digitalValue;
    char buffer[6];
    int Tc; int Tf;                                                     // variables to store temperature in celcius and fahrenheit

    adc_init();                                                         // initalize the ADC
    uart_init();                                                        // initialize the UART
    DDRC = 0x0F;                                                        // a to d uses PC0-PC3
    DDRD = 0xF0;                                                        // e to g && dp uses PD4-PD7
    DDRB = 0xFF;                                                        // Digit enable pins

    PORTD = 1<<PORTD2;                                                  // Set pull up on D2

    
    /* Start the Thermometer */
    while(1){


        /* Thermometer setup */
        digitalValue = get_adc();
        itoa(digitalValue, buffer, 10);                                 // Convert to charcter string     
        send_string(buffer);                                            // Tx string
        Tc = 10*(digitalValue * 1.1/10.24 - 50);                        // Temp in celcius
        Tf = (Tc * 9/5+32);

        /* Display Fahreniheit */    
        if ((PIND & (1<<PIND2)) != 0){

            if ((PIND & (1<<PIND2)) == 0) break;                        // exit the loop if button is not pressed


            /* thermometer setup */
            digitalValue = get_adc();
            itoa(digitalValue, buffer, 10);                             // Convert to charcter string     
            send_string(buffer);                                        // Print string
            Tc = 10*(digitalValue * 1.1/10.24 - 50);                    // Temp in celcius
            Tf = (Tc * 9/5+32);                                         // Temp in F


            // Set temperature to celcius
            DIG4 =  0x71;                                               // 1s digit (least significant digit)
            DIG3 = (Tf/1)%10;                                           // 10s digit
            DIG2 = (Tf/10)%10;                                          // 100s digit
            DIG1 = (Tf/100)%10;                                         // 1000s digit (most significant digit)

            int j;
            for (j=0; j<COUNTTIME/PERSISTENCE/4; j++) {

                // Display 1000s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG1]&0x0F);
                PORTD = (ledDigits[DIG1]&0xF0)|(PORTD&0x0F);
                PORTB = ~ (1<<4);                                       // Enable DIG1
                _delay_ms(PERSISTENCE);

                // Display 100s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG2]&0x0F);
                PORTD = ((ledDigits[DIG2]+127)&0xF0)|(PORTD&0x0F);      // +127 to enable decimal point
                PORTB = ~ (1<<3);                                       // Enable DIG2
                _delay_ms(PERSISTENCE);

                // Display 10s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG3]&0x0F);
                PORTD = (ledDigits[DIG3]&0xF0)|(PORTD&0x0F);
                PORTB = ~ (1<<2);                                       // Enable DIG3
                _delay_ms(PERSISTENCE);

                // Display 1s digit
                PORTC = DIG4;
                PORTD = DIG4;
                PORTB = ~ (1<<1);                                       // Enable DIG4
                _delay_ms(PERSISTENCE);

                PORTB = 0xFF;                                           // Disable all digits
                uart_send(13);                                          // carriage return (goto beginning of linbe)
                uart_send(10);                                          // line feed (new line)
                
            }

        } else {   

            if ((PIND & (1<<PIND2)) != 0) break;                        // exit the loop if button is not pressed

            // Set temperature to fahreinheit
            DIG4 =  0x39;                                               // 1s digit (least significant digit) –-–> C
            DIG3 = (Tc/1)%10;                                           // 10s digit
            DIG2 = (Tc/10)%10;                                          // 100s digit
            DIG1 = (Tc/100)%10;                                         // 1000s digit (most significant digit)

            int j;
            for (j=0; j<COUNTTIME/PERSISTENCE/4; j++) {

                // Display 1000s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG1]&0x0F);
                PORTD = (ledDigits[DIG1]&0xF0)|(PORTD&0x0F);
                PORTB = ~ (1<<4);                                       // Enable DIG1
                _delay_ms(PERSISTENCE);

                // Display 100s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG2]&0x0F);
                PORTD = ((ledDigits[DIG2]+127)&0xF0)|(PORTD&0x0F);
                PORTB = ~ (1<<3);                                       // Enable DIG2
                _delay_ms(PERSISTENCE);

                // Display 10s digit
                PORTC = (PORTC&0xF0)|(ledDigits[DIG3]&0x0F);
                PORTD = (ledDigits[DIG3]&0xF0)|(PORTD&0x0F);
                PORTB = ~ (1<<2);                                       // Enable DIG3
                _delay_ms(PERSISTENCE);

                // Display 1s digit
                PORTC = DIG4;
                PORTD = DIG4;
                PORTB = ~ (1<<1);                                       // Enable DIG4
                _delay_ms(PERSISTENCE);

                PORTB = 0xFF;                                           // Disable all digits
                uart_send(13);                                          // carriage return (goto beginning of linbe)
                uart_send(10);                                          // line feed (new line)
                
            }
            
        } 

    }
}



/****** Functions *******/
// here are the function definitions
void send_string(char *stringAddress){
    unsigned char i;
    for (i = 0; i < strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}

void adc_init(void){
    ADMUX = 0xC5;                                                       // Select ADC5
    ADCSRA = 0x87;                                                      // Enable ADC
}

unsigned int get_adc(){
    ADCSRA |= (1 << ADSC);                                              // ADC conversion
    while((ADCSRA & (1 << ADIF)) == 0);                                 // Wait until ADC finishes
    return ADCL | (ADCH << 8);                                          // Read ADCL first
}

void uart_init(void){
    UCSR0B = (1 << TXEN0);                                              //enable the UART transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                             //set 8 bit character size
    UBRR0L = 103;                                                       //set baud rate to 9600 for 16 MHz crystal
}

void uart_send(unsigned char ch){
    while (!(UCSR0A & (1 << UDRE0)));                                   //wait til tx data buffer empty
    UDR0 = ch;                                                          //write the character to the USART data register
}
