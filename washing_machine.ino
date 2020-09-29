#include <LiquidCrystal.h>                             // Lib for the lcd display
#include <TimerOne.h>


const int pump = 10;    
const int motorCW =  6; // Motor clock wise
const int motorCCW = 9; // Motor counter clock wise
const int soapValve = 7;
const int softenerValve = 8;
const int pressostato = 5;
const int Button1 = 4; // Button Select
const int Button2 = 2; 
const int Button3 = 3; // Button Start
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
int totalTime; 
int timeTankFull;
int startTimer=0; // start regressive counter if equal to 1;

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

    // Setup interrupt
    Timer1.initialize(1000000);         // initialize timer1, and set a 1 second period
    Timer1.attachInterrupt(updateTime);  // attaches updateTime() as a timer overflow interrupt
  
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
    disp.print(F("      2.0 BETA      "));
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


int updateTime(){
    int partInMinutes;
    int partInSeconds;
    
    
    if (startTimer == 1 ){

        if ( totalTime <= 0 ){
            startTimer = 0;
            return 0;
        }

        totalTime = totalTime - 1;

        partInMinutes = (float)1*totalTime/(float)60;
        partInSeconds = totalTime % 60;
        disp.setCursor(0,1);
        disp.print(F("                    "));
        disp.setCursor(0,1);
        disp.print(F("T. Restante: "));
        disp.print(partInMinutes);
        disp.print(F(":"));
        if ( partInSeconds < 10)
            disp.print(F("0"));
        disp.print(partInSeconds);
        Serial.print(partInMinutes);
        Serial.print(F(":"));
        if ( partInSeconds < 10 )
            Serial.print(0);
        Serial.println(partInSeconds);
    }
    return 0;
}


void endBeep(){
    startTimer = 0; // Disable Time
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
    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 200;
    const int soakTime = 300;
    const int flushTime = 130;
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
    // flushTime seconds to empty)
    totalTime = totalTime + 2*(flushTime + centrifugationTime) + timeTankFull;
    startTimer = 1;
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 6        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 6        "));
    centrifuge();
  
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 6        "));
    
    timeTankFull = fillTank(2);
    
    if (timeTankFull == -1){
        errorTank();
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 6        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
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
    startTimer = 1;
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 2        "));

    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
    }
  
    endBeep(); 
}

void simpleWash(){

    const int hitsNumber = 60;
    const int spinTime = 450;
    const int pauseTime = 200;
    const int soakTime = 300;
    const int flushTime = 130;
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
    // flushTime seconds to empty)
    totalTime = totalTime + flushTime + centrifugueTime;
    startTimer = 1;
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 3        "));

    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
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
  
    totalTime = (120  + 180);
    startTimer = 1;
  
  
    //===============================================\\
  
    disp.setCursor(0,2);
    disp.print(F("Passo 1 de 1        "));
    centrifuge();
    
    endBeep();
}


