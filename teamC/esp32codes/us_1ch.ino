//#include "esp_deep_sleep.h"
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSIDを記載";
const char* password = "パスワード";
const char* mqtt_server = "192.168.11.13";//server address 

WiFiClient espClient;
PubSubClient client(espClient);

#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */
#define MICROS_TO_S_FACTOR 1000000

#define PIN_US_RET 17
#define PIN_US_SENS01 16
volatile unsigned long gulRetTime=0;
volatile bool gbFlagGettingTime = 0;
float gfMeasTime=0;

#define NUM_TRY_WIFI 50
#define NUM_PULSE_TIME_OUT ((unsigned long)1<<30)

// WiFi Setup
void setup_wifi()
{
 delay(10);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 int lp;
 for( lp=0;lp<NUM_TRY_WIFI;lp++)
 {
  if(WiFi.status() == WL_CONNECTED )
  {
    break;
  }
   delay(500);
   Serial.print(".");

 }
 if ( lp >= (NUM_TRY_WIFI-1))
 {
 Serial.println("failure Connecting WiFi...reboot");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * MICROS_TO_S_FACTOR);
  esp_deep_sleep_start();
 }

 
// while (WiFi.status() != WL_CONNECTED) {
//   delay(500);
//   Serial.print(".");
// }
 Serial.println();
 Serial.println("WiFi Connected!");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
}

void sendingProcess(float fMeasTime){

  int i=0;
  for(i=0;i<50;i++)
  {
    client.connect("ESP32-IoT-Device");
    if(client.connected()==1)
    {
      break;
    }
  }
 if ( i >= (50-1))
 {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * MICROS_TO_S_FACTOR);
  esp_deep_sleep_start();
 }

  
 if (!client.connected()) {
   reconnect();
  } else{
   String json = buildJson(fMeasTime);
   char jsonStr[200];
   json.toCharArray(jsonStr,200);
   boolean pubresult = client.publish("Time",jsonStr);
   if (pubresult){
     Serial.println("Successfully sent!");
   } else {
     Serial.println("Unsuccessfully sent...");
  }
 }
 
}

// Build JSON DATA
String buildJson(float fMeasTime)
{
 String json = "{";
   json += "\"devices\": \"ESP32\"";
   json += ",";
   json += "\"Sensor01\":";
   json += "{";
   json += "\"Time\":";
   json += fMeasTime;
   json += "}";
   json += "}";
 return json;
}
// reconnect to MQTT broker
void reconnect()
{
 while (!client.connected()){
   Serial.print("Attempting MQTT connection...");
   if (client.connect("ESP32-IoT-Device")) {
     Serial.println("connected");
   } else {
     Serial.print("failed. rc= ");
     Serial.print(client.state());
     Serial.println("Try Again in 5 seconds");
     delay(1000); //waiting 5 sec.
   }
  }
}

void isrRun(){
  gulRetTime = micros();
  gbFlagGettingTime =0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_US_SENS01,OUTPUT);
  pinMode(PIN_US_RET,INPUT);
  attachInterrupt(PIN_US_RET, isrRun, FALLING);
  
  Serial.println("setup done");
  delay(1000);
  Serial.println("start");

  gfMeasTime=0;
  gbFlagGettingTime =1;
  unsigned long ulTxTime= micros();  
  digitalWrite( PIN_US_SENS01,1);
  delayMicroseconds(10);
  digitalWrite( PIN_US_SENS01,0);
  for (unsigned long ullp=0;ullp<NUM_PULSE_TIME_OUT;ullp++)
  {
    if(gbFlagGettingTime==0)
    {
      gfMeasTime=gulRetTime-ulTxTime;
      break;
    }
  }
  //while(gbFlagGettingTime==1){}
  //Serial.println((gulRetTime-ulTxTime));
  Serial.print(gfMeasTime);
  Serial.print("\t");
  Serial.println(buildJson(gfMeasTime));

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  sendingProcess(gfMeasTime );
  
  WiFi.disconnect();
  Serial.print("\n");
  Serial.println("into sleep...");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * MICROS_TO_S_FACTOR);
  esp_deep_sleep_start();

}

void loop() {
  // put your main code here, to run repeatedly:
}
