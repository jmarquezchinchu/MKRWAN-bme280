

#include <MKRWAN.h>

#include <BME280I2C.h>
#include <Wire.h>

BME280I2C bme;


LoRaModem modem;

unsigned long Crono1 = 0;

unsigned int TiempoTx = 60000;  //Transmision cada 60 segundos

//Claves de acceso a red TTN OTAA

String appEui = "xxxxxxxxxxxxxxxx";
String appKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

//Claves de acceso a red TTN ABP

String devAddr = "xxxxxxxxxxx";
String nwkSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
String appSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

void setup() {

  Wire.begin();
  bme.begin();

  pinMode(A0, INPUT);

  modem.begin(EU868); //Frecuencia europea TTN
  delay(1000);

  //int connected = modem.joinOTAA(appEui, appKey);
  int connected = modem.joinABP(devAddr, nwkSKey, appSKey);


  // Set poll interval to 60 secs.

}

void loop() {

    modem.minPollInterval(60);
    modem.dataRate(5);
    analogReadResolution(12);

    float temp(NAN), hum(NAN), pres(NAN);
    int16_t adc0;

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    bme.read(pres, temp, hum, tempUnit, presUnit);

    float celsius = temp;
    float relativehum = hum;
    float pressure = pres;

    // Convertimos valores a bytes

    int16_t temperatura = (int16_t)(celsius * 10); //pendiente de sumar +40 para no transmitir parte negativa
    int16_t humedad = (int16_t)(relativehum * 10);
    int16_t presion = (int16_t)(pressure / 10);

    int16_t bateria = (int16_t)(analogRead(A0));

    //Creamos el array de datos, tantos como se haya definido entre corchetes, contando el cero, con bitshift y relleno de cero hasta los 16 bits

    byte datos[8];
    datos[0] = temperatura >> 8;
    datos[1] = temperatura & 0xFF;
    datos[2] = humedad >> 8;
    datos[3] = humedad & 0xFF;
    datos[4] = presion >> 8;
    datos[5] = presion & 0xFF;
    datos[6] = bateria >> 8;
    datos[7] = bateria & 0xFF;

    //Envio de los datos por el modem LORA
    int err;
    modem.beginPacket();
    modem.write(datos, sizeof(datos));
    err = modem.endPacket();

    delay(60 * 1000); // Pediente de eliminar al probar funcion con millis
 
}
