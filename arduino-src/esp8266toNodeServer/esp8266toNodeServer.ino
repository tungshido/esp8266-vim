#include <ESP8266WiFi.h>
#include "SerialCommands.h"
#include <ArduinoJson.h>
#include <SocketIOClient.h>

SocketIOClient client;

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", ",");

//const char* ssid = "colgate";
//const char* password = "colgate1";
const char* ssid = "VCP_Warehouse";
const char* password = "mj4kh-Q3oH-rpyzXw-qgmYZrq";

//const char* ssid = "Tung";
//const char* password = "123456789";
char host[] = "10.91.36.21"; //web server ip address
int port = 3000;//web server port

long interval = 15000;
unsigned long previousMillis = 0;
uint16_t data = 0;
unsigned long cycletime = 0;
unsigned int cycleCounter = 0;
uint8_t machine = 0;
char* machine_str;

//This is the default handler, and gets called when no other command matches.
void cmd_unrecognized(SerialCommands* sender, const char* cmd) {
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

//expects one single parameter
void cmd_cycletime_read(SerialCommands* sender) {
  //Note: Every call to Next moves the pointer to next parameter
  machine_str = sender->Next();
  if (machine_str == NULL)
  {
    sender->GetSerial()->println("ERROR NO_PORT");
    return;
  }

  //machine = atoi(machine_str);

  //Note: Every call to Next moves the pointer to next parameter
  char* cycletime_str = sender->Next();
  if (cycletime_str == NULL) {
    sender->GetSerial()->println("ERROR NO_PIN");
    return;
  }

  cycletime = atoi(cycletime_str);

  char* cycleCount_str = sender->Next();
  if (cycletime_str == NULL) {
    sender->GetSerial()->println("ERROR NO_PIN");
    return;
  }

  cycleCounter = atoi(cycleCount_str);

  sender->GetSerial()->print("MACHINE: ");
  sender->GetSerial()->print(machine_str);
  sender->GetSerial()->print("CYCLE TIME = ");
  sender->GetSerial()->print(cycletime);
  sender->GetSerial()->print("CYCLE Counter = ");
  sender->GetSerial()->print(cycleCounter);
  sender->GetSerial()->println();
}

SerialCommand cmd_cycletime_read_("CT", cmd_cycletime_read);

void setup() {
  Serial.begin(9600);

  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&cmd_cycletime_read_);
  delay(10);
  Serial.print("Ket noi vao mang ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Thoát ra khỏi vòng
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.println(F("Da ket noi WiFi"));
  Serial.println(F("Dia chi IP cua ESP8266 (Socket Client ESP8266): "));
  Serial.println(WiFi.localIP());
  if (!client.connect(host, port)) {
    Serial.println(F("Ket noi den socket server that bai!"));
  }
  if (client.connected()) {
    client.send("connected", "message", "esp8266 connected");
  }
}


// This is the main method where data gets pushed to the NodeJS web server
void postData(String json) {
  if (!client.connected()) {
    Serial.println("Connecting to client again…");
    client.connect(host, port);
  }
  Serial.println(json);
  client.sendJSON("cycleTime", json);
}

void loop() {
  serial_commands_.ReadSerial();
  //if (cycletime > 0) {
    String json;
    DynamicJsonDocument cycleData(10000);
    JsonObject root = cycleData.to<JsonObject>();
    String machineStr = String(machine_str);
    root["machineName"] = machineStr;
    //String randNumber = String(cycletime);
    root["cycleTime"] = String(cycletime);
    serializeJson(root, json);
    if (millis() - previousMillis > interval) {
    previousMillis = millis();
    postData(json);
    cycletime = 0;
  }
  if (!client.connected()) {
    client.reconnect(host, port);
  }
}
