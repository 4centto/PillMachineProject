// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Servo.h>

SoftwareSerial miBT(10, 11); //(TXD, RXD)
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
Servo servo12, servo34;

//Variables para el bluetooth
int contador = 0, secondCont = 0;
String dia = "", pastilla = "", cant = "", hora = "", minutos = "";
bool printed = false;

//Variables para el control de los servos
int pastillaS = 0, cantS = 0, horaS = 0, minutosS = 0;

int BUZZER = 8;
bool alarm = false;

void setup () 
{
    Serial.begin(57600);
    miBT.begin(38400);

    //Configuracion de los servos
    servo12.attach(3, 500, 2450);
    servo34.attach(6, 400, 2400);
    
    servo12.write(90);
    servo34.write(90);

    pinMode(8, OUTPUT); //Buzzer

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    pinMode(12, OUTPUT);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
        Rtc.SetDateTime(compiled);
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
    
}

void loop () 
{
  if(alarm == false){
    RtcDateTime now = Rtc.GetDateTime();
    //Month(), Day(), Year(), Hour(), Minute(), Second()
    
    if(now.Hour() == 0 && now.Minute() == 0 && now.Second() == 0){
      int currDay = EEPROM.read(0);
      if(currDay < 7){
        currDay ++;
      } else if(currDay == 7){
        currDay = 1;
      }
      
      EEPROM.write(0, currDay);
      getCalendar(currDay);
      delay(1500);
    }

    //Control de servos
    servoControl(now);
    
    //Lectura en Bluetooth
    if(miBT.available()){  //Lee BT y envia a arduino

      printed = false;
      char recvChar = miBT.read();
      
      if(recvChar != '#'){
        if(contador == 0){
          dia = recvChar;
        } else if(contador == 1){
          pastilla = recvChar;
        } else if(contador == 2){
          cant = recvChar;
        } else if(contador == 3){
          if(secondCont == 0){
            hora = recvChar; 
            secondCont += 1;
          } else if(secondCont == 1){
            hora += recvChar;
          }
        } else if(contador == 4){
          if(secondCont == 0){
            minutos = recvChar;
            secondCont += 1;
          } else if(secondCont == 1){
            minutos += recvChar;
          }
        }
      } else {
        contador += 1;
        secondCont = 0;
      }
    
    } else {
      contador = 0;
      if(dia != "" && pastilla != "" && cant != "" && hora != "" && minutos != ""){
        if(printed == false){
          Serial.println("Dia: " + dia);
          Serial.println("Pastilla: " + pastilla);
          Serial.println("Cantidad: " + cant);
          Serial.println("Hora: " + hora);
          Serial.println("Minutos: " + minutos);  

          saveData(dia, pastilla, cant, hora, minutos);
          
          printed = true;
        }
      }
    }

    //Fallo en el reloj
    //if (!now.IsValid()){
    //    Serial.println("RTC lost confidence in the DateTime!");
    //}
  } else {
    for(int i = 0; i < 3; i++){
      song(BUZZER);
      delay(2000);
    }
    alarm = false;
  }
}

void song(int buzzerPin){
  tone(buzzerPin, 1568);
  delay(128);
  noTone(buzzerPin);

  tone(buzzerPin, 2093);
  delay(150);
  noTone(8);
}

void saveData(String dia, String pastilla, String cant, String hora, String minutos){
  int diaInt = dia.toInt();
  int pillInt = castPill(pastilla);

  EEPROM.write(diaInt, pillInt);
  savePill(pillInt, cant.toInt());
  saveHour(diaInt, hora.toInt(), minutos.toInt());
  
}

void saveHour(int d, int h, int m){
  if(d == 1){
    EEPROM.write(16, h);
    EEPROM.write(17, m);
  } else if(d == 2){
    EEPROM.write(18, h);
    EEPROM.write(19, m);
  } else if(d == 3){
    EEPROM.write(20, h);
    EEPROM.write(21, m);
  } else if(d == 4){
    EEPROM.write(22, h);
    EEPROM.write(23, m);
  } else if(d == 5){
    EEPROM.write(24, h);
    EEPROM.write(25, m);
  } else if(d == 6){
    EEPROM.write(26, h);
    EEPROM.write(27, m);
  } else if(d == 7){
    EEPROM.write(28, h);
    EEPROM.write(29, m);
  }
}

