#include "Color.h"
#include "Display.h"
#include <Math8.h>
#include <Math3D.h>


  float x1 = -2 , x2 = 1;
  float z1 = 1, z2 = 2;
  float phase = 0;
  uint16_t wheel = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("F_CPU_ACTUAL=");
  Serial.println(F_CPU_ACTUAL);

  configurePll();
  configureFlexIO();
  configureDma();
}

bool draw(float f);

void loop() {
  for(int y=0;y<16;y++)
    for (int x=0;x<16;x++)
      for(int z=0;z<16;z++)
        setLed(x,y,z,Color(0,0,0));

    draw(0.0025f);
    delayMicroseconds(10);

}  /*
  for(int y=0;y<16;y++) {
    for (int x=0;x<16;x++) {
      for(int z=0;z<16;z++) {
        for(int wheel = 0 ; wheel<256; wheel++) {
          Color c = Color(wheel, &RainbowGradientPalette[0]);
          setLed(x,y,z, c);
          delayMicroseconds(100);
        }
        setLed(x,y,z, Color(0,0,0));
      }
    }
  }
}
*/

bool draw(float dt) {
    phase += PI*dt;
    wheel -= 256*100*dt;
    for(uint8_t x=0;x < 16;x++) {
      float x3,y3,z3;
      x3 = mapf(x, 0, 16-1, x1, x2);
      for(uint8_t z=0;z < 16;z++) {
        z3 = mapf(z, 0, 16-1, z1, z2);
        y3 = sinf(phase + sqrtf(x3*x3 + z3*z3));
        Vector3 v = Vector3(x, mapf(y3, -1, 1, 0, 16-1), z);
        Color c = Color((wheel>>8) + v.y * 10, &RainbowGradientPalette[0]);
        setLed(v.x,v.y,v.z,c);
      }
    }
    return (phase/(12*PI) >= 1);
  };
