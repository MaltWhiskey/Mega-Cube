#include "Timer.h"
/*------------------------------------------------------------------------------
 * TIMER CLASS
 *----------------------------------------------------------------------------*/
Timer::Timer() { operator=(0); }
Timer::Timer(const float alarm) { operator=(alarm); }
Timer::Timer(const float alarm, const float ratio, const bool inversed) {
  operator=(alarm);
  if (inversed && ratio >= 0 && ratio <= 1) {
    m_startTime -= m_alarmTime * (1 - ratio);
  } else {
    m_startTime -= m_alarmTime * ratio;
  }
}
void Timer::operator=(const float alarm) {
  m_alarmCount = 0;
  m_alarmTime = alarm * 1000000.0f;
  m_currentTime = micros();
  m_startTime = m_currentTime;
  m_lastTime = m_currentTime;
}
void Timer::reset() {
  m_alarmCount = 0;
  m_currentTime = micros();
  m_startTime = m_currentTime;
  m_lastTime = m_currentTime;
}
void Timer::update_internals() {
  m_currentTime = micros();
  m_deltaTime = m_currentTime - m_lastTime;
  m_runTime = m_currentTime - m_startTime;
  m_lastTime = m_currentTime;
}
unsigned long Timer::update() {
  update_internals();
  if (m_alarmTime > 0) {
    unsigned long threshold = m_runTime / m_alarmTime;
    if (m_alarmCount < threshold) {
      m_alarmCount = threshold;
      return m_alarmCount;
    }
  }
  return 0;
}
float Timer::ratio() {
  update_internals();
  if (m_alarmTime > 0) {
    float threshold = (double)m_runTime / (double)m_alarmTime;
    return threshold;
  }
  return 0;
}
float Timer::dt() const { return m_deltaTime / 1000000.0f; }
float Timer::run_time() const { return m_runTime / 1000000.0f; }
float Timer::set_time() const { return m_alarmTime / 1000000.0f; }