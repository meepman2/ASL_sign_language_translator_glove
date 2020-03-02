#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library

/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
//ADXL345 adxl = ADXL345(10);           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/****************** INTERRUPT ******************/
/*      Uncomment If Attaching Interrupt       */
int interruptPin = 2;                 // Setup pin 2 to be the interrupt pin (for most Arduino Boards)

int p = 0,i;

int FLEX_PIN1 = A0;
int FLEX_PIN2 = A1;
int FLEX_PIN3 = A2;
int FLEX_PIN4 = A3;
int FLEX_PIN5 = A4;

int data[5];
int flexADC[5];

int fsensor_min[5]={1023,1023,1023,1023,1023};
int fsensor_max[5]={0,0,0,0,0};

int AccelMinX = 0;
int AccelMaxX = 1023;

int AccelMinY = 0;
int AccelMaxY = 1023;

int AccelMinZ = 0;
int AccelMaxZ = 1023;


void setup(){
  pinMode(interruptPin, INPUT);
  
  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Hook Up Guide Example");
  Serial.println();
  
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity
                                      
  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  
  adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.

  adxl.singleTapINT(1);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin),RSI, RISING);   // Attach Interrupt

  Calibrate();
}

void loop(){
  int x,y,z;
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z

  Serial.print(x);
  Serial.print(" ,");
  Serial.print(y);
  Serial.print(" ,");
  Serial.println(z);

  delay(100);

  flexADC[0]=analogRead(FLEX_PIN1);
  flexADC[1]=analogRead(FLEX_PIN2);
  flexADC[2]=analogRead(FLEX_PIN3);
  flexADC[3]=analogRead(FLEX_PIN4);
  flexADC[4]=analogRead(FLEX_PIN5);

  for(i=0;i<5;i++)
  {
   flexADC[i] = constrain(flexADC[i],fsensor_min[i],fsensor_max[i]);
   data[i] = map(flexADC[i],fsensor_min[i],fsensor_max[i],0,90);
  }
  
  for(i=0;i<5;i++){
     Serial.print(data[i]);
     Serial.print(" ,");
  }
  Serial.println();

  if(p==1)
  {
  Calibrate();
  p =0;
  }
}

void Calibrate()
{  
   Serial.println("******CALIBRATING********");
   int t=millis();
   int T=0;
   while((T-t)<1000)
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

   T=millis();
}
}

void RSI()
{
  p = p+1;
}
