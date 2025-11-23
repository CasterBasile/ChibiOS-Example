#ifndef DATA_H
#define DATA_H

#define RX_MEX_DIM 4
#define WATER_TH 1000
#define HUM_TH 40
#define TEMP_TH 50

//BaseSequentialStream * chpSERI = (BaseSequentialStream *) &SD2;
//BaseSequentialStream * chpWIFI = (BaseSequentialStream *) &SD1;

/*
 * Sensor Data Struct
 */
typedef struct {
  int lightVal;
  int waterVal;
  int tempVal;
  int humVal;
  uint8_t pirOn;
  uint8_t vibOn;
  uint8_t butOn;
} tx_data_t;

/*
 * User WebGui Commands Data Struct
 */
typedef struct {
  uint8_t ledOn;
  uint8_t windowOn;
  uint8_t alarmOn;
  uint8_t antiTheftOn;
}rx_data_t;




#endif
