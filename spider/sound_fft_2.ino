/*----------------------------------------------------------------------------------------------------*\
My code was originally copied from G6EJD on Github as well as the ArduinoFFT library examples:
    https://github.com/G6EJD/ESP32-8-Octave-Audio-Spectrum-Display
    https://github.com/kosme/arduinoFFT

However I've modified the way it displays to look better on 
my single LED strips. It will have to be slightly modified for
different numbers of LEDs, for the best output.

FFT primer: (F)ast (F)ourier (T)ransform
  The "Nyquist" frequency is the *highest* frequency you can detect.
  It is exactly half your sampling rate - basically, you can reconstruct
  nearly any analog wave with a MINIMUM of 2 points.
  So, to detect sounds up to x Hz (your Nyquist frequency), you MUST sample at 2*x sps(samples per second)
  15kHz Nyquist = 30ksps sampling frequency
 
  The number of samples you take *at that sampling speed* determines the *lowest* frequency you can detect.
  Sampling_freq / num_samples = lowest frequency
  40000 kHz / 512 samples ~~> 78 Hz
  NOTE: by the end of the FFT calculations, your samples are transformed into samples/2 usable frequency
  values, so we discard the top half of the data

If your ESP32 is running at 240MHz, it can handily handle:
- (mono) one channel of audio at 1024 samples, or
- (stereo) two independent channels at 512 samples per channel
If, like mine, your ESP32 is locked at 160 MHz, stick with one channel, 512 samples 
\*----------------------------------------------------------------------------------------------------*/
//#define debug
#define LeftPin  34
#define RightPin 39

#define samples  512 // must ALWAYS be a power of 2 // VERY IMPORTANT
#define samplingFrequency 25000 // samples per second, not to be confused with Nyquist frequency which will be half of this
//// you don't always need all sounds up to 20kHz
  // 10-12kHz tends to work fine for most songs and speech
  // also picks up lower frequencies due to the math relation mentioned above
  // 25000 / 512 ~~> 50Hz

#define noise 1500
#define MAX 50000
int scale_vreal = 8;
int max_value_yres = 40;

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[2][samples];
double vImag[2][samples];
double spectrum[3][samples/2];

arduinoFFT LFFT = arduinoFFT(vReal[0], vImag[0], samples, samplingFrequency);
arduinoFFT RFFT = arduinoFFT(vReal[1], vImag[1], samples, samplingFrequency);
#define NUM_LEDS 180


void set_scale_vreal(int val) {
    scale_vreal = val;
}

int get_scale_vreal() {
    return scale_vreal;
}

void set_max_value_yres(int val) {
    max_value_yres = val;
}

int get_max_value_yres() {
    return max_value_yres;
}

int get_freq_val(CRGB *_leds ,int num_leds, int base){
   CRGB tempRGB1, tempRGB2;
    uint8_t pos = 0, h = 0, s = 0, v = 0;
    double temp1 = 0, temp2 = 0;
    int v_total = 0;
    int s_total = 0;
    int range = 40;
    for(int i = base; i < base + range && i < 120; i++){
        pos = spectrum[0][i];
        h = pos/(num_leds/2.0)*224;
        temp1 = spectrum[1][i]/MAX;
        s = 255 - (temp1*30.0);
        v = temp1*255.0;
        v_total= v_total + v;
        s_total= s_total + s;
        tempRGB1 = CHSV(h, s, v);
        //if(tempRGB1 > _leds[pos]){
        //    _leds[pos] = tempRGB1;
        //}

        uint8_t p = num_leds/2-1-pos;
    }
    return v_total/range;
}

void music(CRGB *_leds, int num_leds, CRGB baseColor) {

    uint8_t fadeval = 90;
    nscale8(_leds, num_leds, fadeval); // smaller = faster fade
    CRGB tempRGB1, tempRGB2;
    uint8_t pos = 0, h = 0, s = 0, v = 0;
    double temp1 = 0, temp2 = 0;
    for(int i = 2; i < samples/2; i++){
        pos = spectrum[0][i];
        h = pos/(num_leds/2.0)*224;
        temp1 = spectrum[1][i]/MAX;
        s = 255 - (temp1*30.0);
        v = temp1*255.0;
        tempRGB1 = CHSV(h, s, v);
        if(tempRGB1 > _leds[pos]){
            _leds[pos] = tempRGB1;
        }

        uint8_t p = num_leds/2-1-pos;
#ifdef STEREO
        temp2 = spectrum[2][i]/MAX;
        s = 255 - (temp2*30.0);
        v = temp2*255.0;
        tempRGB2 = CHSV(h, s, v);
        if(tempRGB2 > LEFT[pos]){
            LEFT[p] = tempRGB2;
        }
#else
        //LEFT[p] = RIGHT[pos];
#endif
        yield();
    }
    FastLED.show();
}

