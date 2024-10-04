#include <Joystick.h>


#define motorPinA 6
#define motorPinB 3


#define TOTAL_derece 900
#define MIN_derece 512 - (TOTAL_derece / 3)
#define MAX_derece 512 + (TOTAL_derece / 3)

#define MAX_PWM 50

const int satirPinleri[2] = {2, 5}; // Satır pinleri
const int sutunPinleri[4] = {9, 7, 4, 8}; // Sütun pinleri

int butonDurumlari[2][4]; // 2 satır ve 4 sütun için buton durumları
int oncekiButonDurumlari[2][4]; // Önceki buton durumlarını saklamak için


bool isOutOfRange = false;
int32_t forces[2]={0};
Gains gains[2];
EffectParams effectparams[2];

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK,
  8, 0,                  // Button Count, Hat Switch Count
  true, true, true,     // X and Y, but no Z Axis
  true, true, true,   //  Rx, Ry, no Rz
  true, true,          // No rudder or throttle
  true, true, true);    // No accelerator, brake, or steering

volatile long value = 0;
int32_t g_force = 0;

bool ayarlandi = false;

void setup() 
{
  Serial.begin(115200);

  Joystick.setZAxisRange(MIN_derece, MAX_derece);                       
  Joystick.setRxAxisRange(95, 215);
  Joystick.setRyAxisRange(40, 95);
  Joystick.setRzAxisRange(75, 140);

  Joystick.setGains(gains);
  Joystick.begin(true);

  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);

  pinMode(A3, INPUT);
  pinMode(A2, INPUT);
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);

  // Satır pinlerini çıkış olarak ayarla
  for (int i = 0; i < 2; i++) {
    pinMode(satirPinleri[i], OUTPUT);
    digitalWrite(satirPinleri[i], HIGH); // Satırları başlangıçta HIGH yapıyoruz
  }
  
  // Sütun pinlerini giriş olarak ayarla ve pull-up dirençlerini aktif et
  for (int j = 0; j < 4; j++) {
    pinMode(sutunPinleri[j], INPUT_PULLUP);
  }

  // Önceki buton durumlarını sıfırla
  memset(oncekiButonDurumlari, 0, sizeof(oncekiButonDurumlari));
  
  cli();
  TCCR3A = 0; //set TCCR1A 0
  TCCR3B = 0; //set TCCR1B 0
  TCNT3  = 0; //counter init
  OCR3A = 399;
  TCCR3B |= (1 << WGM32); //open CTC mode
  TCCR3B |= (1 << CS31); //set CS11 1(8-fold Prescaler)
  TIMSK3 |= (1 << OCIE3A);
  sei();
  
  /*while(!ayarlandi)
  {
    baslangic_ayar();
  }*/
}

ISR(TIMER3_COMPA_vect){
  Joystick.getUSBPID();
}
int hedef = MAX_derece;
int motor_ayar_hiz = 70;
int bolen = 1;//8
int direksiyon_offset = 512-560;

unsigned int interval = 0;
void loop() 
{
  value = analogRead(A3) + direksiyon_offset;
  

  
  if(value > MAX_derece)
  {
    isOutOfRange = true;
    value = MAX_derece;
  }else if(value < MIN_derece)
  {
    isOutOfRange = true;
    value = MIN_derece;
  }else{
    isOutOfRange = false;
  }

  Joystick.setZAxis(value);
  Joystick.setRxAxis(analogRead(A2));
  Joystick.setRyAxis(analogRead(A1));
  Joystick.setRzAxis(analogRead(A0));

  effectparams[0].springMaxPosition = MAX_derece;
  effectparams[0].springPosition = value;
  effectparams[1].springMaxPosition = 255;
  effectparams[1].springPosition = 0;
  Joystick.setEffectParams(effectparams);
  Joystick.getForce(forces);
  
  
  Serial.print("isOutOfRange: ");
  Serial.print(isOutOfRange);
  Serial.print("\t");
  
  Serial.print("forces[0]: ");
  Serial.print(forces[0]/bolen);
  Serial.print("\t");

  Serial.print("value: ");
  Serial.print(value);
  Serial.print("\t");

  Serial.print("gaz: ");
  Serial.print(analogRead(A2));
  Serial.print(" \t ");

  Serial.print("fren: ");
  Serial.print(analogRead(A1));
  Serial.print("\t");

  Serial.print("debriyaj: ");
  Serial.print(analogRead(A0));
  Serial.print("\t");

  if(!isOutOfRange){
    if(forces[0] > 0 && abs(forces[0]/bolen) > 55)
    {
      analogWrite(motorPinA, abs(forces[0])/bolen);
      analogWrite(motorPinB, 0);
    }
    else if(forces[0] < 0 && abs(forces[0]/bolen) > 55)
    {
      analogWrite(motorPinA, 0);
      analogWrite(motorPinB, abs(forces[0])/bolen);
    }
    else
    {
      analogWrite(motorPinA, 0);
      analogWrite(motorPinB, 0);
    }
  }
  else
  {
    
    int volue_ = analogRead(A3) - direksiyon_offset;
    if(value < 512)
    {
      analogWrite(motorPinA, 0);
      analogWrite(motorPinB, (value - volue_) / 1.5 + MAX_PWM);

      Serial.print(volue_);
      Serial.print("\t");
      Serial.print(value);
      Serial.print("\t");
      Serial.print((value - volue_) / 1.5 + MAX_PWM);
    }else{
      analogWrite(motorPinA, volue_ / 1.5 + MAX_PWM);
      analogWrite(motorPinB, 0);

      Serial.print(volue_);
      Serial.print("\t");
      Serial.print((volue_ - value) / 1.5 + MAX_PWM);
    }
  }
  

  vites_kontrol();

  

  Serial.println();
}

