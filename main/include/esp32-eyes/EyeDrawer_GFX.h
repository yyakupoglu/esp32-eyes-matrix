/***************************************************
Copyright (c) 2023 Alastair Aitchison, Playful Technology, (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#ifndef _EYEDRAWER_GFX_h
#define _EYEDRAWER_GFX_h

#include <Arduino.h>
#include "EyeConfig.h"

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "esp_log.h"
#include <inttypes.h>

// static const char *TAG = "EYEDRAWER";
enum CornerType {T_R, T_L, B_L, B_R};

#define cyan 852
#define black 0
#define yellow 65472
#define red 63552
#define white 65535
/**
 * Contains all functions to draw eye based on supplied (expression-based) config
 */
class EyeDrawer {
  public:
    static void Draw(MatrixPanel_I2S_DMA *dma_display, int16_t centerX, int16_t centerY, EyeConfig *config) {
      // Amount by which corners will be shifted up/down based on requested "slope" 
      int32_t delta_y_top = config->Height * config->Slope_Top / 2.0;
      int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;
      // Full extent of the eye, after accounting for slope added at top and bottom
      auto totalHeight = config->Height + delta_y_top - delta_y_bottom;
      // If the requested top/bottom radius would exceed the height of the eye, adjust them downwards 
      if (config->Radius_Bottom > 0 && config->Radius_Top > 0 && totalHeight - 1 < config->Radius_Bottom + config->Radius_Top) {
        int32_t corrected_radius_top = (float)config->Radius_Top * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        int32_t corrected_radius_bottom = (float)config->Radius_Bottom * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        config->Radius_Top = corrected_radius_top;
        config->Radius_Bottom = corrected_radius_bottom;
      }

      // ESP_LOGI("EyeDraw", "%d %d %f %f %d %d %d %d", config->Height, config->Width, 
      //config->Slope_Top, config->Slope_Bottom, int(delta_y_top), int(delta_y_bottom),int(config->Radius_Top), int(config->Radius_Bottom));

      // Calculate _inside_ corners of eye (TL, TR, BL, and BR) before any slope or rounded corners are applied
      int32_t TLc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top - delta_y_top;
      int32_t TLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Top;
      int32_t TRc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top + delta_y_top;
      int32_t TRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Top;
      int32_t BLc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom - delta_y_bottom;
      int32_t BLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Bottom;
      int32_t BRc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom + delta_y_bottom;
      int32_t BRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Bottom;
      // ESP_LOGI("TLBR", "\nTLc_y: %d TLc_x: %d\n TRc_y: %d TRc_x: %d\n BLc_y: %d BLc_x: %d\n BRc_y: %d BRc_x: %d\n", 
      // int(TLc_y), int(TLc_x), int(TRc_y), int(TRc_x), int(BLc_y), int(BLc_x), int(BRc_y), int(BRc_x));

      // Calculate interior extents
      int32_t min_c_x = min(TLc_x, BLc_x);
      int32_t max_c_x = max(TRc_x, BRc_x);
      int32_t min_c_y = min(TLc_y, TRc_y);
      int32_t max_c_y = max(BLc_y, BRc_y);

      // Fill eye centre
      EyeDrawer::FillRectangle(dma_display, min_c_x, min_c_y, max_c_x, max_c_y, cyan);

      // // Fill eye outwards to meet edges of rounded corners 
      EyeDrawer::FillRectangle(dma_display, TRc_x, TRc_y, BRc_x + config->Radius_Bottom, BRc_y, cyan); // Right
		  EyeDrawer::FillRectangle(dma_display, TLc_x - config->Radius_Top, TLc_y, BLc_x, BLc_y, cyan); // Left
		  EyeDrawer::FillRectangle(dma_display, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y, cyan); // Top
		  EyeDrawer::FillRectangle(dma_display, BLc_x, BLc_y, BRc_x, BRc_y + config->Radius_Bottom, cyan); // Bottom
        
      // Draw slanted edges at top of bottom of eyes 
      // +ve Slope_Top means eyes slope downwards towards middle of face
      if(config->Slope_Top > 0) {
        EyeDrawer::FillRectangularTriangle(dma_display, TLc_x, TLc_y-config->Radius_Top, TRc_x, TRc_y-config->Radius_Top, cyan);
        EyeDrawer::FillRectangularTriangle(dma_display, TRc_x, TRc_y-config->Radius_Top, TLc_x, TLc_y-config->Radius_Top, black);
      } 
      else if(config->Slope_Top < 0) {
        EyeDrawer::FillRectangularTriangle(dma_display, TRc_x, TRc_y-config->Radius_Top, TLc_x, TLc_y-config->Radius_Top, cyan);
        EyeDrawer::FillRectangularTriangle(dma_display, TLc_x, TLc_y-config->Radius_Top, TRc_x, TRc_y-config->Radius_Top, black);
      }
      // Draw slanted edges at bottom of eyes
      if(config->Slope_Bottom > 0) {
        EyeDrawer::FillRectangularTriangle(dma_display, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, yellow);
        EyeDrawer::FillRectangularTriangle(dma_display, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, white);
      }
      else if (config->Slope_Bottom < 0) {
        EyeDrawer::FillRectangularTriangle(dma_display, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, red);
        EyeDrawer::FillRectangularTriangle(dma_display, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, white);
      }

      // Draw corners (which extend "outwards" towards corner of screen from supplied coordinate values)
      if(config->Radius_Top > 0) {
        EyeDrawer::FillEllipseCorner(dma_display, T_L, TLc_x, TLc_y, config->Radius_Top, config->Radius_Top, cyan);
        EyeDrawer::FillEllipseCorner(dma_display, T_R, TRc_x, TRc_y, config->Radius_Top, config->Radius_Top, cyan);
      }
      if(config->Radius_Bottom > 0) {
        EyeDrawer::FillEllipseCorner(dma_display, B_L, BLc_x, BLc_y, config->Radius_Bottom, config->Radius_Bottom, cyan);
        EyeDrawer::FillEllipseCorner(dma_display, B_R, BRc_x, BRc_y, config->Radius_Bottom, config->Radius_Bottom, cyan);
      }
    }

