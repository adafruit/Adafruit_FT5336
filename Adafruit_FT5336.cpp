/*!
 * @file Adafruit_FT5336.cpp
 *
 * @mainpage Adafruit FT5336 Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the Adafruit Capacitive Touch Screens
 *
 * ----> http://www.adafruit.com/products/1947
 *
 * Check out the links above for our tutorials and wiring diagrams
 * This chipset uses I2C to communicate
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License

 * MIT license, all text above must be included in any redistribution
 */

#include <Adafruit_FT5336.h>

//#define FT5336_DEBUG

/**************************************************************************/
/*!
    @brief  Instantiates a new FT5336 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
Adafruit_FT5336::Adafruit_FT5336() { touches = 0; }

/**************************************************************************/
/*!
    @brief  Setups the I2C interface and hardware, identifies if chip is found
    @param theWire Which I2C bus to use, defaults to &Wire
    @returns True if an FT5336 is found, false on any failure
*/
/**************************************************************************/
bool Adafruit_FT5336::begin(uint8_t i2c_addr, TwoWire *theWire) {
  if (i2c_dev)
    delete i2c_dev;
  i2c_dev = new Adafruit_I2CDevice(i2c_addr, theWire);
  if (!i2c_dev->begin())
    return false;

#ifdef FT5336_DEBUG
  Serial.print("Vend ID: 0x");
  Serial.println(readRegister8(FT53XX_REG_VENDID), HEX);
  Serial.print("Chip ID: 0x");
  Serial.println(readRegister8(FT53XX_REG_CHIPID), HEX);
  Serial.print("Firm V: ");
  Serial.println(readRegister8(FT53XX_REG_FIRMVERS));
  Serial.print("Thresh: ");
  Serial.println(readRegister8(FT5336_REG_THRESHGROUP));

  // dump all registers
  for (int16_t i = 0; i < 0x10; i++) {
    Serial.print("I2C $");
    Serial.print(i, HEX);
    Serial.print(" = 0x");
    Serial.println(readRegister8(i), HEX);
  }
#endif

  // change threshhold to be higher/lower
  //writeRegister8(FT53XX_REG_THRESHHOLD, thresh);

  if (readRegister8(FT53XX_REG_VENDID) != FT53XX_VENDID) {
    return false;
  }
  uint8_t id = readRegister8(FT53XX_REG_CHIPID);
  if (id != FT5336_CHIPID) {
    return false;
  }

  return true;
}

/**************************************************************************/
/*!
    @brief  Determines if there are any touches detected
    @returns Number of touches detected, can be 0 or 1-5 for multitouch
*/
/**************************************************************************/
uint8_t Adafruit_FT5336::touched(void) {
  uint8_t n = readRegister8(FT53XX_REG_NUMTOUCHES);
  if (n > 5) {
    n = 0;
  }
  return n;
}

/**************************************************************************/
/*!
    @brief  Queries the chip and retrieves a point data
    @param  n The # index (0 or 1) to the points we can detect. In theory we can
   detect 2 points but we've found that you should only use this for
   single-touch since the two points cant share the same half of the screen.
    @returns {@link TS_Point} object that has the x and y coordinets set. If the
   z coordinate is 0 it means the point is not touched. If z is 1, it is
   currently touched.
*/
/**************************************************************************/
TS_Point Adafruit_FT5336::getPoint(uint8_t n) {
  readData();
  if ((touches == 0) || (n > 1)) {
    return TS_Point(0, 0, 0);
  } else {
    return TS_Point(touchX[n], touchY[n], 1);
  }
}


void Adafruit_FT5336::getPoints(TS_Point *points, uint8_t maxtouches) {
  readData();
  
  // zero out all the points
  for (uint8_t i=0; i<maxtouches; i++) {
    points[i].x = 0;
    points[i].y = 0;
    points[i].z = 0;
  }

  // identify all points and assign
  for (uint8_t i=0; i<touches; i++) {
    uint8_t id = touchID[i];
    if (id >= maxtouches) continue;
    points[id].x = touchX[i];
    points[id].y = touchY[i];
    points[id].z = 1;
  }
}


/************ lower level i/o **************/

/**************************************************************************/
/*!
    @brief  Reads the bulk of data from captouch chip. Fill in {@link touches},
   {@link touchX}, {@link touchY} and {@link touchID} with results
*/
/**************************************************************************/
void Adafruit_FT5336::readData(void) {

  uint8_t i2cdat[32];
  uint8_t addr = 0;
  i2c_dev->write_then_read(&addr, 1, i2cdat, 32);

  touches = i2cdat[FT5336_TD_STATUS];
  if ((touches > 5) || (touches == 0)) {
    touches = 0;
  }
  gesture = i2cdat[FT5336_GEST_ID];
#ifdef FT5336_DEBUG
  if (gesture) {
    Serial.print("Gesture #"); 
    Serial.println(gesture);
  }
#endif

  for (uint8_t i=0; i<touches; i++) {
      touchX[i] = i2cdat[FT5336_TOUCH1_XH + i*6] & 0x0F;
      touchX[i] <<= 8;
      touchX[i] |= i2cdat[FT5336_TOUCH1_XL + i*6]; 

      touchY[i] = i2cdat[FT5336_TOUCH1_YH + i*6] & 0x0F;
      touchY[i] <<= 8;
      touchY[i] |= i2cdat[FT5336_TOUCH1_YL + i*6];

      touchID[i] = i2cdat[FT5336_TOUCH1_YH + i*6] >> 4;
  }

#ifdef FT5336_DEBUG
  Serial.println();
  for (uint8_t i = 0; i < touches; i++) {
    Serial.print("ID #");
    Serial.print(touchID[i]);
    Serial.print("\t(");
    Serial.print(touchX[i]);
    Serial.print(", ");
    Serial.print(touchY[i]);
    Serial.print(") ");
  }
  Serial.println();
#endif
}

uint8_t Adafruit_FT5336::readRegister8(uint8_t reg) {
  uint8_t buffer[1] = {reg};
  i2c_dev->write_then_read(buffer, 1, buffer, 1);
  return buffer[0];
}

void Adafruit_FT5336::writeRegister8(uint8_t reg, uint8_t val) {
  uint8_t buffer[2] = {reg, val};
  i2c_dev->write(buffer, 2);
}

/**************************************************************************/
/*!
    @brief  Instantiates a new touchpoint class with x, y and z set to 0 by default
*/
/**************************************************************************/
TS_Point::TS_Point(void) { x = y = z = 0; }

/**************************************************************************/
/*!
    @brief  Instantiates a new touchpoint class with x, y and z set by params.
    @param  _x The X coordinate
    @param  _y The Y coordinate
    @param  _z The Z coordinate
*/
/**************************************************************************/

TS_Point::TS_Point(int16_t _x, int16_t _y, int16_t _z) {
  x = _x;
  y = _y;
  z = _z;
}

/**************************************************************************/
/*!
    @brief  Simple == comparator for two TS_Point objects
    @returns True if x, y and z are the same for both points, False otherwise.
*/
/**************************************************************************/
bool TS_Point::operator==(TS_Point p1) {
  return ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

/**************************************************************************/
/*!
    @brief  Simple != comparator for two TS_Point objects
    @returns False if x, y and z are the same for both points, True otherwise.
*/
/**************************************************************************/
bool TS_Point::operator!=(TS_Point p1) {
  return ((p1.x != x) || (p1.y != y) || (p1.z != z));
}
