#ifndef MEMORY_H
#define MEMORY_H

#include "rpcparser.h"
#include "ArduinoJson.h"
#include "JsonAdapterRPC.h"

void EEPROMConfig();
void readEEPROM();
int readBootStatus();
void readMode();
int storeBootStatus(int boot_status);
void storeSetMode(int set_mode);
void storeConfig(JsonObject params);

struct MemStruct{
    int address_fw;
    int address_hbth;
    int address_lbth;
    int address_bon;
    int address_boff;
    int address_vpvf;
    int address_ipvf;
    int address_vbatf;
    int address_boot_status;  
    int address_mode;
};

#endif