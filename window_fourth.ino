/*This program by Tom Sheppard February2022
   One touch window controller for early MGF. Each switch grounds to run relays
  short push is one touch, long one cut on release
  push opposite direction stops window
   optional 3 minute timeout when ignition turns off during which time all functions work.
   normal operation by taking pin 11 high.
  optional by connecting pin 12:
  two flashes of locking system within timeout will trigger rollup of both windows

*/
byte status = 0;
/*
   0=left dn run
   1=left dn ot
   2=right dn run
   3=right dn ot
   4=left up run
   5=left up ot
   6=right up run
   7=right up ot
*/
byte lupcounter = 0;
byte ldncounter = 0;
byte rupcounter = 0;
byte rdncounter = 0;
byte flashes = 0;
int  timeoutflag = 0;
int  a = 0;
long timeoutstart = 0;
long flashbegin = 0;
byte runoutflag = 0;

void setup()
{
  Serial.begin(9600);
  pinMode (2, INPUT_PULLUP);//lup
  pinMode (3, INPUT_PULLUP);//ldn
  pinMode (4, INPUT_PULLUP); //rup
  pinMode (5, INPUT_PULLUP);//rdn
  pinMode (6, OUTPUT);//lup motor
  pinMode (7, OUTPUT);//ldn motor
  pinMode (8, OUTPUT);//rup motor
  pinMode (9, OUTPUT);//rdn motor
  pinMode (10, OUTPUT);//power hold relay
  pinMode (11, INPUT);//ignition off input
  pinMode (12, INPUT_PULLUP);//indicator
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  lupcounter = 0;
  ldncounter = 0;
  rupcounter = 0;
  rdncounter = 0;
  status = 0;
  timeoutflag = 0;
  flashes = 0;
  runoutflag = 0;
}

