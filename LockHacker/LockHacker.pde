/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
 The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 6
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int i, toggle, myRand;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);  

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("HACKING CODE...");

  i = 0;
  toggle = 0;
  myRand = 343717;
}

void loop() {
  if (!digitalRead(8))
  {
    toggle = !toggle;
    delay(150);
  }

  if (!toggle)
  {
    lcd.setCursor(0, 0);
    lcd.print("HACKING CODE... ");
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis()*myRand);

    i++;

    if (i == 3)
    {
      digitalWrite(13, HIGH);
    }
    else if (i == 6)
    {
      digitalWrite(13, LOW);
      i = 0;
    }
    digitalWrite(12, LOW);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("DOOR UNLOCKED!!!");
    digitalWrite(13, LOW);
    digitalWrite(12, HIGH);
  }

  delay(100);
}

