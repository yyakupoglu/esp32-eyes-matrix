/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#ifndef _FACE_h
#define _FACE_h

#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include <Arduino.h>
#include "Animations.h"
#include "EyePresets.h"
#include "Eye.h"
#include "FaceExpression.h"
#include "FaceBehavior.h"
#include "LookAssistant.h"
#include "BlinkAssistant.h"

class Face {

public:
    Face(MatrixPanel_I2S_DMA *buffer , uint16_t width, uint16_t height, uint16_t eyeSize);
    
    bool i;
    MatrixPanel_I2S_DMA *dma_display = nullptr;

    uint16_t Width;
    uint16_t Height;
    uint16_t CenterX;
    uint16_t CenterY;
    uint16_t EyeSize;
    uint16_t EyeInterDistance = 4;

    Eye LeftEye;
    Eye RightEye;
    BlinkAssistant Blink;
    LookAssistant Look;
    FaceBehavior Behavior;
    FaceExpression Expression;

    void Update();
    void DoBlink();

    bool RandomBehavior = false;
    bool RandomLook = false;
    bool RandomBlink = false;

    void LookLeft();
    void LookRight();
    void LookFront();
    void LookTop();
    void LookBottom();
    void Wait(unsigned long milliseconds);

protected:
    void Draw();
};

#endif