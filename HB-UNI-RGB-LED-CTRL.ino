//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2018-08-03 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2021-08-20 patman15 Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------
// ci-test=yes board=328p aes=no

// define this to read the device id, serial and device type from bootloader section
// #define USE_OTA_BOOTLOADER
#define NDEBUG   // disable all serial debug messages; comment if you want to get debug messages in the serial monitor
#define FASTLED_USE_PROGMEM 1
//#define FASTLED_ALLOW_INTERRUPTS 0      // disable IRQs (recommended if not necessary)

#define EI_NOTEXTERNAL
#include <EnableInterrupt.h>
#include <SPI.h>
#include <AskSinPP.h>
#include <LowPower.h>
#include <Register.h>
#include "analog.h"

#define ENABLE_RGBW            // for SK6812 LEDs

#define WSNUM_LEDS    125        // Anzahl angeschlossener LEDs 125
#define WSLED_PIN     18         //GPIO Pin LED Anschluss
#define WSLED_TYPE    SK6812     //LED Typ
#define WSCOLOR_ORDER GRB        //Farbreihenfolge

/*
#define PWM_ENABLED
#define PWM_RED_PIN     3
#define PWM_GREEN_PIN   5
#define PWM_BLUE_PIN    6
#define PWM_WHITE_PIN   9       //Pin für weiße LED, auskommentieren, wenn keine weiße LED vorhanden ist
#define PWM_WHITE_ONLY  true    //Wenn weiße LED vorhanden ist, soll nur diese angesteuert werden wenn die Farbe weiß ist?
*/

#define SLOW_PROGRAM_TIMER     250     //ms Wartezeit für den Übergang
#define NORMAL_PROGRAM_TIMER   40     //ms Wartezeit für den Übergang
#define FAST_PROGRAM_TIMER     10      //ms Wartezeit für den Übergang
#define FIRE_PROGRAM_COOLING   65
#define FIRE_PROGRAM_SPARKLING 110

#if defined __AVR_ATmega2560__
#define CONFIG_BUTTON_PIN 13
#else
#define CONFIG_BUTTON_PIN 4
#endif
#define ONBOARD_LED_PIN   3

#ifdef ENABLE_RGBW
#include "FastLED_RGBW.h"
#endif
#include "RGBCtrl.h"

#define PEERS_PER_CHANNEL 4

using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  {0xF3, 0x41, 0x01},     // Device ID
  "JPRGB00014",           // Device Serial
  {0xF3, 0x41},           // Device Model
  0x25,                   // Firmware Version
  as::DeviceType::Dimmer, // Device Type
  {0x01, 0x00}            // Info Bytes
};

/**
   Configure the used hardware
*/
#if defined __AVR_ATmega2560__
typedef AskSin<StatusLed<ONBOARD_LED_PIN>, NoBattery, Radio<LibSPI<53>, 2>> HalType;
#else
typedef AskSin<StatusLed<ONBOARD_LED_PIN>, NoBattery, Radio<LibSPI<10>, 2>> HalType;
#endif

DEFREGISTER(Reg0, MASTERID_REGS, 0x20, 0x21)
class Ws28xxList0 : public RegList0<Reg0> {
  public:
    Ws28xxList0(uint16_t addr) : RegList0<Reg0>(addr) {}

    void defaults () {
      clear();
    }
};

typedef RGBLEDChannel<HalType, PEERS_PER_CHANNEL, Ws28xxList0> ChannelType;
typedef RGBLEDDevice<HalType, ChannelType, 3, Ws28xxList0> RGBLEDType;

HalType hal;
RGBLEDType sdev(devinfo, 0x20);
ConfigButton<RGBLEDType> cfgBtn(sdev);

void setup () {
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);
  buttonISR(cfgBtn, CONFIG_BUTTON_PIN);
  sdev.initDone();
}

void loop() {
  // disabled for temporal dimming
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if (brightness == 0 && worked == false && poll == false ) {
#ifndef PWM_ENABLED
    hal.activity.savePower<Idle<true>>(hal);
#endif
  }
  sdev.handleLED();
}
