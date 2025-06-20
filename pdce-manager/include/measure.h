#ifndef MEASURE_H
#define MEASURE_H


//Invokes measurents of VPV, VBAT, IPV, Analog and Digital
class Measure{
    private:
        const float filteralpha = 0.2;
        const int vbatadc = 18;       // Read battery voltage
        const int ipvadc = 19;        // Read PV current (ZXCT1110)
        const int vpvadc = 20;        // Read PV voltage
        const int analogtempadc = 23;      // Read analog temperature (LMT87)
        int vbatsense = 0;
        int ipvsense = 0;
        int vpvsense = 0;
        int vbat_new = 0;
        int ipv_new = 0;
        int vpv_new = 0;
        int vbat_ant = 12600;
        int ipv_ant = 0;
        int vpv_ant = 0;

    public:
        void measure();
        int VBAT();
        int VPV();
        int IPV();
        int temperature();
        int gpios();

};



#endif