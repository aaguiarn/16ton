#include <EnableInterrupt.h>
#include <Servo.h>
Servo ESC;     // create servo object to control the ESC

#define SERIAL_PORT_SPEED 9600
#define RC_NUM_CHANNELS  6

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_CH5  4
#define RC_CH6  5

#define RC_CH1_INPUT  A0
#define RC_CH2_INPUT  A1
#define RC_CH3_INPUT  A2
#define RC_CH4_INPUT  A3
#define RC_CH5_INPUT  A4
#define RC_CH6_INPUT  A5

// Variáveis Úteis
int i = 0;
int j = 0;
int state_rec;
int vSpeed = 200;   // Define velocidade padrão 0 < x < 255.
char state;
int V1;
int V2;

// Define os pinos de utilização do Driver L298.
//Motor A
const int motorA1  = 4;    // Pin  5 of L293.
const int motorA2  = 5;    // Pin  6 of L293.
const int velocidadeA = 3; 

//Motor B
const int motorB1  = 6;   // Pin 10 of L293.
const int motorB2  = 7;   // Pin 9 of L293.
const int velocidadeB = 11; 

uint16_t rc_values[RC_NUM_CHANNELS];
uint32_t rc_start[RC_NUM_CHANNELS];
volatile uint16_t rc_shared[RC_NUM_CHANNELS];

void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1() { calc_input(RC_CH1, RC_CH1_INPUT); }
void calc_ch2() { calc_input(RC_CH2, RC_CH2_INPUT); }
void calc_ch3() { calc_input(RC_CH3, RC_CH3_INPUT); }
void calc_ch4() { calc_input(RC_CH4, RC_CH4_INPUT); }
void calc_ch5() { calc_input(RC_CH5, RC_CH5_INPUT); }
void calc_ch6() { calc_input(RC_CH6, RC_CH6_INPUT); }

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);

  pinMode(RC_CH1_INPUT, INPUT);
  pinMode(RC_CH2_INPUT, INPUT);
  pinMode(RC_CH3_INPUT, INPUT);
  pinMode(RC_CH4_INPUT, INPUT);
  pinMode(RC_CH5_INPUT, INPUT);
  pinMode(RC_CH6_INPUT, INPUT);
    
  enableInterrupt(RC_CH1_INPUT, calc_ch1, CHANGE);
  enableInterrupt(RC_CH2_INPUT, calc_ch2, CHANGE);
  enableInterrupt(RC_CH3_INPUT, calc_ch3, CHANGE);
  enableInterrupt(RC_CH4_INPUT, calc_ch4, CHANGE);
  enableInterrupt(RC_CH5_INPUT, calc_ch5, CHANGE);
  enableInterrupt(RC_CH6_INPUT, calc_ch6, CHANGE);

//Define as portas dos motores como saida
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(velocidadeA, OUTPUT); 
  pinMode(velocidadeB, OUTPUT);

//Inicia com os motores parados
  digitalWrite(motorB1, V2);
  digitalWrite(motorA1, V2);
  digitalWrite(motorA2, V2);
  digitalWrite(motorB2, V2);

//Definição Motor Arma
ESC.attach(9); // (pin, min pulse width, max pulse width in microseconds) 

}

