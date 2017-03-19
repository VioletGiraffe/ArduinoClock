#include "PDQ_ST7735_config.h"
#include <PDQ_FastPin.h>
#include <PDQ_ST7735.h>

#include <gfxfont.h>
#include <PDQ_GFX.h>

#include <TimerOne.h>

#ifndef _PDQ_ST7735H_
#define TFT_RST 0  // you can also connect this to the Arduino reset, in which case, set this #define pin to 0!
Adafruit_ST7735 tft = Adafruit_ST7735(ST7735_CS_PIN, ST7735_DC_PIN, TFT_RST);
#else
PDQ_ST7735 tft;
#endif

constexpr auto HourButtonPin = 2, MinuteButtonPin = 3;

// Time since 00:00 (12 AM)
struct
{
	uint8_t hh = 23, mm = 59, ss = 55;
} time;

inline void onTick()
{
	++time.ss;

	if (time.ss == 60)
	{
		time.ss = 0;
		++time.mm;

		if (time.mm == 60)
		{
			time.mm = 0;
			time.hh = time.hh == 23 ? 0 : time.hh + 1;
		}
	}

	drawClock();
}

void processButtons()
{
	const bool hrBtnPressed = digitalRead(HourButtonPin) == HIGH;
	const bool minuteBtnPressed = digitalRead(MinuteButtonPin) == HIGH;

	if (hrBtnPressed)
		time.hh = time.hh == 23 ? 0 : time.hh + 1;

	if (minuteBtnPressed)
		time.mm = time.mm == 59 ? 0 : time.mm + 1;

	if (minuteBtnPressed || hrBtnPressed)
	{
		time.ss = 0;
		drawClock();
	}
}

#define RGB_to_565(R, G, B) static_cast<uint16_t>(((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3))

void setup()
{
	tft.initR(ST7735_INITR_144GREENTAB); // initialize a ST7735S chip, 1.44" TFT, black tab
	tft.fillScreen(ST7735_BLACK);
	tft.setTextWrap(false);

	Timer1.initialize(1000000UL); // 1 second
	Timer1.attachInterrupt(&onTick);

	tft.setTextSize(5);
	tft.setTextColor(RGB_to_565(0, 127, 255), RGB_to_565(0, 0, 0));
	tft.fillScreen(RGB_to_565(0, 0, 0));

	pinMode(HourButtonPin, INPUT); // Hours inc button
	pinMode(MinuteButtonPin, INPUT); // Minutes inc button

	drawClock();
}

void loop()
{
	processButtons();
}

constexpr uint16_t ScreenWidth = 128, ScreenHeight = 128;

inline void printTwoDigits(uint32_t number)
{
	if (number < 10)
		tft.print('0');

	tft.print(number);
}

inline void drawClock()
{
	tft.setCursor(35, 5);
	printTwoDigits(time.hh);

	tft.setCursor(35, 85);
	printTwoDigits(time.mm);

	if (time.ss > 0)
		tft.fillRect(0, 55, ScreenWidth * time.ss / 60, 5, RGB_to_565(0, 127, 255));
	else
		tft.fillRect(0, 55, ScreenWidth, 5, RGB_to_565(0, 0, 0));
}

