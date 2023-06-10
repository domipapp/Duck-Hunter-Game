#include <stdlib.h>
#include <time.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "segmentlcd.h"
#include "caplesense.h"
#include "My_Segment_functions.h"

//Állapotok a játékhoz
typedef enum{
  SLEEP = 0,
  PREPARE = 1,
  RUNNING = 2
} Game_State;

HUNTER_state hunterState; //Vadász helyét mentjük el vele
extern bool difficultySet; //Van-e nehézségi szint beállítva
volatile bool shoot; //Épp lövünk-e
static volatile Game_State gameState = SLEEP; //Érintésérzékelő alapértelmezetten sleep

void GPIO_ODD_IRQHandler(void){ //PB0 interrupt kezelő
  /* Interrupt elfogadása */
  GPIO_IntClear(1 << 9);
  shoot = true;//PB0-val lövünk

}

void GPIO_EVEN_IRQHandler(void){ //PB1 interrupt kezelő
  /* Interrupt elfogadása */
  GPIO_IntClear(1 << 10);
  difficultySet = true;//PB1 benyomásával választjuk ki a nehézségi szintet
}

void gpioSetup(void){
  /* GPIO órajel engedélyezés */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* PB0 és PB1 gombok beállítása */
  GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);

  /* PB0 és PB1 benyomásra kér interruptot */
  GPIO_IntConfig(gpioPortB, 9, false, true, true);
  GPIO_IntConfig(gpioPortB, 10, false, true, true);

  /* PB0 és PB1 interrupt engedélyezés */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void capSenseScanComplete(void){//Érintésérzékelő függvény
  ;
}

void capSenseChTrigger(void){//Érintésérzékelő függvény
	gameState = PREPARE;
}


void Lsense_duck (void){//Játékot vezérlő függvény
      int32_t slider;//Ujj pozícióját tárolja az érintésérzékelőn
      CAPLESENSE_setupCallbacks(&capSenseScanComplete, &capSenseChTrigger);//Érintésérzékelő interruptok beállítása

      while (1){
    	  switch (gameState){
    	  	  case SLEEP://Alvó állapot, az érintésérzékelő alvó állapotban. Bekpacsolás után itt vagyunk
    	  		  CAPLESENSE_Sleep();
    	  		  break;

    	  	  case PREPARE:{//Érrzékelés történt
    	  		  CAPLESENSE_setupLESENSE(false);//Érintésérzékelő felébresztése
    	  		  SegmentLCD_Init(false);//LCD inicializálása feszboost nélkül
    	  		  gameState = RUNNING;//Mostmár fut a játék
    	  	  }
          	  	  break;

    	  	  case RUNNING:{//A játék aktív, fut
    	  		  CAPLESENSE_Sleep();//Ki fogjuk olvasni az értéket, addig ne változzon
    	  		  slider = CAPLESENSE_getSliderPosition();//Lekérdezzuk az ujj helyét

    	  		  if(!difficultySet){//Ha nincs beállítva nehézség, beállíttatjuk
    	  			  Difficulty_ring(slider);//Az ujj pozíciójának megfelelően kirajzoltatjuk a gyűrűre a nehézséget
    	  			  Write_Difficulty();//Kiírjuk, hogy "DIFFICULTY" balra shiftelve a kijelzzőn
    	  		  }
    	  		  else{//Nehézség be van állítva
    	  			  duck_generator();//Véletlenszerűen generáltatunk egy kacsát a kijelzőre
    	  			  Led_with_Touch(slider);//Az ujj pozíciójának megfelelően elhelyezzük a vadászt
    	  			  if(shoot)//Ha lőttünk, PB0 interruptja igazra állítja
    	  				  shoot_duck(hunterState);//Lövünk a kacsa irányába
    	  			  shoot = false;//Minden esetben mostmár nem lövünk
    	  		  }
    	  	  }
    	  	  	  break;

    	  }
      }
}

int main(void){
	/* Inincializálások */
  CHIP_Init();
  srand(time(NULL)); //A random szám generáláshoz biztosítja a seedet
  CAPLESENSE_Init(true);
  gpioSetup();
  SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000);
  Lsense_duck();//Játék futtatása
}
