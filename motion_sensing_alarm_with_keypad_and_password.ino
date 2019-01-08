#include <Keypad.h>                // library for keyboard
#include <Password.h>              // library for password
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, Enable, R/W, RS, d4, d5, d6, d7, backlightpin, polarity

char passwd1[] = { '1', '2', '3', '1', '5', 0x0 };    // "Guest" password
Password password1 = Password( passwd1 );  // password
char passwd2[] = { '9', '2', '3', '9', '2', 0x0 };     // "Master" password
Password password2 = Password( passwd2 );  // password



int passwordTries=0;                      // How many "bad passwords" have been tried in a row
const byte rows = 4;                     // four rows       
const byte cols = 4;                     // three columns
char keys[rows][cols] = {                // keys on keypad

{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'},

};

byte rowPins[rows] = {6,7,8,9};
byte colPins[cols] = {2,3,4,5};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);


#define KEY_CHECK_PASSWORD  '*'   // Key to press after entering password to check if it's correct
#define KEY_RESET_ENTRY     '#'   // Key to press to "start over" / clear incorrect password, and try again

#define MAX_PASSWORD_ATTEMPTS 3

#define pirSensor     A3    // pin for PIR sensor data
#define siren         10    // pin for siren, buzzer
#define redLed        A2    // pin for red led
#define greenLed      A0    // pin for green led
#define yellowLed     A1    // pin for yellow led
int ledBlink;

int pirSensorData;
unsigned long now, tempTime;

int exitDelay = 6L;    // Delay (in seconds) after arming the system before enabling the PIR trigger (so-called "Exit delay")
int entryDelay = 2L; // Delay (in seconds) after motion-detected to alarm activated (so-called "Entry delay")
int sirenAutoResetDuration = 40L;   // Max. time (in seconds) siren will stay on before alarm auto-resets if no one enters the password 


int systemState = 0;      // system is 0 for off and 1 for on. Gets toggled whenever a valid password is entered

// Alarm states -
#define STATE_READY           0     // System powered up, not armed
#define STATE_EXIT_DELAY      1     // Password has been entered, exit-delay countdown
#define STATE_ARMED           2     // Fully armed! Ready to detect intruders, or be deactivated by password
#define STATE_MOTION_DETECTED 3 // PIR motion detection! Allow entry-delay until activating siren
#define STATE_ALARM           4 // Siren on!!


// Current alarm state
char alarmState = STATE_READY;


