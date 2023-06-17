#define ir1  3
#define ir2  A0
#define ir3  A1
#define ir4  A2
#define ir5  A3
#define ir6  A4
#define ir7  A5
#define ir8  2
#define falcon 7
#define led 8
//const int trigPin = 12;
//const int echoPin = 4;
const int state = 9;
//const int delta = 50; //(in bestestest)
const int delta = 80;
const long threshold = 5;
int cnt = 0;
int flag = 0;
int t;

long duration;
long distance;
int ir[8];                      
// ir[0] -> rightfar
// ir[1] ->
// ir[2] -> rightnear
// ir[3] -> rightcenter
// ir[4] -> leftcenter
// ir[5] -> leftnear
// ir[6] ->
// ir[7] -> leftfar
int irr[8];
//// out 4
#define ML1 10
#define ML2 11

#define MR1 5
#define MR2 6

int leapTime = 150;

int v = 120;
int error = 0, prev_error = 0;
float Kp = 18, Kd = 8; // KP=14, kD=6
int weights[8] = { -10, -7, -5, -1, 1, 5, 7, 10};
int avg_error = 0, sensors = 0, output = 0, leftmot = 0, rightmot = 0;

char path[60] = {};
int pathLength = 0;
int readLength = 0;
int stage = 0;
//unsigned long t;// 0 -> 1st stage   ,,  1 -> going with shorted path
int prevTime;

void setup() {
  // put your setup code here, to run once
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);
  pinMode(ir4, INPUT);
  pinMode(ir5, INPUT);
  pinMode(ir6, INPUT);
  pinMode(ir7, INPUT);
  pinMode(ir8, INPUT);
  pinMode(falcon, INPUT);
  pinMode(led, OUTPUT);

  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
  pinMode(MR1, OUTPUT);
  pinMode(MR2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

 Serial.begin(9600);
 prevTime = millis();
}

/*void getDistance(){
  long total = 0;
  long n = 20;
  for(int i = 0; i < n; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
    duration=pulseIn(echoPin, HIGH);
      // Calculating the distance
    distance=duration*0.034 / 2;
    if(distance==0){
      i--;
      n--;
      continue;
    }
    total += distance;
    delayMicroseconds(300);
  }
  long average_distance = total / n;
  Serial.println("Distance: "+ String(average_distance));
//
//  //obstacle avoidance
   //ultrasonic
   
  if(average_distance < threshold && average_distance > 0){
      cnt++;
      analogWrite(ML1, v);
      analogWrite(ML2, 0);
      analogWrite(MR1, 0);
      analogWrite(MR2, v);
      delay(300);
     while (!digitalRead(ir3) && !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7)) {
      analogWrite(ML1, v);
      analogWrite(ML2, 0);
      analogWrite(MR1, 0);
      analogWrite(MR2, v);
    }
    analogWrite(ML1, 0);   // this thing is due overshoot of motors
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(10);
    //////Serial.println("turn around ends");
  }
  */
}

void loop() {
  Serial.println(path[pathLength]);
  for(int i=0; i<pathLength; i++){
    Serial.print(path[i]);
  }

  /*
  if(millis()-prevTime >= 100){
    getDistance();
    prevTime = millis();
  }
 */
  readSensors();   //// '0' -> white      '1' -> black
//  Serial.println(digitalRead(falcon));
  if ((ir[0] && ir[1] && ir[2]) || (ir[5] && ir[7]) || 
      (ir[0] && ir[1] && ir[2] && ir[3] && ir[4] && ir[7]) ||
      (!ir[0] && !ir[1] &&  !ir[2] && !ir[3] && !ir[4] && !ir[5] &&  !ir[6] && !ir[7]))
  {
    Serial.println("going for LSR");
    for(int i=0; i<8 ; i++)
       irr[i] = 0;
    t = millis();
    while (millis() - t < 1.4 * delta) {
      readSensors();
      for (int i = 0; i < 8; i++)
      {
        if (irr[i] == 0)
          irr[i] = ir[i];
          Serial.print(ir[i]);
          Serial.print("---");
      }Serial.println("");
      straight();
    }
      for(int i=7; i>=0; i--)
      {
        Serial.print(irr[i]);
        Serial.print("   ");
        }
        Serial.println("");
    if(!stage) {
    LSR();        // when we get any junction point. it will deside the priorirty of turn by (LSR)
    }
    else {
      if(path[readLength] == 'L') {turnLeft(); Serial.println('L');}
      else if(path[readLength] == 'R') {turnRight(); Serial.println('R');}
      else if(path[readLength] == 'S') {straight(); Serial.println('S');}
      else {turnAround(); Serial.println('B');}
      readLength++;
    }
  }
  else {
    //////Serial.println("going straight");
    straight();
  }
}


