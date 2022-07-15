Teensy footprint library for KiCAD
=======================================

This repo includes KiCAD footprints for the following Teensy versions:

  - Teensy 1.0
  - Teensy++ 1.0
  - Teensy 2.0
  - Teensy++ 2.0
  - Teensy 3.0
  - Teensy 3.1
  - Teensy 3.2
  - Teensy LC
  - Teensy 3.5 (Includes 3D Model by Darcy)
  - Teensy 3.6 (Includes 3D Model by Darcy)
  - Teensy 4.0 (Includes 3D Model)
  - Teensy 4.0 SMT (Includes 3D Model)
  - Teensy 4.1 (Includes 3D model by Zack Kummer)

Notes:
  - The Teensy 4.0 SMT footprint requires the addition of edge cuts (indicated
by lines in the Dwgs.User layer) which create castellated connections for most
pins.  Pins 2, 3, 31, 32 & 34 are not castellated and will require careful soldering
and probably a wire to ensure a reliable connection.

For symbols look here: https://github.com/XenGi/teensy_library
