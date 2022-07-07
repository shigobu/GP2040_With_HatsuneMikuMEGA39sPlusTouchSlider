#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include "BoardConfig.h"
#include <string.h>
#include <MPGS.h>
#include "pico/stdlib.h"
#include "storage.h"
#include "Adafruit_MPR121.h"

#define GAMEPAD_FEATURE_REPORT_SIZE 32
#define NOT_TOUCHED -1

struct GamepadButtonMapping
{
	GamepadButtonMapping(uint8_t p, uint16_t bm) : pin(p), pinMask((1 << p)), buttonMask(bm) {}

	uint8_t pin;
	uint32_t pinMask;
	const uint16_t buttonMask;

	inline void setPin(uint8_t p)
	{
		pin = p;
		pinMask = 1 << p;
	}
};

class Gamepad : public MPGS
{
public:
	Gamepad(int debounceMS = 5, GamepadStorage *storage = &GamepadStore)
			: MPGS(debounceMS, storage) {}

	void setup();
	void read();
	void slideBar();
	void makeTouchedPosition(uint32_t touched, int8_t &left, int8_t &right);
	int8_t getTouchWidth();

	void process()
	{
		memcpy(&rawState, &state, sizeof(GamepadState));
		MPGS::process();
	}

	inline bool __attribute__((always_inline)) pressedF1()
	{
#ifdef PIN_SETTINGS
		return state.aux & (1 << 0);
#else
		return MPGS::pressedF1();
#endif
	}

	GamepadState rawState;

	GamepadButtonMapping *mapDpadUp;
	GamepadButtonMapping *mapDpadDown;
	GamepadButtonMapping *mapDpadLeft;
	GamepadButtonMapping *mapDpadRight;
	GamepadButtonMapping *mapButtonB1;
	GamepadButtonMapping *mapButtonB2;
	GamepadButtonMapping *mapButtonB3;
	GamepadButtonMapping *mapButtonB4;
	GamepadButtonMapping *mapButtonL1;
	GamepadButtonMapping *mapButtonR1;
	GamepadButtonMapping *mapButtonL2;
	GamepadButtonMapping *mapButtonR2;
	GamepadButtonMapping *mapButtonS1;
	GamepadButtonMapping *mapButtonS2;
	GamepadButtonMapping *mapButtonL3;
	GamepadButtonMapping *mapButtonR3;
	GamepadButtonMapping *mapButtonA1;
	GamepadButtonMapping *mapButtonA2;

	GamepadButtonMapping **gamepadMappings;

	Adafruit_MPR121 *mpr121_1 = nullptr, *mpr121_2 = nullptr, *mpr121_3 = nullptr;
	bool isTouch32Bit = false;
	uint32_t currtouched = 0;
	int8_t startTouchedPositionL = -1;
	int8_t startTouchedPositionR = -1;
	int8_t currTouchedPositionL = -1;
	int8_t currTouchedPositionR = -1;
	int8_t lastTouchedPositionL = -1;
	int8_t lastTouchedPositionR = -1;
};

enum positionFoundState
{
	notFound,
	min1,
	max1,
	min2,
	max2
};

#endif
