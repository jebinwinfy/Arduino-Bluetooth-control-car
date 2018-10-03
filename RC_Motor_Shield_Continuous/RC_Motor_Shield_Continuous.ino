/*
  Front Motor (Steering) => Channel A
  Back Motor => Channel B
  
  Since the motor shield hijacks 6 pins for the motors'
  control, they are declared in the MotorShieldR3 library.
*/
#include <Servo.h>
#include <MotorShieldR3.h>

//MotorShieldR3 yellowCar;
Servo myservo;

#define pinHeadLight    7    //Pin that activates the Front lights.
#define pinHeadLight2     4    //Pin that activates the Back lights.  

#define pinHorn  12 //Pin that activates the horn.
#define pinInteriorLight  10  //Pin that activates the interior lights.
#define pinIndicatorLeft  6 //Pin that activates the Indicator left lights.
#define pinIndicatorRight  9  //Pin that activates the Indicator right lights.

#define maxDegree 135 // Max degree to set servo
#define minDegree 45 // Min dree to set servo
#define pinServo 5

#define pinMotorControl1 13 // L293D control line 1
#define pinMotorControl2 8  // L293D control line 2
#define pinMotorSpeedControl 11 // L293D speed control by using 'Enable' pin
 
char command = 'S';
char prevCommand = 'A';
int velocity = 100;   
unsigned long timer0 = 2000;  //Stores the time (in millis since execution started) 
unsigned long timer1 = 0;  //Stores the time when the last command was received from the phone

int degree = 90;
int isDegreeSet = 0;
int hazardLightCounter = 0;

bool isHazardLightOn = false;
bool isHazardLightGlow = false;

bool isIndicatorOn = false;
bool isLeftIndicatorOn = false;
bool isRightIndicatorOn = false;

bool isFogLightOn = false;

bool isHornOn = false;

bool isAutoModeOn = false;

void setup() 
{       
  Serial.begin(9600);  //Set the baud rate to that of your Bluetooth module.
  pinMode(pinHeadLight , OUTPUT);
  pinMode(pinHeadLight2 , OUTPUT);

  pinMode(pinMotorControl1, OUTPUT);
  pinMode(pinMotorControl2, OUTPUT);
  pinMode(pinMotorSpeedControl, OUTPUT);
  
  myservo.attach(pinServo);  // attaches the servo on pin 5 to the servo object(Steering motor!)
}

void loop(){

  if(isDegreeSet == 0) {
    //Set degree of servomotor
    myservo.write(90);  
    delay(500);
    isDegreeSet = 1;
  } else {
    if(Serial.available() > 0){ 
      timer1 = millis();   
      prevCommand = command;
      command = Serial.read(); 
      //Change pin mode only if new command is different from previous.   
//      if(command!=prevCommand){
        //Serial.println(command);
        switch(command){
        case 'F':  
          moveForward();
          if(isAutoModeOn) {
            turnToResetAngle();
          }
          break;
        case 'B':  
           moveBackword();
           if(isAutoModeOn) {
            turnToResetAngle();
          }
          break;
        case 'L':  
          turnLeft();
          stopMovement();
          break;
        case 'R':
          turnRight();
          stopMovement();
          break;
        case 'S':  
          stopMovement();
          break; 
        case 'I':  //FR  
          turnRight();
          moveForward();
          break; 
        case 'J':  //BR  
          turnRight();
          moveBackword();
          break;        
        case 'G':  //FL  
          turnLeft();
          moveForward();
          break; 
        case 'H':  //BL
          turnLeft();
          moveBackword();
          break;
        case 'W':  //Head Light ON 
          digitalWrite(pinHeadLight, HIGH);
          break;
        case 'w':  //Head Light OFF
          digitalWrite(pinHeadLight, LOW);
          break;
        case 'U':  //Head Light2 ON 
          digitalWrite(pinHeadLight2, HIGH);
          break;
        case 'u':  //Head Light2 OFF 
          digitalWrite(pinHeadLight2, LOW);
          break; 
        case 'A': //Interior light On
          digitalWrite(pinInteriorLight, HIGH);
          break;
        case 'a': //Interior light Off
          digitalWrite(pinInteriorLight, LOW);
          break;
        case 'X': //Hazard light On
          isHazardLightOn = true;
          break;
        case 'x': //Hazard light Off
          isHazardLightOn = false;
          break;
        case 'M': //Indicator Left On
          isIndicatorOn = true;
          isLeftIndicatorOn = true;
          break;
        case 'N': //Indicator Right On
          isIndicatorOn = true;
          isRightIndicatorOn = true;
          break;
        case 'p': //Indicator both Off
          isIndicatorOn = false;
          isLeftIndicatorOn = false;
          isRightIndicatorOn = false;
          break;
        case 'Z': //Fog light On
          isFogLightOn = true;
          break;
        case 'z': //Fog light Off
          isFogLightOn = false;
          break;
        case 'V': //Horn On
          isHornOn = true;
          break;
        case 'v': //Horn Off
          isHornOn = false;
          break;
        case 'T': //Auto mode On
          isAutoModeOn = true;
          break;
        case 't': //Auto mode off
          isAutoModeOn = false;
          break;
        case 'D':  //Everything OFF 
          digitalWrite(pinHeadLight, LOW);
          digitalWrite(pinHeadLight2, LOW);
          stopMovement();
          break;         
        default:  //Get velocity
          if(command=='q'){
            velocity = 255;  //Full velocity
          }
          else{ 
            //Chars '0' - '9' have an integer equivalence of 48 - 57, accordingly.
            if((command >= 48) && (command <= 57)){ 
              //Subtracting 48 changes the range from 48-57 to 0-9.
              //Multiplying by 25 changes the range from 0-9 to 0-225.
              velocity = (command - 48)*25;       
//              yellowCar.SetSpeed_4W(velocity);
            } else {
              stopMovement();
            }
          }
        }

        //---------------Hazard Light/Fog light/Indicator On/Off control----------------
        if(isHazardLightOn) {
          if(hazardLightCounter > 1000) {
            hazardLightCounter = 0;
            
            if(isHazardLightGlow) {
              digitalWrite(pinIndicatorLeft, LOW);
              digitalWrite(pinIndicatorRight, LOW);
            } else {
              digitalWrite(pinIndicatorLeft, HIGH);
              digitalWrite(pinIndicatorRight, HIGH);
            }
            isHazardLightGlow = !isHazardLightGlow;
          }
          hazardLightCounter = hazardLightCounter + 10;
        } else {

          if(isFogLightOn) {
            digitalWrite(pinIndicatorLeft, HIGH);
            digitalWrite(pinIndicatorRight, HIGH);
          } else {
            if(isIndicatorOn) {
              if(isLeftIndicatorOn) {
                //Left indicator on

                if(hazardLightCounter > 1000) {
                  hazardLightCounter = 0;
            
                  if(isHazardLightGlow) {
                    digitalWrite(pinIndicatorLeft, LOW);
                    digitalWrite(pinIndicatorRight, LOW);
                  } else {
                    digitalWrite(pinIndicatorLeft, HIGH);
                    digitalWrite(pinIndicatorRight, LOW);
                  }
                  isHazardLightGlow = !isHazardLightGlow;
                }
                hazardLightCounter = hazardLightCounter + 10;
                
              } else {
                //Right indicator on
                if(hazardLightCounter > 1000) {
                  hazardLightCounter = 0;
            
                  if(isHazardLightGlow) {
                    digitalWrite(pinIndicatorLeft, LOW);
                    digitalWrite(pinIndicatorRight, LOW);
                  } else {
                    digitalWrite(pinIndicatorLeft, HIGH);
                    digitalWrite(pinIndicatorRight, LOW);
                  }
                  isHazardLightGlow = !isHazardLightGlow;
                }
                hazardLightCounter = hazardLightCounter + 10;
              }
            } else {
              digitalWrite(pinIndicatorLeft, LOW);
              digitalWrite(pinIndicatorRight, LOW);
              hazardLightCounter = 0;
            }
          }
          
        }

        //--------------------Horn control------------------
        if(isHornOn) {
          tone(pinHorn, 150);
        } else {
          noTone(pinHorn);
        }

        
      }
//    }
//    else{
//      timer0 = millis();  //Get the current time (millis since execution started).
//      //Check if it has been 500ms since we received last command.
//      if((timer0 - timer1)>500){  
//        //More tan 500ms have passed since last command received, car is out of range.
//        //Therefore stop the car and turn lights off.
//        digitalWrite(pinHeadLight, LOW);
//        digitalWrite(pinHeadLight2, LOW);
//        yellowCar.Stopped_4W();
//      }
//    }
  }

  delay(10);
}

