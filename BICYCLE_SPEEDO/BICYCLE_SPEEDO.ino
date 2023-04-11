/* 
 * This Bicycle Speedo is using just plain states and could be used on any controller
 * It's your choice on how you see each rotation
 * You can use a IR sensor or a Hall effect sensor.
 * Just be mindful of what state you are watching for.
 * This IR sensor I tested with was Active "LOW" and the sketch reflects that.
 * 
 * Any LCD Display will work just setup the constructor accordingly
 * This sketch uses an I2C 2004 LCD
 * Be mindful that there are multiple LCD I2C libraries and they have different methods
 * 
 * Features in this sketch
 * Trip Clock in HH:MM:SS format. When you stop the clock stops
 * Total trip distance in kilometers
 * Current speed in KPH
 * The refresh rate is one second
 * The sketch is non-blocking
 * 
 * The minimum time for one wheel revolution must be less than 3 seconds or just above walking pace
 * 
 */

#define LCD_2004                                    // Comment out if using the 1604 LCD Display

#include "LiquidCrystal_I2C.h"                      // There is more than one library out there for
                                                    // I2C LCD and they use different methods so check your's
#define ONE_SEC 1000                                // One Second in milliseconds
#define LED     13                                  // LED PIN
#define STATE   2                                   // IR or Hall Effect sensor pin
#define ONE_REV (double)0.00223                     // 29" Wheel Circumfrence as a decimal of one kilometer (223cm)

#if defined LCD_2004
    LiquidCrystal_I2C lcd(0x27, 20, 4);             //LCD Object for 2004
#else
    LiquidCrystal_I2C lcd(0x27, 16, 2);             //LCD Object for 1602
#endif

uint32_t  prevTime = 0, lastTime = 0, tripTimer = 0;
double    rpm = 0;
uint16_t  lastTrip = 0, trip = 0;
uint8_t   counter = 0;
bool      timerStop = true, ledState, lastLedState = 0, 
          state, lastState = 1; 

String getTripTime(){                               // This turns our millisecond trip clock
   String t;                                        // Into human readable time
   uint16_t s = tripTimer / 1000;
   uint16_t m  = s / 60;
   s %= 60;
   uint16_t h = m / 60;
   m %= 60;
   char buf[9];
   sprintf(buf,"%02u:%02u:%02u", h, m, s);
   t += buf;
   return t;
}
#if defined LCD_2004                                    // LCD 2004
    void updateLCD(void){                               // Print RPM every second   
        String tripTime = getTripTime();                // Update trip clock
        lcd.setCursor(7, 1);
        lcd.print(tripTime);
        lcd.setCursor(7, 2);
        double thisTrip = ((double)trip * ONE_REV);
        lcd.print(thisTrip, 1);
        lcd.print(" ");       
        lcd.setCursor(7, 3);
        double speed = ((rpm * ONE_REV) * 60);     
        lcd.print(speed, 1);
        lcd.print(" ");
        return;
    }
#else                                                   // LCD 1602
    void updateLCD(void){                               // Print RPM every second   
        lcd.setCursor(3, 0);
        double speed = ((rpm * ONE_REV) * 60);
        lcd.print(speed, 1);
        lcd.print(" ");
        lcd.setCursor(12, 0);
        double thisTrip = ((double)trip * ONE_REV);
        lcd.print(thisTrip, 1);
        lcd.print(" ");
        String tripTime = getTripTime();                // Update trip clock
        lcd.setCursor(6, 1);
        lcd.print(tripTime);
        return;
    }
#endif


void setup(void) {
    pinMode(LED, OUTPUT);
    pinMode(STATE, INPUT);
    lcd.begin(); // initialize the lcd
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MY BIKE SPEEDO");
    delay(3000);
    #if defined LCD_2004
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TRIP  CURRENT  DATA");
        lcd.setCursor(0, 1);
        lcd.print("Time");
        lcd.setCursor(0, 2);
        lcd.print("Dist");
        lcd.setCursor(13, 2);
        lcd.print("klm");
        lcd.setCursor(0, 3);
        lcd.print("Speed");
        lcd.setCursor(13, 3);
        lcd.print("kph");
    #else
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("S:");
        lcd.setCursor(9, 0);
        lcd.print("D:");
        lcd.setCursor(0, 1);
        lcd.print("Time");
    #endif
}

void loop(void){
    state = digitalRead(STATE);
    if(state != lastState){
       uint32_t currentTime = millis();
            if(lastTime > 0){
                if(state == LOW){                 //Active LOW state for IR Sensor Trigger
                    trip++;
                    ledState = 0;
                    rpm = (60000 / (currentTime - lastTime));
                }else{
                    ledState = 1;
                }
            }
        lastTime = currentTime;
    }
    if ((millis() - prevTime) >= ONE_SEC) {
        counter++;
            if(counter > 2){
              counter = 0; 
                if(trip == lastTrip){
                    timerStop = true;
                    lastTime = 0;
                    rpm = 0;
                }else{
                    timerStop = false;
                }
            }
            if(timerStop == false){
                tripTimer += (millis() - prevTime);
            }
            updateLCD();
            lastTrip = trip;
            prevTime = millis();
    }
    if(ledState != lastLedState){
        digitalWrite(LED, ledState);
    }
    lastLedState = ledState;
    lastState = state;
}
