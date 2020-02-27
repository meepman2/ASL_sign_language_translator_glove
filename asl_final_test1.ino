#include <SparkFun_ADXL345.h>         

ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

int interruptPin = 2, button = 3;                 // Setup pin 2 and 3 to be the interrupt pin 
int i = 0;   
int FLEX_PIN1 = A0; 
int FLEX_PIN2 = A1;
int FLEX_PIN3 = A2;
int FLEX_PIN4 = A3;
int FLEX_PIN5 = A4;  

int AccelMinX = 0;
int AccelMaxX = 0;
int AccelMinY = 0;
int AccelMaxY = 0;
int AccelMinZ = 0;
int AccelMaxZ = 0; 

float flexADC[5], data[5];
float fsensor_min[] = {0,0,0,0,0} , fsensor_max[] = {1023,1023,1023,1023,1023};

char temp; 

void setup(){
  
  Serial.begin(9600);                 // Start the serial terminal
  while (!Serial) 
  {
   ; // wait for serial port to connect. Needed for native USB port only
  }
  adxl.powerOn();                     // Power on the ADXL345
  adxl.setRangeSetting(8);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity
   
  adxl.setInactivityXYZ(1, 1, 1);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
 
  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.singleTapINT(1);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt
  attachInterrupt(digitalPinToInterrupt(button), Calibrate, RISING);
  
  //Calibrate flex sensors and accelerometer 
  Calibrate();
}

/****************** MAIN CODE ******************/
/*     Accelerometer Readings and Interrupt    */
void loop(){
  
  // Accelerometer Readings
  int x,y,z;   
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z

  flexADC[0] = analogRead(FLEX_PIN1);
  flexADC[1] = analogRead(FLEX_PIN2);
  flexADC[2] = analogRead(FLEX_PIN3);
  flexADC[3] = analogRead(FLEX_PIN4);
  flexADC[4] = analogRead(FLEX_PIN5);

  for(i=0;i<5;i++)
  {
   flexADC[i] = constrain(flexADC[i],fsensor_min[i],fsensor_max[i]);
   data[i] = map(flexADC[i],fsensor_min[i],fsensor_max[i],0,90);
  }
  
  /* write */ 

  ADXL_ISR();
}

/* Look for Interrupts and Triggered Action    */
void ADXL_ISR() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     //add code here to do when a tap is sensed
  } 
}

void Calibrate()
{   
   while(millis()<2000)
   {
   flexADC[0] = analogRead(FLEX_PIN1);
   flexADC[1] = analogRead(FLEX_PIN2);
   flexADC[2] = analogRead(FLEX_PIN3);
   flexADC[3] = analogRead(FLEX_PIN4);
   flexADC[4] = analogRead(FLEX_PIN5);

   for(i=0;i<5;i++)
   {
    if(flexADC[i] < fsensor_min[i])fsensor_min[i] = flexADC[i];
    if(flexADC[i] > fsensor_max[i])fsensor_max[i] = flexADC[i];
   }
   
   i = 0;

   int x,y,z;                          
   adxl.readAccel(&x, &y, &z);         

   if(x < AccelMinX) AccelMinX = x;
   if(x > AccelMaxX) AccelMaxX = x;

   if(y < AccelMinY) AccelMinY = y;
   if(y > AccelMaxY) AccelMaxY = y;

   if(z < AccelMinZ) AccelMinZ = z;
   if(z > AccelMaxZ) AccelMaxZ = z;
   
}
}

void printl(char p) //to avoid printing repeating symbols
{
if(p!=temp)
{
Serial.print(p);
temp=p;
}
}
