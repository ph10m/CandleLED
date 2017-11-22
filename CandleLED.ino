#include <FastLED.h>
// LED definitions
#define LED_PIN     5
#define NUM_LEDS    112
#define BRIGHTNESS  80
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 60
CRGB leds[NUM_LEDS];
// END LED definitions

// general definitions
#define NEXT_BTN 2
#define PREV_BTN 3
#define AMOUNT_OF_PALETTES 8

// changing variables
int btnCounter = 0; // a counter to determine states
int nextState = 0;
int prevState = 0;
int lastBtnState = 0;     // previous state of the button

// LED pallette setup
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 CozyPalette;
extern const TProgmemPalette16 CozyPalette_p PROGMEM;


void setup() {
  delay(1000); // power-up safety delay
  Serial.begin(9600);
  Serial.println("Setup done");

  // set up FASTLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);

  // General setup
  pinMode(NEXT_BTN, INPUT);
  pinMode(PREV_BTN, INPUT);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}


void loop()
{
  random16_add_entropy(random());

  bool isFire = ChangePaletteOnButtonPress();
  if (!isFire) {
      static uint8_t startIndex = 0;
      startIndex = startIndex + 1; /* motion speed */
      FillLEDsFromPaletteColors(startIndex);
  }
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

bool ChangePaletteOnButtonPress()
{
  nextState = digitalRead(NEXT_BTN);
  prevState = digitalRead(PREV_BTN);
  if (nextState == HIGH){
    if (btnCounter >= AMOUNT_OF_PALETTES){
      btnCounter = 0;
    }
    else {
      btnCounter += 1;
    }
    delay(500);
  }
  if (prevState == HIGH){
    if (btnCounter <= 0){
      btnCounter = AMOUNT_OF_PALETTES;
    }
    else {
      btnCounter -= 1;
    }
    delay(500);
  }
  Serial.println(btnCounter);
  bool isFire = false;
  switch(btnCounter){
    case 0:
      currentPalette = RainbowColors_p;
      break;
    case 1:
      currentPalette = RainbowStripeColors_p;
      break;
    case 2:
      SetupBlackAndWhiteStripedPalette();
      break;
    case 3:
      SetupPurpleAndGreenPalette();
      break;
    case 4:
      currentPalette = CloudColors_p;
      break;
    case 5:
      currentPalette = PartyColors_p;
      break;
    case 6:
      currentPalette = CozyPalette_p;
      break;
    case 7:
      SetupTotallyRandomPalette();
      break;
    case 8:
      Fire2012WithPalette();
      isFire = true;
      break;
    default:
      currentPalette = RainbowColors_p;
      break;
  }
  return isFire;
}


// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
                     green,  green,  black,  black,
                     purple, purple, black,  black,
                     green,  green,  black,  black,
                     purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 CozyPalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Yellow, // 'white' is too bright compared to red and blue
  CRGB::Orange,
  CRGB::Black,

  CRGB::Red,
  CRGB::Yellow,
  CRGB::Orange,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Orange,
  CRGB::Orange,
  CRGB::Black,
  CRGB::Black
};

// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  80
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 150
void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  currentPalette = CRGBPalette16(CRGB::Red, CRGB(255,90,0), CRGB(255,150,10));
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette(currentPalette, colorindex);
      int pixelnumber;
      pixelnumber = (NUM_LEDS-1)-j;
      leds[pixelnumber] = color;
    }
}

