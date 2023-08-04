#include <SoftwareSerial.h>

/**
 * oo: loop
 * ii: if
 * ee: else
 * cc: fim da condicional
 * 
 **/

//Declaração pins do bluetooth
const int PIN_RX = 10;
const int PIN_TX = 11;

const int arrayLength = 200;
const int infoLength = 2;

SoftwareSerial serialHC08(PIN_RX, PIN_TX );

//declaracao dos pinos utilizados para controlar a velocidade de rotacao
const int PINO_ENA = 7; 
const int PINO_ENB = 6;


//declaracao dos pinos utilizados para controlar o sentido do motor
const int PINO_IN1A = 12; 
const int PINO_IN2A = 13;
const int PINO_IN1B = 9; 
const int PINO_IN2B = 8;

//declaração do pino de sensor de distância
const int SENSOR_DISTANCIA = 5;


const char stopSimbol = '$';


const int TEMPO_ESPERA = 0; //declaracao do intervalo de 1 segundo entre os sentidos de rotacao do motor

const int TEMPO_RAMPA_CURVE1 = 1500; //declaracao do intervalo de 30 ms para as rampas de aceleracao e desaceleracao
const int TEMPO_RAMPA_CURVE2 = 3700; //declaracao do intervalo de 30 ms para as rampas de aceleracao e desaceleracao


const int TEMPO_RAMPA_MOVE2 = 2000; 
const int TEMPO_RAMPA_MOVE1 = 2000; 

//-------variaveis

int i = 0; //declaracao da variavel para as rampas
byte moviments[arrayLength][infoLength];
byte ifMoviments[2][arrayLength][infoLength];
int state = 0;
int numInfo = 0;
byte moviment[2];
boolean canMove = true;
boolean insideIfBlock = false;
boolean insideElseBlock = false;
boolean ifExecuted = false;
boolean elseExecuted = false;

const nloop = 0;    // numero de iterações

void setup() {
    //configuração dos pinos como saida
  pinMode(PINO_ENA, OUTPUT); 
  pinMode(PINO_ENB, OUTPUT);
   
  pinMode(PINO_IN1A, OUTPUT);
  pinMode(PINO_IN2A, OUTPUT);
  
  pinMode(PINO_IN1B, OUTPUT);
  pinMode(PINO_IN2B, OUTPUT);

  //configuração dos pinos como entrada
  pinMode(SENSOR_DISTANCIA, INPUT);

  digitalWrite(PINO_IN1A, LOW); 
  digitalWrite(PINO_IN2A, LOW);
  digitalWrite(PINO_ENA, LOW);
  
  digitalWrite(PINO_IN1B, LOW); 
  digitalWrite(PINO_IN2B, LOW);
  digitalWrite(PINO_ENB, LOW);

  Serial.begin(9600);
  Serial.println("Teste");
  serialHC08.begin(9600);

  cleanMoviments();

}

