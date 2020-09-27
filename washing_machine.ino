#include <LiquidCrystal.h>                             // Lib for the lcd display
#include <TimerOne.h>


const int pump = 10;    
const int motorCW =  6; // Motor clock wise
const int motorCCW = 9; // Motor counter clock wise
const int soapValve = 7;
const int softenerValve = 8;
const int pressostato = 5;
const int Button1 = 4;
const int Button2 = 2;
const int Button3 = 3;
const int beepPin = 11;
int TankStatus; // 1 is full and 0 is empty
//int timeTankFull = 0; // Time the tank takes to fill
//int timeWashing = 0; // Time spent in washing step

//Menu arrow character
byte arrow[8] = { 
    B10000,
    B11100,
    B11110,
    B11111,
    B11110,
    B11100,
    B10000,
};

//Variables for work time estimation
float totalTime; 
int totalTimeMin;
int timeTankFull;

LiquidCrystal disp(12,  //RS  digital 3
                   13,  //EN  digital 4
                   A0,  //D4  digital 5
                   A1,  //D5  digital 6
                   A2,  //D6  digital 7
                   A3); //D7  digital 8

void setup() {
    disp.createChar(0, arrow); //Menu arrow character
    
    disp.begin(20,4);      // Display initialization
    
    pinMode(pump, OUTPUT);
    pinMode(motorCW, OUTPUT);
    pinMode(motorCCW, OUTPUT);
    pinMode(soapValve, OUTPUT);
    pinMode(softenerValve, OUTPUT);
    pinMode(pressostato, INPUT); // Sensor: If HIGH the tank is full
    pinMode(Button1, INPUT);
    pinMode(Button3, INPUT);
    pinMode(Button2, INPUT);
    Serial.begin(9600);
  
}

void loop() {
    //Turning everything off before starting
    digitalWrite(pump,LOW);
    digitalWrite(motorCW,LOW);
    digitalWrite(motorCCW,LOW);
    digitalWrite(soapValve,LOW);
    digitalWrite(softenerValve,LOW);
    
    menu();


}

void menu(){
    disp.clear();
    disp.setCursor(0,1);
    // Always use F() in prints to store the strings in PROGAM MEMORY 
    // and save RAM. This prevents unstable behavior.
    disp.print(F("  FIRMWARE VERSION  "));
    disp.setCursor(0,2);
    disp.print(F("        1.4 BETA    "));
    delay(2000);
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("Selecione o programa"));
  
    int menuLength = 5;
    int programa = 2;
    int pointerPos = 1;
    int meIndex = 1;
    int statusButton2;
    int statusButton3;
    int statusButton1;
    
    int buttonStateButton2;             // the current reading from the input pin
    int lastButtonStateButton2 = LOW;   // the previous reading from the input pin
    int buttonStateButton3;             // the current reading from the input pin
    int lastButtonStateButton3 = LOW;   // the previous reading from the input pin
    int buttonStateButton1;             // the current reading from the input pin
    int lastButtonStateButton1 = LOW;   // the previous reading from the input pin
  
  
    // the following variables are unsigned longs because the time, measured in
    // milliseconds, will quickly become a bigger number than can be stored in an int.
    unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
    unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
  
    printMenu(meIndex);
  
    while(1 == 1){
        statusButton2 = digitalRead(Button2);
        statusButton3 = digitalRead(Button3);
        statusButton1 = digitalRead(Button1);
  
  
  
        // read the state of the switch into a local variable:
  
  
        // check to see if you just pressed the button
        // (i.e. the input went from LOW to HIGH), and you've waited long enough
        // since the last press to ignore any noise:
  
        // If the switch changed, due to noise or pressing:
        if (statusButton1 != lastButtonStateButton1) {
            // reset the debouncing timer
            lastDebounceTime = millis();
        }
  
        if ((millis() - lastDebounceTime) > debounceDelay) {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:
  
            // if the button state has changed:
            if (statusButton1 != buttonStateButton1) {
                buttonStateButton1 = statusButton1;
  
                if (buttonStateButton1 == HIGH) {
                
                    if (meIndex == 6  ){ meIndex = 0;}
                    meIndex++;   
                    printMenu(meIndex);
                      
                    // Print the current menu to the serial port
                    printMenuItem(meIndex,1);
  
                }
            }
        }
        lastButtonStateButton1 = statusButton1;
  
  //    ---------------------------------------------------------\\
          
        // If the switch changed, due to noise or pressing:
        if (statusButton3 != lastButtonStateButton3) {
          // reset the debouncing timer
          lastDebounceTime = millis();
        }
  
        if ((millis() - lastDebounceTime) > debounceDelay) {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:
  
            // if the button state has changed:
            if (statusButton3 != buttonStateButton3) {
                buttonStateButton3 = statusButton3;
  
                if (buttonStateButton3 == HIGH) {
                    disp.clear();
                    disp.setCursor(0,0);
                    disp.print(F("Programa Selecionado!"));
                    disp.setCursor(0,2);
                    disp.write(byte(0)); //Menu arrow
                    printMenuItem(meIndex,0);
                    delay(2000);
                    programa = meIndex;
                    Serial.print(F("Selected Program = "));
                    printMenuItem(programa,1);
                    break;
                }
  
            }
        }
        lastButtonStateButton3 = statusButton3; 
          
   }
  
  
  
    switch (programa){
      case 1: 
          normalWashing();
          break;
      case 2:
          justCentrifugue();
          break;
      case 3:
          simpleWash();
          break;
      case 4:
          doubleWash();
          break;
      case 5:
          justSoak();
          break;
      case 6:
          delicateWash();
          break;
      
    }
  
  
    
}