void printMenuItem(int menuIndex, int printToSerial){
    switch (menuIndex){
        case 1:
            if ( printToSerial == 1){
                Serial.println(F("Lavagem Completa"));
            }
            else{
                disp.print(F("LAVAGEM COMPLETA   "));
            }
            break;
            
        case 2:
            if ( printToSerial == 1){
                Serial.println(F("Apenas Centrifugar"));
            }
            else{
                disp.print(F("APENAS CENTRIFUGAR "));
            }
            break;
  
        case 3:
            if ( printToSerial == 1 ){
                Serial.println(F("Um Enxague"));
            }
            else{
                disp.print(F("UM ENXAGUE         "));
            }
            break;
            
        case 4:
            if ( printToSerial == 1){
                Serial.println(F("Enxague Duplo"));
            }
            else{
                disp.print(F("ENXAGUE DUPLO      "));
            }
            break;
  
        case 5:
            if ( printToSerial == 1){
                Serial.println(F("Deixar de Molho"));
            }
            else{
                disp.print(F("DEIXAR DE MOLHO    "));
            }
            break;
            
        case 6:
            if ( printToSerial == 1){
                Serial.println(F("Delicada"));
            }
            else{
                disp.print(F("DELICADA           "));
            }
            break;
  
        case 7:
            if ( printToSerial == 1){
                Serial.println(F("Lavagem Completa"));
            }
            else{
                disp.print(F("LAVAGEM COMPLETA   "));
            }
            break;
  
        case 8:
            if ( printToSerial == 1){
                Serial.println(F("Apenas Centrifugar "));
            }
            else{
                disp.print(F("APENAS CENTRIFUGAR "));
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
    startTimer = 0; // Disable the timer
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
    // Return the time spent to fill the tank in seconds
    const unsigned long maxTimeToWait = 1800000;
    unsigned long timeTankFullInMilliseconds = 0;
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


    // This while loop monitors the pressostato state
    while(millis() - timeStartFlood < maxTimeToWait){
        if(digitalRead(pressostato) == 1){
            delay(1500);
            Serial.println(F("Is the tank really full?"));

            if(digitalRead(pressostato) == 1){
                timeTankFullInMilliseconds = millis() - timeStartFlood;
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
    int timeTankFullInSeconds = (float)1*timeTankFullInMilliseconds/(float)1000;
    Serial.println(timeTankFullInSeconds);
    disp.setCursor(0,3);
    disp.print(F("                    "));
    disp.setCursor(0,3);
    disp.print(F("TANQUE CHEIO "));
    int dispTank = (float)1*timeTankFullInMilliseconds/(float)60000;
    disp.print(dispTank);
    disp.print(F("min"));
    Serial.print(F("Tempo in Min: "));
    Serial.println(dispTank);

    delay(5000);
    return timeTankFullInSeconds;
  
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
    const int firstSoakTime = 900;
    const int soakTime = 300;
    const int flushTime = 130; // Additional time to flush the tank after pressure switch is comutted
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
    totalTime = ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + firstSoakTime); // The first soakTime step has longer duration
    totalTime = totalTime+8*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
    //totalTime = 9.*(2.*60.*(450. + 200.)/1000. + 300.);
    // Wash time + centrifugue time (assuming that the tank takes
    // flushTime seconds to empty)
  
    totalTime = totalTime + 3*(flushTime + centrifugationTime);
  
    // wash() time + time taken to fill the tank
    totalTime = totalTime + 2*timeTankFull;
    startTimer = 1;
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 9        "));
    
  
    // The first soakTime step has longer duration
    wash(hitsNumber, spinTime, pauseTime, firstSoakTime, 1, 3);
    
    for (int i=0;i<2;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 2, 3);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 9        "));
    centrifuge();
  
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1,3);
        
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 6 de 9        "));
    centrifuge();
  
    disp.setCursor(0,2);
    disp.print(F("Passo 7 de 9        "));
    timeTankFull = fillTank(2);

    if (timeTankFull == -1){
        errorTank();
    }

      
    disp.setCursor(0,2);
    disp.print(F("Passo 8 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
  
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
    const int firstSoakTime = 900;
    const int soakTime = 300;
    const int flushTime = 130;
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
    totalTime = ((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + firstSoakTime); // The first soakTime step has longer duration
    totalTime = totalTime + 8*((float)2*hitsNumber*(spinTime + pauseTime)/(float)1000 + soakTime);
    // Wash time + centrifugue time (guessing that the tank takes
    // flushTime seconds to empty)
  
    totalTime = totalTime + 3*(flushTime + centrifugationTime);
  
    // Wash time + tankFill
    totalTime = totalTime + 2*timeTankFull;
    startTimer = 1;
  
    
    disp.setCursor(0,2);
    disp.print(F("Passo 2 de 9        "));
    
    wash(hitsNumber, spinTime, pauseTime, firstSoakTime,1,3);
      
    for (int i=0;i<2;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 2,3);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 3 de 9        "));
    centrifuge();
  
    disp.setCursor(0,2);
    disp.print(F("Passo 4 de 9        "));
    timeTankFull = fillTank(1);

    if (timeTankFull == -1){
        errorTank();
    }

    
    disp.setCursor(0,2);
    disp.print(F("Passo 5 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
    }
  
    disp.setCursor(0,2);
    disp.print(F("Passo 6 de 9        "));
    centrifuge();
  
    disp.setCursor(0,2);
    disp.print(F("Passo 7 de 9        "));
    timeTankFull = fillTank(2);

    if (timeTankFull == -1){
        errorTank();
    }

    
    disp.setCursor(0,2);
    disp.print(F("Passo 8 de 9        "));
    for (int i=0;i<3;i++){
        wash(hitsNumber, spinTime, pauseTime, soakTime, i + 1, 3);
    } 
  
    disp.setCursor(0,2);
    disp.print(F("Passo 9 de 9        "));
    centrifuge();
  
    endBeep();
    
  }
  