void leftupoff()
{
  digitalWrite(6, HIGH);
  bitWrite(status, 4, 0);
  bitWrite(status, 5, 0);
  lupcounter = 0;
}
void leftdnoff()
{
  digitalWrite(7, HIGH);
  bitWrite(status, 0, 0);
  bitWrite(status, 1, 0);
  ldncounter = 0;
}
void rightupoff()
{
  digitalWrite(8, HIGH);
  bitWrite(status, 6, 0);
  bitWrite(status, 7, 0);
  rupcounter = 0;
}
void rightdnoff()
{
  digitalWrite(9, HIGH);
  bitWrite(status, 2, 0);
  bitWrite(status, 3, 0);
  rdncounter = 0;
}
void luprevcheck()
{
  a = (bitRead(status, 0)); //if the other side was triggered
  if (a == 1)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    bitWrite(status, 0, 0);
    bitWrite(status, 1, 0);
    bitWrite(status, 4, 0);
    bitWrite(status, 5, 0);
    while (digitalRead(3) != HIGH); {}
    a = digitalRead(3);
    delay(300);
  }
}
void ldnrevcheck()
{
  a = (bitRead(status, 4));
  if (a == 1)
  {
    digitalWrite(6, HIGH);   //turn off my motor
    digitalWrite(7, HIGH);
    bitWrite(status, 0, 0);
    bitWrite(status, 1, 0);
    bitWrite(status, 4, 0);
    bitWrite(status, 5, 0);
    while (digitalRead(2) != HIGH); {}
    a = digitalRead(2);
    delay(300);
  }
}
void ruprevcheck()
{
  a = (bitRead(status, 2)); //if the other side was triggered
  if (a == 1)
  {
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    bitWrite(status, 2, 0);
    bitWrite(status, 3, 0);
    bitWrite(status, 6, 0);
    bitWrite(status, 7, 0);
    while (digitalRead(5) != HIGH); {}
    a = digitalRead(5);
    delay(300);
  }
}
void rdnrevcheck()
{
  a = (bitRead(status, 6));
  if (a == 1)
  {
    digitalWrite(8, HIGH);   //turn off my motor
    digitalWrite(9, HIGH);
    bitWrite(status, 2, 0);
    bitWrite(status, 3, 0);
    bitWrite(status, 6, 0);
    bitWrite(status, 7, 0);
    while (digitalRead(4) != HIGH); {}
    a = digitalRead(4);
    delay(300);
  }
}
void lup()
{
  lupcounter++;
  if (lupcounter == 1)
  { if (digitalRead(2) == HIGH)
    {
      lupcounter = 0;     //don't start counting until 2 goes low
    }
    if (digitalRead(2) == LOW)
    {
      bitWrite (status, 4, 1); //prepare for debounce: set running flag
    }
  }
  if (lupcounter == 2)
  { if (digitalRead(2) == HIGH)
    { lupcounter = 0;//debounce fail
      bitWrite (status, 4, 0);
    } //clear the run flag
    if (digitalRead(2) == LOW)
    { a = (bitRead(status, 0));
      if (a == 0)
      { digitalWrite((6), LOW); //all happy turn the motor on
        luprevcheck();
      }          //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (2 < lupcounter && lupcounter < 18)
  { if (digitalRead(2) == HIGH)  // switch released in this time
    { bitWrite(status, 5, 1);    //turns on the onetouch facility
      luprevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (17 < lupcounter && lupcounter < 150)
  { luprevcheck();
    if (digitalRead(2) == HIGH)//if the switch is released
    { a = (bitRead(status, 5));
      if (a == 0) //and the onetouch is inactive
      {
        leftupoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if ( lupcounter > 151)// timeout. turn the motor off anyway
  {
    leftupoff();
  }
}
void rup()
{
  rupcounter++;
  if (rupcounter == 1)
  { if (digitalRead(4) == HIGH)
    {
      rupcounter = 0;     //don't start counting until 2 goes low
    }
    if (digitalRead(4) == LOW)
    {
      bitWrite (status, 6, 1); //prepare for debounce: set running flag
    }
  }
  if (rupcounter == 2)
  { if (digitalRead(4) == HIGH)
    { rupcounter = 0;//debounce fail
      bitWrite (status, 6, 0);
    } //clear the run flag
    if (digitalRead(4) == LOW)
    { a = (bitRead(status, 2));
      if (a == 0)
      { digitalWrite((8), LOW); //all happy turn the motor on
        ruprevcheck();
      }          //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (2 < rupcounter && rupcounter < 18)
  { if (digitalRead(4) == HIGH)  // switch released in this time
    { bitWrite(status, 7, 1);    //turns on the onetouch facility
      ruprevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (17 < rupcounter && rupcounter < 150)
  { ruprevcheck();
    if (digitalRead(4) == HIGH)//if the switch is released
    { a = (bitRead(status, 7));
      if (a == 0) //and the onetouch is inactive
      {
        rightupoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if ( rupcounter > 151)// timeout. turn the motor off anyway
  {
    rightupoff();
  }
}
void ldn()
{
  ldncounter++;
  if (ldncounter == 1)
  { if (digitalRead(3) == HIGH)
    {
      ldncounter = 0;     //don't start counting until 2 goes low
    }
    if (digitalRead(3) == LOW)
    {
      bitWrite (status, 0, 1); //prepare for debounce: set running flag
    }
  }
  if (ldncounter == 2)
  { if (digitalRead(3) == HIGH)
    { ldncounter = 0;//debounce fail
      bitWrite (status, 0, 0);
    } //clear the run flag
    if (digitalRead(3) == LOW)
    { a = (bitRead(status, 4));
      if (a == 0)
      { digitalWrite((7), LOW); //all happy turn the motor on
        ldnrevcheck();
      }          //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (2 < ldncounter && ldncounter < 18)
  { if (digitalRead(3) == HIGH)  // switch released in this time
    { bitWrite(status, 1, 1);    //turns on the onetouch facility
      ldnrevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (17 < ldncounter && ldncounter < 150)
  { ldnrevcheck();
    if (digitalRead(3) == HIGH)//if the switch is released
    { a = (bitRead(status, 1));
      if (a == 0) //and the onetouch is inactive
      {
        leftdnoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if (ldncounter > 151) // timeout. turn the motor off anyway
  {
    leftdnoff();
  }
}
void rdn()
{
  rdncounter++;
  if (rdncounter == 1)
  { if (digitalRead(5) == HIGH)
    {
      rdncounter = 0;     //don't start counting until 2 goes low
    }
    if (digitalRead(5) == LOW)
    {
      bitWrite (status, 2, 1); //prepare for debounce: set running flag
    }
  }
  if (rdncounter == 2)
  { if (digitalRead(5) == HIGH)
    { rdncounter = 0;//debounce fail
      bitWrite (status, 2, 0);
    } //clear the run flag
    if (digitalRead(5) == LOW)
    { a = (bitRead(status, 6));
      if (a == 0)
      { digitalWrite((9), LOW); //all happy turn the motor on
        rdnrevcheck();
      }          //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (2 < rdncounter && rdncounter < 18)
  { if (digitalRead(5) == HIGH)  // switch released in this time
    { bitWrite(status, 3, 1);    //turns on the onetouch facility
      rdnrevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (17 < rdncounter && rdncounter < 150)
  { rdnrevcheck();
    if (digitalRead(5) == HIGH)//if the switch is released
    { a = (bitRead(status, 3));
      if (a == 0) //and the onetouch is inactive
      {
        rightdnoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if (rdncounter > 151) // timeout. turn the motor off anyway
  {
    rightdnoff();
  }
}
void offtest()
{
  if (digitalRead(11) == 0) // the ignition is off
  {
    timeoutflag = 1;

  }
  if (digitalRead(11) == 1) // the ignition is on again
  {
    timeoutflag = 0;
    runoutflag = 0;
  }
}
void timeouttest()
{
  if (timeoutflag == 1)
  {
    if ( runoutflag == 0)
    {
      timeoutstart = millis();
      runoutflag = 1;
    }
  }
  if (millis() > timeoutstart + 180000)
  {
    digitalWrite(10, LOW); //crash burn die.
  }
}

void rolluptest()
{
  if (timeoutflag == 1) //ignition off
  {
    if (digitalRead(12) == LOW) //the indicator is flashing so the car is being locked. the window switches will not be used again
    {
      delay(100);
      if (digitalRead(12) == LOW);
      {
        while (digitalRead(12) == LOW); {}
        a = digitalRead(12);// wait for it to go high
        flashes = 1; //  gone high again now looking for another flash within 2 seconds
        delay(400);
        flashbegin = millis();
        while (millis() < flashbegin + 2000)
        {
          if (digitalRead(12) == LOW) //the indicator is flashing so the car is being locked. the window switches will not be used again
          {
            delay(100);
            if (digitalRead(12) == LOW);
            {
              rollup();
            }
            digitalWrite(10, LOW); //crash burn die.
          }
        }
      }
    }
  }
}
void rollup()
{
  if (digitalRead(12) == LOW);
  {
    delay(100);
    if (digitalRead(12) == LOW);
    {
      digitalWrite(6, LOW);
      digitalWrite(8, LOW);
         delay(7000);
      digitalWrite(6, HIGH);
      digitalWrite(8, HIGH);
   

    }
  }
}
void runwindows()
{
  lup();
  delay(10);
  ldn();
  delay(10);
  rup();
  delay(10);
  rdn();
  delay(10);
}
void loop()
{
  runwindows();
  offtest(); //check ignition
  timeouttest(); //if the ignition is off
  rolluptest();    //do the indicator flash test
}