void readSensors() {

  ir[0] = digitalRead(ir1);
  ir[1] = digitalRead(ir2);
  ir[2] = digitalRead(ir3);
  ir[3] = digitalRead(ir4);
  ir[4] = digitalRead(ir5);
  ir[5] = digitalRead(ir6);
  ir[6] = digitalRead(ir7);
  ir[7] = digitalRead(ir8);

  //Serial.print(ir[0]);
  //Serial.print("   ");
  //Serial.print(ir[1]);
  //Serial.print("   ");
  //Serial.print(ir[2]);
  //Serial.print("   ");
  //Serial.print(ir[3]);
  //Serial.print("   ");
  //Serial.print(ir[4]);
  //Serial.print("   ");
  //Serial.print(ir[5]);
  //Serial.print("   ");
  //Serial.print(ir[6]);
  //Serial.print("   ");
  //Serial.print(ir[7]);
  //Serial.println("   ");
  //Serial.println("---------------------");
}


void LSR() {    // for junction points

    
  /* analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    delay(50);*/
  
  if (digitalRead(ir1) && digitalRead(ir2) && digitalRead(ir4) && digitalRead(ir6) && digitalRead(ir8) && digitalRead(falcon)){
    Serial.println(digitalRead(falcon));
    done();}



  if (irr[7] || irr[6] )
  { t = millis();
    //if(( ir[0] && ir[7]) || ( ir[1] && ir[6])){     // case '1' or case '2'
    Serial.println("case '1' or case '2'");
    while (millis() - t < delta)
      straight();
    path[pathLength]='L';
    turnLeft();
  }

  else if (irr[0] || irr[1] ) {              // case '4' or case'6'  and one more hiden case to be find!
    if (digitalRead(ir1) || digitalRead(ir2) || digitalRead(ir3) || digitalRead(ir4) || digitalRead(ir5) || digitalRead(ir6) || digitalRead(ir7) || digitalRead(ir8)) {
      Serial.println("case '4' \n stright");
      straight();// only case'6'
      path[pathLength]='S';
    }
    else {
      t = millis();
      while (millis() - t < delta)
        straight();
       path[pathLength]='R';
       Serial.println("case '6'");
      turnRight();
    }
  }
  else if (!irr[0] && !irr[1] &&  !irr[2] && !irr[3] && !irr[4] && !irr[5] &&  !irr[6] && !irr[7]) {        // case'7'
    Serial.println("case '7'");
    coloraround();
    pathLength--;
//    turnAround();
//    path[pathLength]='B';

  }
  pathLength++;
  if(!stage)
    {
        if(pathLength>=3 && path[pathLength-2]=='B')
          shortpath();
    }
}

void coloraround(){
            // case'7'
    //Serial.println("case '7'");
      t=millis();
      while(millis()-t<130)
      {
        readSensors();
       straight();
      }
     
   if(digitalRead(ir1)||digitalRead(ir2)||digitalRead(ir3)||digitalRead(ir4)||digitalRead(ir5)||digitalRead(ir6)||digitalRead(ir7)||digitalRead(ir8))
     {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    delay(10);
      } 
   else  {
    Serial.println("turn around");
      path[pathLength]='B';
      pathLength++;
     turnAround();
   }
}



void shortpath()
{
  int shortDone=0;
  if(path[pathLength-3]=='L' && path[pathLength-1]=='R'){     // case 1       .--------
    pathLength-=3;                                            //                       |
    path[pathLength]='B';                                     //                       |
    //Serial.println("test1");                                //                       |
    shortDone=1;                                              //                       |
  }
   
  if(path[pathLength-3]=='L' && path[pathLength-1]=='S' && shortDone==0){    // case 2   .------------->
    pathLength-=3;                                                           //                 |
    path[pathLength]='R';                                                    //                 |
    //Serial.println("test2");                                               //                 |
    shortDone=1;                                                             //                 |
  }
   
  if(path[pathLength-3]=='R' && path[pathLength-1]=='L' && shortDone==0){  // case 3       ---------.   
    pathLength-=3;                                                         //              |
    path[pathLength]='B';                                                  //              |
    //Serial.println("test3");                                             //              |
    shortDone=1;                                                           //              |
  }
  
   
  if(path[pathLength-3]=='S' && path[pathLength-1]=='L' && shortDone==0){   //case 4       .
    pathLength-=3;                                                          //             |
    path[pathLength]='R';                                                   //             |------->
    //Serial.println("test4");                                              //             |
    shortDone=1;                                                            //             |
  }
     
  if(path[pathLength-3]=='S' && path[pathLength-1]=='S' && shortDone==0){  // case 5                .
    pathLength-=3;                                                         //                       |
    path[pathLength]='B';                                                  //               ._|
    //Serial.println("test5");                                             //                       |
    shortDone=1;                                                           //                       |  
  }
    if(path[pathLength-3]=='L' && path[pathLength-1]=='L' && shortDone==0){   // case 6         ^
    pathLength-=3;                                                            //                |
    path[pathLength]='S';                                                     //         .------|
    //Serial.println("test6");                                                //                |
    shortDone=1;                                                              //                |
  }
  path[pathLength+2]=NULL;
  path[pathLength+3]=NULL;
  pathLength++;
  //Serial.print("Path length: ");
  //Serial.println(pathLength);
  //printPath();
}


