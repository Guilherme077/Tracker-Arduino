

/*
TRACKER COM CÂMERA PARA PROFESSORES/PALESTRANTES

DATA DE INÍCIO: 01/04/2024
VERSÃO: 0.1.6
*/

//Bibliotecas
#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Constantes
#define Motor1 6  //Servo Motor
#define Echo1 8   //Sensor Esq.
#define Trig1 7   //Sensor Esq.
#define Echo2 12  //Sensor Dir.
#define Trig2 11  //Sensor Dir.

#define MaxDist 200  // Limite dos sensores
#define MinDist 3    // Limite dos sensores

const float Versao_Sistema = 0.1;

const float VelSom_mpus = 0.000340;  //Velocidade do Som em metros por microsegundo

const float Tol_MedBase = 25;  // Tolerancia de posição com base na medida base

const float Dist_Entre_Sensores = 47; // Distância entre os dois sensores em cm

//Objetos
Servo servomotor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

NewPing sonar2(Trig1, Echo1, MaxDist);
NewPing sonar1(Trig2, Echo2, MaxDist);

//Variáveis
bool TecInfo = true; //Mostrar mais informações no monitor serial
int pos = 90;            //Utilizado para a posição do motor principal
int MovTolerancia = 10;  //em 'cm'
int ControleDebug = 0;
float MedBaseEsq = MaxDist;  //Medida base no lado esquerdo
float MedBaseDir = MaxDist;  //Medida base no lado direito
float MedBaseCen = MaxDist;  //Medida base no centro

void setup() {
  //Config dos sensores
  pinMode(Trig1, OUTPUT);
  digitalWrite(Trig1, LOW);
  pinMode(Echo1, INPUT);
  pinMode(Trig2, OUTPUT);
  digitalWrite(Trig2, LOW);
  pinMode(Echo2, INPUT);

  Serial.begin(9600);

  //Config do motor
  servomotor.attach(Motor1, 500, 2500);

  //Inicialização do LCD
  lcd.init();

  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Tracker para");
  lcd.setCursor(0, 1);
  lcd.print("camera v: ");
  lcd.setCursor(10, 1);
  lcd.print(Versao_Sistema, 1);
  delay(2000);
  lcd.clear();

  //Executa o escaneamento de verificação de medidas base
  Scan();
}

void loop() {
  //servomotor.write(UpdatedPos());  //Move o motor conforme a posição da função UpdatePos.

  //Se a distância de base + a tolerância for menor que a distância observada
  if ((MedBase(pos) + Tol_MedBase) < DistReta()) {
    CorrigePos();
  }

  if (ControleDebug == 0) {
    DebugInfo();  // Mostra informações de verificação no serial.
  }
  ControleDebug++;
  if (ControleDebug >= 100) {
    ControleDebug = 0;
  }
}

float DistReta(){
  // Fará a triangulação para obter a dintância em linha reta do obstáculo com base nas medidas dos sensores.
  float a = Dist_Entre_Sensores/2;
  float b = DistUltrassonico(Echo1, Trig1);
  float c = DistUltrassonico(Echo2, Trig2);
  return (sqrt(2*(pow(b, 2) + pow(c, 2)) - pow(a, 2)))/2;
}

void CorrigePos() {  // Vai procurar a menor distância (executa no caso de perder o alvo)
  servomotor.write(UpdatedPos());
}
void Scan() {
  //Aqui será realizado um escaneamento para descobrir a distância base do usuário
  lcd.setCursor(0, 0);
  lcd.print("Siga as proximas");
  lcd.setCursor(0, 1);
  lcd.print("instrucoes      ");
  servomotor.write(180);
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Va em frente ao ");
  lcd.setCursor(0, 1);
  lcd.print("tracker         ");
  for (int i = 0; i < 500; i++) {
    MedBaseEsq = UpdateMedBase(MedBaseEsq);  //Registra a medida de base se for menor
    delay(10);
  }
  servomotor.write(90);
  for (int i = 0; i < 500; i++) {
    MedBaseCen = UpdateMedBase(MedBaseCen);  //Registra a medida de base se for menor
    delay(10);
  }
  servomotor.write(0);
  for (int i = 0; i < 500; i++) {
    MedBaseDir = UpdateMedBase(MedBaseDir);  //Registra a medida de base se for menor
    delay(10);
  }
  lcd.setCursor(0, 0);
  lcd.print("Verificacao     ");
  lcd.setCursor(0, 1);
  lcd.print("concluida!      ");
  delay(2000);
}

