
# Stopwatch with Dual Mode (Increment and Countdown) Using ATmega32 and Seven-Segment Display.

![Screenshot 2024-09-16 021000](https://github.com/user-attachments/assets/fbeee9a2-ece6-4823-9811-812835d724a0)

# Project Overview: 
This project aims to design and implement a digital stopwatch using the ATmega32 microcontroller and six multiplexed seven-segment displays (common anode). The stopwatch offers two operational modes:

1. Increment Mode (Default): The stopwatch counts upwards, displaying hours, minutes, and seconds.
2. Countdown Mode: The stopwatch counts downwards from a user-defined time, functioning as a countdown timer.
   
The system includes features like reset, pause, resume, mode toggle, and time adjustments via push buttons. LED indicators and a buzzer provide visual and auditory alerts.

# Features: 
• Reset Function: Resets the time to zero in both modes.

• Pause Function: Pauses the counting, allowing the stopwatch to hold its current value.

• Resume Function: Resumes counting from the paused time.

• Mode Toggle: Switches between increment mode and countdown mode.

• Time Adjustment: Allows the user to adjust the hours, minutes, and seconds when in countdown mode.

• Alarm Function: Triggers a buzzer when the countdown reaches zero.

• LED Indicators: Indicates the current mode of the stopwatch:

/   1. Red LED for increment mode.
 /  2. Green LED for countdown mode.

# Hardware Components: 
1. Microcontroller: ATmega32
   
2. Display: Six multiplexed seven-segment displays (common anode), driven by a 7447 BCD to seven-segment decoder connected to PORTC.
   
3. Push Buttons:
   
  • Reset: Connected to PD2 (INT0) with an internal pull-up resistor.
  
  • Pause: Connected to PD3 (INT1) with an external pull-up resistor.

  • Resume: Connected to PB2 (INT2) with an internal pull-up resistor.
  
  • Mode Toggle: Connected to PB7.
  
  • Hour Adjustment:
      • Increment Hours: Connected to PB1 with an internal pull-up resistor.
      • Decrement Hours: Connected to PB0 with an internal pull-up resistor.
      
  • Minute Adjustment:
      • Increment Minutes: Connected to PB4 with an internal pull-up resistor.
      • Decrement Minutes: Connected to PB3 with an internal pull-up resistor.
      
  • Second Adjustment:
      • Increment Seconds: Connected to PB6 with an internal pull-up resistor. 
      • Decrement Seconds: Connected to PB5 with an internal pull-up resistor
      
4. Alarm/Buzzer: Connected to PD0, activated when the countdown reaches zero.

5. LED Indicators:

  • Red LED for increment mode (connected to PD4).

  • Yellow LED for countdown mode (connected to PD5).

# Operation Description: 
• Increment Mode: By default, the stopwatch starts in increment mode, where the time counts up continuously from zero. The time is displayed across six seven segment displays, showing hours, minutes, and seconds. The red LED on PD4 is turned on to indicate increment mode. 

• Countdown Mode: When the mode toggle button is pressed, the stopwatch switches to countdown mode. In this mode, the user can set the desired countdown time using the hour, minute, and second adjustment buttons. The countdown begins once the start/resume button is pressed. A yellow LED on PD5 is turned on to indicate countdown mode. An alarm is triggered when the countdown reaches zero.