void setup()
  {
  keypad.addEventListener(keypadEvent); // an object is created for tracking keystrokes
  
  Serial.begin(9600);  //Used for troubleshooting
  lcd.clear();
  lcd.begin(16,2);
  lcd.backlight();
  pinMode(siren, OUTPUT);
  pinMode(pirSensor, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  Serial.println("System startup"); //Used for troubleshooting
  lcd.clear();
  lcd.print("System startup");
  Serial.println("Alarm button status:"); //used for troubleshooting
  passwordTries=0;
  }

void loop()
{
     
  now = millis();    // read the internal clock
  
  keypad.getKey();

    
    if (systemState%2 == 0)
    {
    // System is disabled
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);

    digitalWrite(siren, LOW);
    alarmState = 0;
    Serial.println("System disabled!"); // Used for troubleshooting
    lcd.clear();
    lcd.print("System disabled");
    passwordTries=0;

    }

  else
    {
    // System is enabled
    
    if(alarmState == STATE_READY) 
     {
     alarmState = STATE_EXIT_DELAY;
     tempTime = now;
     digitalWrite(greenLed, HIGH);
     digitalWrite(siren, LOW);
     }

    if(alarmState == STATE_EXIT_DELAY)              // system armed, counting down the "exit delay"
      {

      if ((now%1000L)<500L)
      {
        digitalWrite(yellowLed, HIGH);
      }
      else
      {
        digitalWrite(yellowLed, LOW);
      }
      keypad.getKey();
      Serial.println("System is arming! Counting down exit delay..."); // Used for troubleshooting
      lcd.clear();
      lcd.print("Exit delay...");
      if(now >= tempTime + exitDelay * 1000L) {alarmState = STATE_ARMED;}
      }
      
    if(alarmState == STATE_ARMED)              // system is armed
      {
      digitalWrite(yellowLed, HIGH);
      keypad.getKey();
      digitalWrite(greenLed, LOW);
      digitalWrite(siren, LOW);
   
      Serial.println("System armed!"); // Used for Troubleshooting
      lcd.clear();
      lcd.print("Armed!");

      pirSensorData = digitalRead(pirSensor);  
      Serial.print("pirSensordData = "); //Used for troubleshooting
      Serial.println(pirSensorData); //Used for troubleshooting
   
  //    if(pirSensorData > 600) {alarmState = 3; tempTime = now;}
     if(pirSensorData == HIGH)
       {
       alarmState = STATE_MOTION_DETECTED;
       tempTime = now;
       digitalWrite(yellowLed, LOW);
       Serial.println("Motion detected!"); // Used for troubleshooting
       lcd.clear();
       lcd.print("Motion detected!");
       }

       // If password has been tried too many times, ALARM INSTANTLY!
       if (passwordTries >= MAX_PASSWORD_ATTEMPTS)
       {
        alarmState = STATE_ALARM; tempTime = now;
        Serial.println("Too many bad-password attempts!"); // Used for troubleshooting
        lcd.clear();
        lcd.print("Hacker detected!");
       }

      }

    if(alarmState == STATE_MOTION_DETECTED)              // system is triggered, counting down to sound alarm ("entry delay")
      {             

      if ((now%500L)<100L) digitalWrite(redLed, HIGH);
      else digitalWrite(redLed, LOW);
      digitalWrite(siren, LOW);
      digitalWrite(greenLed, LOW);
      Serial.println("Motion detected! Counting down entry delay..."); //Used for troubleshooting
      lcd.clear();
      lcd.print("Entry delay...");
      keypad.getKey();
      if(now >= tempTime + entryDelay * 1000L) {alarmState = STATE_ALARM; tempTime = now;}
      
      }

    if(alarmState == STATE_ALARM)              // siren (buzzer) is active
      {
      digitalWrite(siren, HIGH);
      digitalWrite(redLed, HIGH);
      Serial.println("Siren is active !"); //Used for troubleshooting
      lcd.clear();
      lcd.print("ALARM!! ALARM!!");
      


// For siren

    //tone( 10, 10000, 100);  // Simple Alarm Tone
    for(double x = 0; x < 0.92; x += 0.01){  // Elegant Alarm Tone
        tone(10, sinh(x+8.294), 10);
        delay(1);
        }   
    
      keypad.getKey();      
      if(now >= tempTime + sirenAutoResetDuration * 1000L) {alarmState = STATE_ARMED; digitalWrite(siren, LOW); digitalWrite(redLed, LOW);}
      }
    }
}


  //take care of some special events
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
    case PRESSED:
  Serial.print("Pressed: ");
  Serial.println(eKey);
  lcd.clear();
  lcd.print(eKey );
  switch (eKey){
    case KEY_CHECK_PASSWORD: checkPassword(); break;
    case KEY_RESET_ENTRY: password1.reset(); break;
    default: password1.append(eKey); password2.append(eKey);
     }
  }
}

  
  void checkPassword(){
  if (password1.evaluate() || password2.evaluate()){
    Serial.println("Successful password entry"); //Used for troubleshooting
    lcd.clear();
    lcd.print("Password correct!");
    systemState++;
    passwordTries=0;
  }else{
    Serial.println("Incorrect password!"); //Used for troubleshooting
    lcd.clear();
    lcd.print("Wrong password!");
    passwordTries++;

    //add code to run if it did not work
    ledBlink = 0;
    while (ledBlink <= 5){
      digitalWrite(redLed, HIGH);
      delay(100);
      digitalWrite(redLed, LOW);
      delay(100);
      ledBlink++;
    }
  }
  password1.reset();
  password2.reset();
}