void printMenu(int mIndex) {

  for (int i=0;i<3;i++){
      disp.setCursor(1,1+i);
      printMenuItem(mIndex+i,0);
  }
  

  disp.setCursor(0,1);
  //disp.print(F(">"));
   disp.write(byte(0));
  disp.setCursor(0,2);
  disp.print(F(" "));
  disp.setCursor(0,3);
  disp.print(F(" "));

}



void updateTime(int timeC){
    totalTimeMin = (float)1*timeC/(float)60;
    disp.setCursor(0,1);
    disp.print(F("                    "));
    disp.setCursor(0,1);
    disp.print(F("T. Restante: "));
    disp.print(totalTimeMin);
    disp.print(F("min"));
}

void endBeep(){
    disp.setCursor(0,1);
    disp.print(F("                    "));
    disp.setCursor(0,2);
    disp.print(F("    FINALIZADO!     "));
    disp.setCursor(0,3);
    disp.print(F("                    "));
    
    for (int i = 0; i<15; i++){
        beep(2000);
    }
  
    while(1){
        delay(1000);
    }
}

void doubleWash(){
  
    int totalTime;
    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 200;
    const int soakTime = 300;
    const int flushTime = 120;
    const int centrifugationTime = 180;
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("ENXAGUE DUPLO"));
    
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 6        "));
    timeTankFull = fillTank(1);
  
    if (timeTankFull == -1){
        errorTank();
    }
  
    // Estimating cicle time:
    // Wash time
    totalTime = 6*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
  
    // Wash time + centrifugue time (guessing that the tank takes
    // 120s to empty)
    totalTime = totalTime + 2*(flushTime + centrifugationTime) + timeTankFull;
  
    updateTime(totalTime);
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 6        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
  
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 6        "));
    centrifuge();
    totalTime = totalTime - flushTime - centrifugationTime;
    updateTime(totalTime);
  
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 6        "));
    
    timeTankFull = fillTank(2);
    
    if (timeTankFull == -1){
        errorTank();
    }
  
    totalTime = totalTime - timeTankFull;
    updateTime(totalTime);
    
  
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 6        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
  
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    }
  
  
    disp.setCursor(0,2);
    disp.print(F("Passo 6 de 6        "));
    centrifuge();
  
  
    endBeep(); 
  
  
    
    
}
  

void justSoak(){

    int totalTime;
    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 200;
    const int soakTime = 300;
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("DEIXAR DE MOLHO     "));
    
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 2        "));
    timeTankFull = fillTank(1);
  
    if (timeTankFull == -1){
        errorTank();
    }
  
    // Estimating cicle time:
    // Wash time
    totalTime = 3*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
  
    updateTime(totalTime);
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 2        "));

    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    }
  
    endBeep(); 
  
}

void simpleWash(){

    int totalTime;
    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 200;
    const int soakTime = 300;
    const int flushTime = 120;
    const int centrifugueTime = 180;
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("UM ENXAGUE"));
    
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 3        "));
    timeTankFull = fillTank(1);
  
    if (timeTankFull == -1){
        errorTank();
    }
  
    // Estimating cicle time:
    // Wash time
    totalTime = 3*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
  
    // Wash time + centrifugue time (guessing that the tank takes
    // 120s to empty)
    totalTime = totalTime + flushTime + centrifugueTime;
  
    updateTime(totalTime);
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 3        "));

    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
  
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 3        "));
    centrifuge();
  
    endBeep(); 
}

