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
 * Returns an integer of the times the timer has counted 0.10 seconds but only
 *if the next alarm threshold has passed since last call to update, otherwise
 *returns zero. t.update();
 *----------------------------------------------------------------------------*/
class Timer {
 public:
  Timer();
  Timer(const float alarm);
  void operator=(const float alarm);
  unsigned long update();
  float dt() const;
  float rt() const;
  float percent() const;

 private:
  // alarm time in seconds
  float m_alarmTime = 0;
  // amount of times counted to alarm
  unsigned long m_alarmCount = 0;
  // time management
  unsigned long m_startTime = 0;
  unsigned long m_lastTime = 0;
  unsigned long m_currentTime = 0;
  unsigned long m_deltaTime = 0;
  unsigned long m_runTime = 0;
};
#endif