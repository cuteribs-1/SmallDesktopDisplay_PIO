/*!
 * @file Adafruit_ST7796S.h
 *
 * This is the documentation for Adafruit's ST7796S driver for the
 * Arduino platform.
 *
 * These displays use SPI to communicate, 4 or 5 pins are required
 * to interface (RST is optional).
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_ST7796SH_
#define _ADAFRUIT_ST7796SH_

#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "Print.h"
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <SPI.h>

#define ST7796S_TFTWIDTH 320  ///< ST7796S max TFT width
#define ST7796S_TFTHEIGHT 480 ///< ST7796S max TFT height

#define ST7796S_NOP 0x00     ///< No-op register
#define ST7796S_SWRESET 0x01 ///< Software reset register
#define ST7796S_RDDID 0x04   ///< Read display identification information
#define ST7796S_RDDST 0x09   ///< Read Display Status

#define ST7796S_RDDPM 0x0A       ///< Read Display Power Mode
#define ST7796S_RDDMADCTL 0x0B   ///< Read Display MADCTL
#define ST7796S_RDDCOLMOD 0x0C   ///< Read Display Pixel Format
#define ST7796S_RDDIM 0x0D       ///< Read Display Image Format
#define ST7796S_RDDSM 0x0E      ///< Read Display Image Format
#define ST7796S_RDDSDR 0x0F     ///< Read Display Self-Diagnostic Result

#define ST7796S_SLPIN 0x10  ///< Enter Sleep Mode
#define ST7796S_SLPOUT 0x11 ///< Sleep Out
#define ST7796S_PTLON 0x12  ///< Partial Mode ON
#define ST7796S_NORON 0x13  ///< Normal Display Mode ON

#define ST7796S_INVOFF 0x20   ///< Display Inversion OFF
#define ST7796S_INVON 0x21    ///< Display Inversion ON
#define ST7796S_DISPOFF 0x28  ///< Display OFF
#define ST7796S_DISPON 0x29   ///< Display ON

#define ST7796S_CASET 0x2A ///< Column Address Set
#define ST7796S_PASET 0x2B ///< Page Address Set
#define ST7796S_RAMWR 0x2C ///< Memory Write
#define ST7796S_RAMRD 0x2E ///< Memory Read

#define ST7796S_PTLAR 0x30    ///< Partial Area
#define ST7796S_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ST7796S_TEOFF 0x34    ///< Tearing Effect Line OFF
#define ST7796S_TEON 0x35     ///< Tearing Effect Line On
#define ST7796S_MADCTL 0x36   ///< Memory Access Control
#define ST7796S_VSCSAD 0x37   ///< Vertical Scrolling Start Address
#define ST7796S_IDMOFF 0x38   ///< Idle Mode Off
#define ST7796S_IDMO 0x39     ///< Idle Mode On
#define ST7796S_COLMOD 0x3A   ///< COLMOD: Pixel Format Set
#define ST7796S_WRMEMC 0x3C   ///< Write Memory Continue
#define ST7796S_RDMEMC 0x3E   ///< Read Memory Continue

#define ST7796S_STE 0x44         ///< Set Tear Scanline
#define ST7796S_GSCAN 0x45       ///< Get Scanline

#define ST7796S_WRDISBV 0x51     ///< Write Display Brightness
#define ST7796S_RDDISBV 0x52     ///< Read Display Brightness Value
#define ST7796S_WRCTRLD 0x53     ///< Write CTRL Display
#define ST7796S_RDCTRLD 0x54     ///< Read CTRL value Display
#define ST7796S_WRCABC 0x55      ///< Write Adaptive Brightness Control
#define ST7796S_RDCABC 0x56      ///< Read Content Adaptive Brightness Control
#define ST7796S_WRCABCMB 0x5E    ///< Write CABC Minimum Brightness
#define ST7796S_RDCABCMB 0x5F    ///< Read CABC Minimum Brightness

#define ST7796S_RDFCS 0xAA       ///< Read First Checksum
#define ST7796S_RDCFCS 0xAF      ///< Read Continue Checksum

#define ST7796S_IFMODE 0xB0  ///< Interface Mode Contro
#define ST7796S_FRMCTR1 0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ST7796S_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ST7796S_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ST7796S_DIC 0xB4     ///< Display Inversion Control
#define ST7796S_BPC 0xB5     ///< Blanking Porch Control
#define ST7796S_DFC 0xB6     ///< Display Function Control
#define ST7796S_EM 0xB7      ///< Entry Mode Set

#define ST7796S_PWR1 0xC0      ///< Power Control 1
#define ST7796S_PWR2 0xC1      ///< Power Control 2
#define ST7796S_PWR3 0xC2      ///< Power Control 3
#define ST7796S_VCMPCTL 0xC5   ///< VCOM Control 1
#define ST7796S_VCMOFFSET 0xC6 ///< VCOM Offset Register

#define ST7796S_RDID1 0xDA       ///< Read ID1
#define ST7796S_RDID2 0xDB       ///< Read ID2
#define ST7796S_RDID3 0xDC       ///< Read ID3

#define ST7796S_PGC 0xE0     ///> Positive Gamma Control
#define ST7796S_NGC 0xE1     ///> Negative Gamma Control
#define ST7796S_DGC1 0xE2    ///> Digital Gamma Control 1
#define ST7796S_DGC2 0xE3    ///> Digital Gamma Control 2
#define ST7796S_DOCA 0xE8    ///> Display Output Ctrl Adjust
#define ST7796S_CSCON 0xF0   ///> Command Set Control
#define ST7796S_SPI 0xFB     ///> Read Control
//#define ST7796S_PWCTR6     0xFC

// Color definitions
#define ST7796S_BLACK 0x0000       ///<   0,   0,   0
#define ST7796S_NAVY 0x000F        ///<   0,   0, 123
#define ST7796S_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ST7796S_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ST7796S_MAROON 0x7800      ///< 123,   0,   0
#define ST7796S_PURPLE 0x780F      ///< 123,   0, 123
#define ST7796S_OLIVE 0x7BE0       ///< 123, 125,   0
#define ST7796S_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ST7796S_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ST7796S_BLUE 0x001F        ///<   0,   0, 255
#define ST7796S_GREEN 0x07E0       ///<   0, 255,   0
#define ST7796S_CYAN 0x07FF        ///<   0, 255, 255
#define ST7796S_RED 0xF800         ///< 255,   0,   0
#define ST7796S_MAGENTA 0xF81F     ///< 255,   0, 255
#define ST7796S_YELLOW 0xFFE0      ///< 255, 255,   0
#define ST7796S_WHITE 0xFFFF       ///< 255, 255, 255
#define ST7796S_ORANGE 0xFD20      ///< 255, 165,   0
#define ST7796S_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ST7796S_PINK 0xFC18        ///< 255, 130, 198

/**************************************************************************/
/*!
@brief Class to manage hardware interface with ST7796S chipset (also seems to
work with ILI9340)
*/
/**************************************************************************/

class Adafruit_ST7796S : public Adafruit_SPITFT {
public:
  Adafruit_ST7796S(int8_t _CS, int8_t _DC, int8_t _MOSI, int8_t _SCLK,
                   int8_t _RST = -1, int8_t _MISO = -1);
  Adafruit_ST7796S(int8_t _CS, int8_t _DC, int8_t _RST = -1);
#if !defined(ESP8266)
  Adafruit_ST7796S(SPIClass *spiClass, int8_t dc, int8_t cs = -1,
                   int8_t rst = -1);
#endif // end !ESP8266
  Adafruit_ST7796S(tftBusWidth busWidth, int8_t d0, int8_t wr, int8_t dc,
                   int8_t cs = -1, int8_t rst = -1, int8_t rd = -1);

  void begin(uint32_t freq = 0);
  void setRotation(uint8_t r);
  void invertDisplay(bool i);
  void scrollTo(uint16_t y);
  void setScrollMargins(uint16_t top, uint16_t bottom);

  // Transaction API not used by GFX
  void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

  uint8_t readcommand8(uint8_t reg, uint8_t index = 0);
};

#endif // _ADAFRUIT_ST7796SH_
