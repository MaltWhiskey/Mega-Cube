![build status](https://github.com/atomic14/wio-terminal-audio-monitor/actions/workflows/build-on-push.yml/badge.svg)

# WIO Terminal Audio Monitor

[Explanatory video](https://youtu.be/p2Oxx9tI3jI)

[![Demo Video](https://img.youtube.com/vi/p2Oxx9tI3jI/0.jpg)](https://www.youtube.com/watch?v=p2Oxx9tI3jI)

This is a port of my audio monitor for the ESP32 (https://github.com/atomic14/esp32-tft-audio-monitor) to the WIO Terminal.

There's a couple of changes. The biggest change is that the samples are now collected using a timer instead of DMA transfers. It is possible to set up a DMA transfer to read from the Wio's microphone, but I wanted to keep this code as simple as possible.

# Building

You'll need VS Code and the PlatformIO plugin installed. It should just build and run.

Press the five-way button to toggle the different visualisations.
