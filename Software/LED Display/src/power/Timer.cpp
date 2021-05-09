#include "Timer.h"
/*------------------------------------------------------------------------------
 * TIMER CLASS
 *----------------------------------------------------------------------------*/
Timer::Timer() { operator=(0); }
Timer::Timer(const float alarm) { operator=(alarm); }
void Timer::operator=(const float alarm) {
  m_alarmCount = 0;
  m_startTime = 0;
  m_alarmTime = alarm;
  m_currentTime = micros();
  m_startTime = m_currentTime;
  m_lastTime = m_currentTime;
}
unsigned long Timer::update() {
  m_currentTime = micros();
  m_deltaTime = m_currentTime - m_lastTime;
  m_runTime = m_currentTime - m_startTime;
  m_lastTime = m_currentTime;
  if (m_alarmTime > 0) {
    unsigned long threshold = m_runTime / (m_alarmTime * 1000000.0f);
    if (m_alarmCount < threshold) {
      m_alarmCount = threshold;
      return m_alarmCount;
    }
  }
  return 0;
}
float Timer::dt() const { return m_deltaTime / 1000000.0f; }
float Timer::rt() const { return m_runTime / 1000000.0f; }
float Timer::percent() const {
  float threshold = m_runTime / (m_alarmTime * 1000000.0f);
  return threshold;
}