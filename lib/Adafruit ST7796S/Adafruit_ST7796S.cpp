/*!
 * @file Adafruit_ST7796S.cpp
 *
 * @mainpage Adafruit ST7796S TFT Displays
 *
 * @section intro_sec Introduction
 *
 * These displays use SPI to communicate, 4 or 5 pins are required
 * to interface (RST is optional).
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_ST7796S.h"
#ifndef ARDUINO_STM32_FEATHER
#include "pins_arduino.h"
#ifndef RASPI
#include "wiring_private.h"
#endif
#endif
#include <limits.h>

#if defined(ARDUINO_ARCH_ARC32) || defined(ARDUINO_MAXIM)
#define SPI_DEFAULT_FREQ 16000000
// Teensy 3.0, 3.1/3.2, 3.5, 3.6
#elif defined(__MK20DX128__) || defined(__MK20DX256__) ||                      \
    defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(__AVR__) || defined(TEENSYDUINO)
#define SPI_DEFAULT_FREQ 8000000
#elif defined(ESP8266) || defined(ESP32)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(RASPI)
#define SPI_DEFAULT_FREQ 80000000
#elif defined(ARDUINO_ARCH_STM32F1)
#define SPI_DEFAULT_FREQ 36000000
#else
#define SPI_DEFAULT_FREQ 24000000 ///< Default SPI data clock frequency
#endif

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7796S driver with software SPI
    @param    cs    Chip select pin #
    @param    dc    Data/Command pin #
    @param    mosi  SPI MOSI pin #
    @param    sclk  SPI Clock pin #
    @param    rst   Reset pin # (optional, pass -1 if unused)
    @param    miso  SPI MISO pin # (optional, pass -1 if unused)
*/
/**************************************************************************/
Adafruit_ST7796S::Adafruit_ST7796S(int8_t cs, int8_t dc, int8_t mosi,
                                   int8_t sclk, int8_t rst, int8_t miso)
    : Adafruit_SPITFT(ST7796S_TFTWIDTH, ST7796S_TFTHEIGHT, cs, dc, mosi, sclk,
                      rst, miso) {}

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7796S driver with hardware SPI using the
            default SPI peripheral.
    @param  cs   Chip select pin # (OK to pass -1 if CS tied to GND).
    @param  dc   Data/Command pin # (required).
    @param  rst  Reset pin # (optional, pass -1 if unused).
*/
/**************************************************************************/
Adafruit_ST7796S::Adafruit_ST7796S(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_SPITFT(ST7796S_TFTWIDTH, ST7796S_TFTHEIGHT, cs, dc, rst) {}

#if !defined(ESP8266)
/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7796S driver with hardware SPI using
            a specific SPI peripheral (not necessarily default).
    @param  spiClass  Pointer to SPI peripheral (e.g. &SPI or &SPI1).
    @param  dc        Data/Command pin # (required).
    @param  cs        Chip select pin # (optional, pass -1 if unused and
                      CS is tied to GND).
    @param  rst       Reset pin # (optional, pass -1 if unused).
*/
/**************************************************************************/
Adafruit_ST7796S::Adafruit_ST7796S(SPIClass *spiClass, int8_t dc, int8_t cs,
                                   int8_t rst)
    : Adafruit_SPITFT(ST7796S_TFTWIDTH, ST7796S_TFTHEIGHT, spiClass, cs, dc,
                      rst) {}
#endif // end !ESP8266

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ST7796S driver using parallel interface.
    @param  busWidth  If tft16 (enumeration in Adafruit_SPITFT.h), is a
                      16-bit interface, else 8-bit.
    @param  d0        Data pin 0 (MUST be a byte- or word-aligned LSB of a
                      PORT register -- pins 1-n are extrapolated from this).
    @param  wr        Write strobe pin # (required).
    @param  dc        Data/Command pin # (required).
    @param  cs        Chip select pin # (optional, pass -1 if unused and CS
                      is tied to GND).
    @param  rst       Reset pin # (optional, pass -1 if unused).
    @param  rd        Read strobe pin # (optional, pass -1 if unused).
*/
/**************************************************************************/
Adafruit_ST7796S::Adafruit_ST7796S(tftBusWidth busWidth, int8_t d0, int8_t wr,
                                   int8_t dc, int8_t cs, int8_t rst, int8_t rd)
    : Adafruit_SPITFT(ST7796S_TFTWIDTH, ST7796S_TFTHEIGHT, busWidth, d0, wr, dc,
                      cs, rst, rd) {}

// clang-format off
static const uint8_t PROGMEM initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ST7796S_PWR1  , 1, 0x23,             // Power control VRH[5:0]
  ST7796S_PWR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ST7796S_VCMPCTL  , 2, 0x3e, 0x28,       // VCM control
  ST7796S_MADCTL  , 1, 0x48,             // Memory Access Control
  ST7796S_VSCSAD, 1, 0x00,             // Vertical scroll zero
  ST7796S_COLMOD  , 1, 0x55,
  ST7796S_FRMCTR1 , 2, 0x00, 0x18,
  ST7796S_DFC , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ST7796S_PGC , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ST7796S_NGC , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ST7796S_SLPOUT  , 0x80,                // Exit Sleep
  ST7796S_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};
