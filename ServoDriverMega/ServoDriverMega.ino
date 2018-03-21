#include <SPI.h>
#include <Wire.h>
#include <Servo.h>   // librairie pour les servomoteur RC
#include <Herkulex.h> // servo herkulex
#include <LiquidCrystal.h> //ajout de la librairie pour l'afficheur
#include <Dynamixel_Serial.h> // servomoteur dynamixel RS485
#include <Keypad.h> // clavier 3x4
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/// LCD /////
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// Potentiometre
byte POTAR = 0;

// Clavier
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
byte rowPins[ROWS] = {30, 32, 34, 36}; 
byte colPins[COLS] = {38, 40, 42}; 
Keypad keypad ( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Servo myservo[4];  // create servo object to control a servo 
                // a maximum of eight servo objects can be created

int pos = 0;    // variable to store the servo position 
int ID_menu = 0;
char key =' ';
byte pointeur=0x00;
boolean Servo_Valide = false;
int ID_servo = 0, ID_servo_souhaite = 0;
byte Model_servo=0x01;
int count=0;
String servomoteur = " ";

void setup() 
{ 
  myservo[1].attach(11); myservo[2].attach(12); myservo[3].attach(13);  
  
  keypad.begin( makeKeymap(keys) );
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
  
    // We now need to set Ardiuno to the new Baudrate speed
    Dynamixel.setDirectionPin(2);  
 
  delay(500);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);  display.println("Menu :");
  display.setCursor(0,17); display.println("Press *");
  display.display();
  delay(500);
} 

