#include <Servo.h>

////////////////////////////////////
const byte numChars = 17;         //number of characters from incoming string msg from ESP32
char recievedChars[numChars];     //char array that will save the invoming string from ESP32

int index = 0;                    //to count the number of times messages are sent

boolean newData = false;          //switches when string is transfered from ESP32

int l(0), w(0), holes(0), seeds(0), rows(0);    //individual variables that will be saved from user input
///////////////////////////////////
//define the two direction logic pins and the speed / PWM pin
const int DIR_A = 5;
const int DIR_B = 4;
const int DIR_C = 13;
const int DIR_D = 12;
const int PWM = 6;
const int PWM2 = 11;
boolean test=true;
///////////////////////////////////

Servo seedServo;                  //initialize servo motor

void setup() {
  seedServo.attach(23);           //set output pin for servo
  seedServo.write(175);           //initialize the servo position
  
  pinMode(DIR_A, OUTPUT);         //outputs for every dc motor
  pinMode(DIR_B, OUTPUT);
  pinMode(DIR_C, OUTPUT);
  pinMode(DIR_D, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(PWM2, OUTPUT);
  
  Serial.begin(115200);           //initialize serial monitor
  Serial.println("<Arduino is ready>");   //for debbuging, indicates the arduino is ready to recieve data
}

void loop() {
  
  recvWithEndMarker();            //to recieve the data from ESP32
  showNewData();                  //to display the data recieved as a string
  if (index==3) {                 //when ready to initialize
    //seedDropper(seeds);         //for debugging
    movement(holes, l, w, rows);  //function that launches the rover with the given parameters 
    index=0;
  }
  
}

void recvWithEndMarker() {        //function that recieves the string from ESP32 over Rx pin 0
  static byte ndx = 0;            //initializing variables needed
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {    //loop runs only when there is serial information incoming
    rc = Serial.read();                                   //temporarily stores the data

    if(rc != endMarker) {         //if else statement reads the temp variable and stores it into a character array if it is of any value
      recievedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      recievedChars[ndx] = "\0";
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {          //function that outputs the recieved data
  if (newData == true) {      //runs if there is new recieved data
    Serial.println(recievedChars);  //for debbuging, prints out the string
    newData = false;          //resets newData variable for reciWithEndMarker function to reset
    index++;                  //adds 1 to the count of recieved strings
    convertToArr(index);      //runs the function to convert the character array into individual parameters variables
  }
}

int convertToArr (int index) {    //function that converts the char array to individual variables
  int temp[3] = {0,0,0};          //creates a temporary array to store each place of the number being analyzed (1, 10, 100)
  int instructions[6];            //final array containing the instruction as values and not characters
  int value = 0;                  //used as a selector for the instructions array
  int placeholder=0;              //used as a selector to look at the temp array
  //Serial.println(index);

  
  if(index > 2) {

    for(int i=0; i<sizeof(recievedChars); i++) {                  //loop runs for each value in the char array
      if(recievedChars[i] == ',' || recievedChars[i] == NULL) {   //if the character is a comma, or blank, save the values in temp array as a single number
        //instructions[value] = (temp[2]*pow(10,0))+(temp[1]*pow(10,1))+(temp[0]*pow(10,2));
        
        switch (placeholder) {            //this statement calculates the value of the number
          case 1:
            instructions[value] = (temp[0]*pow(10,0));    //if there is only 1 value recorded, then the variable has that value
            break;
          case 2:
            instructions[value] = (temp[0]*pow(10,1)) + (temp[1]*pow(10,0));    //if there are 2 values recorded, then the first is multiplied by 10 and added to the second
            break;
          case 3:
            instructions[value] = (temp[2]*pow(10,0))+(temp[1]*pow(10,1))+(temp[0]*pow(10,2));    //if there are 3 values, then the first is mulitplied by 100, the second is multiplied by 10, 
            break;                                                                                //and all values are summed
        }
 
        placeholder = 0;      //reset the placeholder for the next value
        value++;              //move to the next spot in instructions array
        for (int j=0;j<6;j++) {   //reset temp array
          temp[j]=0;
        }
        delay(50);            //delay before looping
      }
      else {
        temp[placeholder] = recievedChars[i] - '0';   //this records the current char as a number in temp array
        if(temp[placeholder]<0){                      //for debugging
          temp[placeholder]=0;
        }
        //Serial.println(temp[placeholder]);
        placeholder++;                                //move to next value in temp array
        delay(50);
      }
    }
  }

  l = instructions[0];        //to record each value in instructions array as individual variables 
  w = instructions[1];        //this will be handy when passing variables to functions
  holes = instructions[2];
  seeds = instructions[3];
  rows = instructions[4];
  /*Serial.println(l);
  Serial.println(w);
  Serial.println(holes);
  Serial.println(numSeeds);
  Serial.println(rows);*/
  
}

////////////////////////////////////////////////////////////////////////

void seedDropper (int numSeeds) {           //function that drops the seeds

  const int Degrotation[5] = {160, 120, 85, 50, 15};   //these are the degrees corresponding to the holes on the wheel 175, 120, 85, 50, 0
  int servoPosition;                    //initialize servo position
  int numSeedsInterval;                 //to calculate how many turns if the number of seeds is higher than the max possible in one rotation
  int place = 0;                        //counter
  //Serial.println(numSeeds);             //for debbuging
  
  place = numSeeds;                     //set counter equal to the user given number of seeds to drop per hole
  
  if(numSeeds>4) {                      //if the number is over the max possible rotations
    numSeedsInterval = 4;               //set interval to 4 (max value)
  }
  else { 
    numSeedsInterval = numSeeds;        //otherwise, set it equal to the number of seeds
  }

  while (place>=0) {                    //as long as place is over 0
    
    for (int i=0; i<numSeedsInterval; i++) {    //for as many seeds as the value of the interval
      
      for (servoPosition=Degrotation[i]; servoPosition>=Degrotation[i+1]; servoPosition -= 1) {   //function that slows the spin of the wheel to the next value on the degrotation array
        seedServo.write(servoPosition);
        delay(70);
      }      
      place -= 1;                   //count down one value of place
      if (place<=0) {               //if place is less than or equal to 0, break out of the for loop
        break;  
      }
      
    }
    
    seedServo.write(175);         //reset to the initial position of the servo
    if (place<=0) {               //for redundancy 
      break;
    }

  }
  
}
//////////////////////////////////////////
void forward(int speedX){         //function that moves the rover forward
  //for(int i=0;i<10;i++){
  digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,LOW);
  digitalWrite(DIR_C,HIGH);
  digitalWrite(DIR_D,LOW);
  analogWrite(PWM,speedX);
  analogWrite(PWM2,speedX);
}

void turnRight(int speedX){       //function that turns the rover right
  digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,LOW);
  digitalWrite(DIR_C,LOW);
  digitalWrite(DIR_D,HIGH);
  analogWrite(PWM,speedX);
  analogWrite(PWM2,speedX);
}

void turnLeft(int speedX){      //function that turns the rover left
  digitalWrite(DIR_A,LOW);
  digitalWrite(DIR_B,HIGH);
  digitalWrite(DIR_C,HIGH);
  digitalWrite(DIR_D,LOW);
  analogWrite(PWM,speedX);
  analogWrite(PWM2,speedX);
}

void stop(){                    //function that stops the rover
digitalWrite(DIR_A,HIGH);
  digitalWrite(DIR_B,LOW);
  digitalWrite(DIR_C,HIGH);
  digitalWrite(DIR_D,LOW);
  analogWrite(PWM,0);
  analogWrite(PWM2,0);
}

void movement (float holecount, float distance, float width, float rows) {    //function that runs the program to plant the seeds on the field
  Serial.println(holecount);
  Serial.println(distance);
  float distbetwnhol = distance/holecount;        //calculates the distance between each hole                         
  float distbetwnrow = width / rows;                //calculates the distance between each row
  float disttime = (distbetwnhol/0.44)*1000;      //calculates the time it takes to drive between each hole
  float distrowtime = (distbetwnrow/0.44)*1000;   //calculates the time it takes to drive between each row
  int mod;                                      //to determine if the rover should turn right or left

  Serial.println(distbetwnhol);
  Serial.println(disttime);
  

  for(int i=0; i<1; i++) {                  //this loop will run for as many rows as indicated by user

    mod = i%2;                                  //odd i means turn left, even i means turn right
    
    for(int j=0; j<holecount; j++) {           //this loops for as many holes as indicated by user
      seedDropper(seeds);                       //run the seed dropping function to drop the number of seeds specified
      delay(500);
      forward(255);                             //move forward at max speed
      delay(disttime);                          //keep moving forward for the time it takes to get to next hole
      stop();                                   //stop on hole and loop back to seed drop function
    }

    /*if(mod==0) {                                //if the i value is even, turn the rover right
      turnRight(255);           
      delay(500);
      forward(255);
      delay(4000);
      stop();
      turnRight(255);
      delay(500);
    }
    else if(mod==1) {                           //if the i value is odd, turn the rover left
      turnLeft(255);
      delay(500);
      forward(255);
      delay(4000);
      stop();
      turnLeft(255);
      delay(500);
    } */
    
  }
  
}