// clang-format on

/**************************************************************************/
/*!
    @brief   Initialize ST7796S chip
    Connects to the ST7796S over SPI and sends initialization procedure commands
    @param    freq  Desired SPI clock frequency
*/
/**************************************************************************/
void Adafruit_ST7796S::begin(uint32_t freq) {

  if (!freq)
    freq = SPI_DEFAULT_FREQ;
  initSPI(freq);

  if (_rst < 0) {                 // If no hardware reset pin...
    sendCommand(ST7796S_SWRESET); // Engage software reset
    delay(150);
  }

  uint8_t cmd, x, numArgs;
  const uint8_t *addr = initcmd;
  while ((cmd = pgm_read_byte(addr++)) > 0) {
    x = pgm_read_byte(addr++);
    numArgs = x & 0x7F;
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;
    if (x & 0x80)
      delay(150);
  }

  _width = ST7796S_TFTWIDTH;
  _height = ST7796S_TFTHEIGHT;
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Adafruit_ST7796S::setRotation(uint8_t m) {
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
  case 0:
    m = (MADCTL_MX | MADCTL_BGR);
    _width = ST7796S_TFTWIDTH;
    _height = ST7796S_TFTHEIGHT;
    break;
  case 1:
    m = (MADCTL_MV | MADCTL_BGR);
    _width = ST7796S_TFTHEIGHT;
    _height = ST7796S_TFTWIDTH;
    break;
  case 2:
    m = (MADCTL_MY | MADCTL_BGR);
    _width = ST7796S_TFTWIDTH;
    _height = ST7796S_TFTHEIGHT;
    break;
  case 3:
    m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    _width = ST7796S_TFTHEIGHT;
    _height = ST7796S_TFTWIDTH;
    break;
  }

  sendCommand(ST7796S_MADCTL, &m, 1);
}

/**************************************************************************/
/*!
    @brief   Enable/Disable display color inversion
    @param   invert True to invert, False to have normal color
*/
/**************************************************************************/
void Adafruit_ST7796S::invertDisplay(bool invert) {
  sendCommand(invert ? ST7796S_INVON : ST7796S_INVOFF);
}

/**************************************************************************/
/*!
    @brief   Scroll display memory
    @param   y How many pixels to scroll display by
*/
/**************************************************************************/
void Adafruit_ST7796S::scrollTo(uint16_t y) {
  uint8_t data[2];
  data[0] = y >> 8;
  data[1] = y & 0xff;
  sendCommand(ST7796S_VSCSAD, (uint8_t *)data, 2);
}

/**************************************************************************/
/*!
    @brief   Set the height of the Top and Bottom Scroll Margins
    @param   top The height of the Top scroll margin
    @param   bottom The height of the Bottom scroll margin
 */
/**************************************************************************/
void Adafruit_ST7796S::setScrollMargins(uint16_t top, uint16_t bottom) {
  // TFA+VSA+BFA must equal 320
  if (top + bottom <= ST7796S_TFTHEIGHT) {
    uint16_t middle = ST7796S_TFTHEIGHT - (top + bottom);
    uint8_t data[6];
    data[0] = top >> 8;
    data[1] = top & 0xff;
    data[2] = middle >> 8;
    data[3] = middle & 0xff;
    data[4] = bottom >> 8;
    data[5] = bottom & 0xff;
    sendCommand(ST7796S_VSCRDEF, (uint8_t *)data, 6);
  }
}

/**************************************************************************/
/*!
    @brief   Set the "address window" - the rectangle we will write to RAM with
   the next chunk of      SPI data writes. The ST7796S will automatically wrap
   the data as each row is filled
    @param   x1  TFT memory 'x' origin
    @param   y1  TFT memory 'y' origin
    @param   w   Width of rectangle
    @param   h   Height of rectangle
*/
/**************************************************************************/
void Adafruit_ST7796S::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w,
                                     uint16_t h) {
  uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
  writeCommand(ST7796S_CASET); // Column address set
  SPI_WRITE16(x1);
  SPI_WRITE16(x2);
  writeCommand(ST7796S_PASET); // Row address set
  SPI_WRITE16(y1);
  SPI_WRITE16(y2);
  writeCommand(ST7796S_RAMWR); // Write to RAM
}

/**************************************************************************/
/*!
    @brief  Read 8 bits of data from ST7796S configuration memory. NOT from RAM!
            This is highly undocumented/supported, it's really a hack but kinda
   works?
    @param    commandByte  The command register to read data from
    @param    index  The byte index into the command to read from
    @return   Unsigned 8-bit data read from ST7796S register
 */
/**************************************************************************/
uint8_t Adafruit_ST7796S::readcommand8(uint8_t commandByte, uint8_t index) {
  uint8_t data = 0x10 + index;
  sendCommand(0xD9, &data, 1); // Set Index Register
  return Adafruit_SPITFT::readcommand8(commandByte);
}
