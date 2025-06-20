#ifndef MEMORY_H
#define MEMORY_H

void EEPROMConfig();
void readEEPROM();
int readBootStatus();
int readMode();
int storeBootStatus(int boot_status);
int storeSetMode(int set_mode);
void storeBatThresh(int highthresh, int lowthresh);

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