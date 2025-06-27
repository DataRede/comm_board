#include "rpcparser.h"
#include "ArduinoJson.h"
#include "JsonAdapterRPC.h"
#include "memory.h"
#include "measure.h"
#include "Arduino.h"

extern Measure measure;
extern struct MemStruct Memo; 

extern int buzzerstate;
extern int buzzerperiod;

extern SensorReadings sensors;
extern GPIOStatus gpio;
extern DeviceConfig config;
extern int chgstatus;

extern int clearserialbuffer;
extern int buzzerflag;
extern int set_color;

//UPDATE WHEN ADDING MORE METHODS
JsonAdapterRPC rpc(14);

const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 125;
DynamicJsonDocument rpcjsonrequest(capacity);
DynamicJsonDocument rpcjsonresponse(capacity);
StaticJsonDocument<256> obj;

void msgBegin() {
  Serial1.write(0x02);
}

void msgEnd() {
  Serial1.write(0x03);
}

void jsonrpcparser() {

  JsonObject obj = rpcjsonrequest.as<JsonObject>();

  rpc.processMessage(obj);

}

void emptySerial1() {
  //Emptys Serial1
  while (Serial1.available()) {
    clearserialbuffer = Serial1.read();
  }

  delay(10);
}

void setAlarm(JsonObject)
{
  boolean requestedStatus = rpcjsonrequest["params"]["status"];
  if (rpcjsonrequest["params"].containsKey("onPeriod")) {
    config.buzzerOnperiod = rpcjsonrequest["params"]["onPeriod"];
  }
  if (rpcjsonrequest["params"].containsKey("offPeriod")) {
    config.buzzerOffperiod = rpcjsonrequest["params"]["offPeriod"];
  }

  if (requestedStatus)
  {
    buzzerflag = 1;
    DynamicJsonDocument doc(125);
    doc["jsonrpc"] = "2.0";
    doc["method"] = "alarm";
    JsonObject result = doc.createNestedObject("result");
    result["status"] = true;
    result["on_time"] = config.buzzerOnperiod;
    result["off_time"] = config.buzzerOffperiod;

    msgBegin();
    serializeJson(doc, Serial1);
    msgEnd();
    emptySerial1();
  }
  else
  {
    buzzerflag = 0;
    DynamicJsonDocument doc(125);
    doc["jsonrpc"] = "2.0";
    doc["method"] = "alarm";
    JsonObject result = doc.createNestedObject("result");
    result["status"] = false;

    msgBegin();
    serializeJson(doc, Serial1);
    msgEnd();
    emptySerial1();
  }
}

