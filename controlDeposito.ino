#include <NewPing.h>
#include <LiquidCrystal.h>

/********************************************************************************************
Marc Cobler Cosmen - Febrero 2014
Sensor de nivel de depositos. Sensor HC-SR04 ultrasonico. 

Obteniendo la distancia de la superficie y sabiendo las medidas del deposito, podemos calcular
el volumen de liquido contenido. El volumen se muestra en una pantalla LCD 16x2 Hitachi HD44780 driver

Utilizaremos la libreria NewPing.h https://code.google.com/p/arduino-new-ping/ que nos hace 
los calculos de distancia y la libreria LiquidCrystal.h
El pinout de la pantalla es:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*********************************************************************************************/
//Dimensiones del deposito en metros
const float altura = 2;
const float anchura = 5.4;
const float profundidad = 1.8; 

#define trigger 12 //pin para el emisor de la señal
#define echo 11 //pin para el retorno de la onda
#define distancia_max 250 //Máxima distancia que nos calculará la libreria

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //Creamos el objeto lcd (pantalla) MODIFICAR LOS PINES!

NewPing sensor(trigger, echo, distancia_max); //Creamos una variable del tipo sensor
//--------------------------------------------------------
void setup()
{
  lcd.begin(16,2); //inicializamos la pantalla 16 caracteres y 2 lineas
  lcd.setCursor(0,1); //Nos ponemos en la primera linea primera columna
  Serial.begin(19200); //19200 baudios es suficiente
}
//--------------------------------------------------------
void loop()
{
  float nivel=obtenerDistancia();
  calcularVolumen(nivel);
}
//--------------------------------------------------------
float obtenerDistancia() {  
  delay(100); // Esperamos X milisegundos entre lecturas
  unsigned int tiempo = sensor.ping(); //obtenemos el tiempo de un pulso en microsegundos
  unsigned int distancia = tiempo / US_ROUNDTRIP_CM; //Pasamos el tiempo a centímetros mediante una constante
  float distanciaMetros = distancia / 100;
  Serial.print("Distancia: "); //Distancia a la superficie del liquido
  Serial.print(distancia); 
  Serial.println(" cm"); //Retorno de carro
  
  //Mostramos los datos en la pantalla
  lcd.print("Nivel: ");
  lcd.print(altura - distanciaMetros);
  lcd.print(" m");
  
  return distanciaMetros;
}

void calcularVolumen(float nivel){
  
  int volumen = nivel * anchura * profundidad; //Volumen en metros cubicos
  int litros = volumen * 1000; //litros de liquido
  
  //Por el puerto serie
  Serial.print("Volumen: ");
  Serial.print(litros);
  Serial.println(" litros");
  
  //Mostramos los datos en la pantalla
  lcd.setCursor(1,0);
  lcd.print("Vol: ");
  lcd.print(litros);
  lcd.print(" l");
  
}
