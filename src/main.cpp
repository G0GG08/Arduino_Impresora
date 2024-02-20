#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <Arduino_JSON.h>
#include <aWOT.h>
#include <ArduinoHttpClient.h>
#include "wiring_private.h"


//#define WIFI_SSID "Red-Operaciones"
//#define WIFI_PASSWORD "P3g4dur0"
  
#define WIFI_SSID "LAPTOP-0S7H8CRD 2665"
#define WIFI_PASSWORD "pegaduro"

WiFiServer server(80);
Application app;
;
JSONVar myObject;
String jsonData;
Uart mySerial (&sercom3, 1, 0, SERCOM_RX_PAD_1, UART_TX_PAD_0);
IPAddress ip(192,168,137,39);
IPAddress subnet(255,255,255,0);
int impresora = 2;
DynamicJsonDocument doc(2048);
String color;
String orden;
const unsigned int MAX_MESSAGE_LENGTH = 100;
char bufferDatosImpresora1[1000];
char bufferPesoImpresora1[1000];
char bufferMensajeImpresora2[1000];
char bufProducto[MAX_MESSAGE_LENGTH];
char bufLote[MAX_MESSAGE_LENGTH];
char bufPeso[MAX_MESSAGE_LENGTH];
char mensaje[MAX_MESSAGE_LENGTH];
char weight[MAX_MESSAGE_LENGTH];
char titulo[MAX_MESSAGE_LENGTH];

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
  //char bufProducto[lenProducto];
  if(producto=="Cemento"){
    titulo="CEMENTO";
  }
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
  //char bufLote[lenLote];
  orden.toCharArray(bufLote,lenLote);
  //-----------------------------------------------------ALMACENA BUFFER PESO---------------------------------------------------//
  String peso = doc["peso"];
  if (peso == "") {
    res.set("Content-Type", "application/json");
    res.print("{\"status\": \"error\"}");
    return res.status(400);
  }
  int lenPeso=peso.length() +1;
  //char bufPeso[lenPeso];
  peso.toCharArray(bufPeso,lenPeso);
  // ---------------------------------------------------JUNTAR MENSAJE PARA IMPRIMIR-----------------------------------------------//
  sprintf(bufferDatosImpresora1,"*\r\n%s\r\n%s\r\n",bufProducto,bufLote);
  sprintf(bufferPesoImpresora1,"%s\r\n",bufPeso);
  
  serializeJson(doc, Serial);
  Serial.println("");
  //--------------------------------------------------IMPRIMIR SOLO EL PESO O IMPRIMIR NUEVA CONFIGURACION-----------------------//
  if (impresora==1){
    if (peso != "null"){
      Serial.println(bufferPesoImpresora1);
      Serial1.print(bufferPesoImpresora1);
    }else{
      Serial.println(bufferDatosImpresora1);
      Serial1.print(bufferDatosImpresora1);
    }
  }else{
    if (peso != "null"){
      sprintf(bufferMensajeImpresora2,"SIZE 2.29,1.61\rGAP 0\rDIRECTION 1,0\rREFERENCE 0,0\rOFFSET 0\rSET PEEL ON\rSET CUTTER 1\r@LABEL=2\rCLS\rTEXT 40,30,\"2\",0,1,1,\"%s\"\rTEXT 280,30,\"1\",0,1,1,FORMAT$(NOW,\"dd/mm/yy hh:nn AM/PM\")\rBAR 20,60,420,3\rTEXT 40,75,\"D.FNT\",0,1,1,\"PRODUCTO:\"\rTEXT 280,75,\"2\",0,1,1,\"%s\"\rTEXT 40,105,\"D.FNT\",0,1,1,\"CONSECUTIVO:\"\rTEXT 280,105,\"2\",0,1,1,@LABEL\rTEXT 40,135,\"D.FNT\",0,1,1,\"LOTE:\"\rTEXT 280,135,\"2\",0,1,1,\"%s\"\rTEXT 40,185,\"2\",0,1,1,\"PESO (KG):\"\rTEXT 40,235,\"5.EFT\",0,1,1,\"%s\"\rPUTBMP 740,165, \"Logo.bmp\",1,60\rPRINT 1\rEND\r\n",titulo,bufProducto,bufLote,bufPeso);
      Serial.println(bufferMensajeImpresora2);
      Serial1.print(bufferMensajeImpresora2);
    }
  }
  

  //Serial1.print(bufferDatos);
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
  mySerial.begin(9600);

  pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
  pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0

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
  while (mySerial.available()) {
  
  static unsigned int posMensaje = 0;
  char arrCharByte = mySerial.read();
  // gestionar si hay llegado mensaje por el serial
  if ( arrCharByte != '\n' && (posMensaje < MAX_MESSAGE_LENGTH - 1) ){
    //en cada ciclo, acumula caracter por caracter hasta terminal el mensaje
    mensaje[posMensaje] = arrCharByte;
    posMensaje++;
  }else{
    // si ya se acumuló todo el mensaje, se guarda en un json, y se imprime el mensaje recibido
    mensaje[posMensaje] = '\0';
    //imprime el mensaje recibido
    Serial.print("Mensaje Recibido: ");
    Serial.println(mensaje);
    Serial.println();
    // se resetea la variable donde se guarda el mensaje para acumular otro nuevo
    posMensaje = 0;

    if (impresora==1){
      Serial1.print(mensaje);
    }else{
      sscanf(mensaje,"Weight: %s",&weight);
      Serial.println(weight);
      sprintf(bufferMensajeImpresora2,"SIZE 2.29,1.61\rGAP 0\rDIRECTION 1,0\rREFERENCE 0,0\rOFFSET 0\rSET PEEL ON\rSET CUTTER 1\r@LABEL=2\rCLS\rTEXT 40,30,\"2\",0,1,1,\"%s\"\rTEXT 280,30,\"1\",0,1,1,FORMAT$(NOW,\"dd/mm/yy hh:nn AM/PM\")\rBAR 20,60,420,3\rTEXT 40,75,\"D.FNT\",0,1,1,\"PRODUCTO:\"\rTEXT 280,75,\"2\",0,1,1,\"%s\"\rTEXT 40,105,\"D.FNT\",0,1,1,\"CONSECUTIVO:\"\rTEXT 280,105,\"2\",0,1,1,@LABEL\rTEXT 40,135,\"D.FNT\",0,1,1,\"LOTE:\"\rTEXT 280,135,\"2\",0,1,1,\"%s\"\rTEXT 40,185,\"2\",0,1,1,\"PESO (KG):\"\rTEXT 40,235,\"5.EFT\",0,1,1,\"%s\"\rPUTBMP 740,165, \"Logo.bmp\",1,60\rPRINT 1\rEND\r\n",titulo,bufProducto,bufLote,weight);
      Serial1.print(bufferMensajeImpresora2);
      Serial.println(bufferMensajeImpresora2);
    }
    
    
  }
  

}
  
}

// Attach the interrupt handler to the SERCOM
void SERCOM3_Handler()
{

  mySerial.IrqHandler();
}