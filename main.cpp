#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "canboard_config.h"
#include "can.h"

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  InitCan();
  //Init digital inputs
  //Init analog inputs
  //Init digital outputs
  
  while (true) {
    //Read digital inputs

    //Read analog inputs

    //Set digital outputs
    
    chThdSleepMilliseconds(500);
  }
}
