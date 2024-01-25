#include "leds.h"
CRGB* get_jleds_0();
int get_jleds_0_num();
int pattern = 0;
typedef void (*SimplePatternList[])(CRGB *_leds, int num_leds);
typedef void (*_SimplePatternList[])();
//_SimplePatternList _autoPatterns = {  rainbow, rainbowWithGlitter, rainbow_scaling, fire, fireSparks, fireRainbow, noise1, noise2, noise3, blendwave, confetti, ripple_blur, sinelon, dot_beat, juggle };
SimplePatternList autoPatterns_blue = {pride_blue, pride_blue};
SimplePatternList autoPatterns = {pride, pride};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
bool button_pressed = false;
unsigned long pressed_until_milli;
extern void music(CRGB *_leds, int num_leds, CRGB baseColor);

#define leds_a2 leds_0
#define leds_b1 leds_0
#define leds_b2 leds_0
#define leds_c  leds_0

uint8_t get_gCurrentPatternNumber(){
  return gCurrentPatternNumber;
}
void set_gCurrentPatternNumber(uint8_t val){
  gCurrentPatternNumber = val;
}
#define PATTERN_PERIOD_SEC 500
#define SOUND_REACT_PERIOD_MINUTES 4

#define P0 0
#define P1 21
#define P2 P1+7
#define P3 P2+21
#define P4 P3+8
#define P5 P4+19
#define P6 P5+6
#define P7 P6+19
#define P8 P7+5
#define P9 P8+18
#define P10 P9+4
#define P11 P10+18
#define P12 P11+4
int get_freq_val(CRGB *_leds ,int num_leds, int base);
//void music(CRGB *_leds, int num_leds, CRGB baseColor);
uint8_t getLEDBrightness(const CRGB& led) {
    return (led.r + led.g + led.b) / 3;
}

// Music reaction in 3 strips
void just_dance_led_test(){
  static int out = 0;
  static int med = 0;
  static int in = 0;
  EVERY_N_MILLISECONDS(10){
    uint8_t led_color = 0;
    size_t num_leds;
    CRGB *_leds = get_leds(0, num_leds);
    int val = get_freq_val(_leds,num_leds, 20);
    if(val > out)
    {
      val = out;
    }else{
      EVERY_N_MILLISECONDS(30){ out = max (out-5,0);}
    }
    set_outer(CHSV(0,255, out));
    val = get_freq_val(_leds,num_leds, 40);
    if(val > med)
    {
      med =  val;
    }else{
      EVERY_N_MILLISECONDS(30){ med  = max (med-5,0);}
    }
    set_med(CHSV(80,255, med));
    val = get_freq_val(_leds,num_leds, 60);
    if(val > in)
    {
      in = val;
    }else{
      EVERY_N_MILLISECONDS(30){ in = max (in-5,0);}
    }
    set_inner(CHSV(160,255, in));
    Serial.println(in);
    EVERY_N_MILLISECONDS(500){ 
      //nscale8(_leds, num_leds, 240); // smaller = faster fade
    }
    FastLED.show();

  }
}

// Simple music test
void run_music_reactive(){
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  music(_leds,num_leds, CRGB::Blue);
}


// Simple test
void _just_dance_led_test(){
  uint8_t led_color = 0;
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  FastLED.show();
  delay(2000);
  set_outer(CRGB::Red);
  set_med(CRGB::Blue);
  set_inner(CRGB::Green);
  FastLED.show();
  delay(10000);
}


void set_outer(CRGB color){
  uint8_t led_color = 0;
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  for (int i = 0; i < P4; i++) {
   _leds[i] = color;
  }
   FastLED.show();
}

void set_med(CRGB color){
  uint8_t led_color = 0;
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  for (int i = P4; i < P8; i++) {
   _leds[i] = color;
  }
   FastLED.show();
}

void set_inner(CRGB color){
  uint8_t led_color = 0;
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  for (int i = P8; i < P11; i++) {
   _leds[i] = color;
  }
   FastLED.show();
}

void fade_all(){
      for (int i = 120; i >= 0; i--) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(50);
    }
}

void unfade_all(){
      for (int i = 0; i < 255 ; i++) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(10);
    }
}

CRGB *get_leds(int led_index, size_t &num_leds);

void notify_button_pressed(){
  Serial.println("Button press");
  button_pressed = true;
  pressed_until_milli = millis() + 1000;
}

bool get_music_mode(){
  if (millis() > pressed_until_milli){
    button_pressed = false;
  }
  return button_pressed;

}

void run_just_dance(){
  bool faded = false;
  size_t num_leds;
  CRGB *_leds = get_leds(0, num_leds);
  if(get_music_mode()){
    run_music_reactive();
  }else{
    if (digitalRead(mode_button_pin) == LOW){
      autoPatterns[pattern](_leds,num_leds);
    } else{
      autoPatterns_blue[pattern](_leds,num_leds);
    }
  }
  duplicate_led();
  
  EVERY_N_SECONDS(PATTERN_PERIOD_SEC){
    pattern = (pattern +1 ) % 2;
    fade_all();
    faded = true;
    duplicate_led();
    FastLED.show();
    if ( faded){
      unfade_all();
      faded = false;
      FastLED.setBrightness(250);
    }
  }
}

void duplicate_led(){
  size_t to_num_leds;
  size_t from_num_leds;
  CRGB *_from_leds = get_leds(0, from_num_leds);
  for (auto i = 1 ; i < 4; i++){
    CRGB *_to_leds = get_leds(i, to_num_leds);
    for (auto i = 0 ; i < to_num_leds ; i++){
      _to_leds[i] = _from_leds[i];
    }
  }
}