void fftSetup(){
    sampling_period_us = round(1000000*(1.0/samplingFrequency));
    
    double exponent = 0.66;  //// this number will have to change for the best output on different numbers of LEDs and different numbers of samples.
    for (uint16_t i = 2; i < samples/2; i++){
        spectrum[0][i] = pow((i-2)/(samples/2.0-2), exponent) * NUM_LEDS; // **
        spectrum[1][i] = 0; // left  channel values
        spectrum[2][i] = 0; // right channel values
    }
    // ** the purpose of this line is to set up the 'logarithmic' spacing 
    // of the LED lights for different frequencies. The human perception of audio
    // frequency doesn't ascend linearly, so 
    // 120Hz vs 130Hz is 'musically' (and proportionally) a bigger change than
    // 1020Hz vs 1030Hz which is itself a bigger change than 
    // 10020Hz vs 10030 Hz
    // For this reason, I tried to set the exponent such that the lowest frequencies
    // each get their 'own' LED spot, while the higher frequencies 'share' LED spots
     // right now it is optimized for 72 LEDs, 512 samples ~ 0.66
     // with more LEDs you'll want a higher exponent, less LEDs a lower exponent, but it's a logarithmic relationship not linear.
     // let me know if you figure out a way to calculate it automatically, I figured it manually by trying different numbers.
     
    for (uint16_t i = 0; i < samples; i++){
        vReal[0][i] = 0; vReal[1][i] = 0;
        vImag[0][i] = 0; vImag[1][i] = 0;
    }
}

//// remember that this code is running independently of your 
  // main LED code, on the other core of the ESP32.
void fftLoop(){
#ifdef debug
    Serial.println("Starting fftLoop");
#endif

    //// audio signal capture happens here
    microseconds = micros();
    for(int i=0; i<samples; i++){
        vReal[0][i] = analogRead(LeftPin);
        vImag[0][i] = 0;
#ifdef STEREO
        vReal[1][i] = analogRead(RightPin);
        vImag[1][i] = 0;
#endif
        while(micros() - microseconds < sampling_period_us){  }
        microseconds += sampling_period_us;
    }

    //// FFT magic happens here
    LFFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    LFFT.Compute(FFT_FORWARD);
    LFFT.ComplexToMagnitude();
    //// audio frequencies are output here into the 'spectrum' array which is then read in the LED code
    PrintVector(vReal[0], (samples >> 1), 1);

#ifdef STEREO
    //// FFT magic happens here
    RFFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    RFFT.Compute(FFT_FORWARD);
    RFFT.ComplexToMagnitude();
    //// audio frequencies are output here into the 'spectrum' array which is then read in the LED code
    PrintVector(vReal[1], (samples >> 1), 2);
#endif

#ifdef debug
    Serial.println("Ending fftLoop");
#endif
}

void PrintDrumValues(double *vData, uint16_t bufferSize, int leftRight) {
    int startBin = 0;
    int endBin = 0;

    // Define the frequency ranges for the drums
    // Adjust these ranges as needed for your specific drum sounds
    if (leftRight == 1) {
        // Left channel (adjust these values for kick, snare, etc.)
        startBin = map(50, 0, samplingFrequency / 2, 0, bufferSize);
        endBin = map(10000, 0, samplingFrequency / 2, 0, bufferSize);
    } else if (leftRight == 2) {
        // Right channel (adjust these values for cymbals, toms, etc.)
        startBin = map(1000, 0, samplingFrequency / 2, 0, bufferSize);
        endBin = map(10000, 0, samplingFrequency / 2, 0, bufferSize);
    }

    for (uint16_t i = startBin; i < endBin; i++) {
        if (vData[i] > noise) {
            spectrum[leftRight][i] = vData[i] - noise;
            if (spectrum[leftRight][i] > MAX) {
                spectrum[leftRight][i] = MAX;
            }
            if((vData[i] - noise) >5000){
              Serial.print(vData[i] - noise);
              Serial.print(":");
              Serial.print(i);
              Serial.print(",");
            }
        } else {
            spectrum[leftRight][i] = 0;
        }
        yield();
    }
    Serial.print("\n");
}

void PrintVector(double *vData, uint16_t bufferSize, int leftRight) {
    //PrintDrumValues(vData, bufferSize, leftRight);
    //return;
    for (uint16_t i = 2; i < bufferSize; i++){
        if(vData[i] > noise){
            spectrum[leftRight][i] = vData[i]-noise;
            if(spectrum[leftRight][i] > MAX)
                spectrum[leftRight][i] = MAX;
                 if((vData[i] - noise) >1000){
        Serial.print(vData[i]-noise);
        Serial.print(":");
        Serial.print(i);
        Serial.print(",");
                 }
        }else{
            spectrum[leftRight][i] = 0;
        }
        yield();
    }
  Serial.print("\n");
}