    // Draw rounded corners
    static void FillEllipseCorner(MatrixPanel_I2S_DMA *dma_display, CornerType corner, int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
      if (rx < 2) return;
      if (ry < 2) return;
      int32_t x, y;
      int32_t rx2 = rx * rx;
      int32_t ry2 = ry * ry;
      int32_t fx2 = 4 * rx2;
      int32_t fy2 = 4 * ry2;
      int32_t s;

      if (corner == T_R) {
        for(x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          dma_display->writeFastHLine(x0, y0 - y, x, color);
          if(s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }         
        for(x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          dma_display->writeFastHLine(x0, y0 - y, x, color);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == B_R) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          dma_display->writeFastHLine(x0, y0 + y -1, x, color);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          dma_display->writeFastHLine(x0, y0 + y -1, x, color);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == T_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          dma_display->writeFastHLine(x0-x, y0 - y, x, color);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          dma_display->writeFastHLine(x0-x, y0 - y, x, color);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == B_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          dma_display->writeFastHLine(x0-x, y0 + y - 1, x, color);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          dma_display->writeFastHLine(x0-x, y0 + y , x, color);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }
    }

    // Fill a solid rectangle between specified coordinates
    static void FillRectangle(MatrixPanel_I2S_DMA *dma_display, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) {
      // Always draw from TL->BR
      int32_t l = min(x0, x1);
      int32_t r = max(x0, x1);
      int32_t t = min(y0, y1);
      int32_t b = max(y0, y1);
      int32_t w = r-l;
      int32_t h = b-t; 
      if (w>0 && h>0) dma_display->fillRect(l,t,w,h,color);
    //   u8g2.setDrawColor(color);
    //   u8g2.drawBox(l, t, w, h);
    //   u8g2.setDrawColor(1);
    }

    static void FillRectangularTriangle(MatrixPanel_I2S_DMA *dma_display, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) {
    //   u8g2.setDrawColor(color);
    //   u8g2.drawTriangle(x0, y0, x1, y1, x1, y0);
    //   u8g2.setDrawColor(1);
        dma_display->fillTriangle(x0, y0, x1, y1, x0, y1, color);
    }

    static void FillTriangle(MatrixPanel_I2S_DMA *dma_display, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color) {
        // u8g2.setDrawColor(color);
        // u8g2.drawTriangle(x0, y0, x1, y1, x2, y2);
        // u8g2.setDrawColor(1);
        dma_display->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    }
};

#endif