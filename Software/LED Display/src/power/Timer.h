#ifndef TIMER_H
#define TIMER_H
#include <Arduino.h>
#include <stdint.h>
/*------------------------------------------------------------------------------
 * TIMER CLASS
 *------------------------------------------------------------------------------
 * The Timer class is used to do timing specific actions. All time is in seconds
 *
 * Sets a timer for 0.10 seconds:
 * Timer t = 0.10f;
 * Timer t = Timer(0.10f);
 *
 * t.update();
 * Returns an integer of the times the timer has counted 0.10 seconds but only
 * if the next alarm threshold has passed since last call to update, otherwise
 * returns zero.
 *----------------------------------------------------------------------------*/
class Timer {
 public:
  Timer();
  Timer(const float alarm);
  Timer(const float alarm, const float ratio, const bool inversed);
  void operator=(const float alarm);
  // returns > 0 if set time has passed
  unsigned long update();
  // ratio of run time over set time
  float ratio();
  // reset the clock and start counting
  void reset();
  // delta time, run time and set time are in seconds
  float dt() const;
  float run_time() const;
  float set_time() const;

 private:
  void update_internals();
  // alarm time in microseconds
  unsigned long m_alarmTime = 0;
  // amount of times counted to alarm
  unsigned long m_alarmCount = 0;
  // time management in microseconds
  unsigned long m_startTime = 0;
  unsigned long m_lastTime = 0;
  unsigned long m_currentTime = 0;
  unsigned long m_deltaTime = 0;
  unsigned long m_runTime = 0;
};
#endif