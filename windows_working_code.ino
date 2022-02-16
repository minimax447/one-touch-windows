/*This program by Tom Sheppard February2022
   One touch window controller for early MGF. Each switch grounds to run relays
  short push is one touch, long one cut on release
  push opposite direction stops window
   optional 3 minute timeout when ignition turns off during which time all functions work.
   normal operation by taking pin 11 high.
  optional by connecting pin 12:
  two flashes of locking system within timeout will trigger rollup of both windows
  status byte: 0=left dn run,1=left dn ot,2=right dn run,3=right dn ot,4=left up run,5=left up ot,6=right up run.7=right up ot
*/
const int runtime = 110;
const int touchtime = 15;
byte status = 0;
byte lupcounter = 0;
byte ldncounter = 0;
byte rupcounter = 0;
byte rdncounter = 0;
byte flashes = 0;
long loops = 0;
int  a = 0; 

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
  pinMode (10, OUTPUT);//power hold relay goes low to switch off
  pinMode (11, INPUT);//ignition off input active high
  pinMode (12, INPUT_PULLUP);//indicator input active low
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH); //turn the system on
  status = 0;
  lupcounter = 0;
  ldncounter = 0;
  rupcounter = 0;
  rdncounter = 0;
  flashes = 0;
  loops = 0;
}

void leftupoff()
{
  digitalWrite(6, HIGH);
  status = (status & B11001111);
  lupcounter = 0;
}
void leftdnoff()
{
  digitalWrite(7, HIGH);
  status = (status & B11111100);
  ldncounter = 0;
}
void rightupoff()
{
  digitalWrite(8, HIGH);
  status = (status & B00111111);
  rupcounter = 0;
}
void rightdnoff()
{
  digitalWrite(9, HIGH);
  status = (status & B11110011);
  rdncounter = 0;
}
void luprevcheck()
{
  a = (bitRead(status, 0)); //if the other direction was triggered
  if (a == 1)
  {
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);//stop both relays
    status = (status & B11001100);
    lupcounter = 0; //clear the two counters
    ldncounter = 0;
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
    status = (status & B11001100);
    lupcounter = 0; //clear the two counters
    ldncounter = 0;
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
    status = (status & B00110011);
    rupcounter = 0;
    rdncounter = 0;
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
    status = (status & B00110011);
    rupcounter = 0;
    rdncounter = 0;
    while (digitalRead(4) != HIGH); {}
    a = digitalRead(4);
    delay(300);
  }
}
void lup()//status bits 4 and 5 other direction 0 and 1
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
      bitWrite (status, 4, 0);//clear the run flag
    }
    if (digitalRead(2) == LOW) //debounce pass
    { a = (bitRead(status, 0)); //check the other direction isn't going
      if (a == 0)               //and if it is off
      { digitalWrite((6), LOW); //all happy turn the motor on
        luprevcheck();
      }          //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (2 < lupcounter && lupcounter < touchtime)
  { if (digitalRead(2) == HIGH)  // switch released in this time
    { bitWrite(status, 5, 1);    //turns on the onetouch facility
      luprevcheck();             //unless we are already running in the other direction -stop the motor and start again.
    }
  }
  if (touchtime < lupcounter && lupcounter < runtime )
  { luprevcheck();
    if (digitalRead(2) == HIGH)//if the switch is released
    { a = (bitRead(status, 5));
      if (a == 0) //and the onetouch is inactive
      {
        leftupoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if ( lupcounter == runtime) // timeout. turn the motor off anyway
  {
    leftupoff();
  }
}
void rup()//status bits 6 and 7 other direction 2 and 3 inpin 4 outpin 8
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
  if (2 < rupcounter && rupcounter < touchtime)
  { if (digitalRead(4) == HIGH)  // switch released in this time
    { bitWrite(status, 7, 1);    //turns on the onetouch facility
      ruprevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (touchtime < rupcounter && rupcounter < runtime)
  { ruprevcheck();
    if (digitalRead(4) == HIGH)//if the switch is released
    { a = (bitRead(status, 7));
      if (a == 0) //and the onetouch is inactive
      {
        rightupoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if ( rupcounter == runtime) // timeout. turn the motor off anyway
  {
    rightupoff();
  }
}
void ldn()//status bits 0 and 1 other direction 4 and 5 inpin 3 outpin 7
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
  if (2 < ldncounter && ldncounter < touchtime)
  { if (digitalRead(3) == HIGH)  // switch released in this time
    { bitWrite(status, 1, 1);    //turns on the onetouch facility
      ldnrevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (touchtime < ldncounter && ldncounter < runtime)
  { ldnrevcheck();
    if (digitalRead(3) == HIGH)//if the switch is released
    { a = (bitRead(status, 1));
      if (a == 0) //and the onetouch is inactive
      {
        leftdnoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if (ldncounter == runtime) // timeout. turn the motor off anyway
  {
    leftdnoff();
  }
}
void rdn()//status bits 2 and 3 other direction 6 and 7 inpin 5 outpin 9
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
  if (2 < rdncounter && rdncounter < touchtime)
  { if (digitalRead(5) == HIGH)  // switch released in this time
    { bitWrite(status, 3, 1);    //turns on the onetouch facility
      rdnrevcheck();
    }            //unless we are already running in the other direction -stop the motor and start again.
  }
  if (touchtime < rdncounter && rdncounter < runtime)
  { rdnrevcheck();
    if (digitalRead(5) == HIGH)//if the switch is released
    { a = (bitRead(status, 3));
      if (a == 0) //and the onetouch is inactive
      {
        rightdnoff(); //turn off the motor but don't clear the other direction status
      }
    }
  }
  if (rdncounter == runtime) // timeout. turn the motor off anyway
  {
    rightdnoff();
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
  a = (digitalRead(11)); //the ignition
  if ( a == 1)           // if the ignition is on
  {
    loops = 0;
    flashes = 0;        //the shutdown triggers are held at zero
  }
  runwindows();         //do the work
  loops++;              //this gets cleared if the ignition is on
  if (loops > 3000)     // but if not it increments until time out
  {
    digitalWrite(10, LOW);  //shutdown. Cuts the power to this circuit. Should cut all relays
  }
  if (loops > 50)           //ignition definitely off for two seconds   
  {
     a = (digitalRead(12)); //the indicators
    if (flashes == 0)       //no flash sequence from fob
    {
      if (a == 0)           //test the indicator input for indicator on
      {
        flashes = 1;        //and set the first stage flash
      }
    }                        
    if (flashes == 1)
    {
      if (a == 1)// test for indicator off. Next flash is rollup
      {
        flashes = 2;//second stage set
      }
    }
    if (flashes == 2)
    {
      if (a == 0 ) //and if the indicator is flashing again
      
      {
        digitalWrite(5, HIGH);//make sure that the motors are not running down
        digitalWrite(7, HIGH);//make sure that the motors are not running down
        delay(5);
        digitalWrite(6, LOW);
        delay(5);
        digitalWrite(8, LOW);
        delay(40 * runtime);
        digitalWrite(6, HIGH);
        delay(5);
        digitalWrite(8, HIGH);
        delay(5);
        digitalWrite(10, LOW);
      }
    }
  }
}
