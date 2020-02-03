#include "sensores.h"
#include "flash.h"
#include "led.h"
#include "http.h"

int tam;

String montaMetricas(String timestamp, String sensor, String valor);
int valorCorrente1;
int valorCorrente2;
int valorCorrente3;

//void modoAP(void * parameter){
//  while(1){
//    delay(5);
//    
//  }
//}

void sendData(void * parameter){
  while(1){
    delay(3);
    if(WiFi.status() == WL_CONNECTED){
      acendeLedVerde();
      File root = SPIFFS.open("/json");
      File file = root.openNextFile();
  
      while(file){
        //INFORMAÇÕES SOBRE O ARQUIVO
        head = "";
//        Serial.print("ENVIANDO ARQUIVO: ");
//        Serial.println(file.name());
        //CONCATENA COM O CABEÇALHO
        head.concat("{ \"seq\":");
        head.concat(sequencial);
        head.concat(",\"now\":");
        head.concat(millis());
        head.concat(",\"mac-address\":\"");
        head.concat(mac);
        head.concat("\",\"token\":\"corinthians\",\"metrics\": [");
  
        //LER AS MÉTRICAS GRAVADAS NA FLASH
        jSon = lerArquivo(file.name());
        //Serial.println(jSon);
        //SE NÃO RETORNAR NADA A LEITURA FALHOU
        if(jSon == ""){
          Serial.println("FALHA AO ABRIR O ARQUIVO");
        } 
        else{
          tam = jSon.length();
          jSon.remove(tam-1);
          head.concat(jSon);
          head.concat("]}");
          Serial.println("");
          Serial.println(head);
          Serial.println("");
          // 
          while(!EnviaJson(head) && cEnvia < 100 && WiFi.status() == WL_CONNECTED){
              cEnvia++;
              piscaLedVermelho();
              piscaLedVermelho();
              delay(1000);
            }
          
          escreveArquivo("/config/sequencial.txt", (String)sequencial);  
          sequencial++;
          Serial.print("ENVIANDO ARQUIVO: ");
          Serial.println(file.name());
//          Serial.print("TAMANHO: ");
//          Serial.println(file.size());
          if(cEnvia >= 100){
            Serial.println("FALHA AO ENVIAR!!!");
          }else{
            Serial.println("ENVIADO COM SUCESSO!!!");
          }
          cEnvia = 0;
          SPIFFS.remove(file.name());
        }
        file = root.openNextFile();
        delay(200);
      }
    }else{
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        piscaLedVerde();
        Serial.println("Listando arquivos:");
        listaArquivos("/json");
    }
  }
}

void recordMetrics(void * parameter){
  while(1){
    delay(1);
    if(flagRecordMetrics){
      String nameFile = "/json/metricas";
      nameFile.concat((String)seqMetrics);
      nameFile.concat(".txt"); 
      Serial.println("GRAVANDO SEQMETRICAS"); 
      escreveArquivo("/config/seqMetrics.txt", (String)seqMetrics);
      Serial.println("GRAVANDO METRICAS");
      escreveArquivo(nameFile, auxMetricas);
      seqMetrics++; 
      flagRecordMetrics = false;
    }
    delay(10);
  }
  
}

void taskSensores(void * parameter){
  while(1){
    delayMicroseconds(5);
    
    if(flagSensores){
//      Serial.print("I");
//      Serial.println(millis());
      porta_1 = digitalRead(PRT_01);
      porta_2 = digitalRead(PRT_02);
      porta_3 = digitalRead(PRT_03);
      Serial.println("");
      Serial.print("Sensor P1: ");
      Serial.println(porta_1);
      Serial.print("Sensor P2: ");
      Serial.println(porta_2);
      Serial.print("Sensor P3: ");
      Serial.println(porta_3);
      Serial.println("");
      metricas = montaMetricas((String)millis(),"1",(String)porta_1);
      metricas = montaMetricas((String)millis(),"2",(String)porta_2);
      metricas = montaMetricas((String)millis(),"3",(String)porta_3); 
    
    if(countTemp == tempTemperatura){
      //Serial.println("GRAVANDO DADOS DE TEMPERATURA");
      //Ler sensores de temperatura
      countTemp = 0;
      sensors1.requestTemperatures();
      temp_1 = sensors1.getTempCByIndex(0);
      sensors2.requestTemperatures();
      temp_2 = sensors2.getTempCByIndex(0);
      sensors3.requestTemperatures();
      temp_3 = sensors3.getTempCByIndex(0);
      Serial.println("");
      Serial.print("Sensor T1: ");
      Serial.println(temp_1);
      Serial.print("Sensor T2: ");
      Serial.println(temp_2);
      Serial.print("Sensor T3: ");
      Serial.println(temp_3);
      Serial.println("");
      metricas = montaMetricas((String)millis(),"4",(String)temp_1);
      metricas = montaMetricas((String)millis(),"5",(String)temp_2);
      metricas = montaMetricas((String)millis(),"6",(String)temp_3);
    }

    if(countCorrente == tempCorrente){
      //Serial.println("GRAVANDO DADOS DE CORRENTE");
      //Ler sensores de corrente
      countCorrente = 0;
      float auxCorrente;
      valorCorrente1 = 0;
      valorCorrente2 = 0;
      valorCorrente3 = 0;
      
      for(int i = 0; i < 500; i++){
        
        auxCorrente = adc1_get_raw(ADC1_CHANNEL_4);
        if(valorCorrente1 < auxCorrente){
          valorCorrente1 = auxCorrente;
        }
      
        auxCorrente = adc1_get_raw(ADC1_CHANNEL_5);
        if(valorCorrente2 < auxCorrente){
          valorCorrente2 = auxCorrente;
        }
      
        auxCorrente = adc1_get_raw(ADC1_CHANNEL_6);
        if(valorCorrente3 < auxCorrente){
          valorCorrente3 = auxCorrente;
        }
        delayMicroseconds(3);

     }

      Serial.println("");
      Serial.print("Sensor C1: ");
      Serial.println(valorCorrente1);
      Serial.print("Sensor C2: ");
      Serial.println(valorCorrente2);
      Serial.print("Sensor C3: ");
      Serial.print(valorCorrente3);
      Serial.println("");
      metricas = montaMetricas((String)millis(),"7",(String)valorCorrente1);
      metricas = montaMetricas((String)millis(),"8",(String)valorCorrente2);
      metricas = montaMetricas((String)millis(),"9",(String)valorCorrente3);
    }
    
   if(countEnvio == tempEnvio){
    countEnvio = 0;
    auxMetricas = metricas;
    flagRecordMetrics = true;
    metricas = "";
    delay(10);
    }
    
    countTemp++;
    countCorrente++;
    countEnvio++;
    flagSensores = false;
    delay(50);
//    Serial.print("F");
//    Serial.println(millis());
  }
  }
}

//Montar variável de metricas
String montaMetricas(String timestamp, String sensor, String valor){
  metricas.concat("{\"timestamp\":");
  metricas.concat(timestamp);
  metricas.concat(",\"sensor\":");
  metricas.concat(sensor);
  metricas.concat(",\"value\":");
  metricas.concat(valor);
  metricas.concat("},");
  //Serial.println(metricas);
  return metricas;
}

void setupTask(){
  xTaskCreate(sendData, "sendData",10000,NULL, 2, NULL);
  //xTaskCreate(modoAP, "modoAP",10000,NULL, 1, NULL);
  xTaskCreate(taskSensores, "taskSensores",10000,NULL, 1, NULL);
  xTaskCreate(recordMetrics, "recordMetrics",10000,NULL, 2, NULL);
}