void savePill(int p, int c){
  if(p == 8){
    EEPROM.write(p, 12);
    EEPROM.write(12, c);
  } else if(p == 9){
    EEPROM.write(p, 13);
    EEPROM.write(13, c);
  } else if(p == 10){
    EEPROM.write(p, 14);
    EEPROM.write(14, c);
  } else if(p == 11){
    EEPROM.write(p, 15);
    EEPROM.write(15, c);
  }
}

int castPill(String p){
  if(p == "1"){
    return 8;
  } else if(p == "2"){
    return 9;
  } else if(p == "3"){
    return 10;
  } else if(p == "4"){
    return 11;
  }
}

int castPill(int p){
  if(p == 8){
    return 1;
  } else if(p == 9){
    return 2;
  } else if(p == 10){
    return 3;
  } else if(p == 11){
    return 4;
  }
}

void getPillsCalendar(){
  for(int i = 1; i < 8; i++){
    getCalendar(i);
  }
}

void getCalendar(int dia){
  int p = EEPROM.read(dia);
  int c = EEPROM.read(EEPROM.read(p));
  int h = 0, m = 0;

  if(dia == 1){
    Serial.print("Lunes (Pastilla: ");
    h = EEPROM.read(16);
    m = EEPROM.read(17);
  } else if(dia == 2){
    Serial.print("Martes (Pastilla: ");
    h = EEPROM.read(18);
    m = EEPROM.read(19);
  } else if(dia == 3){
    Serial.print("Miercoles (Pastilla: ");
    h = EEPROM.read(20);
    m = EEPROM.read(21);
  } else if(dia == 4){
    Serial.print("Jueves (Pastilla: ");
    h = EEPROM.read(22);
    m = EEPROM.read(23);
  } else if(dia == 5){
    Serial.print("Viernes (Pastilla: ");
    h = EEPROM.read(24);
    m = EEPROM.read(25);
  } else if(dia == 6){
    Serial.print("Sabado (Pastilla: ");
    h = EEPROM.read(26);
    m = EEPROM.read(27);
  } else if(dia == 7){
    Serial.print("Domingo (Pastilla: ");
    h = EEPROM.read(28);
    m = EEPROM.read(29);
  }
  
  Serial.print(castPill(p));
  Serial.print(" Cantidad: ");
  Serial.print(c);
  Serial.print(" Hora: ");
  Serial.print(h);
  Serial.print(" Minutos: ");
  Serial.print(m);
  Serial.println(")"); 
}

void servoControl(RtcDateTime now){
  int currDay = EEPROM.read(0);
  int pillS = EEPROM.read(currDay);
  int cantS = EEPROM.read(EEPROM.read(pillS));
  int h = 0, m = 0;

  if(currDay == 1){
    h = EEPROM.read(16);
    m = EEPROM.read(17);
  } else if(currDay == 2){
    h = EEPROM.read(18);
    m = EEPROM.read(19);
  } else if(currDay == 3){
    h = EEPROM.read(20);
    m = EEPROM.read(21);
  } else if(currDay == 4){
    h = EEPROM.read(22);
    m = EEPROM.read(23);
  } else if(currDay == 5){
    h = EEPROM.read(24);
    m = EEPROM.read(25);
  } else if(currDay == 6){
    h = EEPROM.read(26);
    m = EEPROM.read(27);
  } else if(currDay == 7){
    h = EEPROM.read(28);
    m = EEPROM.read(29);
  }
    
  //Control de los servos
  if(now.Hour() == h && now.Minute() == m && now.Second() == 0){
    if(pillS == 8){
      moveOne(cantS, servo12);
      alarm = true;
    } else if(pillS == 9){
      moveTwo(cantS, servo12);
      alarm = true;
    } else if(pillS == 10){
      moveOne(cantS, servo34);
      alarm = true;
    } else if(pillS == 11){
      moveTwo(cantS, servo34);
      alarm = true;
    }
  } else {
    servo12.write(90);
    servo34.write(90);
  }
  
}

void moveTwo(int c, Servo s){
  for(int i = 0; i < c; i++){
    for(int j = 90; j > 45; j--){
      s.write(j);
      delay(20);
    }
    delay(1000);
    for(int j = 45; j < 90; j++){
      s.write(j);
      delay(20);
    }
    delay(1000);
  }
}

void moveOne(int c, Servo s){
  for(int i = 0; i < c; i++){
    for(int j = 90; j < 135; j++){
      s.write(j);
      delay(20);
    }
    delay(1000);
    for(int j = 135; j > 90; j--){
      s.write(j);
      delay(20);
    }
    delay(1000);
  }
}
