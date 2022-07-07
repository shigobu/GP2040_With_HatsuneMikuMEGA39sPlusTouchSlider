/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include "pico/stdlib.h"
#include "gamepad.h"
#include "display.h"
#include "storage.h"
#include "display.h"
#include "OneBitDisplay.h"
#include "Adafruit_MPR121.h"
#include "Arduino.h"

void Gamepad::setup()
{
	load();

	// Configure pin mapping
	f2Mask = (GAMEPAD_MASK_A1 | GAMEPAD_MASK_S2);
	BoardOptions boardOptions = getBoardOptions();

	mapDpadUp    = new GamepadButtonMapping(boardOptions.pinDpadUp,    GAMEPAD_MASK_UP);
	mapDpadDown  = new GamepadButtonMapping(boardOptions.pinDpadDown,  GAMEPAD_MASK_DOWN);
	mapDpadLeft  = new GamepadButtonMapping(boardOptions.pinDpadLeft,  GAMEPAD_MASK_LEFT);
	mapDpadRight = new GamepadButtonMapping(boardOptions.pinDpadRight, GAMEPAD_MASK_RIGHT);
	mapButtonB1  = new GamepadButtonMapping(boardOptions.pinButtonB1,  GAMEPAD_MASK_B1);
	mapButtonB2  = new GamepadButtonMapping(boardOptions.pinButtonB2,  GAMEPAD_MASK_B2);
	mapButtonB3  = new GamepadButtonMapping(boardOptions.pinButtonB3,  GAMEPAD_MASK_B3);
	mapButtonB4  = new GamepadButtonMapping(boardOptions.pinButtonB4,  GAMEPAD_MASK_B4);
	mapButtonL1  = new GamepadButtonMapping(boardOptions.pinButtonL1,  GAMEPAD_MASK_L1);
	mapButtonR1  = new GamepadButtonMapping(boardOptions.pinButtonR1,  GAMEPAD_MASK_R1);
	mapButtonL2  = new GamepadButtonMapping(boardOptions.pinButtonL2,  GAMEPAD_MASK_L2);
	mapButtonR2  = new GamepadButtonMapping(boardOptions.pinButtonR2,  GAMEPAD_MASK_R2);
	mapButtonS1  = new GamepadButtonMapping(boardOptions.pinButtonS1,  GAMEPAD_MASK_S1);
	mapButtonS2  = new GamepadButtonMapping(boardOptions.pinButtonS2,  GAMEPAD_MASK_S2);
	mapButtonL3  = new GamepadButtonMapping(boardOptions.pinButtonL3,  GAMEPAD_MASK_L3);
	mapButtonR3  = new GamepadButtonMapping(boardOptions.pinButtonR3,  GAMEPAD_MASK_R3);
	mapButtonA1  = new GamepadButtonMapping(boardOptions.pinButtonA1,  GAMEPAD_MASK_A1);
	mapButtonA2  = new GamepadButtonMapping(boardOptions.pinButtonA2,  GAMEPAD_MASK_A2);

	gamepadMappings = new GamepadButtonMapping *[GAMEPAD_DIGITAL_INPUT_COUNT]
	{
		mapDpadUp,   mapDpadDown, mapDpadLeft, mapDpadRight,
		mapButtonB1, mapButtonB2, mapButtonB3, mapButtonB4,
		mapButtonL1, mapButtonR1, mapButtonL2, mapButtonR2,
		mapButtonS1, mapButtonS2, mapButtonL3, mapButtonR3,
		mapButtonA1, mapButtonA2
	};

	for (int i = 0; i < GAMEPAD_DIGITAL_INPUT_COUNT; i++)
	{
		gpio_init(gamepadMappings[i]->pin);             // Initialize pin
		gpio_set_dir(gamepadMappings[i]->pin, GPIO_IN); // Set as INPUT
		gpio_pull_up(gamepadMappings[i]->pin);          // Set as PULLUP
	}

	#ifdef PIN_SETTINGS
		gpio_init(PIN_SETTINGS);             // Initialize pin
		gpio_set_dir(PIN_SETTINGS, GPIO_IN); // Set as INPUT
		gpio_pull_up(PIN_SETTINGS);          // Set as PULLUP
	#endif

	//エラー表示用設定　25番はビルドインLED
	pinMode(25, OUTPUT);
	digitalWrite(25, 0);

	isTouch32Bit = boardOptions.isTouch32Bit;
	mpr121_1 = new Adafruit_MPR121(0x5A, (boardOptions.i2cBlock == 0) ? i2c0 : i2c1, boardOptions.i2cSDAPin, boardOptions.i2cSCLPin, true, boardOptions.i2cSpeed);
	if(!mpr121_1->begin())
	{
		digitalWrite(25, 1);
		delete(mpr121_1);
		mpr121_1 = nullptr;
	}
	if (boardOptions.isTouch32Bit)
	{
		mpr121_2 = new Adafruit_MPR121(0x5B, (boardOptions.i2cBlock == 0) ? i2c0 : i2c1, boardOptions.i2cSDAPin, boardOptions.i2cSCLPin, true, boardOptions.i2cSpeed);
		if(!mpr121_2->begin())
		{
			digitalWrite(25, 1);
			delete(mpr121_2);
			mpr121_2 = nullptr;
		}

		mpr121_3 = new Adafruit_MPR121(0x5C, (boardOptions.i2cBlock == 0) ? i2c0 : i2c1, boardOptions.i2cSDAPin, boardOptions.i2cSCLPin, true, boardOptions.i2cSpeed);
		if(!mpr121_3->begin())
		{
			digitalWrite(25, 1);
			delete(mpr121_3);
			mpr121_3 = nullptr;
		}
	}

	hasLeftAnalogStick = true;
	hasRightAnalogStick = true;
}

