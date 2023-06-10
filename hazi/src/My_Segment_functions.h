#include "stdint.h"

//A vadász jelenlegi pozíciója a kijelzőn. 0 = A legbaloldali szegmens
typedef enum{
	LEFT = 0,
	SECOND_FROM_LEFT = 1,
	SECOND_FROM_RIGHT = 2,
	RIGHT = 3
} HUNTER_state;
extern HUNTER_state hunterState;



void Led_with_Touch(int sliderPos);
void check_if_hit(void);
void duck_generator(void);
void Difficulty_ring(int sliderPos);
void Print_Percentage_Sign(void);
void Game_Over(void);
void Display_Standing(void);
void shoot_duck(HUNTER_state hunterState);
void Delay(uint32_t dlyTicks);
void Write_Difficulty(void);
