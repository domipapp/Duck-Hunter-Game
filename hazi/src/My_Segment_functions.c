#include "My_Segment_functions.h"

#include "segmentlcd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "My_Segmentlcd_help.h"

static const MCU_DISPLAY EFM_Display = EFM_DISPLAY_DEF;

int first_enter_difficulty_ring = true;//Induláskor 8. helyen van az ujj a visszaadó fgv szerint
int segment_to_light_prev = 0; //Alapértelmezett ring szegmens megjelenítés(0-ás nehézséget jelenít meg, 1 karika rész)
int difficulty = 0;//Alapértelmezett nehézség
int Num_Of_Ducks = 0;//Alapértelmezetten 0 kacsa jelent meg
int Ducks_Hit = 0;//Alapértelmezetten nem lőttünk még le kacsát
int duckPos = 0;//Alapértelmezetten a 0. helyen van a kacsa
int diffOffset = 0;//Alapértelmezetten nem forgott még a string, 0 offset

uint32_t start = 0;//Kacsagenergátor időszámlásásának kezdetekori értéke
uint32_t stop = 0;//Kacsagenergátor időszámlásásának végénél lévő érték
uint32_t diff_start = 0;//Difficulty kiírás időszámlásásának kezdetekori értéke
uint32_t diff_stop = 0;//Difficulty kiírás időszámlásásának végénél lévő érték

volatile uint32_t msTicks = 0;//Sys_tick időszámláló, nulláról indul
volatile bool difficultySet = false;//Indításkor még nincs nehézség beállítva

bool Diff_Time_Elapsed = true;//Difficulty kiírásakor a kívánt idő eltelt-e
bool Diff_Time_Start = true;////Difficulty kiírásakor első belépés-e a függvénybe
bool Time_Elapsed = true;
bool Start_Ducks = true;//Duck generátor első belépés-e
bool outOfDucks = false;//Elértük-e a maximum lelőhető kacsákat

const char *msg = "       dIFFICULTY";//"DIFFICULTY" szó a kijelzéshez


void SysTick_Handler(void){//Miliszekundumonként méghívódó interrupt függvény
  msTicks++;
}

