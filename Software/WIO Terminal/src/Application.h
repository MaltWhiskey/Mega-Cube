#pragma once

class UI;
class Processor;
class Sampler;
class Display;
class Serializer;
class TCP;

class Application {
 private:
  int m_window_size;
  UI *m_ui;
  Processor *m_processor;
  Sampler *m_sampler;
  Serializer *m_serializer;
  TCP *m_tcp;

 public:
  Application(Display &display);
  void begin();
  void loop();
};