#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <aWOT.h>

//#define WIFI_SSID "Red-Operaciones"
//#define WIFI_PASSWORD "P3g4dur0"
  
#define WIFI_SSID "LAPTOP-0S7H8CRD 2665"
#define WIFI_PASSWORD "pegaduro"

WiFiServer server(80);
Application app;

//IPAddress ip(192,168,137,39);
//IPAddress subnet(255,255,255,0);

DynamicJsonDocument doc(2048);
String color;
String orden;
char buffer[1000];

void index(Request &req, Response &res) {
  res.print("Hello World!");
}

void imprimir(Request &req, Response &res) {
  deserializeJson(doc, req);
  //-------------------------------------------------ALMACENA BUFFER PRODUCTO----------------------------------------------------//
  String producto = doc["Color"];
  if (producto != "null"){
    color=producto;
  }
  if (producto == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }
  int lenProducto=color.length() +1;
  char bufProducto[lenProducto];
  color.toCharArray(bufProducto,lenProducto);
  //----------------------------------------------------ALMACENA BUFFER LOTE----------------------------------------------------//
  String lote = doc["Lote"];
  if (lote != "null"){
    orden=lote;
  }
  if (lote == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }
  int lenLote=orden.length() +1;
  char bufLote[lenLote];
  orden.toCharArray(bufLote,lenLote);
  //-----------------------------------------------------ALMACENA BUFFER PESO---------------------------------------------------//
  String peso = doc["peso"];
  if (peso == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }
  int lenPeso=peso.length() +1;
  char bufPeso[lenPeso];
  peso.toCharArray(bufPeso,lenPeso);
  
  // ---------------------------------------------------JUNTAR MENSAJE PARA IMPRIMIR-----------------------------------------------//
  sprintf(buffer,"*\r\n%s\r\n%s\r\n",bufProducto,bufLote);
  serializeJson(doc, Serial);
  Serial.println("");
  Serial.println(buffer);
  Serial1.print(buffer);
  res.set("Content-Type", "application/json");
  res.println("{\"status\": \"ok\"}");
  
  return res.status(201);
}

void CLIENTE() {
  WiFiClient client = server.available();
  if (client.connected()){
    app.process(&client);
    client.stop();
  }
}

void CONECTAR() {
    delay(500);
    WiFiDrv::analogWrite(25, 100);
    WiFiDrv::analogWrite(26, 0);
    WiFiDrv::analogWrite(27, 0);
    Serial.print(".");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
  
void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  WiFiDrv::pinMode(25, OUTPUT); //define green pin
  WiFiDrv::pinMode(26, OUTPUT); //define red pin
  WiFiDrv::pinMode(27, OUTPUT); //define blue pin
  app.get("/", &index);
  app.post("/", &imprimir);
  server.begin();
}
  
void loop() { 
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiDrv::analogWrite(25, 0);
    WiFiDrv::analogWrite(26, 0);
    WiFiDrv::analogWrite(27, 100);
   // Serial.print(WiFi.status());
    CLIENTE();
  }else{
    WiFiDrv::analogWrite(25, 0);
    WiFiDrv::analogWrite(26, 100);
    WiFiDrv::analogWrite(27, 0);
    CONECTAR();
  }
}

