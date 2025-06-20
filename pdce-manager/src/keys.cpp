#include <keys.h>
#include <Keyboard.h>
#include <main.h>

unsigned long past_time = 0;
unsigned long time;

#define powerkey KEY_F11
#define coinkey KEY_F12


void sendPwrKey() {
  
  //ignore validation of online parking meter
  
  //int isawakepinstatus = digitalRead(isawake);
  //if(!isawakepinstatus){
    time = millis();
    if(time < past_time){
      past_time = time;
    }

    //New tablet wakeup time 3s
    if (time - past_time > 2000){
      past_time = time;
      buzzerbuttonflag = 1;
      digitalWrite(spare1, HIGH);
      delay(300);
      digitalWrite(spare1, LOW);
      delay(10);
      //Sends button and places the wakeupflag @1.
      //wakeupflag = 0;  //changing to 0, to always send the button 15042025
      past_startup_time = millis();
    }
  //}

}



// Function to send power key to wake up the tablet and then send F12 to send the application to coin payment activity.
void sendCoinKey() {
  //int isawakepinstatus = digitalRead(isawake);
  /*if(!isawakepinstatus){
    time = millis();*/
    if(time < past_time){
      past_time = time;
    }
    if (time - past_time > 2000){
      past_time = time;
      buzzerbuttonflag = 1; 
      
      digitalWrite(spare1, HIGH);
      delay(300);
      digitalWrite(spare1, LOW);
      delay(10);
      //Sends button and places the wakeupflag @1.
      //wakeupflag = 0; //changing to 0, to always send the button 15042025
      past_startup_time = millis();
    }
  //}
  digitalWrite(spare1, HIGH);
  delay(300);
  digitalWrite(spare1, LOW);
  delay(10);
  delay(1500);
  Keyboard.press(coinkey);
  delay(250);
  Keyboard.release(coinkey);
  delay(100);
  Keyboard.press(coinkey);
  delay(250);
  Keyboard.release(coinkey);
  delay(10);
  
}


void sendRestartKey(){
  buzzerbuttonflag = 2; //CHANGE SET MODE FOR PDCE v3.0 REVA
    if(set_mode==0){
      //Testing compatibility with old system
      digitalWrite(batterycontrol, LOW);
      delay(2000);
      digitalWrite(batterycontrol, HIGH);
  }
  else if(set_mode==1){
      /*digitalWrite(spare1, HIGH);
      delay(5000);
      digitalWrite(spare1, LOW);
      delay(1000);
      */
      digitalWrite(spare1, HIGH);
      delay(1000);
      digitalWrite(spare1, LOW);
      delay(1000);
  }
}