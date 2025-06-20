#include "measure.h"
#include "Arduino.h"

extern int vpvfactor;
extern int ipvfactor;
extern int vbatfactor;
extern int itemp;
extern const int analogtempadc;
extern int pdce_temp;
extern int vbat;
extern int vpv;
extern int ipv;

extern const int wakepin;
extern const int coinpin;
extern const int safedoorpin;
extern const int maindoor;
extern const int isawake;
extern const int spare1;


extern int wakepinstatus;
extern int coinpinstatus;
extern int maindoorstatus;;
extern int safedoorpinstatus;
extern int isawakepinstatus;
extern int spare1pinstatus;

void Measure::measure(){

    VBAT();
    VPV();
    IPV();
    temperature();
    gpios();
    
}

int Measure::VBAT(){
    vbatsense = analogRead(vbatadc); //vbatadc
    vbat_new = vbatsense * (vbatfactor * 0.1);
    vbat = filteralpha * vbat_ant + (1 - filteralpha) * vbat_new;
    vbat_ant = vbat;
    return vbat;
}

int Measure::VPV(){
    vpvsense = analogRead(vpvadc);
    vpv_new = vpvsense * (vpvfactor * 0.1);
    vpv = filteralpha * vpv_ant + (1 - filteralpha) * vpv_new;
    vpv_ant = vpv;
    vpv = (vpv - 2750) * 10 + vbat;
    return vpv;
}

int Measure::IPV(){
    ipvsense = analogRead(ipvadc);
    ipv_new = ipvsense * (ipvfactor * 0.1);
    ipv = filteralpha * ipv_ant + (1 - filteralpha) * ipv_new;
    ipv_ant = ipv;
    return ipv;
}

int Measure::temperature(){
    itemp = analogRead(analogtempadc);
    itemp = itemp  * 36.28;
    pdce_temp = 19292 - itemp;
    return pdce_temp;

}

int Measure::gpios(){
    wakepinstatus = digitalRead(wakepin);
    coinpinstatus = digitalRead(coinpin);
    safedoorpinstatus = digitalRead(safedoorpin);
    maindoorstatus = digitalRead(maindoor);
    isawakepinstatus = digitalRead(isawake);

    return wakepinstatus;
    return coinpinstatus;
    return safedoorpinstatus;
    return maindoorstatus;
    return isawakepinstatus;

}
