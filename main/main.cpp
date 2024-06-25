#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "Face.h"
#include "EyePresets.h"
#include "FaceEmotions.hpp"

// Leftside HUB75
#define R1 15
#define BL1 2
#define R2 4
#define BL2 16
#define CH_A 17
#define CH_C 5
#define CLK 18
#define OE 19

// Rightside HUB75
#define G1 32
// GND
#define G2 25
#define CH_E 26 
#define CH_B 27
#define CH_D 14
#define LAT 12
// GND


#define WIDTH 64
#define HEIGHT 64
#define CHAIN 1
#define EYE_SIZE 36

extern "C" void app_main() {
  HUB75_I2S_CFG mxconfig(/* width = */ WIDTH, /* height = */ HEIGHT, /* chain = */ 1);
  HUB75_I2S_CFG::i2s_pins pins={R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};
  mxconfig.gpio = pins;
  mxconfig.double_buff = true;
  MatrixPanel_I2S_DMA* Buffer = new MatrixPanel_I2S_DMA(mxconfig);

  Face* face = new Face(Buffer,WIDTH, HEIGHT, EYE_SIZE);

  face->Expression.GoTo_Skeptic();
   // Automatically choose a new random direction to look
  // Automatically blink
  face->RandomBlink = true;
  // Set blink rate
  face->Blink.Timer.SetIntervalMillis(4000);
  face->RandomLook = true;
  while (true) 
  { 
    // If not vtaskdelay 
    vTaskDelay(10 / portTICK_PERIOD_MS); 
    face->Update();
  }
 
  
  // unsigned long mil;
  // static int lastMoveTime;
    
      
  //   for (int i=Normal; i<EMOTIONS_COUNT; i++){
      
  //     while(true) {
  //     mil = millis();
  //   // To avoid making eyes too twitchy (and to allow time for previous move animation to end),
  //   // only recalculate new position every 5000ms
  //     if(mil - lastMoveTime > 5000) {
  //       eEmotions eCurrent = eEmotions(i);
  //       vTaskDelay(10 / portTICK_PERIOD_MS);
  //       face->Behavior.SetEmotion(eCurrent, 1.0);
  //       lastMoveTime = millis();
  //       ESP_LOGI("MAIN", "Current emotion is: %d", i);
  //       break;
  //     }
  //   vTaskDelay(10 / portTICK_PERIOD_MS);
  //   ESP_LOGI("MAI", "min %lu, %d", mil, lastMoveTime);
  //   face->Update();
  //   }
  //   // 
    
    
  //   }


}