void beep(unsigned char delayms){
    analogWrite(beepPin, 150);  // Almost any value can be used except 0 and 255
                                // experiment to get the best tone
    delay(delayms);             // wait for a delayms ms
    analogWrite(beepPin, 0);    // 0 turns it off
    delay(delayms);             // wait for a delayms ms   
}

  
void justCentrifugue(){

    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("APENAS CENTRIFUGAR   "));
    
    // Estimating cicle time:========================\\
  
    totalTime = (120   + 180);
  
  
    updateTime(totalTime);
    
  
    //===============================================\\
  
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 1        "));
    centrifuge();
    
    endBeep();
}


void printMenuItem(int menuIndex, int printToSerial){
    switch (menuIndex){
        case 1:
            disp.print(F("LAVAGEM COMPLETA   "));
            if ( printToSerial == 1){
                Serial.println(F("Lavagem Completa"));
            }
            break;
            
        case 2:
            disp.print(F("APENAS CENTRIFUGAR "));
            if ( printToSerial == 1){
                Serial.println(F("Apenas Centrifugar"));
            }
            break;
  
        case 3:
            disp.print(F("UM ENXAGUE         "));
            if ( printToSerial == 1 ){
                Serial.println(F("Um Enxague"));
            }
            break;
            
        case 4:
            disp.print(F("ENXAGUE DUPLO      "));
            if ( printToSerial == 1){
                Serial.println(F("Enxague Duplo"));
            }
            break;
  
        case 5:
            disp.print(F("DEIXAR DE MOLHO    "));
            if ( printToSerial == 1){
                Serial.println(F("Deixar de Molho"));
            }
            break;
            
        case 6:
            disp.print(F("DELICADA           "));
            if ( printToSerial == 1){
                Serial.println(F("Delicada"));
            }
            break;
  
        case 7:
            disp.print(F("LAVAGEM COMPLETA   "));
            if ( printToSerial == 1){
                Serial.println(F("Lavagem Completa"));
            }
            break;
  
        case 8:
            disp.print(F("APENAS CENTRIFUGAR "));
            if ( printToSerial == 1){
                Serial.println(F("Apenas Centrifugar "));
            }
            break;
    }
}
 
void centrifuge(){
    Serial.println(F("Flushing the tank"));
    disp.setCursor(0,3);
    disp.print(F("ESVAZIANDO O TANQUE "));
  
    digitalWrite(pump,HIGH); 

    while(digitalRead(pressostato) == 1){
        digitalWrite(pump,HIGH);
    }
    
    // Pressostato is open, wait for 2 min to empty the tank.
    Serial.println(F("Pressostato is open"));
    Serial.println(F("Waiting for 2 min"));
    

    for(int j=0;j<120;j++){
        delay(1000);
    }

    disp.setCursor(0,3);
    disp.print(F("   CENTRIFUGANDO    "));
    Serial.println(F("Centrifugation"));
    
    digitalWrite(motorCCW,HIGH);

    for(int h=0;h<180;h++){
        delay(1000);
    }

    digitalWrite(motorCCW,LOW);
    Serial.println(F("End of centrifugation"));
    digitalWrite(pump,LOW);
}

void errorTank(){
    digitalWrite(soapValve,LOW);
    digitalWrite(softenerValve,LOW);
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("       ERRO!"));
    Serial.println(F("Error: Tank is tanking too long to fill!"));
    disp.setCursor(0,1);
    disp.print(F("Tanque demorando"));
    disp.setCursor(0,2);
    disp.print(F("para encher!"));
  
    for (int i = 0;i<20;i++){
        beep(500);
    }
      
    while(1){
        delay(1000);
    }
}




