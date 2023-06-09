//(c) 2023 Dmitry Grinberg  https://dmitry.gr
//Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//	Redistributions of source code must retain the above copyright notice, this list
//		of conditions and the following disclaimer.
//	Redistributions in binary form must reproduce the above copyright notice, this
//		list of conditions and the following disclaimer in the documentation and/or
//		other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//	POSSIBILITY OF SUCH DAMAGE.[


#ifndef _DISP_WAVESHARE_LCD_H_
#define _DISP_WAVESHARE_LCD_H_

#include <stdbool.h>
#include <stdint.h>


#define MAX_SUPPORTED_BPP						16
#define DISP_WIDTH								240		//the size of the display you want
#define DISP_HEIGHT								320

#define LCD_REAL_WIDTH							240		//actual size of the LCD display
#define LCD_REAL_HEIGHT							320

#ifdef __cplusplus
extern "C" {
#endif

//externally defined
void dispExtTouchReport(int16_t x, int16_t y);	//negative on pen up

//structs
struct ClutEntry {
	uint8_t r, g, b;	
};

//defined here

void dispSetDepth(uint8_t depth);
void dispSetClut(int32_t firstIdx, uint32_t numEntries, const struct ClutEntry *entries);
bool dispInit(void* framebuffer, uint8_t depth);
bool dispOn(void);
bool dispOff(void);

#ifdef __cplusplus
}
#endif
#endif