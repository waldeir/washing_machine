#include <LiquidCrystal.h>                             // Lib for the lcd display


const int pump = 10;    
const int motorCW =  6;
const int motorCCW = 9;
const int soapValve = 7;
const int softenerValve = 8;
const int pressostato = 5;
const int Button1 = 4;
const int Button2 = 2;
const int Button3 = 3;
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
int timeTankFlood;

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
  pinMode(pressostato, INPUT);
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
  disp.setCursor(0,0);
  disp.print("Selecione o programa");

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

      // only toggle the LED if the new button state is HIGH
      if (buttonStateButton1 == HIGH) {
      
            if (meIndex == 4  ){ meIndex = 0;}
            meIndex++;   
            printMenu(meIndex);
              
            Serial.print("printMenu(");
            Serial.print(meIndex);
            Serial.println(");");
            
            
        
        

      }
    }
  }
  lastButtonStateButton1 = statusButton1;

//---------------------------------------------------------\\
    
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

      // only toggle the LED if the new button state is HIGH
      if (buttonStateButton3 == HIGH) {

        
           disp.clear();
           disp.setCursor(0,0);
           disp.print("Programa Selecionado!");
           disp.setCursor(0,2);
           disp.write(byte(0)); //Menu arrow
           printMenuItem(meIndex);
           delay(2000);
           programa = meIndex;
           Serial.print("Programa=");
           Serial.println(programa);
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
        delicateWash();
        break;
    case 5:
        normalWashing();
        break;
    case 6:
        simpleWash();
        break;
    
    
  }


  
}

void printMenu(int mIndex) {
  for (int i=0;i<3;i++){
      disp.setCursor(1,1+i);
      printMenuItem(mIndex+i);
  }
  

      disp.setCursor(0,1);
      //disp.print(">");
       disp.write(byte(0));
      disp.setCursor(0,2);
      disp.print(" ");
      disp.setCursor(0,3);
      disp.print(" ");

}



void updateTime(int timeC){
      totalTimeMin = (float)1*timeC/(float)60;
      disp.setCursor(0,1);
      disp.print("                    ");
      disp.setCursor(0,1);
      disp.print("T. Restante: ");
      disp.print(totalTimeMin);
      disp.print("min");
}



void simpleWash(){

  int totalTime;
  int hits = 60;
  int spin = 450;
  int pause = 200;
  int soak = 300;
  disp.clear();
  disp.setCursor(0,0);
  disp.print("ENXAGUE SIMPLES");
  
  disp.setCursor(0,2);
  disp.print("Passo 1 de 3        ");
  timeTankFlood = fillTankSoapV();

  // Estimating cicle time:
  // Wash time
  totalTime = 3*((float)2*hits*(spin + pause)/(float)1000 + soak);

  // Wash time + centrifugue time (guessing that the tank takes
  // 60s to empty)
  totalTime = totalTime + 120 + 20 + 180;

  updateTime(totalTime);
  
  disp.setCursor(0,2);
  disp.print("Passo 2 de 3        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);

      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);
  }

  disp.setCursor(0,2);
  disp.print("Passo 3 de 3        ");
  centrifuge();
  
  
}

void justCentrifugue(){

  disp.clear();
  disp.setCursor(0,0);
  disp.print("APENAS CENTRIFUGAR   ");
  
  // Estimating cicle time:========================\\

  totalTime = (120 + 20 + 180);


  updateTime(totalTime);
  

  //===============================================\\

  disp.setCursor(0,2);
  disp.print("Passo 1 de 1        ");
  centrifuge();
  
  
}



  void printMenuItem(int menuIndex){
    switch (menuIndex){
      case 1:
          disp.print("NORMAL             ");
         break;
          
      case 2:
          disp.print("APENAS CENTRIFUGAR ");
          break;

      case 3:
          disp.print("UM ENXAGUE         ");
          break;
          
      case 4:
          disp.print("DELICADA           ");
          break;

      case 5:
          disp.print("NORMAL             ");
          break;

      case 6:
          disp.print("APENAS CENTRIFUGAR ");
          break;
    }
  }
 
