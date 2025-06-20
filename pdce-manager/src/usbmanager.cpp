#include "usbmanager.h"
#include "Arduino.h"

extern int usbstatus;
extern int wakeStatus;

void USB::resume(){

    //USBDevice.attach();
    //power_usb_enable();
    //USBCON &= ~_BV(USBE);   // disable USB
    PLLCSR |= (1 << PLLE);    // PLLCSR.PLLE = 1; // Enable_pll();
    while ( (PLLCSR & (1 << PLOCK)) == 0) {}  // while (PLLCSR.PLOCK != 1);   // while (!Pll_ready());

    USBCON &= ~(1 << FRZCLK); // USBCON.FRZCLK = 0;   // Usb_unfreeze_clock();
    //UHWCON |= (1<<UVREGE);      // power internal reg

    //TRY THIS ON ANDROID - DOESNT WORK
    //USBCON = (USBCON & (1<<OTGPADE)) | ~(1<<FRZCLK);

    UDIEN &= -(1 << SUSPE);

    //Sends Remote wake-up signal to host.Not needed in Android.
    //UDCON |= (1 << RMWKUP);

    usbstatus = 1;

    //digitalWrite(9,LOW);
    wakeStatus = 0;
    return;
}

void USB::suspend(){
    /* USB suspend:
    - Clear suspend bit.
    - Freeze USB clock.
    - Disable PLL.
    - Enable interrupts so processor can wake.
    - Enter sleep mode.

    */
    UDIEN |= (1 << SUSPE); //Clear suspend bit
    UDINT &= -(1 << SUSPI);

    USBCON |= _BV(FRZCLK);  //freeze USB clock

    PLLCSR &= ~_BV(PLLE);   // turn off USB PLL
    //interrupts();
    //USBCON &= ~_BV(USBE);   // disable USB

    UDINT &= ~((1 << WAKEUPI) | (1 << SUSPI)); // // clear any already pending WAKEUP IRQs and the SUSPI request
    //interrupts();
    usbstatus = 2;
    wakeStatus = 1;
    return;
}