#include "Application.h"

#include <Arduino.h>

#include "AudioProcessing/Processor.h"
#include "Display.h"
#include "Sampler.h"
#include "Serializer.h"
#include "TCP.h"
#include "UI/UI.h"
#include "config.h"

Application::Application(Display &display) {
  m_window_size = WINDOW_SIZE;
  m_ui = new UI(display, m_window_size);
  m_processor = new Processor(m_window_size);
  m_sampler = new Sampler(SAMPLE_RATE, m_window_size);
  m_tcp = new TCP();
  m_serializer = new Serializer(m_window_size, m_tcp);

  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}

void Application::begin() {
  // start sampling from the microphone
  m_sampler->start();
}

unsigned long process_time = 0;
int process_count = 0;
int last_push = 0;

void Application::loop() {
  auto start = millis();
  // run the fft
  m_processor->update(m_sampler->get_buffer());
  auto end = millis();
  process_time += end - start;
  process_count++;
  if (process_count == 20) {
    Serial.printf("Processing time %ld\n", process_time / 20);
    process_count = 0;
    process_time = 0;
  }
  // m_ui->update(m_processor->m_fft_input, m_processor->m_energy);
  m_serializer->update(m_processor->m_energy);
  //  change display on button push
  if (digitalRead(WIO_5S_PRESS) == LOW && millis() - last_push > 500) {
    last_push = millis();
    // m_ui->toggle_display();
  }
  m_tcp->loop();
}