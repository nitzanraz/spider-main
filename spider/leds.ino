




void show_grow( CRGB *_leds, int num_leds, bool up , unsigned char hue, unsigned char bright, int _delay_msec){
  // Set the rest of the LEDs to CHSV(0, 255, 255) (Hue 0, Saturation 255, Value 255)
  for ( auto i = 1; i < num_leds; i++) {
    auto led_id = i;
    if (!up){
      led_id = num_leds -i ;
    }
    _leds[led_id] = CHSV(hue, 255, bright);
    FastLED.show();//[0].showLeds(255);
    delay(_delay_msec);
  }
}

void wave( CRGB *_leds, int num_leds, bool up , unsigned char hue, unsigned char bright, int _delay_msec){
  // Set the rest of the LEDs to CHSV(0, 255, 255) (Hue 0, Saturation 255, Value 255)
    for ( auto i=0; i < num_leds; i++)
    {
        auto sinh = sin16( (i * 65536)/(num_leds)*45 + (millis() +1)/1001);
        auto hue =  map(sinh, -32767, 32767, 0, 100);
        CHSV color = CHSV(hue, 255, 255);
        //generate wave for changing led pixels
        uint16_t sinBeat = beatsin16(10, 0, num_leds, 0, 0);
//generate wave for colors
        uint8_t colBeat  = beatsin8(20, 0, 255, 0, 0);
        fadeToBlackBy(_leds, num_leds, 10);

        _leds[i] = CHSV(colBeat, 255, 255);
        /*light_led(leds_1, sizeof(leds_1), i, color);
        light_led(leds_2, sizeof(leds_2), i, color);
        light_led(leds_3, sizeof(leds_3), i, color);*/
        FastLED.show();
    }
}
 

void wave2(CRGB *_leds, int num_leds ) {
  int timeA = 30000;
  int timeB = 35000;
  int timeC = 65000;
  int timeEND = 70000;
  
  int timerInCycle = millis() % timeEND;
  int hueRangeMin = 0;
  int hueRangeMax = 255;
  if (timerInCycle < timeA) {
    hueRangeMin = 180;
    hueRangeMax = 240;
  } else if (timerInCycle < timeB) {
    hueRangeMin = map(timerInCycle, timeA, timeB, 180, 80);
    hueRangeMax = map(timerInCycle, timeA, timeB, 240, 160);
  } else if (timerInCycle < timeC) {
    hueRangeMin = 80;
    hueRangeMax = 160;
  } else if (timerInCycle < timeEND) {
    hueRangeMin = map(timerInCycle, timeA, timeEND, 80, 180);
    hueRangeMax = map(timerInCycle, timeA, timeEND, 160, 240);
  }

  for (uint16_t i = 0; i < num_leds; i++) {
    int speedBri2 = 10;
    int waveFraction2 = 4;
    int sinBri2 = sin16((i * 65536) / (num_leds * waveFraction2) - millis() * speedBri2 );
    int bri2 = map(sinBri2, -32767, 32767, 50, 255);
    bri2 = constrain(bri2, 50, 255);

    int speedbri3 = 4;
    int waveFraction3 = 2;
    int sinbri3 = sin16((i * 65536) / (num_leds * waveFraction3) - millis() * speedbri3);
    int bri3 = map(sinbri3, -32767, 32767, -100, 200);
    bri3 = constrain(bri3, 60, 200);

    int speedbri1 = 20;
    int numWavesBri = 1;
    int sinbri1 = sin16((i * 65536 * numWavesBri) / num_leds - millis() * speedbri1);
    int bri1 = map(sinbri1, -32767, 32767, -10, 50);
    
    int bri = max(max(bri2, bri2), bri3);
    bri = constrain(bri, 80, 120);
    bri = bri % 256;

    int speed1 = 8;
    int numWaves1 = 2;
    int sin1 = sin16(-(numWaves1 * i * (65536 / num_leds) + (65536 * 3 / 4) - millis() * speed1));
    int hue1 = map(sin1, -32767, 32767, 0, 256);

    int speed2 = -8;
    int numWaves2 = 3;
    int sin2 = sin16((numWaves2 * i * (65536 / num_leds) + (65536 * 2 / 4) - millis() * speed2));
    int hue2 = map(sin2, -32767, 32767, 0, 245);

    int speed3 = 10;
    int numWaves3 = 1;
    int sin3 = sin16((numWaves3 * i * (65536 / num_leds) + (65536 * 1 / 4) - millis() * speed3));
    int hue3 = map(sin3, -32767, 32767, 0, 150);

    int hue = max(max(hue1, hue2), hue3);
    //hue = hue1;
    hue = hue % 256;
    hue = constrain(hue, hueRangeMin, hueRangeMax);

    int sat = 255;
    int brightness = bri;
    _leds[i] = CHSV(hue, 255, brightness);
    //EVERY_N_MILLISECONDS(30){
    //FastLED.show();
    //}
  }
  //FastLED.show();
  //apply_to_all_led_from_leds();       
  //Serial.println(String(timerInCycle) + "\t" + String(hueRangeMin) + "\t" + String(hueRangeMax));
  //EVERY_N_MILLISECONDS(10){
  FastLED.show();
  //};
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride(CRGB *_leds, int num_leds) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < num_leds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (num_leds-1) - pixelnumber;
    
    nblend( _leds[pixelnumber], newcolor, 64);
  }
  FastLED.show();
}


// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride_blue(CRGB *_leds, int num_leds) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = (ms - sLastMillis)/2 ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < num_leds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    //CRGB newcolor = CHSV( hue8, sat8, bri8);
    CRGB newcolor = CHSV( map(hue8,0,255,100,140), sat8, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (num_leds-1) - pixelnumber;
    
    nblend( _leds[pixelnumber], newcolor, 64);
  }
  FastLED.show();
}


void fadeOutLeds(int durationMillis) {
  int fadeSteps = 256; // Number of steps for fading (0-255)
  int fadeDelay = durationMillis / fadeSteps;

  for (int i = 255; i >= 0; i--) {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(fadeDelay);
  }

  // Turn off all LEDs
  FastLED.clear();
   FastLED.setBrightness(255);
  FastLED.show();
}

 




void fadeOutLeds(CRGB *_leds, int num_leds, int durationMillis) {
  int fadeSteps = 256; // Number of steps for fading (0-255)
  int fadeDelay = durationMillis / fadeSteps;

  for (int i = 255; i >= 0; i--) {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(fadeDelay);
  }

  // Turn off all LEDs
  for (int i = 0; i < num_leds; i++) {
    _leds[i] = CRGB::Black;
  }
  FastLED.setBrightness(255);
  FastLED.show();
}

CHSV getLEDColor(int displayValue) {
    int hue = map(displayValue, 0, 255, 50, 255); // Maps displayValue from a blue to red hue.
    return CHSV(hue, 255, 255);  // Full saturation and full value for maximum brightness.
}

CRGB getLEDColorRGB(int displayValue) {
    if (displayValue > 200) return CRGB::Red;
    if (displayValue > 100) return CRGB::Yellow;
    return CRGB::Green; 
}

void rainbow(CRGB *_leds, int num_leds) {
    gHue1++;
    gHue2++;
    fill_rainbow(_leds, num_leds, gHue1);
    FastLED.show();
}



void addGlitter(CRGB *_leds, int num_leds) {
    EVERY_N_MILLISECONDS(1000/30){
        if( random8() < 80) {
            _leds[random16(num_leds)] += CRGB::White;
        }
    }
}

void confetti(CRGB *_leds, int num_leds) {
    EVERY_N_MILLISECONDS(1000/30){
        fadeToBlackBy(_leds, num_leds, 30);
        int pos = random16(num_leds/2);
        _leds[pos] += CHSV(gHue1 + random8(64), 190 + random8(65), 255);
    }
}

