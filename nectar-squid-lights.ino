#define FASTLED_ALLOW_INTERRUPTS 0

#include <U8x8lib.h>
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define PIXEL_PIN_A 12
#define PIXEL_PIN_B 13
#define PIXEL_COUNT 150
#define PIXEL_TYPE WS2812
#define COLOR_ORDER GRB
#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  120

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

CRGB ledsA[PIXEL_COUNT];
CRGB ledsB[PIXEL_COUNT];

uint32_t ms = 0;

#define HUE_A HUE_RED
#define HUE_B HUE_PURPLE

CHSVPalette16 GP(
                  CHSV(HUE_B + 0, 255, 128),
                  CHSV(HUE_A + 4, 255, 192),
                  CHSV(HUE_A + 8, 255, 192),
                  CHSV(HUE_A + 8, 64, 156),
                  CHSV(HUE_A,     96, 172),
                  CHSV(HUE_A,     255,255),
                  CHSV(HUE_A,     255,255),
                  CHSV(HUE_A,     255,128),
                  CHSV(HUE_A + 2, 255,128),
                  CHSV(HUE_A + 4, 255,255),
                  CHSV(HUE_A + 6, 255,255),
                  CHSV(HUE_A + 4, 255,128),
                  CHSV(HUE_A + 2, 255,128),
                  CHSV(HUE_A,     255,255),
                  CHSV(HUE_A,     255,255),
                  CHSV(HUE_A,     255,255));

void doWash_pattern(const CHSVPalette16& pal);
                
void setup(void) {
  u8x8.begin();
  u8x8.clear();

  FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN_A, COLOR_ORDER>(ledsA, PIXEL_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<PIXEL_TYPE, PIXEL_PIN_B, COLOR_ORDER>(ledsB, PIXEL_COUNT).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(255);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0,"Aberration!");
  u8x8.drawString(0, 2,"The Parker");
  u8x8.drawString(0, 3, "Bridge Horror");
  u8x8.drawString(0, 4,"Emerges");
}

void loop() {
  doWash_pattern(GP);
  
  FastLED.show();  
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void doWash_pattern(const CHSVPalette16& pal) {
    uint32_t ms = millis();
    
    int dtheta1 = 5300;
    uint16_t starttheta1 = ms * 15;
    uint16_t theta1 = starttheta1;

    int dtheta2 = 7300;
    uint16_t starttheta2 = ms * 17;
    uint16_t theta2 = starttheta2;

    int dtheta3 = 2300;
    uint16_t starttheta3 = ms * 3;
    uint16_t theta3 = starttheta3;

    int brightness = beatsin88(1706, 40, 255);

    for(uint16_t i = 0; i < PIXEL_COUNT; i++) {
        int s1 = sin16( theta1);
        theta1 += dtheta1;

        int s2 = sin16( theta2);
        theta2 += dtheta2;

        int s3 = sin16( theta3);
        theta3 += dtheta3;

        byte bri = ((s1 + 32768) >> 8);

        byte sat = ((s2 + 32768) >> 8);
        sat = (sat / 4) + 192;
        sat = brighten8_video(sat);
        byte desat = 255 - sat;
        desat = scale8( desat, 16);

        byte hue = ((s3 + 32768) >> 8);
        hue = scale8(hue, 240);
        
        CRGB rgb = ColorFromPalette( pal, hue, brightness);
        rgb += CRGB( desat, desat, desat);
        CRGB old = ledsA[i];
        old.nscale8(192);
        rgb.nscale8_video(64);

        rgb += old;
        ledsA[i] = rgb;
        ledsB[i] = rgb;
    }
}