int fillTank(int whatValve){
    // Fill the tank with water. If whatValve is 1, it uses the soap valve, if
    // not 1, it uses the softener valve
    const int maxTimeToWait = 1800000;
    unsigned long timeTankFull = 0;
    unsigned long timeStartFlood = millis();
    if( whatValve == 1){
        // fill tank using the soap valve
        disp.setCursor(0,3);
        Serial.println(F("Flooding the tank"));
        disp.print(F("INUNDANDO O TANQUE  "));
        digitalWrite(soapValve,HIGH);
    }
    else {
        // fill tank using the softener valve
        Serial.println(F("Opening softener valve"));
        disp.setCursor(0,3);
        disp.print(F("COLOCANDO AMACIANTE "));
        digitalWrite(softenerValve,HIGH);
    }

    /* Uncomment this in case of misdetection of tank level occur.

    // Verify if the tank have some water and let the valve open for 
    // 12 min. This procedure is necessary, because sometimes the 
    // machine started to shake the tank with no water in it, despite
    // the fact that the pressostato was not HIGH. A debug procedure showed
    // that the arduino is randomly ignoring the while loop
    // responsable for monitoring the pressostato state and proceeding to
    // shaking the tank. I could not determine the cause of such behavior.

    // A double verification using the if's ensure the the value
    // was not misread and let the valve open during at least 12 min, which is
    // suficient to elevate the wather level above the lowest pallets.
    if(digitalRead(pressostato) == 0){
        Serial.println(F("Tank is empty, wait for 9 min with soap valve opened."));
        delay(1000);
        if(digitalRead(pressostato) == 0){
        while (millis() - timeStartFlood < 540000){
        delay(1000);// This while loop waits for 9 min
        }
        }
    }
    */
    

    // This while loop monitors the pressostato state
    while(millis() - timeStartFlood < maxTimeToWait){
        if(digitalRead(pressostato) == 1){
            delay(1500);
            Serial.println(F("Is the tank really full?"));

            if(digitalRead(pressostato) == 1){
                timeTankFull = millis() - timeStartFlood;
                Serial.println(F("Yes, the tank is full."));
                break;
            }
            else{
                Serial.println(F("No, it is not."));
                continue;
            }
	}
        //Serial.println(millis() - timeStartFlood);

    }
    if (millis() - timeStartFlood >= maxTimeToWait){
        return -1;
    }

    if ( whatValve == 1) {
        digitalWrite(soapValve,LOW);
        Serial.println(F("Closing soap valve"));
    }
    else{
        digitalWrite(softenerValve,LOW);
        Serial.println(F("Closing softener valve"));
    }

    Serial.print(F("Tempo = "));
    int timeTankFullInInt = (float)1*timeTankFull/(float)1000;
    Serial.println(timeTankFullInInt);
    disp.setCursor(0,3);
    disp.print(F("                    "));
    disp.setCursor(0,3);
    disp.print(F("TANQUE CHEIO "));
    int dispTank = (float)1*timeTankFull/(float)60000;
    disp.print(dispTank);
    disp.print(F("min"));
    Serial.print(F("Tempo in Min: "));
    Serial.println(dispTank);

    delay(5000);
    return timeTankFullInInt;
  
}


void wash(int hitsNumber, int spinTime, int pauseTime, int soakTime, int currentStep, int totalSteps){
    // spinTime and pauseTime determine the behavior of the blades and are in
    // milliseconds

    // soakTime is in seconds
    
    // totalSteps can not have more than one digit, because that would overflow
    // the LCD display

    disp.setCursor(0,3);
    disp.print(F("ENXAGUE:   BATER "));
    disp.print(currentStep); 
    disp.print(F("/"));
    disp.print(totalSteps);
    Serial.print(F("Shaking "));
    Serial.print(currentStep);
    Serial.print(F(" of "));
    Serial.println(totalSteps);
    

    while (hitsNumber > 0){
        digitalWrite(motorCW,HIGH);
        delay(spinTime);
        digitalWrite(motorCW,LOW);
        delay(pauseTime);
        digitalWrite(motorCCW,HIGH);
        delay(spinTime);
        digitalWrite(motorCCW, LOW);
        delay(pauseTime);
  
        hitsNumber = hitsNumber - 1;
    }

    disp.setCursor(0,3);
    disp.print(F("ENXAGUE:   MOLHO "));
    disp.print(currentStep);
    disp.print(F("/"));
    disp.print(totalSteps);
    Serial.print(F("Soak "));
    Serial.print(currentStep);
    Serial.print(F(" of "));
    Serial.println(totalSteps);

    for(int j=0;j<=soakTime;j++){
        delay(1000);
    }
     
       
}


