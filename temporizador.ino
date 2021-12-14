/*  Programa..: Temporizador para forno microondas com Arduino
 *  Criação...: 20/10/2021  
 *  Alterações:
 * 
 * 
 */
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"

const byte PIN_CLK = 11;   // define CLK pin (any digital pin)
const byte PIN_DIO = 10;   // define DIO pin (any digital pin)
SevenSegmentExtended      display(PIN_CLK, PIN_DIO);

const unsigned int clockSpeed = 10000;    // speed up clock for demo

#define pinMin 2
#define pinSeg 3
#define pinRst 4
#define pinIni 5
#define pinRELE 8
#define pinBuzz 9
#define pinPorta 12

byte minutos = 0;
byte segundos = 0;
byte ligado = 0;
byte pausado = 0;
int tempo = 0;

byte btMinAnt=1;
byte btSegAnt=1;
byte btRstAnt=1;
byte btIniAnt=1;

byte btMinAtu=1;
byte btSegAtu=1;
byte btRstAtu=1;
byte btIniAtu=1;

unsigned long valmillis = 0;
unsigned long ult_tecla = 0;

void bip_fim()
{
   for(int vc=1;vc<=4;vc++) {
     tone(pinBuzz,1000,500);
     delay(1000);
   }
}


void setup()
{
  pinMode(pinMin, INPUT_PULLUP);
  pinMode(pinSeg, INPUT_PULLUP);
  pinMode(pinRst, INPUT_PULLUP);
  pinMode(pinIni, INPUT_PULLUP);
  pinMode(pinRELE, OUTPUT);
  pinMode(pinBuzz, OUTPUT);
  pinMode(pinPorta, INPUT_PULLUP);
  
  Serial.begin(9600);         // initializes the Serial connection @ 9600 baud
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  display.setColonOn(1);
  //delay(1000);                // wait 1000 ms
  
  display.printTime(minutos, segundos, false);
  tone(pinBuzz,1000,1000);
}

void loop()
{
  // -- só permite ajustar tempo se estiver desligado
  if (ligado == 0) {

    // -- botão minuto
    btMinAtu=digitalRead( pinMin);
    if (btMinAtu==0 && btMinAtu!=btMinAnt) {
      ult_tecla=millis();
      tone(pinBuzz,1000,250);
      //Serial.println("+1 min");
      minutos += 1;
      //Serial.println(minutos * 100 + segundos);
      display.printTime(minutos, segundos, false);
      delay(200);
    }
    btMinAnt=btMinAtu;

    // -- botão segundos
    btSegAtu=digitalRead(pinSeg);
    if (btSegAtu==0 && btSegAtu!=btSegAnt) {
      ult_tecla=millis();
      tone(pinBuzz,1000,250);
      //Serial.println("+10 seg");
      //segundos += 10;
      minutos = segundos==50 ? minutos+1 : minutos;
      segundos = segundos==50 ? 0 : segundos+10;
      //Serial.println(minutos * 100 + segundos);
      display.printTime(minutos, segundos, false);
      delay(200);
    }
    btSegAnt=btSegAtu;

    // -- botão reset
    btRstAtu=digitalRead(pinRst);
    if (btRstAtu==0 && btRstAtu!=btRstAnt) {
      ult_tecla=millis();
      tone(pinBuzz,1000,250);
      //Serial.println("reset");
      minutos = 0;
      segundos = 0;
      //Serial.println(minutos * 100 + segundos);
      display.printTime(minutos, segundos, false);
      tempo = 0;
      //pausado = 0;
      delay(200);
    }
    btRstAnt=btRstAtu;

  }
  
  // botão inicio / pausa
  btIniAtu=digitalRead(pinIni);
  if (btIniAtu==0 && btIniAtu!=btIniAnt) {
    if(ligado==0 && digitalRead( pinPorta)==1) // se iria ligar e porta está aberta
      tone(pinBuzz,1000,1000);
    else {
      tone(pinBuzz,1000,250);
      if(minutos==0 && segundos==0) {    // se não setou tempo, assume padrão 30seg
        tempo=30;
        minutos=0;
        segundos=30;
        display.printTime(minutos, segundos, false);
      }
      if (ligado == 1) {
        Serial.println("pausa");
        ligado = 0;
        digitalWrite(pinRELE,LOW);
      } else {
        Serial.println("inicio");
        ligado = 1;
        digitalWrite(pinRELE,HIGH);
        // -- converte min:seg p/ segundos
        if (tempo == 0) {
          tempo = int(minutos)*60 + int(segundos);
        }
        valmillis = millis();
        Serial.println( millis() );
      }
    }
    delay(200);
  }
  btIniAnt=btIniAtu;

  // -- se abriu porta e está ligado, pausa por porta aberta
  if(digitalRead(pinPorta)==1 && ligado == 1) {
    ligado = 0;
    //pausado = 1;
    digitalWrite(pinRELE,LOW);
  }
  
  // -- se fechou porta e está pausado por porta aberta, liga
  /*if(digitalRead(pinPorta)==0 && pausado==1 ) {
    ligado = 1;
    pausado = 0;
    digitalWrite(pinRELE,HIGH);
  }*/

  // -- controle do tempo
  if (ligado == 1) {
    // --- se passou 1 seg.
    if(millis()-valmillis >= 1000) {
      // -- calculo com tempo em segundos
      /*tempo = tempo - 1;
      //Serial.println(-( (tempo/60)*100 + tempo % 60));
      Serial.println(-tempo);
      //display.printTime(byte(int(tempo/60)), byte(tempo%60), false);
      */
      // -- calculo com min e seg
      minutos = segundos==0 ? minutos-1 : minutos;
      segundos = segundos==0 ? 59 : segundos-1;
      //Serial.println(minutos*100+segundos);
      display.printTime(minutos, segundos, false);
      //
      valmillis = millis();
      //if(tempo==0) {
      if(segundos==0 && minutos==0) {
        digitalWrite(pinRELE,LOW);
        ligado=0;
        minutos=0;
        segundos=0;
        tempo=0;
        bip_fim();
      }
    }
  }
  else if(millis()-valmillis >= 5000) { 
    display.print("");
    //delay(1000);
    //display.blink();
  }
  //Serial.println( millis() );
  //delay(50);

}
