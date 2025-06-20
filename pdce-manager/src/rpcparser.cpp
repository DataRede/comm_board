#include "rpcparser.h"
#include "ArduinoJson.h"
#include "JsonAdapterRPC.h"
#include "memory.h"
#include "measure.h"
#include "Arduino.h"

extern Measure measure;
extern struct MemStruct Memo; 

extern int wakeStatus;
extern int goingtosleep;
extern int sendkeyStatus;
extern int usbstatus;
extern int buzzing;
extern int buzzerstate;
extern int buzzerperiod;
extern int buzzerOnperiod;
extern int buzzerOffperiod;

extern int wakepinstatus;
extern int coinpinstatus;
extern int maindoorstatus;;
extern int safedoorpinstatus;
extern int isawakepinstatus;
extern int extradigitalpinstatus;

extern const int wakepin;
extern const int coinpin;
extern const int safedoorpin;
extern const int maindoor;
extern const int isawake;
extern const int extradigital;

extern int highbatth;
extern int lowbatth;
extern int vpvfactor;
extern int ipvfactor;
extern int vbatfactor;
extern int vbat;
extern int vpv;
extern int ipv;
extern int temperature;
extern int chgstatus;

extern int fw;
extern bool messageError;
extern int clearserialbuffer;
extern int buzzerflag;
extern int pdce_temp;
extern int boot_status;
extern int set_mode;
extern int set_color;
extern int fw_version;

//UPDATE WHEN ADDING MORE METHODS
JsonAdapterRPC rpc(14);

const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 80;
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
  buzzerOnperiod  =  rpcjsonrequest["params"]["onPeriod"] | buzzerOnperiod;
  buzzerOffperiod  =  rpcjsonrequest["params"]["offPeriod"] | buzzerOffperiod;
  if (requestedStatus)
  {
    buzzerflag = 1;
    DynamicJsonDocument doc(125);
    doc["jsonrpc"] = "2.0";
    doc["method"] = "alarm";
    JsonObject result = doc.createNestedObject("result");
    result["status"] = true;
    result["on_time"] = buzzerOnperiod;
    result["off_time"] = buzzerOffperiod;

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
  result["vpv"] = vpv;
  result["ipv"] = ipv;
  result["vbat"] = vbat;
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

    wakepinstatus = digitalRead(wakepin);
    coinpinstatus = digitalRead(coinpin);
    safedoorpinstatus = digitalRead(safedoorpin);
    maindoorstatus = digitalRead(maindoor);
    isawakepinstatus = digitalRead(isawake);
    extradigitalpinstatus = digitalRead(extradigital);

}

void getGpio(JsonObject) {

  // measure.gpios();
  // readGpios();
  
  // DynamicJsonDocument doc(125);
  // doc["jsonrpc"] = "2.0";
  // doc["method"] = "getGpio";
  // JsonObject result = doc.createNestedObject("result");
  // result["wp"] = wakepinstatus;
  // result["cp"] = coinpinstatus;
  // result["md"] = maindoorstatus;
  // result["sdp"] = safedoorpinstatus;
  // result["iap"] = isawakepinstatus;
  // result["edp"] = extradigitalpinstatus;

  // msgBegin();
  // serializeJson(doc, Serial1);
  // msgEnd();
  // emptySerial1();

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
  result["pdce_temp"] = pdce_temp;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void setBatThresh(JsonObject) {

  int highthresh  =  rpcjsonrequest["params"]["hbth"] | highbatth;
  int lowthresh  =  rpcjsonrequest["params"]["lbth"] | lowbatth;

  if ((lowthresh + 100) < highthresh && lowthresh > 11000 && highthresh < 13000) {

    storeBatThresh(highthresh, lowthresh);
    // EEPROM.writeInt(Memo.address_hbth, highthresh);
    // EEPROM.writeInt(Memo.address_lbth, lowthresh);

    // highbatth = highthresh;
    // lowbatth = lowthresh;

  }

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setBatThresh";
  JsonObject result = doc.createNestedObject("result");
  result["hbth"] = highbatth;
  result["lbth"] = lowbatth;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

}

void getConfig(JsonObject) {
  

  readEEPROM();
  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "getConfig";
  JsonObject result = doc.createNestedObject("result");
  result["hbth"] = highbatth;
  result["lbth"] = lowbatth;
  result["bon"] = buzzerOnperiod;
  result["boff"] = buzzerOffperiod;
  result["vpvf"] = vpvfactor;
  result["ipvf"] = ipvfactor;
  result["vbatf"] = vbatfactor;
  result["fw"] = fw;
  result["mode"] = set_mode;

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
  result["bootstatus"] = boot_status;

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

  int state = HIGH;
  digitalWrite(extradigital, state);

}

//Send QR GPIO pulse to read code.
void qrPulse(JsonObject){

    //digitalWrite(extradigital,HIGH);

}

void setMode(JsonObject) {

  set_mode  =  rpcjsonrequest["params"]["setmode"] | set_mode;

  storeSetMode(set_mode);

  DynamicJsonDocument doc(125);
  doc["jsonrpc"] = "2.0";
  doc["method"] = "setMode";
  JsonObject result = doc.createNestedObject("result");
  result["setmode"] = set_mode;

  msgBegin();
  serializeJson(doc, Serial1);
  msgEnd();
  emptySerial1();

  readMode();
  
}


void setColor(JsonObject) {

  set_color  =  rpcjsonrequest["params"]["setcolor"] | set_color;

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
        messageError = false;
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
    rpc.registerMethod("setBatThresh", &setBatThresh);
    rpc.registerMethod("getConfig", &getConfig);
    rpc.registerMethod("readCode", &qrPulse);
    rpc.registerMethod("bootStatus", &bootStatus);
    rpc.registerMethod("setMode",&setMode);
    rpc.registerMethod("setColor",&setColor);
    rpc.registerMethod("error",&sendError);

}