void Gamepad::read()
{
	// Need to invert since we're using pullups
	uint32_t values = ~gpio_get_all();

	#ifdef PIN_SETTINGS
	state.aux = 0
		| ((values & (1 << PIN_SETTINGS)) ? (1 << 0) : 0)
	;
	#endif

	state.dpad = 0
		| ((values & mapDpadUp->pinMask)    ? (options.invertYAxis ? mapDpadDown->buttonMask : mapDpadUp->buttonMask) : 0)
		| ((values & mapDpadDown->pinMask)  ? (options.invertYAxis ? mapDpadUp->buttonMask : mapDpadDown->buttonMask) : 0)
		| ((values & mapDpadLeft->pinMask)  ? mapDpadLeft->buttonMask  : 0)
		| ((values & mapDpadRight->pinMask) ? mapDpadRight->buttonMask : 0)
	;

	state.buttons = 0
		| ((values & mapButtonB1->pinMask)  ? mapButtonB1->buttonMask  : 0)
		| ((values & mapButtonB2->pinMask)  ? mapButtonB2->buttonMask  : 0)
		| ((values & mapButtonB3->pinMask)  ? mapButtonB3->buttonMask  : 0)
		| ((values & mapButtonB4->pinMask)  ? mapButtonB4->buttonMask  : 0)
		| ((values & mapButtonL1->pinMask)  ? mapButtonL1->buttonMask  : 0)
		| ((values & mapButtonR1->pinMask)  ? mapButtonR1->buttonMask  : 0)
		| ((values & mapButtonL2->pinMask)  ? mapButtonL2->buttonMask  : 0)
		| ((values & mapButtonR2->pinMask)  ? mapButtonR2->buttonMask  : 0)
		| ((values & mapButtonS1->pinMask)  ? mapButtonS1->buttonMask  : 0)
		| ((values & mapButtonS2->pinMask)  ? mapButtonS2->buttonMask  : 0)
		| ((values & mapButtonL3->pinMask)  ? mapButtonL3->buttonMask  : 0)
		| ((values & mapButtonR3->pinMask)  ? mapButtonR3->buttonMask  : 0)
		| ((values & mapButtonA1->pinMask)  ? mapButtonA1->buttonMask  : 0)
		| ((values & mapButtonA2->pinMask)  ? mapButtonA2->buttonMask  : 0)
	;

	state.lx = GAMEPAD_JOYSTICK_MID;
	state.ly = GAMEPAD_JOYSTICK_MID;
	state.rx = GAMEPAD_JOYSTICK_MID;
	state.ry = GAMEPAD_JOYSTICK_MID;
	state.lt = 0;
	state.rt = 0;

	slideBar();
}

