#include <Wire.h> //Necesario para la comunicación I2C
#include <Adafruit_SSD1306.h>
//#include <Adafruit_GFX.h>

// Configuración OLED
#define OLED_ADDR 0x3C //Dirección I2C para Pantalla OLED
#define ancho 128 //Ancho
#define alto 64 //Alto de pantalla
Adafruit_SSD1306 display(ancho, alto, &Wire, -1); // Constructor para pantalla oled
bool oledConnected = false; //Bandera para indicar si esta conectada la pantalla
unsigned long lastCheckTime = 0; 
const unsigned long checkInterval = 2000; // Revisar conexión cada ciertos segundos

// Configuración MPU6050
const int MPU_ADDR = 0x68; //Dirección I2C para MPU6050
const float ACCEL_SCALE = 16384.0; // Escala a 2g 
const float GYRO_SCALE = 131.0;    // Escala a 250°/s
float az_filtrado = 0; //Variable para aceleración filtrada
float gyroMagnitude = 0; //Variable para velocidad angular
bool apogeo_detectado = false; //Bandera para indicar si se detecto apogeo

void setup() {
  Serial.begin(9600);
  Wire.begin(); //Inicia I2C

  // Inicializar MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // Registro PWR_MGMT_1
  Wire.write(0);     // Despertar sensor
  Wire.endTransmission();

  // Configurar acelerómetro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C); // Registro ACCEL_CONFIG
  Wire.write(0x00); // 0X00 = 2g
  Wire.endTransmission();

  // Inicializar OLED
  checkOLEDConnection();
}

void loop() {
  // Verificar conexión OLED periódicamente
  if (millis() - lastCheckTime >= checkInterval) {
    checkOLEDConnection();
    lastCheckTime = millis();
  }

  // Leer y procesar MPU6050
  float az = leerAceleracionZ();
  az_filtrado = 0.9 * az_filtrado + 0.1 * az; // Filtro promedio

  //Leer giroscopio solo si aún no se detectó el apogeo
  if (!apogeo_detectado) {
    gyroMagnitude = leerMagnitudGiroscopio();
    debugSerial(az_filtrado, gyroMagnitude); // Mostrar datos por serial

    // Detectar apogeo
    if (az_filtrado < 0.5 && abs(az_filtrado - (-0.5)) < 0.5 && gyroMagnitude < 45.0) {
      apogeo_detectado = true;
      Serial.println("¡APOGEO DETECTADO!");
    }
  }

  // Actualizar pantalla cada cierto tiempo (500 ms)
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 500) {
    lastUpdate = millis();
    if (oledConnected) {
      updateDisplay(az_filtrado, gyroMagnitude, apogeo_detectado);
    }
  }
  delay(50); // Evitar sobrecarga
}

// --- Funciones OLED ---
// Checar conexión oled e inicializar
void checkOLEDConnection() {
  Wire.beginTransmission(OLED_ADDR);
  byte error = Wire.endTransmission();
  
  if (error == 0 && !oledConnected) {
    //Serial.println("OLED detectada"); 
    initializeOLED();
  } else if (error != 0 && oledConnected) {
    //Serial.println("OLED desconectada");
    oledConnected = false;
  }
}
//Inicializar display oled
void initializeOLED() {
  oledConnected = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  if (oledConnected) {
    display.clearDisplay();//Borrar toda la pantalla
    display.setTextSize(1.2);//Tamaño de texto
    display.setTextColor(SSD1306_WHITE);//Color monocromático
    display.setCursor(0, 0);//Ubicación inicial para bienvenida
    display.println("Sistema OK"); //Palabras de bienvenida
    display.display();
  }
}
//Imprimir valores de sensor en pantalla
void updateDisplay(float az, float gyro, bool apogeo) {
  display.clearDisplay();//Limpiar pantalla
  display.setCursor(0, 4);//Ubicación de cursor
  display.print("Aceleracion Z: "); display.print(az, 2); display.println(" g");
  display.print("Giro: "); display.print(gyro, 1); display.println(" °/s");
  display.print("Estado: ");
  display.println(apogeo ? "APOGEO" : "ASCENSO");
  display.display();
}

// --- Funciones MPU6050 ---
//Leer aceleración Z en 'g'
float leerAceleracionZ() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3F); // Registro ACCEL_ZOUT_H (aceleración en Z)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 2, true);//Lectura 2 bytes (high low)
  int16_t az_raw = Wire.read() << 8 | Wire.read();
  return az_raw / ACCEL_SCALE; //Conversión a fuerzas d egravedad
}
// Magnitud de rotación en grados/segundo
float leerMagnitudGiroscopio() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43); // Registro GYRO_XOUT_H (Inicio lecturas giroscopio)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);//6 bytes para (x,y,z)
  float gx = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE;
  float gy = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE;
  float gz = (Wire.read() << 8 | Wire.read()) / GYRO_SCALE;
  return sqrt(gx*gx + gy*gy + gz*gz); //Magnitud de la velocidad angular
}
//Datos para mostrar en serial
void debugSerial(float az, float gyro) {
  Serial.print("Aceleracion Z: "); Serial.print(az, 2);
  Serial.print(" g | Giro: "); Serial.print(gyro, 1);
  Serial.println(" °/s");
}