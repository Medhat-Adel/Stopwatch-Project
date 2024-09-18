/*
 * Project: Digital Stopwatch with Count Modes
 *
 * Author: Medhat Adel Tawfik
 *
 * Description:
 * This project implements a digital stopwatch using the ATmega32 microcontroller.
 * The stopwatch features two operational modes: count up and count down. It uses
 * six multiplexed 7-segment displays to show time in HH:MM:SS format. The user can
 * reset, pause, and resume the stopwatch, and increment or decrement hours, minutes,
 * and seconds individually. Additionally, the stopwatch can toggle between the two modes
 * with visual indicators for the current mode. An alarm triggers upon reaching 00:00:00
 * in countdown mode.
 *
 * The system uses Timer1 to generate 1-second interrupts for timekeeping. External
 * interrupts are utilized for reset, pause, and resume functionalities, while button presses
 * allow for time adjustments.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Variables to hold time values */
unsigned char secondsUNITS = 0;  /* Units of seconds (0-9) */
unsigned char secondsTENS = 0;  /* Tens of seconds (0-5) */

unsigned char minutesUNITS = 0;  /* Units of minutes (0-9) */
unsigned char minutesTENS = 0;  /* Tens of minutes (0-5) */

unsigned char hoursUNITS = 0;  /* Units of hours (0-9) */
unsigned char hoursTENS = 0;  /* Tens of hours (0-2) */

unsigned char pauseFlag = 0;  /* 0: running, 1: paused */

/* Flags for increment and decrement operations */
unsigned char isr = 0;  /* Flag for Timer1 interrupt */
unsigned char incrementHoursFlag = 0;  /* Flag for incrementing hours */
unsigned char incrementMinutesFlag = 0;  /* Flag for incrementing minutes */
unsigned char incrementSecondsFlag = 0;  /* Flag for incrementing seconds */
unsigned char decrementHoursFlag = 0;  /* Flag for decrementing hours */
unsigned char decrementMinutesFlag = 0;  /* Flag for decrementing minutes */
unsigned char decrementSecondsFlag = 0;  /* Flag for decrementing seconds */

/* Count Mode Variable */
unsigned char countMode = 0;  /* 0: count up, 1: count down */
unsigned char toggleFlag = 0;  /* Flag for toggling count mode */

/* Timer1 initialization to generate interrupts every 1 second */
void timer1_Init()
{
    TCCR1A = (1 << FOC1A) | (1 << FOC1B);  /* Compare Mode */
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);  /* CTC mode, Prescaler 1024 */
    OCR1A = 15624;  /* Compare match value for 1-second intervals */
    TIMSK |= (1 << OCIE1A);  /* Enable Timer1 Compare Match A interrupt */
    SREG = (1 << 7);  /* Enable global interrupts */
}

/* ISR to handle Timer1 compare match (called every second) */
ISR(TIMER1_COMPA_vect)
{
    isr = 1;
}

/* Initialize INT0 for reset button (on PD2) */
void int0_Init()
{
    MCUCR = (1 << ISC01);  /* Falling edge on INT0 triggers interrupt */
    DDRD &= ~(1 << PD2);  /* Set PD2 as input */
    GICR = (1 << INT0);  /* Enable INT0 */
}

/* ISR to handle reset interrupt (INT0), resets the time to 00:00:00 */
ISR(INT0_vect)
{
    /* Reset all time variables to 0 */
    secondsUNITS = 0;
    secondsTENS = 0;
    minutesUNITS = 0;
    minutesTENS = 0;
    hoursUNITS = 0;
    hoursTENS = 0;
}

/* Initialize INT1 for pause button (on PD3) */
void int1_Init()
{
    MCUCR |= (1 << ISC11) | (1 << ISC10);  /* Rising edge on INT1 triggers interrupt */
    DDRD &= ~(1 << PD3);  /* Set PD3 as input */
    GICR |= (1 << INT1);  /* Enable INT1 */
}

/* ISR to handle pause interrupt (INT1), pauses the stopwatch */
ISR(INT1_vect)
{
    pauseFlag = 1;  /* Set pauseFlag to pause the stopwatch */
}

/* Initialize INT2 for resume button (on PB2) */
void int2_Init()
{
    MCUCSR &= ~(1 << ISC2);  /* Falling edge on INT2 triggers interrupt */
    DDRB &= ~(1 << PB2);  /* Set PB2 as input */
    GICR |= (1 << INT2);  /* Enable INT2 */
}

/* ISR to handle resume interrupt (INT2), resumes the stopwatch */
ISR(INT2_vect)
{
    pauseFlag = 0;  /* Clear pauseFlag to resume the stopwatch */
}

