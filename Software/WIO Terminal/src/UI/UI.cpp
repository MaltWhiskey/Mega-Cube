#include "UI.h"

#include <Arduino.h>

#include "../Display.h"
#include "UI/GraphicEqualiser.h"
#include "UI/Palette.h"
#include "UI/Spectrogram.h"
#include "UI/Waveform.h"

UI::UI(Display &display, int window_size) : m_display(display) {
  Serial.printf("Display is %d x %d\n", display.width(), display.height());
  m_palette = new Palette();
  m_waveform = new Waveform(display, 0, 0, display.width(), display.height(),
                            window_size);

  m_graphic_equaliser = new GraphicEqualiser(m_palette, 0, 0, display.width(),
                                             display.height(), window_size);

  // Not enough memory for this
  //  m_spectrogram =      new Spectrogram(m_palette, 0, 0, display.width(),
  //  display.height());

  // start off with the graphic equaliser visible
  m_waveform->visible = false;
  // m_spectrogram->visible = false;
  m_graphic_equaliser->visible = true;
}

void UI::toggle_display() {
  // bool tmp = m_graphic_equaliser->visible;
  // m_graphic_equaliser->visible = m_spectrogram->visible;
  // m_spectrogram->visible = m_waveform->visible;
  // m_waveform->visible = tmp;
  m_waveform->visible = !m_waveform->visible;
  m_graphic_equaliser->visible = !m_graphic_equaliser->visible;
}

void UI::update(int *samples, int *fft) {
  m_waveform->update(samples);
  m_graphic_equaliser->update(fft);
  // m_spectrogram->update(fft);
  draw();
}

unsigned long draw_time = 0;
int draw_count = 0;
void UI::draw() {
  auto start = millis();
  // m_spectrogram->draw(m_display);
  m_graphic_equaliser->draw(m_display);
  m_waveform->draw(m_display);
  auto end = millis();
  draw_time += end - start;
  draw_count++;
  if (draw_count == 20) {
    Serial.printf("Drawing time %ld\n", draw_time / 20);
    draw_count = 0;
    draw_time = 0;
  }
}