// ... continue the modifications for other functions ...

// ... continue for all functions ...


void dot_beat(CRGB *_right,CRGB *_left, int num_leds) {
    uint8_t fadeval = 10;       // Trail behind the LED's. Lower => faster fade.
    // nscale8(leds_0, num_leds, fadeval);    // Fade the entire array. Or for just a few LED's, use  nscale8(&leds_0[2], 5, fadeval);
    fadeToBlackBy( _right, num_leds, fadeval);

    uint8_t BPM, inner, outer, middle;
    
    BPM = 33;

    inner  = beatsin8(BPM, num_leds/2/4, num_leds/2/4*3);    // Move 1/4 to 3/4
    outer  = beatsin8(BPM, 0, num_leds/2-1);               // Move entire length
    middle = beatsin8(BPM, num_leds/2/3, num_leds/2/3*2);   // Move 1/3 to 2/3

    _left[outer]  = CHSV( gHue1    , 200, 255);
    _left[middle] = CHSV( gHue1+96 , 200, 255);
    _left[inner]  = CHSV( gHue1+160, 200, 255);

    BPM = 31;
    
    inner  = beatsin8(BPM, num_leds/2/4, num_leds/2/4*3);    // Move 1/4 to 3/4
    outer  = beatsin8(BPM, 0, num_leds/2-1);               // Move entire length
    middle = beatsin8(BPM, num_leds/2/3, num_leds/2/3*2);   // Move 1/3 to 2/3

    _right[outer]  = CHSV( gHue2    , 200, 255);
    _right[middle] = CHSV( gHue2+96 , 200, 255);
    _right[inner]  = CHSV( gHue2+160, 200, 255);

} // dot_beat()

void juggle(CRGB *_right,CRGB *_left, int num_leds) {
    // colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds_0, num_leds, 5);
    byte dothue1 = 0, dothue2 = 0;
    for( int i = 0; i < 6; i++) {
        _right[beatsin16(i+7,0,num_leds/2-1)] |= CHSV(dothue1, 200, 255);
        _left[beatsin16(i+5,0,num_leds/2-1)] |= CHSV(dothue2, 200, 255);
        dothue1 += 32;
        dothue2 -= 32;
        yield();
    }
}

void bpm(CRGB *_right,CRGB *_left, int num_leds) 
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    // CRGBPalette16 palette = PartyColors_p;
    CRGBPalette16 palette = RainbowColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < num_leds/2; i++) { //9948
        _right[i]              = ColorFromPalette(palette, gHue1+(i*2), beat-gHue1+(i*10));
        _left[num_leds/2-1-i] = ColorFromPalette(palette, gHue2+(i*2), beat-gHue2+(i*10));
        yield();
    }
}

void blendwave(CRGB *_right, CRGB *_left, int num_leds) {
    CRGB clr1, clr2;
    uint8_t speed, loc1;

    speed = beatsin8(6,0,255);

    clr1 = blend(CHSV(beatsin8(3,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), speed);
    clr2 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(3,0,255),255,255), speed);
    loc1 = beatsin8(13,0,num_leds/2-1);

    fill_gradient_RGB(_left, 0, clr2, loc1, clr1);
    fill_gradient_RGB(_left, loc1, clr2, num_leds/2-1, clr1);
    
    speed = beatsin8(7,0,255);

    clr1 = blend(CHSV(beatsin8(4,0,255),255,255), CHSV(beatsin8(5,0,255),255,255), speed);
    clr2 = blend(CHSV(beatsin8(5,0,255),255,255), CHSV(beatsin8(4,0,255),255,255), speed);
    loc1 = beatsin8(11,0,num_leds/2-1);

    fill_gradient_RGB(_right, 0, clr2, loc1, clr1);
    fill_gradient_RGB(_right, loc1, clr2, num_leds/2-1, clr1);
} // blendwave()