void vites_kontrol()
{
  for (int i = 0; i < 2; i++) {
    // Şu anki satırı LOW yap ve diğerlerini HIGH tut
    digitalWrite(satirPinleri[i], LOW);
    
    for (int j = 0; j < 4; j++) {
      int butonOkuma = digitalRead(sutunPinleri[j]); // Sütun pininden dijital okuma yap
      
      if (butonOkuma == LOW) { // Eğer okuma LOW ise buton basılı
        butonDurumlari[i][j] = 1;
      } else {
        butonDurumlari[i][j] = 0;
      }
      
      // Buton ID'si hesapla (0-6 arasında)
      int butonID;
      if (i == 1 && j == 3) butonID = 0; // Buton 1
      else if (i == 0 && j == 3) butonID = 1; // Buton 2
      else if (i == 1 && j == 2) butonID = 2; // Buton 3
      else if (i == 0 && j == 2) butonID = 3; // Buton 4
      else if (i == 1 && j == 1) butonID = 4; // Buton 5
      else if (i == 0 && j == 1) butonID = 5; // Buton 6
      else if (i == 0 && j == 0) butonID = 6; // Buton 7

      
      /*if(!butonOkuma)
      {
        Serial.print("Satır: ");
        Serial.print(i);
        Serial.print(", Sütun: ");
        Serial.print(j);
        Serial.println(" - Durum: ");
      }*/

      // Eğer buton durumu değişmişse, joystick butonunu güncelle
      if (butonDurumlari[i][j] != oncekiButonDurumlari[i][j]) {
        if (butonDurumlari[i][j] == 1) {
          Joystick.pressButton(butonID);
        } else {
          Joystick.releaseButton(butonID);
        }
      }

      // Önceki buton durumunu güncelle
      oncekiButonDurumlari[i][j] = butonDurumlari[i][j];
    }
    
    // Şu anki satırı tekrar HIGH yap
    digitalWrite(satirPinleri[i], HIGH);
  }
}
long int baslangic_ayar_zaman = 0;
bool ortalaniyor = false;
void baslangic_ayar()
{
  value = analogRead(A3) - direksiyon_offset;
  Serial.println(value);

  if(value >= MAX_derece-10 && hedef != MIN_derece)
  {
    hedef = MIN_derece;
    analogWrite(motorPinA, 50);
    analogWrite(motorPinB, 0);
    delay(80);
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, 0);
    delay(200);
  }
  else if(value <= MIN_derece+10 && hedef != 512)
  {
    hedef = 512;
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, 50);
    delay(80);
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, 0);
    delay(200);
  }
  else if(value > 512-20 && value < 512+20 && hedef == 512)
  {
    motor_ayar_hiz = 10;
    while (1)
    {
      value = analogRead(A3) - direksiyon_offset;
      Serial.println(value);
      if(value < 512 - 30)
      {
        analogWrite(motorPinA, 0);
        analogWrite(motorPinB, motor_ayar_hiz);
      }
      else if(value > 512 + 30)
      {
        analogWrite(motorPinA, motor_ayar_hiz);
        analogWrite(motorPinB, 0);
      }
      else if(value < 512 - 5)
      {
        int motor_deger = 512 - value + 5;
        if(motor_deger > motor_ayar_hiz)
          motor_deger = motor_ayar_hiz;

        analogWrite(motorPinA, 0);
        analogWrite(motorPinB, motor_deger);
        Serial.print("\t");
        Serial.println(motor_deger);
      }
      else if(value > 512 + 5)
      {
        int motor_deger = value - 512 + 5;
        if(motor_deger > motor_ayar_hiz)
          motor_deger = motor_ayar_hiz;

        analogWrite(motorPinA, motor_deger);
        analogWrite(motorPinB, 0);
        Serial.print("\t");
        Serial.println(motor_deger);
      }
      else 
      {
        analogWrite(motorPinA, 0);
        analogWrite(motorPinB, 0);
        if(!ortalaniyor)
        {
          baslangic_ayar_zaman = millis();
          ortalaniyor = true;
        }
      }

      if(millis() - baslangic_ayar_zaman > 1500 && ortalaniyor == true)
      {
        ayarlandi = true;
        break;
      }
    }
  }

  int error = hedef - value;

  if(error < -30)
  {
    analogWrite(motorPinA, motor_ayar_hiz);
    analogWrite(motorPinB, 0);
  }
  else if(error > 30)
  {
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, motor_ayar_hiz);
  }
  else if(error > 3)
  {
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, 10);
  }
  else if(error < -3)
  {
    analogWrite(motorPinA, 10);
    analogWrite(motorPinB, 0);
  }
  else
  {
    analogWrite(motorPinA, 0);
    analogWrite(motorPinB, 0);
  }
}