void normalWashing(){
    const int hitsNumber = 60;
    const int spinTime = 400;
    const int pauseTime = 200;
    const int soakTime = 300;
    const int flushTime = 120; // Additional time to flush the tank after pressure switch is comutted
    const int centrifugationTime = 180;
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("LAVAGEM COMPLETA    "));
    
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }

    // Estimating cicle time:========================
    // Wash time
    totalTime = ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + 900); // The first soakTime step has longer duration
    totalTime = totalTime+8*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
    //totalTime = 9.*(2.*60.*(450. + 200.)/1000. + 300.);
    // Wash time + centrifugue time (assuming that the tank takes
    // 60s to empty)
  
    totalTime = totalTime + 3*(flushTime + centrifugationTime);
  
    // Wash time + tankFill
    totalTime = totalTime + 2*timeTankFull;
  
    updateTime(totalTime);
    //===============================================
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 9        "));
    
    wash(hitsNumber, spinTime, pauseTime, 900, 1, 3);
    totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + 900);
    updateTime(totalTime);
    
    
  
    
    for (int i=0;i<2;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 2, 3);
        
        // Update Time ==============================
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        
  
        updateTime(totalTime);
        
        //===========================================
  
  
        
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 9        "));
    centrifuge();
  
    // Update Time ==============================
    totalTime = totalTime - (flushTime + centrifugationTime);
    updateTime(totalTime);
        
    //===========================================
    
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }
  
    
    // Update Time ==============================
    totalTime = totalTime - timeTankFull;
    updateTime(totalTime);
        
    //===========================================
    
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
        
        // Update Time ==============================
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
        //===========================================
  
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 6 de 9        "));
    centrifuge();
    // Update Time ==============================
    totalTime = totalTime - (flushTime + centrifugationTime);
    updateTime(totalTime);
        
    //===========================================
  
    disp.setCursor(0,2);
    disp.print(F("Passo 7 de 9        "));
    timeTankFull = fillTank(2);

    if (timeTankFull == -1){
        errorTank();
    }

      
    // Update Time ==============================
    totalTime = totalTime - timeTankFull;
    updateTime(totalTime);
        
    //===========================================
  
  
    disp.setCursor(0,2);
    disp.print(F("Passo 8 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
        // Update Time ==============================
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
        
        //===========================================
        
  
    } 
  
    disp.setCursor(0,2);
    disp.print(F("Passo 9 de 9        "));
    centrifuge();
  
    endBeep();
  
  
    
    
}


void delicateWash()
{
    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 1000;
    const int soakTime = 300;
    const int flushTime = 120;
    const int centrifugationTime = 180;
    disp.clear();
    disp.setCursor(0,0);
    disp.print(F("LAVAGEM DELICADA"));
  
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }
 
    // Estimating cicle time:========================
    // Wash time
    totalTime = ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + 900); // The first soakTime step has longer duration
    totalTime = totalTime + 8*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
    // Wash time + centrifugue time (guessing that the tank takes
    // 120s to empty)
  
    totalTime = totalTime + 3*(flushTime + centrifugationTime);
  
    // Wash time + tankFill
    totalTime = totalTime + 2*timeTankFull;
  
    updateTime(totalTime);
    //===============================================
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 9        "));
    
    wash(hitsNumber, spinTime, pauseTime, 900,1,3);
    totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + 900);
    updateTime(totalTime);
      
    for (int i=0;i<2;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 2,3);
  
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
  
  
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 9        "));
    centrifuge();
  
    // Update Time ==============================
    totalTime = totalTime - (flushTime + centrifugationTime);
    updateTime(totalTime);
    //===========================================
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }

    
    // Update Time ==============================
    totalTime = totalTime - timeTankFull;
    updateTime(totalTime);
        
    //===========================================
    
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
        
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 6 de 9        "));
    centrifuge();
    // Update Time ==============================
    totalTime = totalTime - (flushTime + centrifugationTime);
    updateTime(totalTime);
  
    disp.setCursor(0,2);
    disp.print(F("Passo 7 de 9        "));
    timeTankFull = fillTank(2);

    if (timeTankFull == -1){
        errorTank();
    }

    
    // Update Time ==============================
    totalTime = totalTime - timeTankFull;
    updateTime(totalTime);
        
    //===========================================
  
    disp.setCursor(0,2);
    disp.print(F("Passo 8 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
        
        totalTime = totalTime - ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
        updateTime(totalTime);
    } 
  
    disp.setCursor(0,2);
    disp.print(F("Passo 9 de 9        "));
    centrifuge();
  
    endBeep();
  
  
    
  }
  