/*
uint8_t fadeval = 235, frameRate = 45;
void fire(CRGB *_right, CRGB *_left, int num_leds){ // my own simpler 'fire' code - randomly generate fire and move it up the strip while fading
    EVERY_N_MILLISECONDS(1000/frameRate){
        for(int i = 0; i < num_leds/4; i++){
            R1[i] = R1[i+1].nscale8(fadeval); if(R1[i].g > 0) R1[i].g--;
            //R2[num_leds/4-i] = R1[i];
            L1[i] = R1[i+1].nscale8(fadeval); if(L1[i].g > 0) L1[i].g--;
            //L2[num_leds/4-i] = L1[i];
        }
        uint8_t _hue = 0, _sat = 255, _val = 0;
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R1[num_leds/4-1] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R2[0] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L1[num_leds/4-1] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L2[0] = CHSV( _hue, _sat, _val*_val/255);
    }
}
*/
void fireSparks(CRGB *_leds, int num_leds){ // randomly generate color and move it up the strip while fading, plus some yellow 'sparkles'
    /*EVERY_N_MILLISECONDS(1000/frameRate){
        for(int i = 0; i < num_leds/4; i++){
            R1[i] = R1[i+1].nscale8(fadeval); if(R1[i].g > 0) R1[i].g--;
            R2[num_leds/4-i] = R1[i];
            L1[i] = R1[i+1].nscale8(fadeval); if(L1[i].g > 0) L1[i].g--;
            L2[num_leds/4-i] = L1[i];
        }
        uint8_t _hue = 0, _sat = 255, _val = 0;
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R1[num_leds/4-1] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R2[0] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L1[num_leds/4-1] = CHSV( _hue, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L2[0] = CHSV( _hue, _sat, _val*_val/255);
        EVERY_N_MILLISECONDS(1000/10){
            CRGB spark = CRGB::Yellow;
            if( random8() < 80)
                R1[num_leds/4-1-random8(num_leds/8)] = spark;
            if( random8() < 80)
                R2[random8(num_leds/8)]              = spark;
            if( random8() < 80)
                L1[num_leds/4-1-random8(num_leds/8)] = spark;
            if( random8() < 80)
                L2[random8(num_leds/8)]              = spark;
        }
    }*/
}

void fireRainbow(CRGB *_leds, int num_leds){ // same as fire, but with color cycling
   /* EVERY_N_MILLISECONDS(1000/frameRate){
        for(int i = 0; i < num_leds/4; i++){
            R1[i] = R1[i+1].nscale8(fadeval);
            R2[num_leds/4-i] = R1[i];
            L1[i] = R1[i+1].nscale8(fadeval);
            L2[num_leds/4-i] = L1[i];
        }
        uint8_t _hue = 0, _sat = 255, _val = 0;
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R1[num_leds/4-1] = CHSV( _hue+gHue1, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        R2[0] = CHSV( _hue+gHue1, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L1[num_leds/4-1] = CHSV( _hue+gHue2, _sat, _val*_val/255);
        _val = random(0, 255);
        _sat = 255 - (_val/255.0 * 50);
        _hue = _val/255.0 * 55;
        L2[0] = CHSV( _hue+gHue2, _sat, _val*_val/255);
    }*/
}

uint8_t blurval = 150;
void ripple_blur(CRGB *_right,CRGB *_left, int num_leds) { // randomly drop a light somewhere and blur it using blur1d
    EVERY_N_MILLISECONDS(1000/30){
        //blur1d( ledsx(0         , num_leds/2-1), num_leds/2, blurval);
        //blur1d( leds_0(num_leds/2, num_leds    ), num_leds/2, blurval);
    }
    EVERY_N_MILLISECONDS(30){
        if( random8() < 15) {
            uint8_t pos = random(num_leds/2);
            _left [pos] = CHSV(random(0, 64)+gHue1, random(250, 255), 255);
        }
        if( random8() < 15) {
            uint8_t pos = random(num_leds/2);
            _right [pos] = CHSV(random(0, 64)-gHue2, random(250, 255), 255);
        }
    }
}
