#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <aWOT.h>

#define WIFI_SSID "LAPTOP-IV84MQ3J"
#define WIFI_PASSWORD "pega2022"
  
WiFiServer server(80);
Application app;

DynamicJsonDocument doc(2048);
  
void index(Request &req, Response &res) {
  res.print("Hello World!");
}

void imprimir(Request &req, Response &res) {
  deserializeJson(doc, req);

  String titulo = doc["titulo"];
  if (titulo == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }

  String producto = doc["producto"];
  if (producto == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }

  String lote = doc["lote"];
  if (lote == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }

  String peso = doc["peso"];
  if (peso == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }

  String etiqueta = doc["etiqueta"];
  if (etiqueta == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }

  serializeJson(doc, Serial);

  res.set("Content-Type", "application/json");
  res.print("{\"status\": \"ok\"}");

  return res.status(201);
}
  
void setup() {
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  app.get("/", &index);
  app.post("/", &imprimir);
  server.begin();
}
  
void loop() {  
  WiFiClient client = server.available();
  
  if (client.connected()) {
    app.process(&client);
    client.stop();
  }
}