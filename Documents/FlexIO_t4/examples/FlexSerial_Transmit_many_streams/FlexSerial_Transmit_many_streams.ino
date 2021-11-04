//============================================================================
// Test to create multiple Flex Serial output objects and see how they
// all interact...
//============================================================================

#include <FlexIO_t4.h>
#include <FlexSerial.h>

FlexSerial FlexSerials[] = {
  { -1, 2, -1, 0, 0, 0, 0, 0},
  { -1, 3, -1, 0, 0, 0, 1, 1},
  { -1, 4, -1, 0, 0, 0, 2, 2},
  { -1, 5, -1, 0, 0, 0, 3, 3},
  { -1, 6},
  { -1, 18},
  { -1, 19}
};

const uint8_t count_flexSerials = sizeof(FlexSerials) / sizeof(FlexSerials[0]);

Stream *streams[7 + count_flexSerials] = {&Serial1, &Serial2, &Serial3, &Serial4, &Serial5, &Serial6, &Serial7};
const uint8_t count_streams = sizeof(streams) / sizeof(streams[0]);

#define BUFFER_SIZE 75
#define BAUD 115200
uint8_t buffer[BUFFER_SIZE];
uint8_t loop_count = 0;

void outputStr(Stream *pstream, const char *str_buf, uint8_t index) {

  digitalWriteFast(13, HIGH);
  pstream->write(str_buf);
  pstream->print(index,DEC);
  //  pstream->flush();
  digitalWriteFast(13, LOW);

}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  while (!Serial && millis() < 4000);
  Serial.begin(115200);
  delay(250);

  // Lets initialize all of the Hardware Serial ports.
  Serial1.begin(BAUD); outputStr(&Serial1, "Serial",1);// 0, 1
  Serial2.begin(BAUD); outputStr(&Serial2, "Serial", 2);// 7, 8
  Serial3.begin(BAUD); outputStr(&Serial3, "Serial", 3);// 15, 14
  Serial4.begin(BAUD); outputStr(&Serial4, "Serial", 4);// 16, 17
  Serial5.begin(BAUD); outputStr(&Serial5, "Serial", 5);// 21, 20
  Serial6.begin(BAUD); outputStr(&Serial6, "Serial", 6);// 25, 24
  Serial7.begin(BAUD); outputStr(&Serial7, "Serial", 7);// 28, 29
  for (uint8_t i = 0; i < count_flexSerials; i++) {
    Serial.println("\nFlexSerial object begin");
    if (!FlexSerials[i].begin(BAUD)) Serial.println("   *** Failed ***\n"); 
    streams[7+i] = &FlexSerials[i];
    outputStr(&FlexSerials[i],"FlexSerial", i);
  }
  delay(500);

  Serial.println("End Setup");
}

uint8_t loop_char = 'a';
void loop() {
  loop_count++;
  // maybe first 10 bytes will bee loop count;
  memset(buffer, loop_count, 10);
  for (uint16_t bi = 10; bi < sizeof(buffer); bi++) buffer[bi] = bi & 0xff;
  digitalWriteFast(13, HIGH);
  uint16_t buffer_indexes[count_streams] = {0};
  uint32_t start_time = millis();
  bool not_done_yet = true;
  while (not_done_yet) {
    digitalWriteFast(12, HIGH);
    not_done_yet = false; // assume we are done.
    for (uint8_t serial_index = 0; serial_index < count_streams; serial_index++) {
      if (buffer_indexes[serial_index] < sizeof(buffer)) {
        uint16_t cbOutput = sizeof(buffer) - buffer_indexes[serial_index];
        uint16_t cbAvailForWrite = streams[serial_index]->availableForWrite();
        if (cbAvailForWrite < cbOutput) {
          cbOutput = cbAvailForWrite;
          not_done_yet = true;
        }
        streams[serial_index]->write(&buffer[buffer_indexes[serial_index]], cbOutput);
        buffer_indexes[serial_index] += cbOutput;
      }
    }
    //delayMicroseconds(10);
    digitalWriteFast(12, LOW);
  }
  // Now lets wait until all of them finish their output
  for (uint8_t serial_index = 0; serial_index < count_streams; serial_index++) {
    streams[serial_index]->flush();
  }
  digitalWriteFast(13, LOW);
  Serial.printf("loop %u time: %u\n", loop_count, millis() - start_time);
  delay(250);
  Serial.printf("end F1:%x %x F2:%x %x F3: %x %x\n", FLEXIO1_SHIFTSTAT, FLEXIO1_SHIFTSIEN,
                FLEXIO2_SHIFTSTAT, FLEXIO2_SHIFTSIEN, FLEXIO3_SHIFTSTAT, FLEXIO3_SHIFTSIEN );
  Serial.printf("CCM_CDCDR: %x\n", CCM_CDCDR);
  Serial.printf("VERID:%x PARAM:%x CTRL:%x PIN: %x\n", IMXRT_FLEXIO1_S.VERID, IMXRT_FLEXIO1_S.PARAM, IMXRT_FLEXIO1_S.CTRL, IMXRT_FLEXIO1_S.PIN);
  Serial.printf("SHIFTSTAT:%x SHIFTERR=%x TIMSTAT=%x\n", IMXRT_FLEXIO1_S.SHIFTSTAT, IMXRT_FLEXIO1_S.SHIFTERR, IMXRT_FLEXIO1_S.TIMSTAT);
  Serial.printf("SHIFTSIEN:%x SHIFTEIEN=%x TIMIEN=%x\n", IMXRT_FLEXIO1_S.SHIFTSIEN, IMXRT_FLEXIO1_S.SHIFTEIEN, IMXRT_FLEXIO1_S.TIMIEN);
  Serial.printf("SHIFTSDEN:%x SHIFTSTATE=%x\n", IMXRT_FLEXIO1_S.SHIFTSDEN, IMXRT_FLEXIO1_S.SHIFTSTATE);
  Serial.printf("SHIFTCTL:%x %x %x %x\n", IMXRT_FLEXIO1_S.SHIFTCTL[0], IMXRT_FLEXIO1_S.SHIFTCTL[1], IMXRT_FLEXIO1_S.SHIFTCTL[2], IMXRT_FLEXIO1_S.SHIFTCTL[3]);
  Serial.printf("SHIFTCFG:%x %x %x %x\n", IMXRT_FLEXIO1_S.SHIFTCFG[0], IMXRT_FLEXIO1_S.SHIFTCFG[1], IMXRT_FLEXIO1_S.SHIFTCFG[2], IMXRT_FLEXIO1_S.SHIFTCFG[3]);
  Serial.printf("TIMCTL:%x %x %x %x\n", IMXRT_FLEXIO1_S.TIMCTL[0], IMXRT_FLEXIO1_S.TIMCTL[1], IMXRT_FLEXIO1_S.TIMCTL[2], IMXRT_FLEXIO1_S.TIMCTL[3]);
  Serial.printf("TIMCFG:%x %x %x %x\n", IMXRT_FLEXIO1_S.TIMCFG[0], IMXRT_FLEXIO1_S.TIMCFG[1], IMXRT_FLEXIO1_S.TIMCFG[2], IMXRT_FLEXIO1_S.TIMCFG[3]);
  Serial.printf("TIMCMP:%x %x %x %x\n", IMXRT_FLEXIO1_S.TIMCMP[0], IMXRT_FLEXIO1_S.TIMCMP[1], IMXRT_FLEXIO1_S.TIMCMP[2], IMXRT_FLEXIO1_S.TIMCMP[3]);


  while (Serial.read() == -1) ;
  while (Serial.read() != -1) ;
  delay(1000);
}