int main(void)
{
    /* Initialize external interrupts and timer */
    int0_Init();  /* Initialize INT0 (reset) */
    int1_Init();  /* Initialize INT1 (pause) */
    int2_Init();  /* Initialize INT2 (resume) */
    timer1_Init();  /* Initialize Timer1 for 1-second ticks */

    /* Set ports for display control */
    DDRA = 0xFF;  /* Set PortA as output for enabling 7-segment displays */
    DDRC = 0x0F;  /* Set lower 4 bits of PortC as output for BCD values (7-segment display) */
    DDRB &= ~0xFF;  /* Set PORTB as input for buttons */
    PORTB = 0xFF;   /* Enable pull-up resistors on PORTB */
    PORTC = 0xF0;   /* Initialize the first four pins of PortC to zero */
    DDRD |= (1 << PD4) | (1 << PD5);  /* Set PD4 and PD5 as output (indicators) */
    PORTD &= ~(1 << PD4) & ~(1 << PD5);  /* Clear PD4 and PD5 */

    unsigned char displaySwitch = 0;  /* Variable to switch between 6 7-segment displays */

    while (1)
    {
        if (isr == 1)
        {
            if (pauseFlag == 0)  /* Only run if not paused */
            {
                if (countMode == 0)  /* Count Up */
                {
                    DDRD |= (1 << PD0);
                    PORTD &= ~(1 << PD0);
                    if (++secondsUNITS == 10)
                    {
                        secondsUNITS = 0;
                        if (++secondsTENS == 6)
                        {
                            secondsTENS = 0;
                            if (++minutesUNITS == 10)
                            {
                                minutesUNITS = 0;
                                if (++minutesTENS == 6)
                                {
                                    minutesTENS = 0;
                                    /* Increment hours when minutes overflow */
                                    if (hoursTENS == 2 && ++hoursUNITS == 4)  /* If the time is 24:00:00 */
                                    {
                                        hoursTENS = 0;  /* Reset hours to 0 after 24 hours */
                                        hoursUNITS = 0;
                                    }
                                    else if (++hoursUNITS == 10)
                                    {
                                        hoursUNITS = 0;
                                        hoursTENS++;
                                    }
                                }
                            }
                        }
                    }
                }
                else  /* Count Down */
                {
                    if (secondsUNITS > 0)
                    {
                        secondsUNITS--;
                    }
                    else
                    {
                        if (secondsTENS > 0)
                        {
                            secondsTENS--;
                            secondsUNITS = 9;
                        }
                        else
                        {
                            if (minutesUNITS > 0 || minutesTENS > 0)
                            {
                                if (minutesUNITS > 0)
                                {
                                    minutesUNITS--;
                                }
                                else
                                {
                                    minutesTENS--;
                                    minutesUNITS = 9;
                                }
                                secondsTENS = 5;
                                secondsUNITS = 9;
                            }
                            else
                            {
                                if (hoursUNITS > 0 || hoursTENS > 0)
                                {
                                    if (hoursUNITS > 0)
                                    {
                                        hoursUNITS--;
                                    }
                                    else
                                    {
                                        hoursTENS--;
                                        hoursUNITS = 9;
                                    }
                                    minutesTENS = 5;
                                    minutesUNITS = 9;
                                    secondsTENS = 5;
                                    secondsUNITS = 9;
                                }
                                else
                                {
                                    DDRD |= (1 << PD0);  /* Trigger event when reaching 00:00:00 */
                                    PORTD |= (1 << PD0);  /* Set PD0 high (indicator or alarm) */
                                }
                            }
                        }
                    }
                }
            }
            isr = 0;
        }

        /* Loop through the 6-digit 7-segment display (hours, minutes, seconds) */
        for (displaySwitch = 0; displaySwitch < 6; displaySwitch++)
        {
            PORTA = (1 << displaySwitch);  /* Enable the appropriate 7-segment display */

            switch (displaySwitch)
            {
                case 5:
                    PORTC = (PORTC & 0xF0) | (secondsUNITS & 0x0F);  /* Display seconds units */
                    break;
                case 4:
                    PORTC = (PORTC & 0xF0) | (secondsTENS & 0x0F);  /* Display seconds tens */
                    break;
                case 3:
                    PORTC = (PORTC & 0xF0) | (minutesUNITS & 0x0F);  /* Display minutes units */
                    break;
                case 2:
                    PORTC = (PORTC & 0xF0) | (minutesTENS & 0x0F);  /* Display minutes tens */
                    break;
                case 1:
                    PORTC = (PORTC & 0xF0) | (hoursUNITS & 0x0F);  /* Display hours units */
                    break;
                case 0:
                    PORTC = (PORTC & 0xF0) | (hoursTENS & 0x0F);  /* Display hours tens */
                    break;
            }

            _delay_ms(4);  /* Short delay for persistence of vision */
        }

        /* Increment hours when button connected to PB1 is pressed */
        if (!(PINB & (1 << PB1)))
        {
            _delay_ms(30);  /* Debounce delay */
            if (!(PINB & (1 << PB1)))
            {
                if (incrementHoursFlag == 0)
                {
                    incrementHoursFlag = 1;
                    hoursUNITS++;
                    if (hoursUNITS == 10)
                    {
                        hoursUNITS = 0;
                        hoursTENS++;
                    }
                }
            }
        }
        else
        {
            incrementHoursFlag = 0;  /* Reset the increment flag when button is released */
        }

        /* Increment minutes when button connected to PB4 is pressed */
        if (!(PINB & (1 << PB4)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << PB4)))
            {
                if (incrementMinutesFlag == 0)
                {
                    incrementMinutesFlag = 1;
                    minutesUNITS++;
                    if (minutesUNITS == 10)
                    {
                        minutesUNITS = 0;
                        minutesTENS++;
                    }
                }
            }
        }
        else
        {
            incrementMinutesFlag = 0;  /* Reset the increment flag when button is released */
        }

        /* Increment seconds when button connected to PB6 is pressed */
        if (!(PINB & (1 << PB6)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << PB6)))
            {
                if (incrementSecondsFlag == 0)
                {
                    incrementSecondsFlag = 1;
                    secondsUNITS++;
                    if (secondsUNITS == 10)
                    {
                        secondsUNITS = 0;
                        secondsTENS++;
                    }
                }
            }
        }
        else
        {
            incrementSecondsFlag = 0;
        }

        /* (Second Decrement  PB5) */
        if (!(PINB & (1 << PB5)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << PB5)))
            {
                if (decrementSecondsFlag == 0)
                {
                    decrementSecondsFlag = 1;
                    if (secondsUNITS > 0)
                    {
                        secondsUNITS--;  /* Decrement seconds units */
                    }
                    else
                    {
                        if (secondsTENS > 0)
                        {
                            secondsTENS--;  /* Decrement seconds tens and reset seconds units to 9 */
                            secondsUNITS = 9;
                        }
                        else
                        {
                            /* Handle minute decrement if seconds overflow */
                            if (minutesUNITS > 0 || minutesTENS > 0)
                            {
                                if (minutesUNITS > 0)
                                {
                                    minutesUNITS--;
                                }
                                else
                                {
                                    minutesTENS--;
                                    minutesUNITS = 9;
                                }
                                secondsTENS = 5;  /* Reset seconds tens to 5 (since 60 seconds) */
                                secondsUNITS = 9;  /* Reset seconds units to 9 */
                            }
                        }
                    }
                }
            }
        }
        else
        {
            decrementSecondsFlag = 0;  /* Reset the decrement flag when button is released */
        }

        /* Minutes Decrement (connected to PB3) */
        if (!(PINB & (1 << PB3)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << 3)))
            {
                if (decrementMinutesFlag == 0)
                {
                    decrementMinutesFlag = 1;
                    if (minutesUNITS > 0)
                    {
                        minutesUNITS--;
                    }
                    else
                    {
                        if (minutesTENS > 0)
                        {
                            minutesTENS--;
                            minutesUNITS = 9;
                        }
                        else
                        {
                            if (hoursUNITS > 0 || hoursTENS > 0)
                            {
                                if (hoursUNITS > 0)
                                {
                                    hoursUNITS--;
                                }
                                else
                                {
                                    hoursTENS--;
                                    hoursUNITS = 9;
                                }
                                minutesTENS = 5;
                                minutesUNITS = 9;

                                secondsUNITS = 9;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            decrementMinutesFlag = 0;
        }

        /* Hours Decrement (connected to PB0) */
        if (!(PINB & (1 << PB0)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << 0)))
            {
                if (decrementHoursFlag == 0)
                {
                    decrementHoursFlag =  1;
                    if (hoursUNITS > 0)
                    {
                        hoursUNITS--;
                    }
                    else
                    {
                        if (hoursTENS > 0)
                        {
                            hoursTENS--;
                            hoursUNITS = 9;
                        }
                    }
                }
            }
        }
        else
        {
            decrementHoursFlag = 0;
        }

        /* Toggle mode between count up and count down (connected to PB7) */
        if (!(PINB & (1 << PB7)))
        {
            _delay_ms(30);
            if (!(PINB & (1 << 7)))
            {
                if (toggleFlag == 0)
                {
                    toggleFlag = 1;
                    countMode ^= (1 << 0);  /* Toggle the countMode variable (0: count up, 1: count down) */
                }
            }
        }
        else
        {
            toggleFlag = 0;
        }

        /* Indicate current mode (count up or count down) */
        if (countMode == 1)
        {
            PORTD |= (1 << PD5);  /* Set PD5 to indicate count down mode */
            PORTD &= ~(1 << PD4);  /* Clear PD4 */
        }
        else
        {
            PORTD |= (1 << PD4);  /* Set PD4 to indicate count up mode */
            PORTD &= ~(1 << PD5);  /* Clear PD5 */
        }
    }

    return 0;
}