void loop() 
{ 
  key = keypad.getKey();
  switch(ID_menu){
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
  case 111: //  RC  Rotation  Point zero  
  case 112: // RC rotation manuel    
  case 113 : // RC Rotation Position
      if(!Servo_Valide){
          display.clearDisplay();
          display.setCursor(0,0);    display.println("RC  Rotation");
           if( ID_menu == 111){display.setCursor(0,10);   display.println("Point zero :");} 
           if( ID_menu == 112){display.setCursor(0,10);   display.println("Manuel :");} 
           if( ID_menu == 113){display.setCursor(0,10);   display.println("Position :");} 
          display.setCursor(0,20);   display.println("Num ID ? (1, 2 ou 3)"); 
          display.display(); 
          ID_servo = 0;
          do{
            key = keypad.waitForKey();
            if(key && key!='*' && key!='#' ){
              ID_servo = (int(key)-48);
              if(ID_servo<4 && ID_servo>=1)
              { 
                display.clearDisplay();
                display.setCursor(0,0);   display.println("Num ID "); 
                display.setCursor(80,0);   display.println(ID_servo); 
                display.setCursor(0,17);   display.println("Valider *"); 
                display.display();  
              }
            }
          }
          while((key!='*') && (ID_servo !=0));
          Servo_Valide = true;
          if(ID_menu == 113){
              display.clearDisplay();
              display.setCursor(0,0);   display.println("Num ID "); 
              display.setCursor(60,0);   display.println(ID_servo); 
              display.setCursor(0,10);   display.println("Position =  "); 
              display.setCursor(80,10);   display.println(" ? "); 
              display.setCursor(0,20);   display.println("Quitter #"); 
              display.display();  
          }
      }else{
          if( ID_menu == 111){
              myservo[ID_servo].write(90);
              ID_menu = 11;  
              display.clearDisplay();
              display.setCursor(0,0);    display.print("RC - Rotation");
              display.setCursor(30,8);   display.println("1-Point zero"); 
              display.setCursor(30,16);   display.println("2-Manuel"); 
              display.setCursor(30,24);  display.println("3-Position"); 
              display.display(); 
          }
          if(ID_menu == 112){
              pos = analogRead(POTAR);
              pos = map(pos, 0, 1024, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 
              myservo[ID_servo].write(pos);              // tell servo to go to position in variable 'pos' 
             // pos = map(pos, 0, 180, -90, 90);     // scale it to use it with the servo (value between -90 and 90) 
              display.clearDisplay();
              display.setCursor(0,0);   display.println("Num ID "); 
              display.setCursor(60,0);   display.println(ID_servo); 
              display.setCursor(0,10);   display.println("Position : "); 
              display.setCursor(60,10);   display.println(pos); 
              display.setCursor(90,10);   display.println("degres"); 
              display.setCursor(0,20);   display.println("Quitter #"); 
              display.display();  
              if(key=='#'){ 
                ID_menu = 11;  
                display.clearDisplay();
                display.setCursor(0,0);    display.print("RC - Rotation");
                display.setCursor(30,8);   display.println("1-Point zero"); 
                display.setCursor(30,16);   display.println("2-Manuel"); 
                display.setCursor(30,24);  display.println("3-Position"); 
                display.display();
              }  
          }
          if(ID_menu == 113){
             count = 0;
             pos = -1;
             do{
                  key = keypad.waitForKey();
                  if(key && key!='*' && key!='#' ){ 
                    if(count==2) {
                      pos = (int(key)-48) + (pos*10);
                    }
                    if(count==1) {
                      pos = (int(key)-48) + (pos*10);
                      count=2;
                    }
                    if(count==0) {
                      pos = (int(key)-48);
                      count=1;
                    }
                    if(pos>180 || pos<0)
                    {
                      pos = -1;
                      count = 0;
                      display.clearDisplay();
                      display.setCursor(0,0);   display.println("Position erreur "); 
                      display.setCursor(0,17);   display.println("Entre 0 et 180"); 
                      display.display();   
                    }else{
                      display.clearDisplay();
                      display.setCursor(0,0);   display.println("Num ID "); 
                      display.setCursor(60,0);   display.println(ID_servo); 
                      display.setCursor(0,10);   display.println("Position =  "); 
                      display.setCursor(80,10);   display.println(pos); 
                      display.setCursor(0,20);   display.println("Valider *  Quitter #");
                      display.display();  
                    }
                  }
              }while( key!='*' && key!='#' );
              if(key=='*'){ 
                 if(pos<=180 && pos>=0){
                    myservo[ID_servo].write(pos);              // tell servo to go to position in variable 'pos' 
                 }
              }
              if(key=='#'){ 
                ID_menu = 11;  
                display.clearDisplay();
                display.setCursor(0,0);    display.print("RC - Rotation");
                display.setCursor(30,8);   display.println("1-Point zero"); 
                display.setCursor(30,16);   display.println("2-Manuel"); 
                display.setCursor(30,24);  display.println("3-Position"); 
                display.display();
              } 
          }
      }
     break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  case 211: // Herkulex, rotation position zero
  case 212: // Herkulex, rotation manuel
  case 213: // Herkulex, rotation position
  case 311: // Dyn3, rotation position zero
  case 312: // Dyn3, rotation manuel
  case 313: // Dyn3, rotation position
      if(!Servo_Valide){
        display.clearDisplay();
         if( ID_menu == 211 ||  ID_menu == 212 ||  ID_menu == 213){display.setCursor(0,0);    display.println("Hkx  Rotation");} 
         if( ID_menu == 311 ||  ID_menu == 312 ||  ID_menu == 313){display.setCursor(0,0);    display.println("Dyn  Rotation");} 
         
         if( ID_menu == 211 ||  ID_menu == 311 ){display.setCursor(0,10);   display.println("Point zero :");} 
         if( ID_menu == 212 ||  ID_menu == 312 ){display.setCursor(0,10);   display.println("Manuel :");} 
         if( ID_menu == 213 ||  ID_menu == 313 ){display.setCursor(0,10);   display.println("Position :");} 
        display.setCursor(0,20);   display.println("Num ID ? "); 
        display.display(); 
        ID_servo = 0;
        count = 0;
        do{
            key = keypad.waitForKey();
            if(key && key!='*' && key!='#' ){ 
              if(count==2) {
                ID_servo = (int(key)-48) + (ID_servo*10);
              }
              if(count==1) {
                ID_servo = (int(key)-48) + (ID_servo*10);
                count=2;
              }
              if(count==0) {
                ID_servo = (int(key)-48);
                count=1;
              }
              if(ID_servo>254 || ID_servo<0)
              {
                ID_servo = 0;
                count = 0;
                display.clearDisplay();
                display.setCursor(0,0);   display.println("ID erreur "); 
                display.setCursor(0,17);   display.println("Entre 1 et 254"); 
                display.display();   
              }else{
                display.clearDisplay();
                display.setCursor(0,0);   display.println("Num ID  ");  
                display.setCursor(50,0);   display.println(ID_servo); 
                display.setCursor(0,17);   display.println("Valider *");
                display.display();  
              }
            }
        }while(key!='*');
        display.clearDisplay(); 
        display.setCursor(0,0);   display.println("Baudrate  ?"); 
        display.setCursor(70,0);   display.println("1-1000000"); 
        display.setCursor(70,8);   display.println("2-666666"); 
        display.setCursor(70,16);   display.println("3-115200"); 
        display.setCursor(70,24);   display.println("4-57600"); 
        display.display(); 
        do{
          key = keypad.waitForKey();
        }
        while((key!='1') && (key!='2') && (key!='3') && (key!='4')); 
        if( ID_menu == 211 ||  ID_menu == 212 ||  ID_menu == 213){
            if(key=='1'){  Herkulex.beginSerial2(1000000); }
            if(key=='2'){  Herkulex.beginSerial2(666666); }
            if(key=='3'){  Herkulex.beginSerial2(115200);  }
            if(key=='4'){  Herkulex.beginSerial2(57600);   }
            Herkulex.reboot(ID_servo); //reboot first motor
            delay(500); 
            Herkulex.initialize(); //initialize motors
            delay(200);  
            Model_servo = Herkulex.model(ID_servo);
        }
        if( ID_menu == 311 ||  ID_menu == 312 ||  ID_menu == 313){
             if(key=='1'){  Dynamixel.begin(Serial3,1000000); }
             if(key=='2'){  Dynamixel.begin(Serial3,666666); }
             if(key=='3'){  Dynamixel.begin(Serial3,115200); }
             if(key=='4'){  Dynamixel.begin(Serial3,57600); }
        }
        Servo_Valide = true;
        if(ID_menu == 213 || ID_menu == 313){
              display.clearDisplay();
              display.setCursor(0,0);   display.println("Num ID "); 
              display.setCursor(60,0);   display.println(ID_servo); 
              display.setCursor(0,10);   display.println("Position =  "); 
              display.setCursor(80,10);   display.println(" ? "); 
              display.setCursor(0,20);   display.println("Quitter #"); 
              display.display();  
          }
      }else{
          if(ID_menu == 211){  //Hkx Rotation  point zero
              if(Model_servo == 0x02 || Model_servo == 0x03) Herkulex.moveOne(ID_servo, 512, 10, 0);   // Move the Servo 0101 or 0201 to ...
              if(Model_servo == 0x05 || Model_servo == 0x07) Herkulex.moveOne(ID_servo, 1024, 10, 0);   // Move the Servo 0401 or 0601 to ...
              if(Model_servo == 0x06 || Model_servo == 0x08) Herkulex.moveOne(ID_servo, 16384, 10, 0);   // Move the Servo 0402 or 0602 to ...
              ID_menu = 21;  
              Herkulex.end();
              display.clearDisplay();
              display.setCursor(0,0);    display.print("Hkx - Rotation");
              display.setCursor(30,8);   display.println("1-Point zero"); 
              display.setCursor(30,16);   display.println("2-Manuel"); 
              display.setCursor(30,24);  display.println("3-Position"); 
              display.display(); 
          }
          if(ID_menu == 311){ //Dyn3 Rotation  point zero
              Dynamixel.ledState(ID_servo,OFF);
              Dynamixel.servo(ID_servo,512,100);  // Move the Servo to 512
              display.clearDisplay();
              ID_menu = 31;  
              display.setCursor(0,0);    display.print("Dyn - Rotation");
              display.setCursor(30,8);   display.println("1-Point zero"); 
              display.setCursor(30,16);   display.println("2-Manuel"); 
              display.setCursor(30,24);  display.println("3-Position"); 
              display.display(); 
          }
          if(ID_menu == 212 || ID_menu == 312){   //Hkx, Dyn  Rotation  manuel
              pos = analogRead(POTAR);
              pos = map(pos, 0, 1024, 0, 320);     // scale it to use it with the servo (value between 0 and 180) 
              display.clearDisplay();
              display.setCursor(0,0);   display.println("Num ID "); 
              display.setCursor(60,0);   display.println(ID_servo); 
              display.setCursor(0,10);   display.println("Position : "); 
              display.setCursor(60,10);   display.println(pos); 
              display.setCursor(90,10);   display.println("degres"); 
              display.setCursor(0,20);   display.println("Quitter #"); 
              display.display();  
              if(ID_menu == 312){
                  pos = map(pos, 0, 320, 21, 1002);
                  Dynamixel.ledState(ID_servo,ON);
                  Dynamixel.servo(ID_servo,pos,100);  // Move the Servo to 512
              }else{
                  if(Model_servo == 0x02 || Model_servo == 0x03) pos = map(pos, 0, 320, 21, 1002);// Move the Servo 0101 or 0201 to ...
                  if(Model_servo == 0x05 || Model_servo == 0x07) pos = map(pos, 0, 320, 42, 2004);  // Move the Servo 0401 or 0601 to ...
                  if(Model_servo == 0x06 || Model_servo == 0x08) pos = map(pos, 0, 320, 10627, 22129); // Move the Servo 0402 or 0602 to ...
                  Herkulex.moveOne(ID_servo, pos, 10, LED_GREEN);   // Move the Servo to
              }
              if(key=='#'){ 
                display.clearDisplay();
                if(ID_menu == 212){ display.setCursor(0,0);  display.println("Hkx - Rotation");ID_menu = 21; Herkulex.end();}
                if(ID_menu == 312){ display.setCursor(0,0);  display.println("Dyn - Rotation");ID_menu = 31; }
                display.setCursor(30,8);   display.println("1-Point zero"); 
                display.setCursor(30,16);   display.println("2-Manuel"); 
                display.setCursor(30,24);  display.println("3-Position"); 
                display.display();
              }  
            }
            if(ID_menu == 213 || ID_menu == 313){  //Hkx, Dyn Rotation position
               count = 0;
               pos = -1;
               do{
                    key = keypad.waitForKey();
                    if(key && key!='*' && key!='#' ){ 
                      if(count==2) {
                        pos = (int(key)-48) + (pos*10);
                      }
                      if(count==1) {
                        pos = (int(key)-48) + (pos*10);
                        count=2;
                      }
                      if(count==0) {
                        pos = (int(key)-48);
                        count=1;
                      }
                      if(pos<0 || pos>330)
                      {
                        pos = -1;
                        count = 0;
                        display.clearDisplay();
                        display.setCursor(0,0);   display.println("Position erreur "); 
                        display.setCursor(0,17);   display.println("Entre 0 et 330"); 
                        display.display();   
                      }else{
                        display.clearDisplay();
                        display.setCursor(0,0);   display.println("Num ID "); 
                        display.setCursor(60,0);   display.println(ID_servo); 
                        display.setCursor(0,10);   display.println("Position =  "); 
                        display.setCursor(80,10);   display.println(pos); 
                        display.setCursor(0,20);   display.println("Valider *  Quitter #");
                        display.display();  
                      }
                    }
                }while( key!='*' && key!='#' );
                if(key=='*'){ 
                   if(pos!=-1){
                       if(ID_menu == 213){
                            if(Model_servo == 0x02 || Model_servo == 0x03) pos = map(pos, 0, 330, 21, 1002);// Move the Servo 0101 or 0201 to ...
                            if(Model_servo == 0x05 || Model_servo == 0x07) pos = map(pos, 0, 330, 42, 2004);  // Move the Servo 0401 or 0601 to ...
                            if(Model_servo == 0x06 || Model_servo == 0x08) pos = map(pos, 0, 330, 10627, 22129); // Move the Servo 0402 or 0602 to ...                      
                            Herkulex.moveOne(ID_servo, pos, 10, LED_GREEN);   // Move the Servo to
                            delay(100);
                            Herkulex.moveOne(ID_servo, pos, 10, 0);   // Move the Servo to
                       }
                       if(ID_menu == 313){
                            pos = map(pos, 0, 330, 21, 1002);
                            Dynamixel.ledState(ID_servo,ON);
                            Dynamixel.servo(ID_servo,pos,100);  // Move the Servo to 512
                            delay(100);
                            Dynamixel.ledState(ID_servo,ON);
                       }
                   }
                }
                if(key=='#'){ 
                  display.clearDisplay();
                  if(ID_menu == 213){ display.setCursor(0,0);  display.println("Hkx - Rotation");ID_menu = 21; Herkulex.end(); }
                  if(ID_menu == 313){ display.setCursor(0,0);  display.println("Dyn - Rotation");ID_menu = 31; }
                  display.setCursor(30,8);   display.println("1-Point zero"); 
                  display.setCursor(30,16);   display.println("2-Manuel"); 
                  display.setCursor(30,24);  display.println("3-Position"); 
                  display.display();
                } 
            }
      }
    break; 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      case 22: //Herkulex scan
      case 32: //Dyn3 scan
              display.clearDisplay(); 
              display.setCursor(0,0);   display.println("Baudrate ? "); 
              display.setCursor(70,0);   display.println("1-1000000"); 
              display.setCursor(70,8);   display.println("2-666666"); 
              display.setCursor(70,16);   display.println("3-115200"); 
              display.setCursor(70,24);   display.println("4-57600");  
              display.display();
              do{
                  key = keypad.waitForKey();
              }while(key!='1' && key!='2' && key!='3' && key!='4');
              if( ID_menu == 22){
                  if(key=='1'){  Herkulex.beginSerial2(1000000); } //open serial port 2 to 1M
                  if(key=='2'){  Herkulex.beginSerial2(666666);  }//open serial port 2 to 666666
                  if(key=='3'){  Herkulex.beginSerial2(115200);  }//open serial port 2 to 115200
                  if(key=='4'){  Herkulex.beginSerial2(57600);  }//open serial port 2 to 57600
                  delay(100); 
                  Herkulex.initialize();
              }else{ 
                  if(key=='1'){ Dynamixel.begin(Serial3,1000000); } //open serial port 2 to 1M
                  if(key=='2'){ Dynamixel.begin(Serial3,666666); }//open serial port 2 to 666666
                  if(key=='3'){ Dynamixel.begin(Serial3,115200);  }//open serial port 2 to 115200
                  if(key=='4'){ Dynamixel.begin(Serial3,57600);  }//open serial port 2 to 57600
              }
              display.clearDisplay(); 
              display.setCursor(0,0);   display.println("  Scan .."); 
              display.display(); 
              pointeur = 0x00;
              if(ID_menu == 22){  // Hkx
                      delay(100); 
                      if((Herkulex.getPosition(219)!=(-1))){ display.setCursor(pointeur,12);  display.println(219); pointeur+=15; display.display(); }
                      delay(100); 
                      if((Herkulex.getPosition(253)!=(-1))){ display.setCursor(pointeur,12);  display.println(253); pointeur+=15; display.display(); }
                   }else{
                      delay(100); 
                      if((Dynamixel.ping(219)==(219))){ display.setCursor(pointeur,12);  display.println(219); pointeur+=15; display.display(); }
                      delay(100); 
                      if((Dynamixel.ping(253)==(253))){ display.setCursor(pointeur,12);  display.println(253); pointeur+=15; display.display(); }
                   }
              for(int i=1; i<253; i++){
                   if(i==50){   display.setCursor(0,0);   display.println("  Scan ...."); display.display(); }
                   if(i==100){  display.setCursor(0,0);   display.println("  Scan ......"); display.display(); }
                   if(i==150){  display.setCursor(0,0);   display.println("  Scan ........"); display.display(); }
                   if(i==200){  display.setCursor(0,0);   display.println("  Scan .........."); display.display(); }
                   if(ID_menu == 22){  // Hkx
                      delay(100); 
                      if((Herkulex.getPosition(i)!=(-1))){ display.setCursor(pointeur,12);  display.println(i); pointeur+=15; display.display(); }
                   }else{
                      delay(100); 
                      if((Dynamixel.ping(i)==(i))){ display.setCursor(pointeur,12);  display.println(i); pointeur+=15; display.display(); }
                   }
                   key = keypad.getKey();
                   if(key == '#'){ break;}
             }
              display.setCursor(0,0);   display.println("  Scan ............"); 
              display.setCursor(pointeur,24);  display.println("...fin #");
              display.display();
              do{
                 key = keypad.waitForKey();
              }while(key!='#');
              display.clearDisplay();
              if(ID_menu==22){
                  ID_menu = 20;  
                  display.setCursor(0,0);        display.print("Hkx");
              }
              if(ID_menu==32){
                  ID_menu = 30;  
                  display.setCursor(0,0);        display.print("Dyn");
              }
              display.setCursor(30,0);       display.println("1-Rotation"); 
              display.setCursor(30,8);       display.println("2-Scan"); 
              display.setCursor(30,16);      display.println("3-Modif ID"); 
              display.setCursor(30,24);      display.println("4-Modif Baudrate");
              display.display();  
      break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////          
      case 23: //Herkulex Modif ID
      case 33: //Dyn Modif ID
              display.clearDisplay(); 
              display.setCursor(0,0);   display.println("Baudrate  ?"); 
              display.setCursor(70,0);   display.println("1-1000000"); 
              display.setCursor(70,8);   display.println("2-666666"); 
              display.setCursor(70,16);   display.println("3-115200"); 
              display.setCursor(70,24);   display.println("4-57600");  
              display.display();
              do{
                  key = keypad.waitForKey();
              }while(key!='1' && key!='2' && key!='3' && key!='4');
              if( ID_menu == 23){
                  if(key=='1'){  Herkulex.beginSerial2(1000000); } //open serial port 2 to 1M
                  if(key=='2'){  Herkulex.beginSerial2(666666);  }//open serial port 2 to 666666
                  if(key=='3'){  Herkulex.beginSerial2(115200);  }//open serial port 2 to 115200
                  if(key=='4'){  Herkulex.beginSerial2(57600);  }//open serial port 2 to 57600
              }else{ 
                  if(key=='1'){ Dynamixel.begin(Serial3,1000000); } //open serial port 2 to 1M
                  if(key=='2'){ Dynamixel.begin(Serial3,666666); } //open serial port 2 to 1M
                  if(key=='3'){ Dynamixel.begin(Serial3,115200);  }//open serial port 2 to 115200
                  if(key=='4'){ Dynamixel.begin(Serial3,57600);  }//open serial port 2 to 57600
              }
              display.clearDisplay();
               if( ID_menu == 23){display.setCursor(0,0);    display.println("Hkx  Modif ID");} 
               if( ID_menu == 33){display.setCursor(0,0);    display.println("Dyn  Modif ID");} 
              display.setCursor(0,20);   display.println("Num ID actuel ? "); 
              display.display(); 
              ID_servo = 0;
              count = 0;
              do{
                key = keypad.waitForKey();
                if(key && key!='*' && key!='#' ){ 
                  if(count==2) {
                    ID_servo = (int(key)-48) + (ID_servo*10);
                  }
                  if(count==1) {
                    ID_servo = (int(key)-48) + (ID_servo*10);
                    count=2;
                  }
                  if(count==0) {
                    ID_servo = (int(key)-48);
                    count=1;
                  }
                  if(ID_servo>253 || ID_servo<0)
                  {
                    ID_servo = 0;
                    count = 0;
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("ID erreur "); 
                    display.setCursor(0,17);   display.println("Entre 1 et 253"); 
                    display.display();   
                  }else{
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("Num ID actuel  ");  
                    display.setCursor(90,0);   display.println(ID_servo); 
                    display.setCursor(0,17);   display.println("Valider *");
                    display.display();  
                  }
                }
            }while(key!='*' && ID_servo != 0);
            if(ID_menu == 23){
                Herkulex.reboot(ID_servo); //reboot first motor
                delay(500); 
                Herkulex.initialize(); //initialize motors
                delay(200); 
            }
             ID_servo_souhaite = 0;
             display.clearDisplay();
             if( ID_menu == 23){display.setCursor(0,0);    display.println("Hkx  Modif ID");} 
             if( ID_menu == 33){display.setCursor(0,0);    display.println("Dyn  Modif ID");} 
             display.setCursor(0,20);   display.println("Nouvel ID ? "); 
             display.display(); 
            do{
                key = keypad.waitForKey();
                if(key && key!='*' && key!='#' ){ 
                  if(count==2) {
                    ID_servo_souhaite = (int(key)-48) + (ID_servo_souhaite*10);
                  }
                  if(count==1) {
                    ID_servo_souhaite = (int(key)-48) + (ID_servo_souhaite*10);
                    count=2;
                  }
                  if(count==0) {
                    ID_servo_souhaite = (int(key)-48);
                    count=1;
                  }
                  if(ID_servo_souhaite>253 || ID_servo_souhaite<0)
                  {
                    ID_servo_souhaite = 0;
                    count = 0;
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("ID erreur "); 
                    display.setCursor(0,17);   display.println("Entre 1 et 253"); 
                    display.display();   
                  }else{
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("Nouvel ID   ");  
                    display.setCursor(90,0);   display.println(ID_servo_souhaite); 
                    display.setCursor(0,14);   display.println("Valider *  ");
                    display.setCursor(0,21);   display.println("puis redemarrer");
                    display.display();  
                  }
                }
            }while(key!='*' && ID_servo_souhaite != 0);
            display.clearDisplay();
            if(ID_menu == 23){
                Herkulex.set_ID(ID_servo,ID_servo_souhaite);
                ID_menu = 20;
                display.setCursor(0,0);        display.print("Hkx");
            }else{
                Dynamixel.setID(ID_servo,ID_servo_souhaite);
                ID_menu = 30; display.setCursor(0,0);        display.print("Dyn");
            }
            display.setCursor(30,0);       display.println("1-Rotation"); 
            display.setCursor(30,8);       display.println("2-Scan"); 
            display.setCursor(30,16);      display.println("3-Modif ID"); 
            display.setCursor(30,24);      display.println("4-Modif Baudrate");
            display.display();       
      break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
     
      case 24: //Herkulex Modif Baudrate
      case 34: //Dyn Modif Baudrate
          ID_servo = 0;
          count = 0;
          display.clearDisplay(); 
          display.setCursor(0,0);   display.println("Baudrate"); 
          display.setCursor(70,0);   display.println("1-1000000"); 
          display.setCursor(70,8);   display.println("2-666666"); 
          display.setCursor(70,16);   display.println("3-115200"); 
          display.setCursor(70,24);   display.println("4-57600");  
          display.display();
          do{
              key = keypad.waitForKey();
          }while(key!='1' && key!='2' && key!='3' && key!='4');
          if( ID_menu == 24){
              if(key=='1'){  Herkulex.beginSerial2(1000000); } //open serial port 2 to 1M
              if(key=='2'){  Herkulex.beginSerial2(666666); } //open serial port 2 to 1M
              if(key=='3'){  Herkulex.beginSerial2(115200);  }//open serial port 2 to 115200
              if(key=='4'){  Herkulex.beginSerial2(57600);  }//open serial port 2 to 57600
          }else{ 
              if(key=='1'){  Dynamixel.begin(Serial3,1000000); } //open serial port 2 to 1M
              if(key=='2'){  Dynamixel.begin(Serial3,666666); } //open serial port 2 to 1M
              if(key=='3'){  Dynamixel.begin(Serial3,115200);  }//open serial port 2 to 115200
              if(key=='4'){  Dynamixel.begin(Serial3,57600);  }//open serial port 2 to 57600
          }
           display.clearDisplay();
           if( ID_menu == 24){display.setCursor(0,0);    display.println("Hkx  Modif Baudrate");} 
           if( ID_menu == 34){display.setCursor(0,0);    display.println("Dyn  Modif Baudrate");} 
           display.setCursor(0,20);   display.println("Num ID ? "); 
           display.display(); 
          do{
              key = keypad.waitForKey();
              if(key && key!='*' && key!='#' ){ 
                if(count==2) {
                  ID_servo = (int(key)-48) + (ID_servo*10);
                }
                if(count==1) {
                  ID_servo = (int(key)-48) + (ID_servo*10);
                  count=2;
                }
                if(count==0) {
                  ID_servo = (int(key)-48);
                  count=1;
                }
                if(ID_servo>253 || ID_servo<0)
                {
                  ID_servo = 0;
                  count = 0;
                  display.clearDisplay();
                  display.setCursor(0,0);   display.println("ID erreur "); 
                  display.setCursor(0,17);   display.println("Entre 1 et 253"); 
                  display.display();   
                }else{
                  display.clearDisplay();
                  display.setCursor(0,0);   display.println("Num ID actuel ");  
                  display.setCursor(80,0);   display.println(ID_servo); 
                  display.setCursor(0,17);   display.println("Valider *");
                  display.display();  
                }
              }
         }while(key!='*');

          display.clearDisplay(); 
          display.setCursor(0,0);   display.println("Nouveau Bd?"); 
          display.setCursor(70,0);   display.println(" 1-1000000"); 
          //display.setCursor(0,8);   display.println("puis"); 
          display.setCursor(70,8);   display.println("2-666666"); 
          //display.setCursor(0,16);   display.println("redemarrer"); 
          display.setCursor(70,16);   display.println("3-115200"); 
          display.setCursor(70,24);   display.println("4-57600");  
          display.display();
          if(ID_menu == 24){
              Herkulex.reboot(ID_servo); //reboot first motor
              delay(500); 
              Herkulex.initialize(); //initialize motors
              delay(200); 
          }
          do{
              key = keypad.waitForKey();
          }while(key!='1' && key!='2' && key!='3' && key!='4');
          if( ID_menu == 24){
              if(key=='1'){  Herkulex.writeRegistryEEP(ID_servo,0x04,0x01); } 
              if(key=='2'){  Herkulex.writeRegistryEEP(ID_servo,0x04,0x02); } 
              if(key=='3'){  Herkulex.writeRegistryEEP(ID_servo,0x04,0x10); }
              if(key=='4'){  Herkulex.writeRegistryEEP(ID_servo,0x04,0x22); }
              delay(200);  
              Herkulex.reboot(ID_servo); //reboot first motor
              delay(200);  
          }else{ 
              if(key=='1'){  Dynamixel.setBaudRate(ID_servo,1000000); } //1
              if(key=='2'){  Dynamixel.setBaudRate(ID_servo,666666); } //2
              if(key=='3'){  Dynamixel.setBaudRate(ID_servo,115200); }//16
              if(key=='4'){  Dynamixel.setBaudRate(ID_servo,57600);  }//34
          }
          
            display.clearDisplay();
            if(ID_menu == 24){
                ID_menu = 20;
                //Herkulex.end();
                display.setCursor(0,0);        display.print("Hkx");
            }else{
                ID_menu = 30; display.setCursor(0,0);        display.print("Dyn");
            }
            display.setCursor(30,0);       display.println("1-Rotation"); 
            display.setCursor(30,8);       display.println("2-Scan"); 
            display.setCursor(30,16);      display.println("3-Modif ID"); 
            display.setCursor(30,24);      display.println("4-Modif Baudrate");
            display.display();       
      break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////          
      case 41: // Config Clear Error 
      case 42: // Config Torque LED Policy
      //case 43: // Config LED Policy
              display.clearDisplay(); 
              display.setCursor(0,0);    display.println("Servomoteur ?"); 
              display.setCursor(30,8);   display.println("1-Herkulex"); 
              display.setCursor(30,16);   display.println("2-Dynamixel"); 
              display.display();
              do{
                  key = keypad.waitForKey();
              }while(key!='1' && key!='2');
              if( key='1'){
                    servomoteur = "Hkx";
              }else{ 
                    servomoteur = "Dyn";
              }
              display.clearDisplay(); 
              display.setCursor(0,0);    display.println("Baudrate ?"); 
              display.setCursor(70,0);   display.println("1-1000000"); 
              display.setCursor(70,8);   display.println("2-666666"); 
              display.setCursor(70,16);   display.println("3-115200"); 
              display.setCursor(70,24);   display.println("4-57600");  
              display.display();
              do{
                  key = keypad.waitForKey();
              }while(key!='1' && key!='2' && key!='3' && key!='4');
              if( servomoteur == "Hkx"){
                  if(key=='1'){  Herkulex.beginSerial2(1000000); } //open serial port 2 to 1M
                  if(key=='2'){  Herkulex.beginSerial2(666666);  }//open serial port 2 to 666666
                  if(key=='3'){  Herkulex.beginSerial2(115200);  }//open serial port 2 to 115200
                  if(key=='4'){  Herkulex.beginSerial2(57600);  }//open serial port 2 to 57600
              }else if (servomoteur == "Dyn"){ 
                  if(key=='1'){ Dynamixel.begin(Serial3,1000000); } //open serial port 2 to 1M
                  if(key=='2'){ Dynamixel.begin(Serial3,666666); } //open serial port 2 to 1M
                  if(key=='3'){ Dynamixel.begin(Serial3,115200);  }//open serial port 2 to 115200
                  if(key=='4'){ Dynamixel.begin(Serial3,57600);  }//open serial port 2 to 57600
              }
              display.clearDisplay();
               if( servomoteur == "Hkx"){display.setCursor(0,0);    display.println("Hkx   ID ?");} 
               if( servomoteur == "Dyn"){display.setCursor(0,0);    display.println("Dyn   ID ?");}
              display.display(); 
              ID_servo = 0;
              count = 0;
              do{
                key = keypad.waitForKey();
                if(key && key!='*' && key!='#' ){ 
                  if(count==2) {
                    ID_servo = (int(key)-48) + (ID_servo*10);
                  }
                  if(count==1) {
                    ID_servo = (int(key)-48) + (ID_servo*10);
                    count=2;
                  }
                  if(count==0) {
                    ID_servo = (int(key)-48);
                    count=1;
                  }
                  if(ID_servo>253 || ID_servo<0)
                  {
                    ID_servo = 0;
                    count = 0;
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("ID erreur "); 
                    display.setCursor(0,17);   display.println("Entre 1 et 253"); 
                    display.display();   
                  }else{
                    display.clearDisplay();
                    display.setCursor(0,0);   display.println("Num ID actuel  ");  
                    display.setCursor(90,0);   display.println(ID_servo); 
                    display.setCursor(0,17);   display.println("Valider *");
                    display.display();  
                  }
                }
            }while(key!='*' && ID_servo != 0);
             display.clearDisplay();
             display.setCursor(0,0);    display.println("Configuration ..."); 
             display.display(); 
            if(ID_menu == 41){
                Herkulex.clearError(ID_servo); 
                Herkulex.setLed(ID_servo,0);
                delay(200); 
                Herkulex.reboot(ID_servo); //reboot first motor
                delay(500); 
                Herkulex.initialize(); //initialize motors
                delay(200); 
            }
            if(ID_menu == 42){
                Herkulex.writeRegistryEEP(ID_servo,0x09,0x14);
                delay(200);
                Herkulex.writeRegistryEEP(ID_servo,0x08,0x14);
                delay(200); 
                Herkulex.reboot(ID_servo); //reboot first motor
                delay(500); 
                Herkulex.initialize(); //initialize motors
                delay(200); 
            }
            ID_servo = 0;
            ID_menu = 40;
            delay(500);
            display.clearDisplay();
            display.setCursor(0,0);        display.print("Config");
            display.setCursor(10,8);       display.println("1-ClearError"); 
            display.setCursor(10,16);       display.println("2-Torque/LedPolicy"); 
          //  display.setCursor(10,24);       display.println("3-LedPolicy"); 
            display.display();       
      break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
      default : 
        delay(50);
      break;
  }
  delay(100);                       // waits 15ms for the servo to reach the position 
} 


// Taking care of some special events.
void keypadEvent(KeypadEvent key){
  display.setTextSize(1);
  display.setTextColor(WHITE);

  if(keypad.getState( ) == PRESSED){
    switch(ID_menu){
/////////////////////////////////////////////////////////////////////////////////////////////////////////    
    case 0:
      if(key == '*'){ 
        ID_menu = 1;
        display.clearDisplay();
        display.setCursor(0,0);         display.print("Menu ");
        display.setCursor(30,0);        display.println("1-ServoMoteur RC"); 
        display.setCursor(30,8);        display.println("2-HerkuleX"); 
        display.setCursor(30,16);       display.println("3-Dynamixel"); 
        display.setCursor(30,24);       display.println("4-ConfigReg");
        display.display(); 
      }
      break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////    
    case 1: // menu
      if(key=='1'){ // Menu Servomoteur RC
        ID_menu = 10;  
        display.clearDisplay();
        display.setCursor(0,0);    display.print("RC");
        display.setCursor(30,8);   display.println("1-Rotation"); 
        display.display(); 
      }
      if(key=='2'){ // Menu Herkulex
        ID_menu = 20;   
        display.clearDisplay();
        display.setCursor(0,0);        display.print("Hkx");
        display.setCursor(30,0);       display.println("1-Rotation"); 
        display.setCursor(30,8);       display.println("2-Scan"); 
        display.setCursor(30,16);      display.println("3-Modif ID"); 
        display.setCursor(30,24);      display.println("4-Modif Baudrate");
        display.display(); 
      }
      if(key=='3'){  // menu Dyn 
        ID_menu = 30;  
        display.clearDisplay();
        display.setCursor(0,0);        display.print("Dyn3");
        display.setCursor(30,0);       display.println("1-Rotation"); 
        display.setCursor(30,8);       display.println("2-Scan"); 
        display.setCursor(30,16);      display.println("3-Modif ID"); 
        display.setCursor(30,24);      display.println("4-Modif Baudrate");
        display.display();  
      }
      if(key=='4'){ // Menu ConfigReg
        ID_menu = 40;  
        display.clearDisplay();
        display.setCursor(0,0);        display.print("Config");
        display.setCursor(10,8);       display.println("1-ClearError"); 
        display.setCursor(10,16);       display.println("2-Torque/LedPolicy"); 
        //display.setCursor(10,24);       display.println("3-LedPolicy"); 
        display.display();  
      }
      if(key=='#'){ 
        ID_menu = 0; 
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,0);        display.println("Menu :");
        display.setCursor(0,17);       display.println("Press *");
        display.display();
        display.setTextSize(1);
      }
      break;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 10:  // Menu RC
      if(key=='1'){ // Menu RC Rotation
        ID_menu = 11;  
        display.clearDisplay();
        display.setCursor(0,0);    display.print("RC - Rotation");
        display.setCursor(30,8);   display.println("1-Point zero"); 
        display.setCursor(30,16);   display.println("2-Manuel"); 
        display.setCursor(30,24);  display.println("3-Position"); 
        display.display(); 
      }
      if(key=='#'){ 
        ID_menu = 1;
        display.clearDisplay();
        display.setCursor(0,0);         display.print("Menu ");
        display.setCursor(30,0);        display.println("1-ServoMoteur RC"); 
        display.setCursor(30,8);        display.println("2-HerkuleX"); 
        display.setCursor(30,16);       display.println("3-Dynamixel"); 
        display.setCursor(30,24);       display.println("4-ConfigReg");
        display.display(); 
      }
      break;
 /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 11:  //Menu RC Rotation
      if(key=='1'){ 
        ID_menu = 111;  //Point zero
        Servo_Valide = false;
      }
      if(key=='2'){ 
        ID_menu = 112;  // Manuel
        Servo_Valide = false;
      }
      if(key=='3'){ 
        ID_menu = 113;  // Position
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 10;
        display.clearDisplay();
        display.setCursor(0,0);    display.print("RC");
        display.setCursor(30,8);   display.println("1-Rotation"); 
        display.display(); 
      }
      break;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 20:  // Menu Herkulex
      if(key=='1'){ // Menu Hkx Rotation
        ID_menu = 21;  
        display.clearDisplay();
        display.setCursor(0,0);    display.print("Hkx - Rotation");
        display.setCursor(30,8);   display.println("1-Point zero"); 
        display.setCursor(30,16);   display.println("2-Manuel"); 
        display.setCursor(30,24);  display.println("3-Position"); 
        display.display(); 
      }
      if(key=='2'){ // Menu Hkx Scan
        ID_menu = 22;  
        Servo_Valide = false;
      }
      if(key=='3'){ // Menu Hkx Modif ID
        ID_menu = 23;  
        Servo_Valide = false;
      }
      if(key=='4'){ // Menu Hkx Modif Baudrate
        ID_menu = 24;  
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 1;
        display.clearDisplay();
        display.setCursor(0,0);         display.print("Menu ");
        display.setCursor(30,0);        display.println("1-ServoMoteur RC"); 
        display.setCursor(30,8);        display.println("2-HerkuleX"); 
        display.setCursor(30,16);       display.println("3-Dynamixel"); 
        display.setCursor(30,24);       display.println("4-ConfigReg");
        display.display(); 
      }
      break;     
   /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 21:  //Menu Hkx Rotation
      if(key=='1'){ 
        ID_menu = 211;  //Point zero
        Servo_Valide = false;
      }
      if(key=='2'){ 
        ID_menu = 212;  // Manuel
        Servo_Valide = false;
      }
      if(key=='3'){ 
        ID_menu = 213;  // Position
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 20;
        display.clearDisplay();
        display.setCursor(0,0);        display.print("Hkx");
        display.setCursor(30,0);       display.println("1-Rotation"); 
        display.setCursor(30,8);       display.println("2-Scan"); 
        display.setCursor(30,16);      display.println("3-Modif ID"); 
        display.setCursor(30,24);      display.println("4-Modif Baudrate");
        display.display(); 
      }
      break;    
  /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 30:  // Menu Dyn
      if(key=='1'){ // Menu Dyn  Rotation
        ID_menu = 31;  
        display.clearDisplay();
        display.setCursor(0,0);    display.print("Dyn3 - Rotation");
        display.setCursor(30,8);   display.println("1-Point zero"); 
        display.setCursor(30,16);   display.println("2-Manuel"); 
        display.setCursor(30,24);  display.println("3-Position"); 
        display.display(); 
      }
      if(key=='2'){ // Menu Dyn  Scan
        ID_menu = 32;  
        Servo_Valide = false;
      }
      if(key=='3'){ // Menu Dyn  Modif ID
        ID_menu = 33;
        Servo_Valide = false;  
      }
      if(key=='4'){ // Menu Dyn  Modif Baudrate
        ID_menu = 34;  
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 1;
        display.clearDisplay();
        display.setCursor(0,0);         display.print("Menu ");
        display.setCursor(30,0);        display.println("1-ServoMoteur RC"); 
        display.setCursor(30,8);        display.println("2-HerkuleX"); 
        display.setCursor(30,16);       display.println("3-Dynamixel"); 
        display.setCursor(30,24);       display.println("4-ConfigReg");
        display.display(); 
      }
      break;     
   /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 31:  //Menu Dyn  Rotation
      if(key=='1'){ 
       ID_menu = 311;  //Point zero
        Servo_Valide = false;
      }
      if(key=='2'){ 
        ID_menu = 312;  // Manuel
        Servo_Valide = false;
      }
      if(key=='3'){ 
        ID_menu = 313;  // Position
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 30;
        display.clearDisplay();
        display.setCursor(0,0);        display.print("Dyn");
        display.setCursor(30,0);       display.println("1-Rotation"); 
        display.setCursor(30,8);       display.println("2-Scan"); 
        display.setCursor(30,16);      display.println("3-Modif ID"); 
        display.setCursor(30,24);      display.println("4-Modif Baudrate");
        display.display(); 
      }
      break;    
  /////////////////////////////////////////////////////////////////////////////////////////////////////////     
    case 40:  // Menu ConfigReg
      if(key=='1'){ // Menu ConfigReg Clear Error
        ID_menu = 41;  
        Servo_Valide = false;
      }
      if(key=='2'){ // Menu ConfigReg Torque Policy
        ID_menu = 42;  
        Servo_Valide = false;
      }
      if(key=='3'){ // Menu ConfigReg Led Policy
        ID_menu = 43;  
        Servo_Valide = false;
      }
      if(key=='#'){ 
        ID_menu = 1;
        display.clearDisplay();
        display.setCursor(0,0);         display.print("Menu ");
        display.setCursor(30,0);        display.println("1-ServoMoteur RC"); 
        display.setCursor(30,8);        display.println("2-HerkuleX"); 
        display.setCursor(30,16);       display.println("3-Dynamixel"); 
        display.setCursor(30,24);       display.println("4-ConfigReg");
        display.display(); 
      }
      break;    
    } 
  }
  delay(50); 
  key = ' ';
}

