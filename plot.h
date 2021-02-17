#ifndef plot_h
#define plot_h

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

const int MaxTimeSize = 601; // maximálně graf dokáže zaznamenat 25 minut po 2.5 vteřinách při tomto nastavení MaxTimeSize
  
void printDegreeCharacter(Adafruit_ILI9341 &disp, int textSize, uint16_t color){
  disp.drawCircle(disp.getCursorX() + textSize, disp.getCursorY() + textSize, textSize, color);
  disp.setCursor(disp.getCursorX() + 2*textSize + 2, disp.getCursorY());
}


class Plot
{
  private:
  int x;
  int y;
  int width;
  int height;
  
  int filled;
  
  int plotX;
  int plotY;
  int plotWidth;
  int plotHeight;
  
  int MaxHum;
  int MinHum;
  int MaxHumUnit;
  int MaxTem;
  int MinTem;
  int MaxTemUnit;
  long int MaxTime;
  int MaxTimeUnit;
  long int timeSection;
  
  float UnitHum;
  float UnitTem;
  float UnitTime;

  uint16_t backgroundColor;
  uint16_t lineTextColor;
  uint16_t humColor;
  uint16_t temColor;
  
  public:
  float EntriesHum[MaxTimeSize];
  float EntriesTem[MaxTimeSize];

    Plot(int x, int y, int width, int height, int MaxHum, int MinHum, int MaxTem, int MinTem, long int MaxTime, long int timeSection, uint16_t humColor, uint16_t temColor)
    {
      this->x = x;
      this->y = y;
      this->width = width;
      this->height = height;

      this->humColor = humColor;
      this->temColor = temColor;
      backgroundColor = ILI9341_WHITE;
      lineTextColor = ILI9341_BLACK;

      plotX = x + 24;
      plotY = y;
      plotWidth = width - 48;
      plotHeight = height - 16;

      filled = -1;
      
      this->MaxHum = MaxHum;
      this->MinHum = MinHum;
      MaxHumUnit = MaxHum - MinHum;
      
      this->MaxTem = MaxTem;
      this->MinTem = MinTem;
      MaxTemUnit = MaxTem - MinTem;
      
      this->MaxTime = MaxTime;
      this->MaxTimeUnit = (int) round((float) MaxTime / ((float) timeSection / 1000.0));
      this->timeSection = timeSection;
      

      UnitHum = (float) (plotHeight-1) / (float) MaxHumUnit;
      UnitTem = (float) (plotHeight-1) / (float) MaxTemUnit;
      UnitTime = (float) (plotWidth-1) / (float) MaxTimeUnit;
    }

   void Show(Adafruit_ILI9341 disp){
    disp.drawFastVLine(plotX - 1, plotY, plotHeight, lineTextColor);
    disp.drawFastHLine(plotX - 1, plotY + plotHeight, plotWidth + 2, lineTextColor);
    disp.drawFastVLine(plotX + plotWidth, plotY, plotHeight, lineTextColor);
    
    disp.setTextSize(2);
    
    disp.setCursor(plotX - 1, plotY + plotHeight + 2);
    disp.print(MaxTime);
    disp.print(" s");

    // popisky
    disp.setRotation(2);
    disp.setCursor(disp.width() - (plotY + plotHeight) + 20, plotX - 17);
    disp.setTextColor(humColor);
    disp.print("-");
    disp.setTextColor(lineTextColor);
    disp.print("Vlhkost");
    disp.setCursor(disp.width() - (plotY + plotHeight) + 20, plotX + plotWidth + 2);
    disp.setTextColor(temColor);
    disp.print("-");
    disp.setTextColor(lineTextColor);
    disp.print("Teplota");
    disp.setRotation(3);

    //popisky hodnot
    disp.setTextSize(1);
    disp.setCursor(plotX - 25, plotY);
    disp.print(MaxHum);
    disp.print("%");    
    disp.setCursor(plotX - 13, plotY + plotHeight - 7);
    disp.print(MinHum);
    disp.print("%");
    disp.setCursor(plotX + plotWidth + 2, plotY);
    disp.print(MaxTem);
    printDegreeCharacter(disp, 1, lineTextColor);
    disp.print("C");
    disp.setCursor(plotX + plotWidth + 2, plotY + plotHeight - 7);
    disp.print(MinTem);
    printDegreeCharacter(disp, 1, lineTextColor);
    disp.print("C");    
    
    disp.setTextSize(3);
   }

   void NewEntryReplace(float hum, float tem){
      for (int i = 0; i < MaxTimeUnit; i++)
      {
        EntriesHum[i] = EntriesHum[i+1];
        EntriesTem[i] = EntriesTem[i+1];
        }
      EntriesHum[MaxTimeUnit] = hum;
      EntriesTem[MaxTimeUnit] = tem;
   }

  void NewEntryAdd(float hum, float tem){
    filled++;
    EntriesHum[filled] = hum;
    EntriesTem[filled] = tem;    
  }

  void Refresh(Adafruit_ILI9341 disp, float hum, float tem){
    if (filled == MaxTimeUnit){ 
     WriteGraph(disp, backgroundColor, backgroundColor);
     NewEntryReplace(hum, tem);
     WriteGraph(disp, humColor, temColor);
    }
    else if (filled > -1)
    {
      NewEntryAdd(hum, tem);
      WriteLineHum(disp, filled - 1, humColor);
      WriteLineTem(disp, filled - 1, temColor);
    }
    else
    {
      NewEntryAdd(hum, tem);
    }
  }

  private:

  void WriteLineHum(Adafruit_ILI9341 disp, int pos, uint16_t color){
      disp.drawLine(plotX + pos*UnitTime, plotY + plotHeight - EntriesHum[pos]*UnitHum, plotX + (pos+1)*UnitTime, plotY + plotHeight - EntriesHum[pos+1]*UnitHum, color);
  }

  void WriteLineTem(Adafruit_ILI9341 disp, int pos, uint16_t color){
      disp.drawLine(plotX + pos*UnitTime, plotY + plotHeight - EntriesTem[pos]*UnitTem, plotX + (pos+1)*UnitTime, plotY + plotHeight - EntriesTem[pos+1]*UnitTem, color);
  }


  void WriteGraph(Adafruit_ILI9341 disp, uint16_t HumColor, uint16_t TemColor){
    for (int i = 0; i < filled; i++){
      WriteLineHum(disp, i, HumColor);
      WriteLineTem(disp, i, TemColor);
    }
  }


};
#endif