void Gamepad::slideBar()
{
	if (mpr121_1 == nullptr)
	{
		return;
	}
	currtouched = mpr121_1->touched();

	if (isTouch32Bit)
	{
		if (mpr121_2 == nullptr || mpr121_3 == nullptr)
		{
			return;
		}
		currtouched |= mpr121_2->touched() << 12;
		currtouched |= mpr121_3->touched() << 24;
	}

	makeTouchedPosition(currtouched, currTouchedPositionL, currTouchedPositionR);

  //左スティックの処理
	if (lastTouchedPositionL == NOT_TOUCHED && currTouchedPositionL == NOT_TOUCHED)
	{
		//離れている
		startTouchedPositionL = NOT_TOUCHED;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionL == NOT_TOUCHED && currTouchedPositionL != NOT_TOUCHED)
	{
		//触れたとき
    startTouchedPositionL = currTouchedPositionL;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionL != NOT_TOUCHED && currTouchedPositionL == NOT_TOUCHED)
	{
		//離れたとき
		startTouchedPositionL = NOT_TOUCHED;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionL != NOT_TOUCHED && currTouchedPositionL != NOT_TOUCHED)
	{
		//触れている途中
		int16_t dist = currTouchedPositionL - startTouchedPositionL;
		if (dist > 3)
		{
			dist = 3;
		}
		else if (dist < -3)
		{
			dist = -3;
		}
		state.lx = GAMEPAD_JOYSTICK_MID + dist * (GAMEPAD_JOYSTICK_MID / 3);
	}

	lastTouchedPositionL = currTouchedPositionL;

  //右スティックの処理
	if (lastTouchedPositionR == NOT_TOUCHED && currTouchedPositionR == NOT_TOUCHED)
	{
		//離れている
		startTouchedPositionR = NOT_TOUCHED;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionR == NOT_TOUCHED && currTouchedPositionR != NOT_TOUCHED)
	{
		//触れたとき
    startTouchedPositionR = currTouchedPositionR;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionR != NOT_TOUCHED && currTouchedPositionR == NOT_TOUCHED)
	{
		//離れたとき
		startTouchedPositionR = NOT_TOUCHED;
		state.lx = GAMEPAD_JOYSTICK_MID;
	}
	else if (lastTouchedPositionR != NOT_TOUCHED && currTouchedPositionR != NOT_TOUCHED)
	{
		//触れている途中
		int16_t dist = currTouchedPositionR - startTouchedPositionR;
		if (dist > 3)
		{
			dist = 3;
		}
		else if (dist < -3)
		{
			dist = -3;
		}
		state.lx = GAMEPAD_JOYSTICK_MID + dist * (GAMEPAD_JOYSTICK_MID / 3);
	}

	lastTouchedPositionR = currTouchedPositionR;

}

void Gamepad::makeTouchedPosition(uint32_t touched, int8_t &left, int8_t &right)
{
	//下位ビットから順に見て、ビットの立っている範囲を2箇所見つける
	//ビットの立っている範囲の端の位置を格納する。-1はビットが立っていないことを示す。
	int8_t bitEdge[] = {-1, -1, -1, -1};
	int8_t bitEdgeIndex = 0;

	//最初の1ビットを判定
	if (touched & 1)
	{
		bitEdge[bitEdgeIndex++] = 0;
	}

	for (int8_t bit = 1; bit < 33; bit++)
	{
		if (bitEdgeIndex > 3)
		{
			break;
		}

		//前のビットと比較して、異なっていたら、ビットが立っている範囲の端で有る
		bool prevBit = (touched & (1 << (bit - 1))) ? true : false;
		bool curBit;
		//32番目のビットは存在しないため、false。
		if (bit == 32)
		{
			curBit = false;
		}
		else
		{
			curBit = (touched & (1 << (bit))) ? true : false;
		}

		if (prevBit != curBit)
		{
			bitEdge[bitEdgeIndex++] = bit;
		}
	}

	int8_t tempRight = (bitEdge[0] + bitEdge[1]) / 2;
	int8_t tempLeft = (bitEdge[2] + bitEdge[3]) / 2;
	int8_t touchHalfPos = getTouchWidth() / 2;

	if (tempRight < 0 && tempLeft < 0)
	{
		left = NOT_TOUCHED;
		right = NOT_TOUCHED;
		return;
	}
	else if (tempRight >= 0 && tempLeft < 0)
	{
		if (left != NOT_TOUCHED)
		{
			left = tempRight;
			right = NOT_TOUCHED;
			return;
		}
		else if (right != NOT_TOUCHED)
		{
			left = NOT_TOUCHED;
			right = tempRight;
			return;
		}
		else
		{
			if (tempRight <= touchHalfPos)
			{
				right = tempRight;
			}
			else
			{
				left = tempRight;
			}
			return;
		}
	}
	else if (tempRight >= 0 || tempLeft >= 0)
	{

	}



}

int8_t Gamepad::getTouchWidth()
{
	if (isTouch32Bit)
	{
		return 32;
	}
	else
	{
		return 12;
	}
}
