/*
  Adattato dallo Sketch di Adrirobot
  sensore-GUVA-S12SD_display.ino
  Lo sketch misura la tensione misurata dal sensore
  GUVA-S12SD, lo trasforma nel'indice UV e lo mostra
  sul display
*/
 

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>

#define DISPLAY_TYPE_LCD
// define DISPLAY_TYPE_OLED

#define LOOP_DELAY 2000
#define SAMPLE_DELAY 10

#define SAMPLES 4
#define SAMPLE_SHIFT 2

// OLED display wire address


#define DISPLAY_LCD_ADDR 0x27
#define DISPLAY_OLED_ADDR 0x3c

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 1


#define ADC_OFFSET 15

// indici UVI 
#define V_UVI_0 0.050
#define V_UVI_1 0.227
#define V_UVI_2 0.318
#define V_UVI_3 0.408
#define V_UVI_4 0.503
#define V_UVI_5 0.606
#define V_UVI_6 0.696
#define V_UVI_7 0.795
#define V_UVI_8 0.881
#define V_UVI_9 0.976
#define V_UVI_10 1.079
#define V_UVI_11 1.170

#ifdef DISPLAY_TYPE_LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

#ifdef DISPLAY_TYPE_OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

int UV_index;
String quality = " -- ";
uint8_t cnt=0;

 
void setup()
{
  Serial.begin(9600);
  // initialize and clear display
  setupDisplay();


  pinMode(LED_BUILTIN, OUTPUT);

}

void setupDisplay(void)
{
#ifdef DISPLAY_TYPE_LCD
  lcd.begin(); //Init with pin default ESP8266 or ARDUINO
  lcd.backlight(); //accende la retroilluminazione
#endif

#ifdef DISPLAY_TYPE_OLED
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_OLED_ADDR);
  display.setTextColor(WHITE);
  display.display();
#endif

  delay(LOOP_DELAY);
}

float interpolate_uvi(float value, float max, float min)
{
  return (((value-min)/(max-min)));
}

float calc_UVI(float voltage)
{

  float uvi=-1;
  
  quality = " -- ";

  if (voltage >= V_UVI_11) 
  {
    quality = " ESTREMO  ";
    uvi = 11;
  }
  else if (voltage >= V_UVI_10)
  {
    quality = "MOLTO ALTO";
    uvi = 10 + interpolate_uvi(voltage, V_UVI_11, V_UVI_10);
  }
  else if (voltage >= V_UVI_9)
  {
    quality = "MOLTO ALTO";
    uvi = 9 + interpolate_uvi(voltage, V_UVI_10, V_UVI_9);
  }
  else if (voltage >= V_UVI_8)
  {
    quality = "MOLTO ALTO";
    uvi = 8 + interpolate_uvi(voltage, V_UVI_9, V_UVI_8);
  }
  else if (voltage >= V_UVI_7)
  {
    quality = "ALTO";    
    uvi = 7 + interpolate_uvi(voltage, V_UVI_8, V_UVI_7);
  }
  else if (voltage >= V_UVI_6)
  {
    quality = "ALTO";        
    uvi = 6 + interpolate_uvi(voltage, V_UVI_7, V_UVI_6);
  }
  else if (voltage >= V_UVI_5)
  {
    quality = " MODERATO ";   
    uvi = 5 + interpolate_uvi(voltage, V_UVI_6, V_UVI_5);
  }
  else if (voltage >= V_UVI_4)
  {
    quality = " MODERATO ";       
    uvi = 4 + interpolate_uvi(voltage, V_UVI_5, V_UVI_4);
  }
  else if (voltage >= V_UVI_3)
  {
    quality = " MODERATO ";       
    uvi = 3 + interpolate_uvi(voltage, V_UVI_4, V_UVI_3);
  }
  else if (voltage >= V_UVI_2)
  {
    quality = "  BASSO  ";    
    uvi = 2 + interpolate_uvi(voltage, V_UVI_3, V_UVI_2);
  }
  else if (voltage >= V_UVI_1)
  {
    quality = "  BASSO  ";
    uvi = 1 + interpolate_uvi(voltage, V_UVI_2, V_UVI_1);
  }
  else if (voltage >= V_UVI_0)
  {
    quality = " -- ";
    uvi = 0 + interpolate_uvi(voltage, V_UVI_1, V_UVI_0);
  } 

  return uvi;
}
 
void loop()
{
  float sensorVoltage;
  int sensorValue;
  uint8_t i;

  // Misura
  sensorValue = 0;
  for (i=0; i < SAMPLES; i++)
  {
    delay(SAMPLE_DELAY);
    sensorValue += analogRead(A0);
  }
  sensorValue = sensorValue >> SAMPLE_SHIFT;

  sensorVoltage = ((float)(sensorValue - ADC_OFFSET) / 1024 * 3.3);
  float UV_index = calc_UVI(sensorVoltage);
 

  // LED
  digitalWrite(LED_BUILTIN, (cnt++)&0x01);

  // Seriale
  //Serial.printf(PSTR("sensor reading = %f \n"),sensorValue);
  Serial.print(sensorValue);
  Serial.println("");
  Serial.print("sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
 
   // Display

#ifdef DISPLAY_TYPE_LCD
  lcd.setCursor(0, 0);
  lcd.print("UVI:");
  if (UV_index < 0)
  {
    lcd.print(" -- ");
  }
  else
  {
    lcd.print(String(UV_index,1));
  }
  lcd.setCursor(8,0);
  lcd.print("        ");
  lcd.setCursor(8,0);
  lcd.print(quality);

  lcd.setCursor(0, 1);
  lcd.print("V:");
  lcd.print(sensorVoltage);
  lcd.setCursor(8, 1);
  lcd.print("ADC: "); 
  lcd.setCursor(12, 1);
  lcd.print("    ");
  lcd.setCursor(12, 1);
  lcd.print(sensorValue);
#endif

#ifdef DISPLAY_TYPE_OLED
 
  display.clearDisplay();

  // linea 1
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("UVI: ");
  display.setTextSize(2);

  if (UV_index < 0)
  {
    display.println(" -- ");
  }
  else
  {
    display.println(String(UV_index,1));
  }
 
  // linea 2
  display.println(quality);
  // linea 3
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print("V: ");
  display.println(sensorVoltage);
  // linea 4
  display.print("ADC: ");
  display.println(sensorValue);
  
  display.display();
#endif




  delay(LOOP_DELAY);


}