void Delay(uint32_t dlyTicks){//dlyTicks miliszekundumig megállítja a program futását
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

void Write_Difficulty(void){//"DIFFICULTY" szót kiírja balra shiftelgetve
    char curr_msg[10];
    if(Diff_Time_Elapsed){
        diff_start = msTicks;
        diff_stop = msTicks;
        Diff_Time_Elapsed = false;
    	}
    if(Diff_Time_Start || ((diff_stop-diff_start) > 700)){
        if(diffOffset > 16 || Diff_Time_Start)
            diffOffset = 1;
        else
            diffOffset++;
        snprintf(curr_msg, 8, "%s", msg + diffOffset);
        SegmentLCD_Write(curr_msg);
        Diff_Time_Start = false;
        Diff_Time_Elapsed = true;
    }
    diff_stop = msTicks;
}



void shoot_duck(HUNTER_state hunterState){//A vadász jelenlegi helyéről kilő egy puskagolyót
	 LCD_SegmentSet(EFM_Display.Text[hunterState].com[12], EFM_Display.Text[hunterState].bit[12], true);
	 Delay(500);//Fél másodpercet vár mielőtt továbbhaladna
	 LCD_SegmentSet(EFM_Display.Text[hunterState].com[12], EFM_Display.Text[hunterState].bit[12], false);
	 LCD_SegmentSet(EFM_Display.Text[hunterState].com[8], EFM_Display.Text[hunterState].bit[8], true);
	 Delay(500);
	 check_if_hit();//Megnézzük eltaláltuk-e
	 LCD_SegmentSet(EFM_Display.Text[hunterState].com[8], EFM_Display.Text[hunterState].bit[8], false);
}

void Led_with_Touch(int sliderPos){//A vadászt az érintésérzékelőnek megfelelően mozgatja

    if (sliderPos == -1){//Ha nem ér senki az érzékelőhöz, egyhelyben marad a vadász
        LCD_SegmentSet(EFM_Display.Text[hunterState].com[3], EFM_Display.Text[hunterState].bit[3], true);
        return;
    }

    int touchPos = sliderPos == 48 ? 3 :(sliderPos*4) / 48;//48. helyen is lehet az érzékelő, akkor az utolsó, 3. részben vagyunk, egyébként a standard algoritmus

    LCD_SegmentSet(EFM_Display.Text[touchPos].com[3], EFM_Display.Text[touchPos].bit[3], true);//Kekapcsoljuk az új helyét a vadásznak
    for(int i = 0; i < 4; i ++)//Töröljuk a vadász régi helyét, bárhol lehetett
        if(i != touchPos)
            LCD_SegmentSet(EFM_Display.Text[i].com[3], EFM_Display.Text[i].bit[3], false);
    hunterState = touchPos;//Jelenlegi helyét beállítjuk a vadásznak
}

void duck_generator(void){
	if(Time_Elapsed){
		start = msTicks;
		stop = msTicks;
		Time_Elapsed = false;
	}
	if(Start_Ducks || ((stop-start) > 2200-(difficulty*225))){//Ha első belépés, vagy eltelt bizonyos idő, belépünk
		int com = (rand() % 4);//Random szám 0 és 3 között
    	SegmentLCD_Write("     ");//Letöröljuk a 7 szegment
    	LCD_SegmentSet(EFM_Display.Text[com].com[0], EFM_Display.Text[com].bit[0], true);//Kacsa elhelyezése a véletlenszerű helyre
		duckPos = com;//Kacsa jelenlegi helyét beállítjuk
		Start_Ducks = false;//Megtörtént az első belépés
		Time_Elapsed = true;
		if (Num_Of_Ducks < 25){//Növeljük a kijelzett kacsák számát
			Num_Of_Ducks++;

		}
		else{
			outOfDucks = true;//Ha elértük a 25 kacsát
		}
    	Display_Standing();//Kiírja az új számokat
	}
	stop = msTicks;

}


void Difficulty_ring(int sliderPos){
    //48 érték az érzékelőn, az 8 részre osztva, 6-osával
    //Ha a nulladik helyen van(alapértelmezett), a nehézség 1
    int segment_to_light = (sliderPos == - 1 || first_enter_difficulty_ring ) ? segment_to_light_prev : (sliderPos * 8) / 48 ;
    for(int i = 0 ; i < 8; i++)//Ring szegmensek 0-7-ig számozva
        if(i <= segment_to_light)//Bekapcsolja a jelenlegi nehézségnek megfelelő szegmensszámot
            SegmentLCD_ARing(i, 1);
        else
            SegmentLCD_ARing(i, 0);//Törli az előző szegmenseket amik most nem kellenek

    difficulty = segment_to_light; //Beállítja a nehézséget
    segment_to_light_prev = segment_to_light; //Elmentjük az előző állapotot
    first_enter_difficulty_ring = false; //Első belépés után false
}


void check_if_hit(void){//Megnézi, hogy a vadász lelőtte-e a kacsát
    if (hunterState  == duckPos){//Ha egy helyen vannak lelőtte, mert ez a fgv lövéskor hívódik meg
    		//Villogtatja a kacsát fel másodpercenként, le lett lőve
            LCD_SegmentSet(EFM_Display.Text[hunterState].com[0], EFM_Display.Text[hunterState].bit[0], false);
            Ducks_Hit++;//Egyel több kacsát lőttünk le
            Display_Standing();//Kiírja az új számokat
            Delay(500);
            LCD_SegmentSet(EFM_Display.Text[hunterState].com[0], EFM_Display.Text[hunterState].bit[0], true);
            Delay(500);
            LCD_SegmentSet(EFM_Display.Text[hunterState].com[0], EFM_Display.Text[hunterState].bit[0], false);
    }
}

void Print_Percentage_Sign(void){//Az utolsó szegmensre kiír egy % jelet
    LCD_SegmentSet(EFM_Display.Text[6].com[6], EFM_Display.Text[6].bit[6],true);
    LCD_SegmentSet(EFM_Display.Text[6].com[9], EFM_Display.Text[6].bit[9],true);
    LCD_SegmentSet(EFM_Display.Text[6].com[10], EFM_Display.Text[6].bit[10],true);
    LCD_SegmentSet(EFM_Display.Text[6].com[13], EFM_Display.Text[6].bit[13],true);
}

void Game_Over(void){//Vége a játéknak
	int result = (100*Ducks_Hit / Num_Of_Ducks);//Arányszám, hogy százalékosan hány kacsát lőttünk le
	char msg_PERCENTAGE[2]; //Kiirandó sztring készítése
    char *msg_OVER = "ENd ";
    char msg_FINAL[sizeof(msg_PERCENTAGE) + sizeof(msg_OVER)];
    sprintf(msg_PERCENTAGE,"%d",result);
   	strcpy(msg_FINAL,msg_OVER);
   	if(result < 10){//Ha a százalék kisebb mint 10 akkor jobbra igazítjuk a százalékos számot
   		strcat(msg_FINAL," ");
   	}
   	strcat(msg_FINAL,msg_PERCENTAGE);
   	if( result == 100){//Ha minden kacsát lelőttünk, nyertünk
   		SegmentLCD_Write("WIN");
   	}
   	else{//Ha nem lőttünk le mindent, kiírjuk hogy "END xy%"
   		SegmentLCD_Write(msg_FINAL);
   		Print_Percentage_Sign();
   	}
	Delay(5000);//5 mp után kikapcsoljuk a kijelzőt
	SegmentLCD_AllOff();
	//Alaphelyzetbe állítunk mindent
	Num_Of_Ducks = 0;
	Ducks_Hit = 0;
	outOfDucks = false;
	difficultySet = false;
	Diff_Time_Start = true;
}

void Display_Standing(void){//Kiírja eddig hány kacsából hányat találtunk el
	LCD_SegmentSet(5, 39, true);// ":" Beállítása a kijelzőn
	SegmentLCD_Number(Num_Of_Ducks * 100 + Ducks_Hit);
	if(outOfDucks){//Ha kijeleztük az összes kacsát vége a játéknak
		Game_Over();
	}
}