float MedBase(float posSM) {
  // Com base nas 3 medidas base obtidas, obtem uma em qualquer angulo informado
  float a = DifModulo(MedBaseDir, MedBaseCen) / 90;
  float b = DifModulo(MedBaseEsq, MedBaseCen) / 90;
  if (posSM > 90) {
    return (posSM - 90) * b;
  } else if (posSM < 90) {
    return posSM * a;
  } else {
    return 90;
  }
}

float DifModulo(float val1, float val2) {
  float resp = 0;
  if (val1 > val2) {
    resp = val1 - val2;
  } else {
    resp = val2 - val1;
  }
  return resp;
}

float UpdateMedBase(float varToUpdate) {
  float dist = DistReta();
  
  if (dist < varToUpdate) {
    varToUpdate = dist;
  }
  return varToUpdate;
}

int UpdatedPos() {
  float S1 = DistUltrassonico(Echo1, Trig1);
  float S2 = DistUltrassonico(Echo2, Trig2);
  if (S1 > (S2 + MovTolerancia)) {
    //Virar para Direita (SU1 com dist maior)
    if (pos > 0) {
      pos -= 1;  //Diminui a posição do motor
    }

    servomotor.write(pos);
  } else if (S2 > (S1 + MovTolerancia)) {
    //Virar para Esquerda (SU2 com dist maior)
    if (pos < 180) {
      pos += 1;  //Aumenta a posição do motor
    }
  }
  return pos;  //Envia o valor de 'pos' para a variável 'UpdatedPos'
}

void DebugInfo() {
  int v1 = DistUltrassonico(Echo1, Trig1);
  int v2 = DistUltrassonico(Echo2, Trig2);
  Serial.print("Sensor_Esq:");
  Serial.print(v1);
  Serial.print(',');
  Serial.print("Sensor_Dir:");
  Serial.print(v2);
  Serial.print(',');
  Serial.print("Giro_Motor:");
  Serial.println(pos);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dir: ");
  lcd.setCursor(5, 0);
  lcd.print(v1);
  lcd.setCursor(8, 0);
  lcd.print("Esq: ");
  lcd.setCursor(13, 0);
  lcd.print(v2);
  lcd.setCursor(0, 1);
  lcd.print("Pos Motor: ");
  lcd.setCursor(11, 1);
  lcd.print(pos);

  if(TecInfo){
    Serial.print("Medida Base Esq: ");
  Serial.print(MedBaseEsq);
  Serial.print(',');
  Serial.print("Medida Base Dir: ");
  Serial.print(MedBaseDir);
  Serial.print(',');
  Serial.print("Medida Base Central: ");
  Serial.println(MedBaseCen);
  Serial.print(',');
  Serial.print("Distancia Reta do alvo: ");
  Serial.println(DistReta());
  Serial.print(',');
  }
}

float DistUltrassonico(int echo, int trigger) {
  float dist = 0;
  if (echo == Echo1) {
    dist = sonar1.ping_cm();  //VARIÁVEL GLOBAL RECEBE O VALOR DA DISTÂNCIA MEDIDA

  } else if (echo == Echo2) {
    dist = sonar2.ping_cm();  //VARIÁVEL GLOBAL RECEBE O VALOR DA DISTÂNCIA MEDIDA
  }

  if (dist > MinDist && dist < MaxDist) {
    return dist;
  } else {
    return MaxDist;
  }
}
