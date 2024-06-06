#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

// #include "Face.h"
// #include <Adafruit_GFX.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;

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

uint16_t myBLUE = dma_display->color565(0, 0, 255);

// Face* face;

extern "C" void app_main() {
  HUB75_I2S_CFG mxconfig(/* width = */ 64, /* height = */ 64, /* chain = */ 1);
  HUB75_I2S_CFG::i2s_pins pins={R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};
  mxconfig.gpio = pins;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90);
  dma_display->clearScreen();
  dma_display->fillScreen(myBLUE);
  dma_display->drawFastHLine(32,32,32,65536);
  // `println` is only available when the Adafruit GFX library is used.
  dma_display->println("Hello\n World");
}
