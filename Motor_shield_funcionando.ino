// MPU6050 & motor brushed

#include <SPI.h>
#include <MPU6050.h>
//#include <I2CDEV.h>
#include <Wire.h>
#include <AFMotor.h>
#define MPU 0x68  // Direccion I2C del MPU-6050

AF_DCMotor motor(1);
double AcX,AcY,AcZ;
int Pitch;
int Led = A12;
int ledCero = 36; 
int a;
uint8_t b=0;

void setup()
{
  Serial.begin(115200);
  motor.setSpeed (240);
  motor.run (RELEASE);
  pinMode(Led, OUTPUT);
   pinMode(ledCero, OUTPUT);
  init_MPU(); // Inicializamos el MPU6050
  //angulo Pin=5
  //angulo Pin=7
}

void init_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Seteamos a cero (wakes up - Despertamos el MPU-6050)
  Wire.endTransmission(true);
}

//Funcion para el calculo del angulo Pitch y Roll

double FunctionsPitchRoll(double A, double B, double C){
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B*B) + (C*C);
  DatoB = sqrt(DatoB);
  
  Value = atan2(DatoA, DatoB);
  Value = Value * 180/3.14;

  return (int)Value;
}


//Función para adquirir los ejes X, Y, Z del MPU6050

void FunctionsMPU()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Empezamos con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);  // requerimos un total de 6 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
} 


 int Angulo()
 {
 Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //Calculo del angulo del Pitch
 return (Pitch);
 }
 
 //condiciones de PWM para el motor  segun la posición
 int FuncionCondiciones(int a){    
   
 if(a>0 && (b<250))
      {
       motor.run(BACKWARD);
       motor.setSpeed(b); 
       analogWrite(Led,b);
       digitalWrite(ledCero,LOW);
       b++;
       return(b);
       }
   else if(a==0)
         {
       b=b;
       motor.run(BACKWARD);    
       motor.setSpeed(b);
        Serial.print("tick");
       analogWrite(Led,b);
       digitalWrite(ledCero,HIGH);
       return(b);
         }
         
  else if(a<0 && (b>=1))
        {
     motor.run(BACKWARD);
     motor.setSpeed(b);
      analogWrite(Led,b);
      digitalWrite(ledCero,LOW);
       b--;
       return(b);
        }
   } 
  
  
 //Función impresion de variables Angulo-PWM
int Impresiones(int a,uint8_t b){
  Serial.print("\t Pitch: "); Serial.print(Pitch);
  Serial.print("\t PWM: "); Serial.print(b);
   Serial.print("\n");

}
 
 
void loop()
{
  FunctionsMPU(); // Adquirimos el eje Y
  Angulo();
  a=Pitch;
  FuncionCondiciones(a);
  Impresiones(a,b);
}
