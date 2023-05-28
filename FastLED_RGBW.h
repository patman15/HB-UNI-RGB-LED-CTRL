/* FastLED_RGBW
 *
 * Hack to enable SK6812 RGBW strips to work with FastLED.
 *
 * Original code by Jim Bumgardner (http://krazydad.com).
 * Modified by David Madison (http://partsnotincluded.com).
 *
*/

#ifndef FastLED_RGBW_h
#define FastLED_RGBW_h

struct CRGBW  {
  union {
    struct {
      union {
        uint8_t g;
        uint8_t green;
      };
      union {
        uint8_t r;
        uint8_t red;
      };
      union {
        uint8_t b;
        uint8_t blue;
      };
      union {
        uint8_t w;
        uint8_t white;
      };
    };
    uint8_t raw[4];
  };

  CRGBW(){}


CRGBW(uint8_t rd, uint8_t grn, uint8_t blu){
w = min(rd, grn);
w = min(w, blu);

r = rd-w;
g = grn-w;
b = blu-w;
w /= 2;
}

CRGBW(const CRGB c){
w = min(c.r, c.g);
w = min(w, c.b);

r = c.r-w;
g = c.g-w;
b = c.b-w;
w /= 2;
}

CRGBW(uint8_t rd, uint8_t grn, uint8_t blu, uint8_t whi){
r = rd;
g = grn;
b = blu;
w = whi;
}

inline void operator = (const CRGB c) __attribute__((always_inline)){
/*uint8_t w = min(c.r, c.g);
w = min(w, c.b); */
this->r = c.r;
this->g = c.g;
this->b = c.b;
this->w = 0;
}

inline CRGBW& operator|= (const CRGB& rhs )
{
  if( rhs.r > r) r = rhs.r;
  if( rhs.g > g) g = rhs.g;
  if( rhs.b > b) b = rhs.b;
  return *this;
}

inline CRGBW& operator|= (const CRGBW& rhs )
{
  if( rhs.r > r) r = rhs.r;
  if( rhs.g > g) g = rhs.g;
  if( rhs.b > b) b = rhs.b;
  if( rhs.w > w) w = rhs.w;
  return *this;
}

inline CRGBW& operator+= (const CRGB& rhs )
 {
     r = qadd8( r, rhs.r);
     g = qadd8( g, rhs.g);
     b = qadd8( b, rhs.b);
     return *this;
 }

inline CRGBW& operator+= (const CRGBW& rhs )
 {
     r = qadd8( r, rhs.r);
     g = qadd8( g, rhs.g);
     b = qadd8( b, rhs.b);
     w = qadd8( w, rhs.w);
     return *this;
 }

/*inline void fill_solid (CRGBW *leds, int numToFill, const CRGB &color) {
  const CRGBW rgbw = CRGBW(color.r, color.g, color.b);
  for ( uint16_t i = 0; i < numToFill; i++) {
    leds[i] = rgbw;
  }
}*/

/// Get the average of the R, G, and B values
inline uint8_t getAverageLight( )  const {
#if FASTLED_SCALE8_FIXED == 1
    const uint8_t eightyfive = 85;
#else
    const uint8_t eightyfive = 86;
#endif
    const uint8_t whi = w*2;
    uint8_t avg = scale8_LEAVING_R1_DIRTY( r+whi, eightyfive) + \
    scale8_LEAVING_R1_DIRTY( g+whi, eightyfive) + \
    scale8_LEAVING_R1_DIRTY( b+whi, eightyfive);
    cleanup_R1();
    return avg;
}

};

inline uint16_t getRGBWsize(uint16_t nleds){
  uint16_t nbytes = nleds * 4;
  if(nbytes % 3 > 0) return nbytes / 3 + 1;
  else return nbytes / 3;
}

#endif
