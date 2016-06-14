#include <LiquidCrystal.h>


#define M1 A0
#define P1 6
#define STATUS 13
#define SOUND 7
#define DISPLAYBTN 8
#define DISPLAYSWITCH 9
#define DISPLAYVCC 10

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int messwert = 0;
int last_pour = 0;
int loop_num = 0;
boolean first_run = false;
int displaybtn_status = 0;
boolean display_status = false;

int num_pour = 0;

//Loop Intervals
long measure_previous = 0;
long measure_interval = 60;
long pump_previous = 0;
long pump_interval = 15 * 60;
long display_previous = 0;
long display_interval = 8;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(STATUS, OUTPUT);
  pinMode(P1, OUTPUT);
  pinMode(SOUND, OUTPUT);
  pinMode(DISPLAYBTN, INPUT);
  pinMode(DISPLAYSWITCH, OUTPUT);
  pinMode(DISPLAYVCC, OUTPUT);

  pump_previous = millis() / 1000;
}

void loop() {
    //loop_num will count +1 every 10 seconds at the end of the loop!

    unsigned long currentMillis = millis() / 1000;


    displaybtn_status=digitalRead(DISPLAYBTN);
    if (displaybtn_status == HIGH && !display_status) {
        //Switch Display on!
        display_status = true;
        display_previous = currentMillis;
        digitalWrite(DISPLAYVCC, HIGH);
        digitalWrite(DISPLAYSWITCH, HIGH);
        lcd.begin(16, 2);
        Serial.println("Button pressed");
        
        
        delay(1000);
        lcd.setCursor(0, 0);
        
        lcd.print("Humidity: " + String(messwert));
        lcd.setCursor(0, 1);
        String last_display = "";
        String unit = "min";
        last_pour = (currentMillis - pump_previous) / 60;
        Serial.println("Time: " + String(last_pour));
        if(last_pour > 120) {
          last_display = String(last_pour / 60) + "." + String(last_pour / 6 - (int(last_pour / 60) * 10));
          unit = "h";
        } else {
          last_display = String(last_pour);
        }
        lcd.print("Last: " + last_display + unit);
    }

    if(currentMillis - display_previous > display_interval) {
        digitalWrite(DISPLAYVCC, LOW);
        digitalWrite(DISPLAYSWITCH, LOW);
        display_status = false;
    }

    
    if(currentMillis - measure_previous > measure_interval) {
        measure_previous = currentMillis;
         // Measure with Status on measure
        digitalWrite(STATUS, HIGH);
        messwert=analogRead(M1);
        Serial.print("Feuchtigkeits-Messwert:");
        Serial.println(messwert);
        delay(1000);
        digitalWrite(STATUS, LOW);
    
       
        //Serial.println("Last: " + last_display + unit);
    

        //Pump if nessecary
        if((!first_run || currentMillis - pump_previous > pump_interval) && messwert <= 500 && num_pour <= 8) {
              pump_previous = currentMillis;
              first_run = true;
              num_pour++;
              
              //Alarm befor pump
              digitalWrite(SOUND, HIGH);
              delay(1000);
              digitalWrite(SOUND, LOW);
        
              //Set to last = 0 minutes
              //lcd.setCursor(0, 1);
              //lcd.print("Last: 0min");
              
              //Pump
              digitalWrite(P1, HIGH);
              delay(30000);
              digitalWrite(P1, LOW);
        }
      
    }


}
