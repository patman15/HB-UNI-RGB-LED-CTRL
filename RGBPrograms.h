void RGBProgramRainbow(uint8_t speed, uint8_t brightness) {
  static uint8_t startIndex = 0;
  static unsigned long lastmillis = millis();
  if (millis() - lastmillis > speed) {
    startIndex = startIndex + 1;
    for ( int i = 0; i < WSNUM_LEDS; i++) {
      leds[i] = ColorFromPalette( RainbowColors_p, startIndex, brightness, LINEARBLEND);
    }
    lastmillis = millis();
  }
}


void RGBProgramFire(/* uint8_t brightness */) {
  static unsigned long lastmillis = millis();
  if (millis() - lastmillis > 25) {

    static byte heat[WSNUM_LEDS];

    for ( int i = 0; i < WSNUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((FIRE_PROGRAM_COOLING * 10) / WSNUM_LEDS) + 2));
    }

    for ( int k = WSNUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    if ( random8() < FIRE_PROGRAM_SPARKLING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160, 255) );
    }

    for ( int j = 0; j < WSNUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;

      pixelnumber = j;

      leds[pixelnumber] = color;
    }
    lastmillis = millis();
  }
}

//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
// https://github.com/FastLED/FastLED/blob/master/examples/Pacifica/Pacifica.ino
//
const TProgmemRGBPalette16 FL_PROGMEM pacifica_palette_1 =
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
const TProgmemRGBPalette16 FL_PROGMEM pacifica_palette_2 =
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
const TProgmemRGBPalette16 FL_PROGMEM pacifica_palette_3 =
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };

// Add one layer of waves into the led array
void pacifica_one_layer( const TProgmemRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  const uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < WSNUM_LEDS; i++) {
    waveangle += 250;
    const uint16_t s16 = sin16( waveangle ) + 32768;
    const uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    const uint16_t sindex16 = sin16( ci) + 32768;
    const uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  const uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );

  for( uint16_t i = 0; i < WSNUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    const uint8_t l = ledsRGB[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
// rotate color to match currentColor
void pacifica_deepen_colors(uint8_t currentColor)
{
  for( uint16_t i = 0; i < WSNUM_LEDS; i++) {
    if (currentColor < 33 || currentColor > 166) {
      leds[i] = CRGB(scale8( leds[i].blue,  145), scale8( leds[i].green, 200), leds[i].red);
      leds[i] |= CRGBW( 5, 3, 0, 2);
    } else if (currentColor < 100) {
      leds[i] = CRGB(leds[i].red, scale8( leds[i].blue,  145), scale8( leds[i].green, 200));
      leds[i] |= CRGBW( 0, 5, 3, 2);
    } else {
      leds[i] = CRGB(leds[i].red, scale8( leds[i].green, 200), scale8( leds[i].blue,  145));
      leds[i] |= CRGBW( 0, 3, 5, 2);
    }
  }
}

void RGBProgramWaterfall (uint8_t color) {
    // Increment the four "color index start" counters, one for each wave layer.
    // Each is incremented at a different speed, and the speeds vary over time.
    static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    static uint32_t sLastms = 0;
    uint32_t ms = GET_MILLIS();
    uint32_t deltams = ms - sLastms;
    if (deltams > 20) {
    sLastms = ms;
    uint16_t speedfactor1 = beatsin16(3, 179, 269);
    uint16_t speedfactor2 = beatsin16(4, 179, 269);
    uint32_t deltams1 = (deltams * speedfactor1) / 256;
    uint32_t deltams2 = (deltams * speedfactor2) / 256;
    uint32_t deltams21 = (deltams1 + deltams2) / 2;
    sCIStart1 += (deltams1 * beatsin88(1011,10,13));
    sCIStart2 -= (deltams21 * beatsin88(777,8,11));
    sCIStart3 -= (deltams1 * beatsin88(501,5,7));
    sCIStart4 -= (deltams2 * beatsin88(257,4,6));

    // Clear out the LED array to a dim background blue-green
    leds[0] = CRGB( 2, 6, 10);
    for ( uint16_t i = 1; i < WSNUM_LEDS; i++) {
      leds[i] = leds[0];
    }
    //fill_solid( ledsRGB, WSNUM_LEDS, CRGB( 2, 6, 10));

    // Render each of four layers, with different scales and speeds, that vary over time
    pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
    pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
    pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
    pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

    // Add brighter 'whitecaps' where the waves lines up more
    pacifica_add_whitecaps();

    // Deepen the blues and greens a bit
    pacifica_deepen_colors(color);
  }
}

void RGBProgramTVSimulation (uint8_t brightness) {}

