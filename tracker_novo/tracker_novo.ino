/*
  TRACKER - CÓDIGO FONTE

  PROJETO DE INICIAÇÃO CIENTÍFICA
  SESI SENAI SC
  ESCOLA SESI DE REFERÊNCIA

  Por: Guilherme P. Moreira
  Professor Orientador: Jonathan Sardo

  Versão 0.1 criado em 1 de abril de 2024

  Versão 0.2.1

  Esquerda: S1
  Direita: S2
*/

//Bibliotecas
#include <Wire.h>
#include <Servo.h>
#include <NewPing.h>
#include <LiquidCrystal_I2C.h>

//Constantes de Portas
#define Motor 6
#define Trig1 7
#define Echo1 8
#define Trig2 11
#define Echo2 12

//Constantes de configuração do sistema de tracker
int Metodo = 0; // 0: O sensor com menor distância faz o tracker girar.
int Toleracia = 700;
int ToleranciaDeErro = 5; // Apos um dar certo, o tracker só vai se mover depois deste tempo acabar.
const float Dist_Entre_Sensores = 50;
const float Angulo_Sensores = 30;

//Constantes de limitação
#define MaxDist 200
#define MinDist 3

//Constantes de natureza
const float VelSom_mpus = 0.000340;

//Variaveis de sistema
String versao = "0.2.1";
bool InfoDetalhada = true;

//Objetos
Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
NewPing sonar1(Trig1, Echo1, MaxDist);
NewPing sonar2(Trig2, Echo2, MaxDist);

//Variáveis de controle de código
int angulo = 90; //Angulo do motor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Inicialização dos sensores/motores
  pinMode(Trig1, OUTPUT);
  pinMode(Trig2, OUTPUT);
  pinMode(Echo1, INPUT);
  pinMode(Echo2, INPUT);
  digitalWrite(Trig1, LOW);
  digitalWrite(Trig2, LOW);
  servo.attach(Motor);

  //Inicialização do LCD
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Tracker");
  lcd.setCursor(0, 1);
  lcd.print("Versao: ");
  lcd.setCursor(9, 1);
  lcd.print(versao);
  delay(2000);
  lcd.clear();
  servo.write(angulo);
}

void loop() {
  MoverComBaseNoSensor();

}

void MoverComBaseNoSensor(){
  float d1 = sonar1.ping_median(3);
  float d2 = sonar2.ping_median(3);
  if(d1 != 0 && d2 != 0){
    if(d1 > d2 + Toleracia){
      if(angulo < 180){
        angulo += 2;
      }
    
    }
    if(d2 > d1 + Toleracia){
      if(angulo > 0){
        angulo -= 2;
      }
    
    }
  
  }else{
    Serial.println("Nada foi feito!");
  }
  Serial.println("Distâncias: ");
  Serial.println(d1);
  Serial.println(d2);
  Serial.println(angulo);
  servo.write(angulo);
}
float DistanciaDoAlvo(float a, float b, float c){
  //a: medida da distância entre os sensores dividido por 2
  //b: Medida da distância de um sensor ao objeto
  //c: Medida da distância de um sensor ao objeto
  return (sqrt(2*(pow(b,2) + pow(c,2)) - pow(a,2)))/2;
}

void Procura(){
}