// Move the back motor to forward direction
void moveForward() {
  analogWrite(pinMotorSpeedControl,velocity);
  digitalWrite(pinMotorControl1,HIGH);
  digitalWrite(pinMotorControl2,LOW);
} 

// Move the back motor to backward direction
void moveBackword() {
  analogWrite(pinMotorSpeedControl,velocity);  
  digitalWrite(pinMotorControl1,LOW);
  digitalWrite(pinMotorControl2,HIGH);
}

// Stop movement of back motor
void stopMovement() {
  digitalWrite(pinMotorControl1,LOW);
  digitalWrite(pinMotorControl2,LOW);
  analogWrite(pinMotorSpeedControl,velocity);
}

// Turn streening to left
void turnLeft() {
  if(degree < maxDegree) {
    myservo.write(degree);
    degree = degree + 2;
//    delay(15);
  }
}

// Trurn steering to right
void turnRight() {
  if(degree > minDegree) {
    myservo.write(degree);
    degree = degree - 2;
//    delay(15);
  }
}

// Turn steering to 90 degree (reset) if auto mode is on!
void turnToResetAngle() {
  if(degree == 90){
  
  } else if(degree < 90) {
    while(degree < 90) {
      myservo.write(degree);
      degree = degree + 1;
      delay(20);
    }
  } else if(degree > 90) {
    while(degree > 90) {
      myservo.write(degree);
      degree = degree - 1;
      delay(20);
    }
  }
}

/**

========= DATA SENDING FROM BLUETOOTH DEVICE ==========

Forward Left    - G ->  
Forward Right   - I ->
Backward Left   - H ->
Backward Right  - J ->
Forward         - F ->
Backward        - B ->
Left            - L ->
Right           - R ->

Stop            - S ->

Head Light On       - W ->  
Head Light Off      - w ->  

Head Light2 On      - U ->  
Head Light2 Off     - u ->

Hazard Light On     - X ->
Hazard Light Off    - x ->

Fog Light On        - Z ->
Fog Light Off       - z ->

Interior Light On   - A ->
Interior Light Off  - a ->

Park Light On       - C . // not in scope
Park Light Off      - c . // not in scope

Horn On             - V ->
Horn Off            - v ->

Indicator Left On   - M ->
Indicator Right On  - N ->
Indicator Off       - p ->

Automode On         - T ->
Automode Off        - t ->

*/

/*
====== PIN CONFIG =======
Motor control1  :-> 13
Motor control2  :-> 8
Motor speed     :-> 11

Servo motor     :-> 5

Head light      :-> 7
Head light2     :-> 4

Indicator Left  :-> 6
Indicator Right :-> 9

Interior light  :-> 10

Horn            :-> 12


*/
