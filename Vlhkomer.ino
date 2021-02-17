#include "DHT.h"
#define pinDHT 53

#define typDHT22 DHT22 

DHT myDHT(pinDHT, typDHT22);

#include <LCDWIKI_GUI.h> 
#include <LCDWIKI_SPI.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC A3
#define TFT_CS A5
#define TFT_MOSI 51
#define TFT_CLK 52
#define TFT_RST A4
#define TFT_MISO 50
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#include <SoftwareSerial.h>
const int RX = 13;
const int TX = 12;
SoftwareSerial bluetooth(TX, RX);

const long int timeUnit = 2500; // po kolika ms to má obnovovat data
const long int showedTime = 60; // kolik vteřin to má ukazovat

#include "plot.h"
Plot myPlot(1, 50, 318, 189, 100, 0, 50, 0, showedTime, timeUnit, ILI9341_BLUE, ILI9341_RED);

float tem;
float hum;
long int timeMillis;
long int nowTime;

void setup() {
  myDHT.begin();
  tft.begin();
  bluetooth.begin(9600);
  bluetooth.println("Arduino vlhkoměr je zapnutý");
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  yield();
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setTextSize(3);
  timeMillis = -timeUnit;
  myPlot.Show(tft);
}

void loop() {
  nowTime = (long int) millis();
  if ((timeUnit - (nowTime - timeMillis)) > 0)
  {
  delay(timeUnit - (nowTime - timeMillis));
  }
  timeMillis = (long int) millis();
  
  tem = myDHT.readTemperature();
  hum = myDHT.readHumidity();
  
  if (isnan(tem) || isnan(hum)) {
    tft.setCursor(1, 1);
    tft.setTextSize(6);
    tft.println("Chyba    ");
    bluetooth.println("Chyba");
    tft.setTextSize(3);
  } else {
    tft.setCursor(1, 1);
    tft.print("Vlhkost: ");
    tft.print(hum);
    tft.println(" %");
    tft.setCursor(1, tft.getCursorY());
    tft.print("Teplota: ");
    tft.print(tem);
    tft.print(" ");
    printDegreeCharacter(tft, 3, ILI9341_BLACK);
    tft.print("C");
    bluetooth.print("Vlhkost: ");
    bluetooth.print(hum);
    bluetooth.print(" %, ");
    bluetooth.print("Teplota: ");
    bluetooth.print(tem);
    bluetooth.println(" °C");
    myPlot.Refresh(tft, hum, tem);
  }
}