void replay(){
 
 readSensors();   //// '0' -> white      '1' -> black

  if ((ir[0] && ir[1] && ir[2]) || (ir[5] && ir[7]) || 
      (ir[0] && ir[1] && ir[2] && ir[3] && ir[4] && ir[7]) )
  {
    Serial.print("      going for LSR of replay     turnning ---");
    Serial.print(path[readLength]);
    Serial.print("     ");
    Serial.println(readLength);
    for(int i=0; i<8 ; i++)
       irr[i] = 0;
    t = millis();
    //while (millis() - t < 2.25*delta) {
    while(millis() - t < delta) {
      readSensors();
      for (int i = 0; i < 8; i++)
      {
        if (irr[i] == 0)
          irr[i] = ir[i];
      }
      straight();
    }
    
    if(path[readLength]=='L'){
     t = millis();
    while (millis() - t < 0.80*delta)
      straight();
    turnLeft();
    }
    else if(path[readLength]=='R'){
     t = millis();
    while (millis() - t < 0.80*delta)
      straight();
    turnRight();
    }
    else if(path[readLength]=='S'){
     straight();
    }
    readLength++;
  }
  
  else if (digitalRead(ir1) && digitalRead(ir2) && digitalRead(ir4) && digitalRead(ir6) && digitalRead(ir8) && digitalRead(falcon)){
    Serial.println(digitalRead(falcon));
    stage = 0;
    done();
  }
  
  else {
    Serial.println("going straight");
    straight();
  }

   replay();
}





void done() {
  //Serial.print("######DONE#######");
  analogWrite(ML1, 0);
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, 0);

  
  /*if(flag == 0) {
    for(int i=0; i<10; i++){
      Serial.print(path[i]);
    }
    for(int i = 0; i < cnt; i++) {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      delay(1000);
    }
    flag = 1;
  }*/
  if(!digitalRead(state)){
    stage=1;
    delay(5000);
    replay();
  }
  if(!stage)
  done();
  else {
    Serial.println("Short starts");
  }
  
}


void straight() { //////////////////////////////////////////////////////////////////////////////////////// By PID controlling
  prev_error = avg_error;
  for (int i = 0; i < 8; i++)
  { if (ir[i])
      sensors++;
    error += ir[i] * weights[7 - i];
  }
  avg_error = error / sensors;
  sensors = 0; error = 0;
  output = Kp * avg_error + Kd * (avg_error - prev_error);

  leftmot = v + output;
  rightmot = v - output;
  leftmot = constrain(leftmot, 0, 255);
  rightmot = constrain(rightmot, 0, 255);
  analogWrite(ML1, leftmot);
  analogWrite(ML2, LOW);
  analogWrite(MR1, rightmot);
  analogWrite(MR2, LOW);

  //////Serial.print(leftmot);
  //////Serial.print("          ");
  //////Serial.println(rightmot);
  output = 0;
}



void turnLeft() {
  Serial.print("turn left starts");
  while (digitalRead(ir8) || digitalRead(ir7) || digitalRead(ir6) || digitalRead(ir5) || digitalRead(ir4) || digitalRead(ir3) || digitalRead(ir2) || digitalRead(ir1))
  {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    Serial.println("In while 1 ");
  }

  while (!digitalRead(ir5) && !digitalRead(ir4) && !digitalRead(ir3) && !digitalRead(ir2)) {
    analogWrite(ML1, 0);
    analogWrite(ML2, v);
    analogWrite(MR1, v);
    analogWrite(MR2, 0);
    Serial.println("In while 2");
  }
  analogWrite(ML1, v);   // this thing is due overshoot of motors
  analogWrite(ML2, 0);
  analogWrite(MR1, 0);
  analogWrite(MR2, v);
  delay(10);
  Serial.println("turn left ends");

}


void turnRight() {
  Serial.println("turn right starts");
  while (digitalRead(ir8) || digitalRead(ir7) || digitalRead(ir6) || digitalRead(ir5) || digitalRead(ir4) || digitalRead(ir3) || digitalRead(ir2) || digitalRead(ir1)) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    Serial.println("In while 1 ");
  }

  while ( !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7) ) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    Serial.println("In while 2 ");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  Serial.println("turn right ends");
}


void turnAround() {
//  cnt++;
  Serial.println("COUNT INCREMENTED!");
  Serial.println(cnt);
  Serial.println("turn around starts");
  analogWrite(ML1, v);
  analogWrite(ML2, 0);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  while (!digitalRead(ir3) && !digitalRead(ir4) && !digitalRead(ir5) && !digitalRead(ir6) && !digitalRead(ir7)) {
    analogWrite(ML1, v);
    analogWrite(ML2, 0);
    analogWrite(MR1, 0);
    analogWrite(MR2, v);
    Serial.println("In while");
  }
  analogWrite(ML1, 0);   // this thing is due overshoot of motors
  analogWrite(ML2, v);
  analogWrite(MR1, v);
  analogWrite(MR2, 0);
  delay(10);
  Serial.println("turn around ends");

}