void sendPower(JsonObject) {

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  JsonObject result = doc.createNestedObject("result");
  result["vpv"] = sensors.vpv;
  result["ipv"] = sensors.ipv;
  result["vbat"] = sensors.vbat;
  result["status"] = chgstatus;


  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void notificationLed(JsonObject) {

  //Implement led notification - color, pwm or 2 led.
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "notificationLed";
  doc["result"] = "OK";

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void readGpios() {

  gpio = measure.measureGPIOs();

}

void getGpio(JsonObject) {

  readGpios();
  
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "getGpio";
  JsonObject result = doc.createNestedObject("result");
  result["wp"] = gpio.wake;
  result["cp"] = gpio.coin;
  result["psr"] = gpio.pmicStbyReq;
  result["dp"] = gpio.door;
  result["iap"] = gpio.isAwake;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}


void setGpio(JsonObject) {

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setGpio";
  doc["result"] = "OK";

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void getAnalog(JsonObject) {

  //Read extra analog pin.
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "getAnalog";
  doc["result"] = "OK";

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void getTemp(JsonObject) {

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "getTemp";
  JsonObject result = doc.createNestedObject("result");
  result["pdce_temp"] = sensors.temperature;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void setConfig(JsonObject) {

  JsonObject params = rpcjsonrequest.as<JsonObject>();;

  storeConfig(params);

  DynamicJsonDocument doc(150);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setConfig";
  JsonObject result = doc.createNestedObject("result");
  result["hbth"] = config.highbatth;
  result["lbth"] = config.lowbatth;
  result["bon"] = config.buzzerOnperiod;
  result["boff"] = config.buzzerOffperiod;
  result["vpvf"] = config.vpvfactor;
  result["ipvf"] = config.ipvfactor;
  result["vbatf"] = config.vbatfactor;
  result["fw"] = config.fw;
  result["mode"] = config.set_mode;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void getConfig(JsonObject) {
  

  readEEPROM();
  DynamicJsonDocument doc(150);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "getConfig";
  JsonObject result = doc.createNestedObject("result");
  result["hbth"] = config.highbatth;
  result["lbth"] = config.lowbatth;
  result["bon"] = config.buzzerOnperiod;
  result["boff"] = config.buzzerOffperiod;
  result["vpvf"] = config.vpvfactor;
  result["ipvf"] = config.ipvfactor;
  result["vbatf"] = config.vbatfactor;
  result["fw"] = config.fw;
  result["mode"] = config.set_mode;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();
}

void bootStatus(JsonObject) {
  
  readBootStatus();
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "bootStatus";
  JsonObject result = doc.createNestedObject("result");
  result["bootstatus"] = config.boot_status;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();
  storeBootStatus(0);
}

void responseRPC(bool type) {

  if (!type) {
    rpcjsonresponse["jsonrpc"] = "2.0";
    rpcjsonresponse["error"] = "ERROR";
    msgBegin();
    serializeJson(rpcjsonresponse, Serial1);
    msgEnd();
    rpcjsonresponse.clear();
    emptySerial1();
  }
  else {
    jsonrpcparser();
    //rpcjsonresponse["jsonrpc"] = "2.0";
    //rpcjsonresponse["result"] = "OK";
    //rpcjsonresponse["id"] = current_id;
  }
}




void setGpios() {

  //int state = HIGH;
  //digitalWrite(extradigital, state);

}

//Send QR GPIO pulse to read code.
void qrPulse(JsonObject){

    //digitalWrite(extradigital,HIGH);

}

void setMode(JsonObject) {

  if (rpcjsonrequest["params"].containsKey("setmode")) {
    config.set_mode = rpcjsonrequest["params"]["setmode"];
  }

  storeSetMode(config.set_mode);
  readMode();

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setMode";
  JsonObject result = doc.createNestedObject("result");
  result["setmode"] = config.set_mode;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();
  
}


void setColor(JsonObject) {

  if (rpcjsonrequest["params"].containsKey("setcolor")) {
    set_color = rpcjsonrequest["params"]["setcolor"];
  }

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setColor";
  JsonObject result = doc.createNestedObject("result");
  result["setcolor"] = set_color;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();
  
}

void sendError(JsonObject) {
  
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  JsonObject result = doc.createNestedObject("error");
  result["code"] = -32601;
  result["message"] = "Method not found";

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();
}

void parseJson(){
    DeserializationError error = deserializeJson(rpcjsonrequest, Serial1);
      emptySerial1();


      // Test if parsing succeeds.
      if (error) {
        responseRPC(false);
      }
      else {
        responseRPC(true);
      }

}


void methodRegistration(){
    rpc.registerMethod("alarm", &setAlarm);
    rpc.registerMethod("powerStatus", &sendPower);
    rpc.registerMethod("ledNotification", &notificationLed);
    rpc.registerMethod("getGpio", &getGpio);
    rpc.registerMethod("setGpio", &setGpio);
    rpc.registerMethod("getAnalog", &getAnalog);
    rpc.registerMethod("getTemp", &getTemp);
    rpc.registerMethod("setConfig", &setConfig);
    rpc.registerMethod("getConfig", &getConfig);
    rpc.registerMethod("readCode", &qrPulse);
    rpc.registerMethod("bootStatus", &bootStatus);
    rpc.registerMethod("setMode",&setMode);
    rpc.registerMethod("setColor",&setColor);
    rpc.registerMethod("error",&sendError);

}





