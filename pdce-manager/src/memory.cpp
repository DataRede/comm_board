#include "memory.h"
#include "EEPROMex.h"
#include "main.h"

extern const int memBase;

extern DeviceConfig config;

const int memBase = 512;

struct MemStruct Memo;
//Store default data in EEPROM on first run
void EEPROMConfig() {

  EEPROM.setMemPool(memBase, EEPROMSizeATmega32u4);

  Memo.address_fw = EEPROM.getAddress(sizeof(int));
  Memo.address_hbth = EEPROM.getAddress(sizeof(int));
  Memo.address_lbth = EEPROM.getAddress(sizeof(int));
  Memo.address_bon = EEPROM.getAddress(sizeof(int));
  Memo.address_boff = EEPROM.getAddress(sizeof(int));
  Memo.address_vpvf = EEPROM.getAddress(sizeof(int));
  Memo.address_ipvf = EEPROM.getAddress(sizeof(int));
  Memo.address_vbatf = EEPROM.getAddress(sizeof(int));
  Memo.address_boot_status = EEPROM.getAddress(sizeof(int));
  Memo.address_mode = EEPROM.getAddress(sizeof(int));

  int local_fw = EEPROM.readInt(Memo.address_fw);
  if (local_fw < 5) {
    EEPROM.writeInt(Memo.address_fw, 5); //Add Firmware version here.
    EEPROM.writeInt(Memo.address_hbth, 12600);
    EEPROM.writeInt(Memo.address_lbth, 12200);
    EEPROM.writeInt(Memo.address_bon, 50);
    EEPROM.writeInt(Memo.address_boff, 2500);
    EEPROM.writeInt(Memo.address_vpvf, 49);
    EEPROM.writeInt(Memo.address_ipvf, 49);
    EEPROM.writeInt(Memo.address_vbatf, 152);
    EEPROM.writeInt(Memo.address_boot_status, 0);
    EEPROM.writeInt(Memo.address_mode, 0);
  }

  //Checks EEPROM Values - Remove in production.
  //Make withing DEBUG TAG
  readEEPROM();
}

void readEEPROM() {
  config.highbatth = EEPROM.readInt(Memo.address_hbth);
  config.lowbatth = EEPROM.readInt(Memo.address_lbth);
  config.buzzerOnperiod = EEPROM.readInt(Memo.address_bon);
  config.buzzerOffperiod = EEPROM.readInt(Memo.address_boff);
  config.vpvfactor = EEPROM.readInt(Memo.address_vpvf);
  config.ipvfactor = EEPROM.readInt(Memo.address_ipvf);
  config.vbatfactor = EEPROM.readInt(Memo.address_vbatf);
  config.fw = EEPROM.readInt(Memo.address_fw);
  config.boot_status = EEPROM.readInt(Memo.address_boot_status);
  config.set_mode = EEPROM.readInt(Memo.address_mode);
}

void storeConfig(JsonObject params){
  // High battery threshold
  if (params["params"].containsKey("hbth")) {
    config.highbatth = params["params"]["hbth"];
    EEPROM.updateInt(Memo.address_hbth, config.highbatth);
  }

  // Low battery threshold
  if (params["params"].containsKey("lbth")) {
    config.lowbatth = params["params"]["lbth"];
    EEPROM.updateInt(Memo.address_lbth, config.lowbatth);
  }

  // Buzzer on period
  if (params["params"].containsKey("bon")) {
    config.buzzerOnperiod = params["params"]["bon"];
    EEPROM.updateInt(Memo.address_bon, config.buzzerOnperiod);
  }

  // Buzzer off period
  if (params["params"].containsKey("boff")) {
    config.buzzerOffperiod = params["params"]["boff"];
    EEPROM.updateInt(Memo.address_boff, config.buzzerOffperiod);
  }

  // VPV factor
  if (params["params"].containsKey("vpvf")) {
    config.vpvfactor = params["params"]["vpvf"];
    EEPROM.updateInt(Memo.address_vpvf, config.vpvfactor);
  }

  // IPV factor
  if (params["params"].containsKey("ipvf")) {
    config.ipvfactor = params["params"]["ipvf"];
    EEPROM.updateInt(Memo.address_ipvf, config.ipvfactor);
  }

  // VBAT factor
  if (params["params"].containsKey("vbatf")) {
    config.vbatfactor = params["params"]["vbatf"];
    EEPROM.updateInt(Memo.address_vbatf, config.vbatfactor);
  }

  // Firmware version
  if (params["params"].containsKey("fw")) {
    config.fw = params["params"]["fw"];
    EEPROM.updateInt(Memo.address_fw, config.fw);
  }

  // Mode
  if (params["params"].containsKey("mode")) {
    config.set_mode = params["params"]["mode"];
    EEPROM.updateInt(Memo.address_mode, config.set_mode);
  }
}

int storeBootStatus(int boot_status){
  EEPROM.updateInt(Memo.address_boot_status, boot_status);
  return boot_status;
}

int readBootStatus(){
  config.boot_status = EEPROM.readInt(Memo.address_boot_status);
  storeBootStatus(0);
  return config.boot_status;
}

void storeSetMode(int set_mode){
  EEPROM.updateInt(Memo.address_mode, set_mode);
}

void readMode(){
  config.set_mode = EEPROM.readInt(Memo.address_mode);
}


