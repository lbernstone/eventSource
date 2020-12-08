#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Ticker.h>

#define TEMP_PIN 16
#define SSID "ssid"
#define PASSWD "passwd"

AsyncWebServer server(80);
AsyncEventSource events("/events");
Ticker tkTemp;
Ticker tkLive;

void sendTemp() {
  uint16_t temp = analogRead(TEMP_PIN);
  char sendStr[128];
  snprintf(sendStr, 128, "{'temp': '%d', 'time': '%llu'}", temp, millis()); 
  events.send(sendStr,"temp");
}

void sendLive() {
  Serial.println(millis());
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(SSID, PASSWD);
  WiFi.waitForConnectResult();
  Serial.println(WiFi.localIP());

  pinMode(TEMP_PIN, INPUT);
  analogSetSamples(1);

  if(!SPIFFS.begin()) log_e("Unable to init SPIFFS");
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.addHandler(&events);
  server.begin();

  tkTemp.attach_ms(18,sendTemp);
  tkLive.attach(30, sendLive);
}

void loop(){
  vTaskDelete(NULL);
}
