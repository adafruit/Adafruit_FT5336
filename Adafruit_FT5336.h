/*!
 * @file Adafruit_FT5336.h
 */

#ifndef ADAFRUIT_FT5336_LIBRARY
#define ADAFRUIT_FT5336_LIBRARY

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#define FT53XX_DEFAULT_ADDR 0x38    //!< I2C address
#define FT53XX_REG_VENDID 0xA3      //!< FocalTech's panel ID
#define FT53XX_REG_FIRMVERS 0xA6    //!< Firmware version
#define FT53XX_REG_CHIPID 0xA8      //!< Chip selecting

#define FT53XX_VENDID 0x11  //!< FocalTech's panel ID
#define FT5336_CHIPID 0x79  //!< Chip selecting

#define FT53XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT5336_REG_THRESHGROUP        0x80

#define FT5336_TD_STATUS   0x02
#define FT5336_GEST_ID     0x01
#define FT5336_TOUCH1_XH   0x03
#define FT5336_TOUCH1_XL   0x04
#define FT5336_TOUCH1_YH   0x05
#define FT5336_TOUCH1_YL   0x06

#define FT53XX_NUM_X 0x33 //!< Touch X position
#define FT53XX_NUM_Y 0x34 //!< Touch Y position

#define FT53XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT53XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT53XX_REG_WORKMODE 0x00    //!< Work mode
#define FT53XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT53XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT53XX_REG_POINTRATE 0x88   //!< Point rate


// calibrated for Adafruit 3.5" ctp screen
#define FT62XX_DEFAULT_THRESHOLD 128 //!< Default threshold for touch detection

/**************************************************************************/
/*!
    @brief  Helper class that stores a TouchScreen Point with x, y, and z
   coordinates, for easy math/comparison
*/
/**************************************************************************/
class TS_Point {
public:
  TS_Point(void);
  TS_Point(int16_t x, int16_t y, int16_t z);

  bool operator==(TS_Point);
  bool operator!=(TS_Point);

  int16_t x; /*!< X coordinate */
  int16_t y; /*!< Y coordinate */
  int16_t z; /*!< Z coordinate (often used for pressure) */
};

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with FT6206
   capacitive touch chips
*/
/**************************************************************************/
class Adafruit_FT5336 {
public:
  Adafruit_FT5336(void);
  bool begin(uint8_t i2c_addr = FT53XX_DEFAULT_ADDR, TwoWire *theWire = &Wire);
  uint8_t touched(void);
  TS_Point getPoint(uint8_t n = 0);
  void getPoints(TS_Point *points, uint8_t maxtouches);

  // void autoCalibrate(void);

private:
  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  void writeRegister8(uint8_t reg, uint8_t val);
  uint8_t readRegister8(uint8_t reg);

  void readData(void);
  uint8_t touches, gesture;
  uint16_t touchX[5], touchY[5], touchID[5];
};

#endif // ADAFRUIT_FT5336_LIBRARY