void loop() {



//------------------ALGORITMO DE RECEBIMENTO BLUETOOTH


   

  if(state == 0){

 
        if (serialHC08.available()) {
            byte value = serialHC08.read();
            if(value != 1 ){
              numInfo++;
              //byte robotVelocity = serialHC08.read();
//              Serial.print("Caracter:::::");    //char i lido ok
//              Serial.println((char) value);
              //Setar direção e velocidade 
              if(numInfo == 1){
                moviment[0] = value;
              }
              else{
                moviment[1] = value;
//                Serial.println(numInfo);
//                Serial.print("insideIfBlock:");
//                Serial.println(insideIfBlock);
                if((char) value == 'i'){
                  insideIfBlock = true;
                  ifExecuted = false;
                  Serial.println("reconheceu o i");
                }
                else if((char) value == 'e'){
                  insideElseBlock = true;
                  elseExecuted = false;
                }
                else if(insideIfBlock){
                  addIfBlock(moviment, 0);
                  Serial.print("ADICIONOU NO IF");
                }
                else if(insideElseBlock){
                  addIfBlock(moviment, 1);
                  Serial.print("ADICIONOU NO ELSE");
                }
                else{
                  if ((char) value == 'o'){
                    Serial.print("#TODO loop");
                  }
                  addMoviment(moviment);
                  Serial.println("ADICIONOU MOVIMENTO");
                }
                Serial.println((char) value);
                numInfo = 0;
              }
          
              if((char) value == stopSimbol){
                state = 1;
                numInfo = 0;
              }
 
            }
           //Serial.println((int) robotVelocity);

        
      }
  }
  else if( state == 1){


//--------------------------ALGORITMO DE MOVIMENTAÇÃO

    // put your main code here, to run repeatedly:
    //digitalWrite(PINO_IN1, LOW); 
    //digitalWrite(PINO_IN2, HIGH);
    boolean startedLoop = false;
    int loopPosition = 0;
    printArrayMoviment();
 
    for(int j =0; j< arrayLength; j++ ){


        
        byte info[2] = {moviments[j][0], moviments[j][1]};
        Serial.print("Direction:");
        Serial.println((char) info[0]);
        Serial.print("Velocity:");
        Serial.println((char) info[1]);

        if (serialHC08.available()){
           byte value = serialHC08.read();
           if(value == 1 ){
              break;
           }
        }
  
      if((char) info[0] != stopSimbol){

          //Loop do robô (for)
          if((char) info[0] == 'o'){ 
            
            if(startedLoop){
              j = loopPosition;     // assim ele repete apenas o array de movimentos ja existente sem parar
              Serial.println("Salvou o index");
            }
            else{
              loopPosition = j;
              /// TODO verificar qtd de nloop restante para limitar as iteracoes
              startedLoop = true;
            }
            break;
          }
          if((char) info[0] == '1'){ 
            nloop++;
            Serial.println("add iteraçao");
          }

          //Serial.println("ENTROU");
          moveRobot(info[0],info[1], j , false);

          if(!canMove){
            Serial.println("Entrou no canMove");  // entra aqui ou seja canmove == false
            if( (char) ifMoviments[0][j][0] == stopSimbol){
              Serial.println("BREAK? quer dizer que tem $$ no if");
              break;
          }
          
          if(!ifExecuted){    // ifExecuted == false
            canMove = true;
            executeConditionalBlock(0);
            ifExecuted = true;
            Serial.println("Block do if chamado");
//            if(!canMove){
//              executeConditionalBlock(1);
//              ifExecuted = false;
//            }
          }else{            
            Serial.println("Block do else chamado");
            if(elseExecuted){
              break;
            }else{
              executeConditionalBlock(1);
              elseExecuted = true;
            }
          }
        }
      }
      else {
            setInitialState();
            canMove = true;
            ifExecuted = false;
            break;
      }
    }
      setInitialState();
      canMove = true;
      ifExecuted = false;

    
  }



}


void printArrayMoviment(){
  for(int j =0; j< arrayLength; j++){

    if(moviments[j][0] != stopSimbol){
      Serial.print("M1: ");
      Serial.print((char)moviments[j][0]);
      Serial.print(", ");
      Serial.print("M2: ");
      Serial.println((char) moviments[j][1]);
    }
    else{
      return;
    }
  }

}

void setInitialState(){
            digitalWrite(PINO_IN1A, LOW);
            digitalWrite(PINO_IN1B, LOW);
            digitalWrite(PINO_IN2A, LOW);
            digitalWrite(PINO_IN2B, LOW);
            state = 0;
            cleanMoviments();
  
}


void cleanMoviments(){

  for(int j =0; j< arrayLength; j++){
   
    moviments[j][0] = stopSimbol;
    moviments[j][1] = stopSimbol;
  }

  for(int p=0; p< 2 ; p++){
    for(int j =0; j< arrayLength; j++){
   
      ifMoviments[p][j][0] = stopSimbol;
      ifMoviments[p][j][1] = stopSimbol;
    }
  }
  
  
}


void moveRobot(byte direct, byte velocity, int j, boolean isIfBlock){
          if ((char) direct == 'f') {

            digitalWrite(PINO_IN1A, LOW);               
            digitalWrite(PINO_IN1B, HIGH);
            digitalWrite(PINO_IN2A, HIGH);
            digitalWrite(PINO_IN2B, LOW);
            canMove = robotAcelerationMove((char)velocity, j , isIfBlock);  // AQUI o canMove vai p/ falso n sei pq D=
            
            
            

        } else if ((char) direct == 'b') {

            digitalWrite(PINO_IN1A, HIGH);
            digitalWrite(PINO_IN1B, LOW);
            digitalWrite(PINO_IN2A, LOW);
            digitalWrite(PINO_IN2B, HIGH);
            
            canMove = robotAcelerationMove((char)velocity, j, isIfBlock);
  

        }
        else if((char) direct == 'r'){

            digitalWrite(PINO_IN1A, LOW);
            digitalWrite(PINO_IN1B, LOW);
            digitalWrite(PINO_IN2A, HIGH);
            digitalWrite(PINO_IN2B, HIGH);
            robotAcelerationCurve((char)velocity);
        }
        else if((char) direct == 'l'){
            digitalWrite(PINO_IN1A, HIGH);
            digitalWrite(PINO_IN1B, HIGH);
            digitalWrite(PINO_IN2A, LOW);
            digitalWrite(PINO_IN2B, LOW);
            robotAcelerationCurve((char)velocity);

        }
}