void loop() {
  rc_read_values();

  Serial.print("CH1:"); Serial.print(rc_values[RC_CH1]); Serial.print("\t");
  Serial.print("CH2:"); Serial.print(rc_values[RC_CH2]); Serial.print("\t");
  Serial.print("CH3:"); Serial.print(rc_values[RC_CH3]); Serial.print("\t");
  Serial.print("CH4:"); Serial.print(rc_values[RC_CH4]); Serial.print("\t");
  Serial.print("CH5:"); Serial.print(rc_values[RC_CH5]); Serial.print("\t");
  Serial.print("CH6:"); Serial.println(rc_values[RC_CH6]);

  // Salva os valores da variável 'state'
  if (Serial.available() > 0) {
    state_rec = Serial.read();
    state = state_rec;
  }
    Serial.print("STATE: ");
    Serial.println(state);
    Serial.print("VSPEED: ");
    Serial.println(vSpeed);
    
  // Altera a velocidade de acordo com valores especificados.
  if (state == '0') {
    vSpeed = 0;
  }
  else if (state == '4') {
    vSpeed = 100;
  }
  else if (state == '6') {
    vSpeed = 155;
  }
  else if (state == '7') {
    vSpeed = 180;
  }
  else if (state == '8') {
    vSpeed = 200;
  }
  else if (state == '9') {
    vSpeed = 230;
  }
  else if (state == 'q') {
    vSpeed = 255;
  }

  if (state != 'S') {
    Serial.print(state);
  }


  //CH5 - Inversão do Movimento
  if (rc_values[RC_CH5] < 1100){ 
    V1 = HIGH;
    V2 = LOW;
    Serial.print("CH5 - Posição 0 ");
    Serial.println(V1);
  }
  else {
    V1 = LOW;
    V2 = HIGH;
    Serial.print("CH5 - Posição 1");
    Serial.println(V1);
  }

  // O carro se movimenta para frente.
  if (rc_values[RC_CH4] > 1600 && ((rc_values[RC_CH1] > 1500) && (rc_values[RC_CH1] < 1600)) ){ 
    digitalWrite(motorB1, V1);
    digitalWrite(motorA1, V1);
    digitalWrite(motorA2, V2);
    digitalWrite(motorB2, V2);
    int speedA = map(rc_values[RC_CH4], 1600, 2000, 0, 255);
    analogWrite(velocidadeA, speedA);
    analogWrite(velocidadeB, speedA);
    Serial.println("FRENTE");
  }

    else if (rc_values[RC_CH4] > 1600 && rc_values[RC_CH1] < 1500) {  //O carro se movimenta para Frente Esquerda.
    digitalWrite(motorB1, V1);
    digitalWrite(motorA1, V1);
    digitalWrite(motorA2, V2);
    digitalWrite(motorB2, V2);
    int speedA = map(rc_values[RC_CH4], 1600, 2000, 0, 255);
    analogWrite(velocidadeA, 125);
    analogWrite(velocidadeB, speedA);
    Serial.println("FRENTE ESQUERDA");
  }

    else if (rc_values[RC_CH4] > 1600 && rc_values[RC_CH1] > 1600) {   //O carro se movimenta para Frente Direita.
    digitalWrite(motorB1, V1);
    digitalWrite(motorA1, V1);
    digitalWrite(motorA2, V2);
    digitalWrite(motorB2, V2);
    int speedA = map(rc_values[RC_CH4], 1600, 2000, 0, 255);
    analogWrite(velocidadeA, speedA );
    analogWrite(velocidadeB, 125);
    Serial.println("FRENTE DIREITA");
  }

  if (rc_values[RC_CH4] < 1500 && ((rc_values[RC_CH1] > 1500) && (rc_values[RC_CH1] < 1600))  ) { //o carro se movimenta para trás.
    digitalWrite(motorA1, V2);
    digitalWrite(motorB1, V2);
    digitalWrite(motorB2, V1);
    digitalWrite(motorA2, V1);
    int speedA = map(rc_values[RC_CH4], 1500, 1180, 0, 255);
    analogWrite(velocidadeA, speedA);
    analogWrite(velocidadeB, speedA);
    Serial.println("TRÁS");
  }

    else if (rc_values[RC_CH4] < 1500 && rc_values[RC_CH1] < 1500) {  //O carro se movimenta para Trás Esquerda.
    digitalWrite(motorB1, V2);
    digitalWrite(motorA1, V2);
    digitalWrite(motorA2, V1);
    digitalWrite(motorB2, V1);
    int speedA = map(rc_values[RC_CH4], 1500, 1180, 0, 255);
    analogWrite(velocidadeA, 125);
    analogWrite(velocidadeB, speedA);
    Serial.println("TRÁS ESQUERDA");
  }
  
    else if (rc_values[RC_CH4] < 1500 && rc_values[RC_CH1] > 1600) {  //O carro se movimenta para Trás Esquerda.
    digitalWrite(motorB1, V2);
    digitalWrite(motorA1, V2);
    digitalWrite(motorA2, V1);
    digitalWrite(motorB2, V1);
    int speedA = map(rc_values[RC_CH4], 1500, 1180, 0, 255);
    analogWrite(velocidadeA, speedA);
    analogWrite(velocidadeB, 125);
    Serial.println("TRÁS DIREITA");
  }

  else if (rc_values[RC_CH1] < 1500 && ((rc_values[RC_CH4] > 1500) && (rc_values[RC_CH4] < 1600)) ) {   //o carro se movimenta para esquerda.
    analogWrite(motorA1, 0);
    analogWrite(motorA2, vSpeed);
    analogWrite(motorB1, vSpeed);
    analogWrite(motorB2, 0);
    int speedA = map(rc_values[RC_CH1], 1500, 1140, 0, 255);
    analogWrite(velocidadeA, speedA);
    analogWrite(velocidadeB, speedA);
    Serial.println("ESQUERDA");
  }
  else if (rc_values[RC_CH1] > 1600 && ((rc_values[RC_CH4] > 1500) && (rc_values[RC_CH4] < 1600))  ) {   //o carro se movimenta para direita.
    analogWrite(motorA1, vSpeed);
    analogWrite(motorA2, 0);
    analogWrite(motorB1, 0);
    analogWrite(motorB2, vSpeed);
    int speedA = map(rc_values[RC_CH1], 1500, 1950, 0, 255);
    analogWrite(velocidadeA, speedA);
    analogWrite(velocidadeB, speedA);
    Serial.println("DIREITA");
  }
  else if ( ((rc_values[RC_CH1] > 1500) && (rc_values[RC_CH1] < 1600))  && ((rc_values[RC_CH4] > 1500) && (rc_values[RC_CH4] < 1600)) ) {   // Se o estado recebido for igual a 'S', o carro permanece parado.
    digitalWrite(motorA1, V2);
    digitalWrite(motorA2, V2);
    digitalWrite(motorB1, V2);
    digitalWrite(motorB2, V2);
    Serial.println("PARADO");
  }

  //Controle ARMA Horario
  if (rc_values[RC_CH3] > 1601 ){ 
    int speedN = map(rc_values[RC_CH3], 1600, 1996, 86, 180);
    ESC.write(speedN);    // Send the signal to the ESC
    Serial.print("ARMA Horario - VALOR: ");
    Serial.println(speedN);
  }

  //Controle ARMA AntiHorario
  else if (rc_values[RC_CH3] < 1499 ){ 
    int speedM = map(rc_values[RC_CH3], 1500, 1208, 84, 0);
    ESC.write(speedM);    // Send the signal to the ESC
    Serial.print("ARMA AntiHorario - VALOR: ");
    Serial.println(speedM);
  }  

  else if ( (rc_values[RC_CH3] > 1500) && (rc_values[RC_CH3] < 1600) ){   
    Serial.println("ARMA PARADA");
    ESC.write(85);    // Send the signal to the ESC
  }


}