void centrifuge(){
  Serial.println("Esvaziando o tanque");
  disp.setCursor(0,3);
  disp.print("CENTRIFUGAR  (BOMBA)");

  digitalWrite(pump,HIGH); 
  while(digitalRead(pressostato) == 1){
     digitalWrite(pump,HIGH);
  }
  for(int j=0;j<120;j++){
  delay(1000);
  }
  disp.setCursor(0,3);
  disp.print("CENTRIFUGAR   (GIRO)");
  Serial.println("Centrifugando");
  // Performing sprints to accomodate the clothers
  for(int i=0;i<=4;i++){
  digitalWrite(motorCCW,HIGH);
  delay(3000);
  digitalWrite(motorCCW, LOW);
  delay(2000);
  }

  digitalWrite(motorCCW,HIGH);
  for(int h=0;h<180;h++){
  delay(1000);
  }
  digitalWrite(motorCCW,LOW);
  Serial.println("Fim da Centrifugação");
  digitalWrite(pump,LOW);
}

int fillTankSoapV(){
  int timeTankFull = 0;
  // fill tank using the soap valve
  // While the tank is not complete keep the valve open
  Serial.println("Inundando o tanque  ");
  disp.setCursor(0,3);
  disp.print("INUNDANDO O TANQUE  ");
  
  while ( digitalRead(pressostato) == 0){
    digitalWrite(soapValve,HIGH);
    delay(1000);
    timeTankFull = timeTankFull + 1;
    // To do: put a timer to not overfill the tank
    }
  digitalWrite(soapValve,LOW);
  Serial.println("Fechando válvula de sabão");
  Serial.print("Tempo = ");
  disp.setCursor(0,3);
  disp.print("                    ");
  disp.setCursor(0,3);
  disp.print("TANQUE CHEIO ");
  int dispTank = (float)1*timeTankFull/(float)60;
  disp.print(dispTank);
  disp.print("min");
  delay(5000);
  timeTankFull++;
  Serial.println(timeTankFull);
  return timeTankFull;

  }

int fillTankSoftV(){
  int timeTankFull = 0;
  // fill tank using the softener valve
  // While the tank is not complete keep the valve open
  Serial.println("Abrindo válvula de amaciante");
  disp.setCursor(0,3);
  disp.print("COLOCANDO AMACIANTE ");
  timeTankFull = 0;
  while ( digitalRead(pressostato) == 0 ){
    digitalWrite(softenerValve,HIGH);
    delay(1000);
    timeTankFull = timeTankFull + 1;
    // To do: put a timer to not overfill the tank
    }
    digitalWrite(softenerValve,LOW);

  disp.setCursor(0,3);
  disp.print("                    ");
  disp.setCursor(0,3);
  disp.print("TANQUE CHEIO ");
  int dispTank = (float)1*timeTankFull/(float)60;
  disp.print(dispTank);
  disp.print("min");
  delay(1000);
  
  Serial.println("Fechando válvula de amaciante");
  Serial.print("Tempo = ");
  Serial.println(timeTankFull);
  timeTankFull++;
  return timeTankFull;

  }

void wash(int hits, int spin, int pause, int soak){
  disp.setCursor(0,3);
  disp.print("ENXAGUE      (BATER)");

  Serial.println("Batendo");
  while (hits > 0){
    digitalWrite(motorCW,HIGH);
    delay(spin);
    digitalWrite(motorCW,LOW);
    delay(pause);
    digitalWrite(motorCCW,HIGH);
    delay(spin);
    digitalWrite(motorCCW, LOW);
    delay(pause);

    hits = hits - 1;
    }
  disp.setCursor(0,3);
  disp.print("ENXAGUE      (MOLHO)");
    for(int j=0;j<=soak;j++)
    {
      delay(1000);
      }
   
     
  }


