#include "../lib/pwm_out.h"

/*
 * Init. BUzzer and Servomotor Pads
 * Starts PWM
 * Sets the Servo to 1000 (window closed)
 */
void pwm_init(void){
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(2));   //Buzzer - PC6 - PWM3 ch 1
  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(2));   //Servo - D15 - PWM4 ch 3
  pwmStart(&PWMD3, &pwmcfg_B);
  pwmStart(&PWMD4, &pwmcfg_S);
  pwmEnableChannel(&PWMD4, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 1000));
}

/*
 * Plays a note with the buzzer changing its PWM period at runtime
 * using 1000000 (pwm timer frequency) divided by the note frequency
 */
void play_note(int freq) {
  if (freq == 0) {
    stop_note();
  }
  else {
    pwmChangePeriod(&PWMD3, 1000000 / freq);
    pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 1000));
  }
}

/*
 * Disables the buzzer PWM channel in order to shut it
 */
void stop_note(void) {
  pwmDisableChannel(&PWMD3, 0);
}

/*
 * Sets the servo to 250 (open window position)
 */
void open_window(void){
  pwmEnableChannel(&PWMD4, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 250));
}

/*
 * Sets the servo to 1000 (closed window position)
 */
void close_window(void){
  pwmEnableChannel(&PWMD4, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 1000));
}

/*
 * terminates PWM
 */
void pwm_term(void){
  pwmDisableChannel(&PWMD3, 0);
  pwmDisableChannel(&PWMD4, 2);
  pwmStop(&PWMD3);
  pwmStop(&PWMD4);
}
