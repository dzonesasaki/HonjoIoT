#define PIN_US_RET 17
#define PIN_US_SENS01 16
volatile unsigned long gulRetTime=0;
volatile bool gbFlagGettingTime = 0;
long glMeasIrqMax=0;
long glMeasIrqMin=1024*1024*1024;
long glMeasFuncMax=0;
long glMeasFuncMin=1024*1024*1024;
int giCount=0;

// circuit
// connect IO16 to IO17 using jumperwire

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_US_SENS01,OUTPUT);
  pinMode(PIN_US_RET,INPUT);
  attachInterrupt(PIN_US_RET, isrRun, FALLING);
  Serial.println("setup done");

}

void isrRun(){
  gulRetTime = micros();
  gbFlagGettingTime =0;
}

void loop() {
  // put your main code here, to run repeatedly:

  if(giCount>512)return;giCount++;
  
  long lMeasIrq=0;
  long lMeasFunc=0;
  
  gbFlagGettingTime =1;
  unsigned long ulTxTime= micros();  
  digitalWrite( PIN_US_SENS01,1);
  delayMicroseconds(10);
  digitalWrite( PIN_US_SENS01,0);
  while(gbFlagGettingTime==1){}
  lMeasIrq = (gulRetTime-ulTxTime);
  //Serial.println(lMeasIrq);

  digitalWrite( PIN_US_SENS01,1);
  delayMicroseconds(10);
  digitalWrite( PIN_US_SENS01,0);
  lMeasFunc = pulseIn(PIN_US_RET,HIGH);

  if (glMeasIrqMax < lMeasIrq) glMeasIrqMax = lMeasIrq;
  if (glMeasIrqMin > lMeasIrq) glMeasIrqMin = lMeasIrq;
  if (glMeasFuncMax < lMeasFunc) glMeasFuncMax = lMeasFunc;
  if (glMeasFuncMin > lMeasFunc) glMeasFuncMin = lMeasFunc;
  
  Serial.print(lMeasIrq);
  Serial.print("\t");
  Serial.print(lMeasFunc);
  Serial.print("\t");
  Serial.print(lMeasIrq-lMeasFunc);
  Serial.print("\t");
  Serial.print(glMeasIrqMax);
  Serial.print("\t");
  Serial.print(glMeasIrqMin);
  Serial.print("\t");
  Serial.print(glMeasFuncMax);
  Serial.print("\t");
  Serial.print(glMeasFuncMin);
  Serial.print("\n");

  delay(10);
}