void addIfBlock(byte* info, int pos){

     for(int j =0; j< arrayLength; j++){

      if(ifMoviments[pos][j][0] == stopSimbol){
        
        ifMoviments[pos][j][0] = info[0];
        ifMoviments[pos][j][1] = info[1];
        break;
      }   
    
   }
  
}

void addMoviment(byte* info){
//Serial.println("Added");

   for(int j =0; j< sizeof(moviments); j++){

      if(moviments[j][0] == stopSimbol){
        
        moviments[j][0] = info[0];
        moviments[j][1] = info[1];
        break;
      }   
    
   }

  
}


void robotAcelerationCurve(char velocity){
  
  //for (i = 0; i < 256; i=i+10){ 
     //analogWrite(PINO_ENA, i);
     //analogWrite(PINO_ENB, i);
    // delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  //}

   analogWrite(PINO_ENA, 255);
   analogWrite(PINO_ENB, 255);
   delay(velocity == '1' ? TEMPO_RAMPA_CURVE1 : TEMPO_RAMPA_CURVE2);
   analogWrite(PINO_ENA, 0);
   analogWrite(PINO_ENB, 0);

    //rampa de desaceleracao
  //for (i = 255; i >= 0; i=i-10){ 
    //analogWrite(PINO_ENA, i);
    //analogWrite(PINO_ENB, i);
    //delay(TEMPO_RAMPA); //intervalo para incrementar a variavel i
  //}

  delay(TEMPO_ESPERA); //intervalo de um segundo

}

// type: 0 - if 1 - else
void executeConditionalBlock(int type){
  Serial.println("Executando bloco condicional");
      for(int j =0; j< arrayLength; j++ ){
      
        byte info[2] = { ifMoviments[type][j][0],  ifMoviments[type][j][1]};
  
        if((char) info[0] != stopSimbol){
          moveRobot(info[0], info[1], j, true);
          Serial.println("move robot");
        }
        else{
            digitalWrite(PINO_IN1A, LOW);
            digitalWrite(PINO_IN1B, LOW);
            digitalWrite(PINO_IN2A, LOW);
            digitalWrite(PINO_IN2B, LOW);
        }

      
      }
  
}


/**
 * 
 */
boolean robotAcelerationMove(char velocity, int index, boolean isIfBlock){

     int lastTimeRobotMoviment = 0;
     int previusTime = 0;
     int delayTime = velocity == '1' ? TEMPO_RAMPA_MOVE1 : TEMPO_RAMPA_MOVE2;
     analogWrite(PINO_ENA, 255);
     analogWrite(PINO_ENB, 255);
     lastTimeRobotMoviment = millis();
     previusTime = lastTimeRobotMoviment;
     do{

           if(digitalRead(SENSOR_DISTANCIA) == LOW && !(index == 0 && isIfBlock)){ //Encontrou obstáculo
              Serial.println("Encontrou obstaculo");
//              // proxs linhas param o robo.
//              analogWrite(PINO_ENA, 0);
//              analogWrite(PINO_ENB, 0);
//              return false;     // RETORNA FALSO SE ENCONTROU OBSTACULO!!! ai o canMove == false
              // mas agora o robo deve se mover mesmo se encontrar obstaculo
              /// entao deva acabar com esse bloco - quando encontrar obstaculo ele deve mesmo é ver se tem IF.
//              ifExecuted = false;
           }
           previusTime = millis();
     }while((previusTime - lastTimeRobotMoviment  ) <=  delayTime);
      analogWrite(PINO_ENA, 0);
      analogWrite(PINO_ENB, 0);
      return true;



  delay(TEMPO_ESPERA); //intervalo de um segundo


  
}
