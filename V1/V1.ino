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

    EEPROM.write(0, 1);

    //Configuracion de los servos
    servo12.attach(3, 500, 2450);
    servo34.attach(6, 400, 2600);
    
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

    getCalendar(EEPROM.read(0));
    
}

void loop () 
{
  if(alarm == false){
    RtcDateTime now = Rtc.GetDateTime();
    //Month(), Day(), Year(), Hour(), Minute(), Second()

    //Cambiar el dia
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
          saveData(dia, pastilla, cant, hora, minutos);
          printed = true;
        }
      }
    }
  } else {
    song(BUZZER);
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
  int pillInt = pastilla.toInt();
  int cantInt = cant.toInt();
  int horaInt = hora.toInt();
  int minutosInt = minutos.toInt();

  //Primero tenemos que validar de que dia estamos hablando
  switch (diaInt) {

    case 1:
      if(pillInt == 1){
        EEPROM.write(8, cantInt);
        EEPROM.write(36, horaInt);
        EEPROM.write(37, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(9, cantInt);
        EEPROM.write(38, horaInt);
        EEPROM.write(39, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(10, cantInt);
        EEPROM.write(40, horaInt);
        EEPROM.write(41, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(11, cantInt);
        EEPROM.write(42, horaInt);
        EEPROM.write(43, minutosInt);
      }
      break;


    case 2:
      if(pillInt == 1){
        EEPROM.write(12, cantInt);
        EEPROM.write(44, horaInt);
        EEPROM.write(45, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(13, cantInt);
        EEPROM.write(46, horaInt);
        EEPROM.write(47, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(14, cantInt);
        EEPROM.write(48, horaInt);
        EEPROM.write(49, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(15, cantInt);
        EEPROM.write(50, horaInt);
        EEPROM.write(51, minutosInt);
      }
      break;


    case 3:
      if(pillInt == 1){
        EEPROM.write(16, cantInt);
        EEPROM.write(52, horaInt);
        EEPROM.write(53, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(17, cantInt);
        EEPROM.write(54, horaInt);
        EEPROM.write(55, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(18, cantInt);
        EEPROM.write(56, horaInt);
        EEPROM.write(57, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(19, cantInt);
        EEPROM.write(58, horaInt);
        EEPROM.write(59, minutosInt);
      }
      break;


    case 4:
      if(pillInt == 1){
        EEPROM.write(20, cantInt);
        EEPROM.write(60, horaInt);
        EEPROM.write(61, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(21, cantInt);
        EEPROM.write(62, horaInt);
        EEPROM.write(63, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(22, cantInt);
        EEPROM.write(64, horaInt);
        EEPROM.write(65, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(23, cantInt);
        EEPROM.write(66, horaInt);
        EEPROM.write(67, minutosInt);
      }
      break;


    case 5:
      if(pillInt == 1){
        EEPROM.write(24, cantInt);
        EEPROM.write(68, horaInt);
        EEPROM.write(69, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(25, cantInt);
        EEPROM.write(70, horaInt);
        EEPROM.write(71, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(26, cantInt);
        EEPROM.write(72, horaInt);
        EEPROM.write(73, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(27, cantInt);
        EEPROM.write(74, horaInt);
        EEPROM.write(75, minutosInt);
      }
      break;


    case 6:
      if(pillInt == 1){
        EEPROM.write(28, cantInt);
        EEPROM.write(76, horaInt);
        EEPROM.write(77, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(29, cantInt);
        EEPROM.write(78, horaInt);
        EEPROM.write(79, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(30, cantInt);
        EEPROM.write(80, horaInt);
        EEPROM.write(81, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(31, cantInt);
        EEPROM.write(82, horaInt);
        EEPROM.write(83, minutosInt);
      }
      break;


    case 7:
      if(pillInt == 1){
        EEPROM.write(32, cantInt);
        EEPROM.write(84, horaInt);
        EEPROM.write(85, minutosInt);
      } else if(pillInt == 2){
        EEPROM.write(33, cantInt);
        EEPROM.write(86, horaInt);
        EEPROM.write(87, minutosInt);
      } else if(pillInt == 3){
        EEPROM.write(34, cantInt);
        EEPROM.write(88, horaInt);
        EEPROM.write(89, minutosInt);
      } else if(pillInt == 4){
        EEPROM.write(35, cantInt);
        EEPROM.write(90, horaInt);
        EEPROM.write(91, minutosInt);
      }
      break;


  }

  //Imprimimos el horario que se ha guardado
  getCalendar(diaInt);
  
}

void getPillsCalendar(){
  for(int i = 1; i < 8; i++){
    getCalendar(i);
  }
}

void getCalendar(int dia){

  //Primero tenemos que validar de que dia estamos hablando
  switch (dia) {

    case 1:
      Serial.println("Lunes (Pastilla 1: " + String(EEPROM.read(8)) + " Hora: " + String(EEPROM.read(36)) + " Minutos: " + String(EEPROM.read(37)) + ")");
      Serial.println("Lunes (Pastilla 2: " + String(EEPROM.read(9)) + " Hora: " + String(EEPROM.read(38)) + " Minutos: " + String(EEPROM.read(39)) + ")");
      Serial.println("Lunes (Pastilla 3: " + String(EEPROM.read(10)) + " Hora: " + String(EEPROM.read(40)) + " Minutos: " + String(EEPROM.read(41)) + ")");
      Serial.println("Lunes (Pastilla 4: " + String(EEPROM.read(11)) + " Hora: " + String(EEPROM.read(42)) + " Minutos: " + String(EEPROM.read(43)) + ")");
      Serial.println("");
      break;


    case 2:
      Serial.println("Martes (Pastilla 1: " + String(EEPROM.read(12)) + " Hora: " + String(EEPROM.read(44)) + " Minutos: " + String(EEPROM.read(45)) + ")");
      Serial.println("Martes (Pastilla 2: " + String(EEPROM.read(13)) + " Hora: " + String(EEPROM.read(46)) + " Minutos: " + String(EEPROM.read(47)) + ")");
      Serial.println("Martes (Pastilla 3: " + String(EEPROM.read(14)) + " Hora: " + String(EEPROM.read(48)) + " Minutos: " + String(EEPROM.read(49)) + ")");
      Serial.println("Martes (Pastilla 4: " + String(EEPROM.read(15)) + " Hora: " + String(EEPROM.read(50)) + " Minutos: " + String(EEPROM.read(51)) + ")");
      Serial.println("");
      break;


    case 3:
      Serial.println("Miercoles (Pastilla 1: " + String(EEPROM.read(16)) + " Hora: " + String(EEPROM.read(52)) + " Minutos: " + String(EEPROM.read(53)) + ")");
      Serial.println("Miercoles (Pastilla 2: " + String(EEPROM.read(17)) + " Hora: " + String(EEPROM.read(54)) + " Minutos: " + String(EEPROM.read(55)) + ")");
      Serial.println("Miercoles (Pastilla 3: " + String(EEPROM.read(18)) + " Hora: " + String(EEPROM.read(56)) + " Minutos: " + String(EEPROM.read(57)) + ")");
      Serial.println("Miercoles (Pastilla 4: " + String(EEPROM.read(19)) + " Hora: " + String(EEPROM.read(58)) + " Minutos: " + String(EEPROM.read(59)) + ")");
      Serial.println("");
      break;


    case 4:
      Serial.println("Jueves (Pastilla 1: " + String(EEPROM.read(20)) + " Hora: " + String(EEPROM.read(60)) + " Minutos: " + String(EEPROM.read(61)) + ")");
      Serial.println("Jueves (Pastilla 2: " + String(EEPROM.read(21)) + " Hora: " + String(EEPROM.read(62)) + " Minutos: " + String(EEPROM.read(63)) + ")");
      Serial.println("Jueves (Pastilla 3: " + String(EEPROM.read(22)) + " Hora: " + String(EEPROM.read(64)) + " Minutos: " + String(EEPROM.read(65)) + ")");
      Serial.println("Jueves (Pastilla 4: " + String(EEPROM.read(23)) + " Hora: " + String(EEPROM.read(66)) + " Minutos: " + String(EEPROM.read(67)) + ")");
      Serial.println("");
      break;


    case 5:
      Serial.println("Viernes (Pastilla 1: " + String(EEPROM.read(24)) + " Hora: " + String(EEPROM.read(68)) + " Minutos: " + String(EEPROM.read(69)) + ")");
      Serial.println("Viernes (Pastilla 2: " + String(EEPROM.read(25)) + " Hora: " + String(EEPROM.read(70)) + " Minutos: " + String(EEPROM.read(71)) + ")");
      Serial.println("Viernes (Pastilla 3: " + String(EEPROM.read(26)) + " Hora: " + String(EEPROM.read(72)) + " Minutos: " + String(EEPROM.read(73)) + ")");
      Serial.println("Viernes (Pastilla 4: " + String(EEPROM.read(27)) + " Hora: " + String(EEPROM.read(74)) + " Minutos: " + String(EEPROM.read(75)) + ")");
      Serial.println("");
      break;


    case 6:
      Serial.println("Sabado (Pastilla 1: " + String(EEPROM.read(28)) + " Hora: " + String(EEPROM.read(76)) + " Minutos: " + String(EEPROM.read(77)) + ")");
      Serial.println("Sabado (Pastilla 2: " + String(EEPROM.read(29)) + " Hora: " + String(EEPROM.read(78)) + " Minutos: " + String(EEPROM.read(79)) + ")");
      Serial.println("Sabado (Pastilla 3: " + String(EEPROM.read(30)) + " Hora: " + String(EEPROM.read(80)) + " Minutos: " + String(EEPROM.read(81)) + ")");
      Serial.println("Sabado (Pastilla 4: " + String(EEPROM.read(31)) + " Hora: " + String(EEPROM.read(82)) + " Minutos: " + String(EEPROM.read(83)) + ")");
      Serial.println("");
      break;


    case 7:
      Serial.println("Domingo (Pastilla 1: " + String(EEPROM.read(32)) + " Hora: " + String(EEPROM.read(84)) + " Minutos: " + String(EEPROM.read(85)) + ")");
      Serial.println("Domingo (Pastilla 2: " + String(EEPROM.read(33)) + " Hora: " + String(EEPROM.read(86)) + " Minutos: " + String(EEPROM.read(87)) + ")");
      Serial.println("Domingo (Pastilla 3: " + String(EEPROM.read(34)) + " Hora: " + String(EEPROM.read(88)) + " Minutos: " + String(EEPROM.read(89)) + ")");
      Serial.println("Domingo (Pastilla 4: " + String(EEPROM.read(35)) + " Hora: " + String(EEPROM.read(90)) + " Minutos: " + String(EEPROM.read(91)) + ")");
      Serial.println("");
      break;


  }
  
}

void servoControl(RtcDateTime now){
  int currDay = EEPROM.read(0);
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;
  int h1 = 0, h2 = 0, h3 = 0, h4 = 0;
  int m1 = 0, m2 = 0, m3 = 0, m4 = 0;

  //Primero tenemos que ver en que dia estamos
  switch (currDay) {

    case 1:
      c1 = EEPROM.read(8);
      h1 = EEPROM.read(36);
      m1 = EEPROM.read(37);

      c2 = EEPROM.read(9);
      h2 = EEPROM.read(38);
      m2 = EEPROM.read(39);

      c3 = EEPROM.read(10);
      h3 = EEPROM.read(40);
      m3 = EEPROM.read(41);

      c4 = EEPROM.read(11);
      h4 = EEPROM.read(42);
      m4 = EEPROM.read(43);
      break;

    case 2:
      c1 = EEPROM.read(12);
      h1 = EEPROM.read(44);
      m1 = EEPROM.read(45);

      c2 = EEPROM.read(13);
      h2 = EEPROM.read(46);
      m2 = EEPROM.read(47);

      c3 = EEPROM.read(14);
      h3 = EEPROM.read(48);
      m3 = EEPROM.read(49);

      c4 = EEPROM.read(15);
      h4 = EEPROM.read(50);
      m4 = EEPROM.read(51);
      break;

    case 3:
      c1 = EEPROM.read(16);
      h1 = EEPROM.read(52);
      m1 = EEPROM.read(53);

      c2 = EEPROM.read(17);
      h2 = EEPROM.read(54);
      m2 = EEPROM.read(55);

      c3 = EEPROM.read(18);
      h3 = EEPROM.read(56);
      m3 = EEPROM.read(57);

      c4 = EEPROM.read(19);
      h4 = EEPROM.read(58);
      m4 = EEPROM.read(59);
      break;

    case 4:
      c1 = EEPROM.read(20);
      h1 = EEPROM.read(60);
      m1 = EEPROM.read(61);

      c2 = EEPROM.read(21);
      h2 = EEPROM.read(62);
      m2 = EEPROM.read(63);

      c3 = EEPROM.read(22);
      h3 = EEPROM.read(64);
      m3 = EEPROM.read(65);

      c4 = EEPROM.read(23);
      h4 = EEPROM.read(66);
      m4 = EEPROM.read(67);
      break;

    case 5:
      c1 = EEPROM.read(24);
      h1 = EEPROM.read(68);
      m1 = EEPROM.read(69);

      c2 = EEPROM.read(25);
      h2 = EEPROM.read(70);
      m2 = EEPROM.read(71);

      c3 = EEPROM.read(26);
      h3 = EEPROM.read(72);
      m3 = EEPROM.read(73);

      c4 = EEPROM.read(27);
      h4 = EEPROM.read(74);
      m4 = EEPROM.read(75);
      break;

    case 6:
      c1 = EEPROM.read(28);
      h1 = EEPROM.read(76);
      m1 = EEPROM.read(77);

      c2 = EEPROM.read(29);
      h2 = EEPROM.read(78);
      m2 = EEPROM.read(79);

      c3 = EEPROM.read(30);
      h3 = EEPROM.read(80);
      m3 = EEPROM.read(81);

      c4 = EEPROM.read(31);
      h4 = EEPROM.read(82);
      m4 = EEPROM.read(83);
      break;

    case 7:
      c1 = EEPROM.read(32);
      h1 = EEPROM.read(84);
      m1 = EEPROM.read(85);

      c2 = EEPROM.read(33);
      h2 = EEPROM.read(86);
      m2 = EEPROM.read(87);

      c3 = EEPROM.read(34);
      h3 = EEPROM.read(88);
      m3 = EEPROM.read(89);

      c4 = EEPROM.read(35);
      h4 = EEPROM.read(90);
      m4 = EEPROM.read(91);
      break;

  }

  if(now.Hour() == h1 && now.Minute() == m1-1 && now.Second() == 51){
    moveOne(c1, servo12);
    delay(500);
    alarm = true;
  }
  if(now.Hour() == h2 && now.Minute() == m2-1 && now.Second() == 51){
    moveTwo(c2, servo12);
    delay(500);
    alarm = true;
  }
  if(now.Hour() == h3 && now.Minute() == m3-1 && now.Second() == 51){
    moveOne(c3, servo34);
    delay(500);
    alarm = true;
  }
  if(now.Hour() == h4 && now.Minute() == m4-1 && now.Second() == 51){
    moveTwo(c4, servo34);
    delay(500);
    alarm = true;
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
