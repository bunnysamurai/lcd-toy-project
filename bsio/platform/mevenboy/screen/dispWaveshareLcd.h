//(c) 2023 Dmitry Grinberg  https://dmitry.gr
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice,
//this list 		of conditions and the following disclaimer. 	Redistributions in
//binary form must reproduce the above copyright notice, this 		list of conditions
//and the following disclaimer in the documentation and/or 		other materials
//provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. 	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, 	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//SERVICES; LOSS OF USE, DATA, OR 	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, 	WHETHER IN CONTRACT, STRICT LIABILITY,
//OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 	ARISING IN ANY WAY OUT OF THE USE
//OF THIS SOFTWARE, EVEN IF ADVISED OF THE 	POSSIBILITY OF SUCH DAMAGE.[

#ifndef _DISP_WAVESHARE_LCD_H_
#define _DISP_WAVESHARE_LCD_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// externally defined
void dispExtTouchReport(int16_t x, int16_t y); // negative on pen up

// structs
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} ClutEntry_t;

// defined here
typedef struct {
  uint32_t width;
  uint32_t height;
} DispDimensions_t;

void dispSetDepth(uint8_t depth);
bool dispSetVirtualDimensions(DispDimensions_t virtual_size);
bool dispSetPhysicalDimensions(DispDimensions_t physical_size);
uint8_t dispGetDepth();
DispDimensions_t dispGetVirtualDimensions();
DispDimensions_t dispGetPhysicalDimensions();

void dispSetClut(int32_t firstIdx, uint32_t numEntries,
                 const ClutEntry_t *entries);
void dispSetVideoBuffer(const uint8_t *framebuffer);
const uint8_t *dispGetVideoBuffer();
bool dispInit(const uint8_t *framebuffer, uint8_t depth,
              DispDimensions_t virtual_size, DispDimensions_t physical_size);
bool dispOn(void);
bool dispOff(void);

typedef struct {
  uint32_t touch_zthresh;
  uint8_t first_toss; // first this many points are tossed
  uint8_t last_toss;  // then this many are averaged (and dropped at end)
} dispTouchCfg_t;
void dispConfigureTouch(dispTouchCfg_t cfg);
void dispGetTouchConfiguration(dispTouchCfg_t *cfg);

#ifdef __cplusplus
}
#endif
#endif