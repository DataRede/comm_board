#include "memory.h"
#include "EEPROMex.h"
//#include "vars.h"

extern const int memBase;
extern int highbatth;
extern int lowbatth;
extern int buzzerOnperiod;
extern int buzzerOffperiod;
extern int vpvfactor;
extern int ipvfactor;
extern int vbatfactor;
extern int fw;
extern int boot_status;
extern int set_mode;

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
  if (local_fw != 4) {
    EEPROM.writeInt(Memo.address_fw, 4); //Add Firmware version here.
    EEPROM.writeInt(Memo.address_hbth, 12500);
    EEPROM.writeInt(Memo.address_lbth, 12000);
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
  #ifdef DEBUG
  Serial1.println("READ EEPROM VALUES");
  Serial1.print("FW: ");
  Serial1.println(fw);
  Serial1.print("HBTTH: ");
  Serial1.println(highbatth);
  Serial1.print("LBTTH: ");
  Serial1.println(lowbatth);
  Serial1.print("BON: ");
  Serial1.println(buzzerOnperiod);
  Serial1.print("BOFF: ");
  Serial1.println(buzzerOffperiod);
  Serial1.print("VPVF: ");
  Serial1.println(vpvfactor);
  Serial1.print("IPVF: ");
  Serial1.println(ipvfactor);
  Serial1.print("VBATF: ");
  Serial1.println(vbatfactor);
  Serial1.print("BOOT_STATUS: ");
  Serial1.println(boot_status);
  Serial1.print("FUNCTION_MODE: ");
  Serial1.println(set_mode);

  Serial1.println("FINISH READING!");
  #endif
}

void readEEPROM() {
  highbatth = EEPROM.readInt(Memo.address_hbth);
  lowbatth = EEPROM.readInt(Memo.address_lbth);
  buzzerOnperiod = EEPROM.readInt(Memo.address_bon);
  buzzerOffperiod = EEPROM.readInt(Memo.address_boff);
  vpvfactor = EEPROM.readInt(Memo.address_vpvf);
  ipvfactor = EEPROM.readInt(Memo.address_ipvf);
  vbatfactor = EEPROM.readInt(Memo.address_vbatf);
  fw = EEPROM.readInt(Memo.address_fw);
  boot_status = EEPROM.readInt(Memo.address_boot_status);
  set_mode = EEPROM.readInt(Memo.address_mode);
}

void storeBatThresh(int highthresh, int lowthresh){
    EEPROM.writeInt(Memo.address_hbth, highthresh);
    EEPROM.writeInt(Memo.address_lbth, lowthresh);
    
    highbatth = highthresh;
    lowbatth = lowthresh;
    return;
}

int storeBootStatus(int boot_status){
  EEPROM.writeInt(Memo.address_boot_status, boot_status);
  return boot_status;
}

int readBootStatus(){
  boot_status = EEPROM.readInt(Memo.address_boot_status);
  storeBootStatus(0);
  return boot_status;
}

int storeSetMode(int set_mode){
  EEPROM.writeInt(Memo.address_mode, set_mode);
  return set_mode;
}

int readMode(){
  set_mode = EEPROM.readInt(Memo.address_mode);
  return set_mode;
}