void normalWashing(){
  int hits = 60;
  int spin = 400;
  int pause = 200;
  int soak = 300;
  disp.clear();
  disp.setCursor(0,0);
  disp.print("LAVAGEM NORMAL      ");
  
  disp.setCursor(0,2);
  disp.print("Passo 1 de 9        ");
  timeTankFlood = fillTankSoapV();

  // Estimating cicle time:========================
  // Wash time
  totalTime = 9*((float)2*hits*(spin + pause)/(float)1000 + soak);
  //totalTime = 9.*(2.*60.*(450. + 200.)/1000. + 300.);
  // Wash time + centrifugue time (guessing that the tank takes
  // 60s to empty)

  totalTime = totalTime + 3*(120 + 20 + 180);

  // Wash time + tankFill
  totalTime = totalTime + 2*timeTankFlood;

  updateTime(totalTime);
  //===============================================
  
  disp.setCursor(0,2);
  disp.print("Passo 2 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);
      
      // Update Time ==============================
      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      
      Serial.println( ((float)2*hits*(spin + pause)/(float)1000 + soak) );
      Serial.println( totalTime );
      updateTime(totalTime);
      
      //===========================================
      
  }

  disp.setCursor(0,2);
  disp.print("Passo 3 de 9        ");
  centrifuge();

  // Update Time ==============================
  totalTime = totalTime - (120 + 20 + 180);
  updateTime(totalTime);
      
  //===========================================
  
  disp.setCursor(0,2);
  disp.print("Passo 4 de 9        ");
  fillTankSoapV();
  
  // Update Time ==============================
  totalTime = totalTime - timeTankFlood;
  updateTime(totalTime);
      
  //===========================================
  
  disp.setCursor(0,2);
  disp.print("Passo 5 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);
      
      // Update Time ==============================
      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);
      //===========================================

  }

  disp.setCursor(0,2);
  disp.print("Passo 6 de 9        ");
  centrifuge();
  // Update Time ==============================
  totalTime = totalTime - (120 + 20 + 180);
  updateTime(totalTime);
      
  //===========================================

  disp.setCursor(0,2);
  disp.print("Passo 7 de 9        ");
  fillTankSoftV();
    
  // Update Time ==============================
  totalTime = totalTime - timeTankFlood;
  updateTime(totalTime);
      
  //===========================================


  disp.setCursor(0,2);
  disp.print("Passo 8 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);
      // Update Time ==============================
      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);
      
      //===========================================
      

  } 

  disp.setCursor(0,2);
  disp.print("Passo 9 de 9        ");
  centrifuge();
  
  
}

void delicateWash(){
  int hits = 60;
  int spin = 450;
  int pause = 1000;
  int soak = 300;
  disp.clear();
  disp.setCursor(0,0);
  disp.print("LAVAGEM DELICADA");


  
  disp.setCursor(0,2);
  disp.print("Passo 1 de 9        ");
  timeTankFlood = fillTankSoapV();

  // Estimating cicle time:========================
  // Wash time
  totalTime = 9*((float)2*hits*(spin + pause)/(float)1000 + soak);
  // Wash time + centrifugue time (guessing that the tank takes
  // 120s to empty)

  totalTime = totalTime + 3*(120 + 20 + 180);

  // Wash time + tankFill
  totalTime = totalTime + 2*timeTankFlood;

  updateTime(totalTime);
  //===============================================
  
  disp.setCursor(0,2);
  disp.print("Passo 2 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);

      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);


  }

  disp.setCursor(0,2);
  disp.print("Passo 3 de 9        ");
  centrifuge();

  // Update Time ==============================
  totalTime = totalTime - (120 + 20 + 180);
  updateTime(totalTime);
  //===========================================
  disp.setCursor(0,2);
  disp.print("Passo 4 de 9        ");
  fillTankSoapV();
  
  // Update Time ==============================
  totalTime = totalTime - timeTankFlood;
  updateTime(totalTime);
      
  //===========================================
  
  disp.setCursor(0,2);
  disp.print("Passo 5 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);
      
      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);
  }

  disp.setCursor(0,2);
  disp.print("Passo 6 de 9        ");
  centrifuge();
  // Update Time ==============================
  totalTime = totalTime - (120 + 20 + 180);
  updateTime(totalTime);

  disp.setCursor(0,2);
  disp.print("Passo 7 de 9        ");
  fillTankSoftV();
  
  // Update Time ==============================
  totalTime = totalTime - timeTankFlood;
  updateTime(totalTime);
      
  //===========================================

  disp.setCursor(0,2);
  disp.print("Passo 8 de 9        ");
  for (int i=0;i<3;i++){
      wash(hits, spin, pause, soak);
      
      totalTime = totalTime - ((float)2*hits*(spin + pause)/(float)1000 + soak);
      updateTime(totalTime);
  } 

  disp.setCursor(0,2);
  disp.print("Passo 9 de 9        ");
  centrifuge();
  
}
  
