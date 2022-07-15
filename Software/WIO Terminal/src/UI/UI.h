#pragma once

class Palette;
class Waveform;
class GraphicEqualiser;
//class Spectrogram;
class Display;

class UI
{
private:
  Palette *m_palette;
  Waveform *m_waveform;
  GraphicEqualiser *m_graphic_equaliser;
//  Spectrogram *m_spectrogram;
  Display &m_display;

public:
  UI(Display &display, int window_size);
  void toggle_display();
  void update(int *samples, int *fft);
  void draw();
};
