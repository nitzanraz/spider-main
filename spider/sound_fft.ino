#include <arduinoFFT.h>
#include <arduinoFFT.h>
#ifdef XXX
// Configuration and Constants
#define SAMPLES 128
#define SAMPLING_FREQUENCY 1000
#define xres 32
//#define yres 12
#define noise 1500
const int SAMPLE_RATE = 44100;

// Arrays for different modes
int MY_ARRAY[] = {0, 128, 192, 224, 240, 248, 252, 254, 255, 8, 16, 32, 64, 80, 90, 100, 110};
int MY_MODE_1[] = {0, 128, 192, 224, 240, 248, 252, 254, 255};
int MY_MODE_2[]={0, 128, 64, 32, 16, 8, 4, 2, 1};
int MY_MODE_3[]={0, 128, 192, 160, 144, 136, 132, 130, 129};
int MY_MODE_4[]={0, 128, 192, 160, 208, 232, 244, 250, 253};
int MY_MODE_5[]={0, 1, 3, 7, 15, 31, 63, 127, 255};
// ... [Add other modes here]

// FFT Variables and Configuration
double spectrum[3][SAMPLES / 2];
double vReal[512];
double vImag[512];
int peaks[xres];
char data_avgs[xres];
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);
unsigned int sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
int scale_vreal = 8;
int max_value_yres = 40;
#define noise 1500

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

void fftSetup() {
    /*for (uint16_t i = 2; i < SAMPLES / 2; i++) {
        spectrum[0][i] = pow((i - 2) / (double)(SAMPLES / 2.0 - 2), 0.66) * NUM_LEDS_0;
        spectrum[1][i] = 0;
        spectrum[2][i] = 0;
    }*/
    for (uint16_t i = 0; i < SAMPLES; i++) {
        vReal[i] = 0;
        vImag[i] = 0;
    }
}

void run_fft() {
    unsigned long microseconds;
    for (int i = 0; i < SAMPLES; i++) {
        microseconds = micros();
        vReal[i] = analogRead(34) / scale_vreal;
        vImag[i] = 0;
        while (micros() - microseconds < sampling_period_us) {}
    }
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    // Processing and Binning of FFT output
    int step = (SAMPLES / 2) / xres;
    for (int i = 0, c = 0; i < SAMPLES / 2; i += step, c++) {
        for (int k = 0; k < step; k++) {
            data_avgs[c] += vReal[i + k];
        }
        data_avgs[c] /= step;
    }
}

void setColumn(CRGB *_leds, int _displaycolumn, int column_sound_level, int num_leds) {
    // [Implementation here]
    int _yres = num_leds / xres;
    int columon_start_index = _displaycolumn *  _yres;
   //Serial.print("setColumn:");Serial.print(_displaycolumn); Serial.print(","); Serial.print(_displayvalue); Serial.println(";");
   CRGB color = CRGB::Black;
   int color_index = 0 ;
   for(int i = 4; i < column_sound_level; i++){
      //color = getLEDColorRGB(MY_ARRAY[i]);
      color = getLEDColor(MY_ARRAY[i]);
      color = CHSV(map(_displaycolumn, 0, xres,0,255),255,255);
      if( columon_start_index + i < num_leds){
        _leds[columon_start_index + i] = color;
      }else{
        Serial.printf("%d > %d , Col start = %d Error \n",columon_start_index+i,num_leds, columon_start_index  );
      }
   }
   for(int i = column_sound_level; i< _yres; i++){
      //if (i < _displayvalue ){
      //color = CRGB::Black;
      if( columon_start_index + i < num_leds){
        color = CHSV(map(_displaycolumn, 0, xres,255,0),255,5);
        _leds[columon_start_index +i] = color; //CRGB::Black;
      }else{
        Serial.printf("%d > %d , Col start = %d Error \n",columon_start_index+i,num_leds, columon_start_index  );      }
   }

}


void music(CRGB *_leds, int num_leds, CRGB baseColor) {
    // [Implementation here]
    // ++ send to display according measured value 
    EVERY_N_MILLISECONDS(120){
      int displaycolumn , displayvalue;
      int max_bands = 10;//was xres
      int start_band = 0;
      int _yres = num_leds/xres;
      /*for(int i=0; i < xres; i++)
      {
        
        data_avgs[i] = constrain(data_avgs[i],0,max_value_yres);            // set max & min values for buckets
        data_avgs[i] = map(data_avgs[i],  0, max_value_yres, 0, _yres);        // remap averaged values to yres
        int yvalue=data_avgs[i];
        peaks[i] = (yvalue > peaks[i]) ? yvalue : peaks[i] - 1;
        yvalue = peaks[i];
        displaycolumn =  i;
        //Serial.printf("data_avgs[i] %d xres %d yres %d numleds %d max_value_yres %d yvalue %d displaycolumn %d \n"
        //              ,data_avgs[i],xres, _yres ,num_leds, max_value_yres, yvalue, displaycolumn);
        setColumn(_leds, displaycolumn, yvalue, num_leds);              //  for left to right
      }*/
      // Shift the colors from LED i to i+
      _leds[0] = map(data_avgs[0],  0, max_value_yres, 0, 255);        // remap averaged values to yres

      for (int i = num_leds - 1; i > 0; i--) {
        _leds[i] = leds[i - 1];
      }
      //apply_to_all_led_from_leds();
      FastLED.show();
  }
}

void music1(CRGB *_leds, int num_leds, CRGB baseColor) {
    // [Implementation here]
    // ++ send to display according measured value 
    EVERY_N_MILLISECONDS(60){
      int displaycolumn , displayvalue;
      int max_bands = 10;//was xres
      int start_band = 0;
      int _yres = num_leds/xres;
      for(int i=0; i < xres; i++)
      {
        
        data_avgs[i] = constrain(data_avgs[i],0,max_value_yres);            // set max & min values for buckets
        data_avgs[i] = map(data_avgs[i],  0, max_value_yres, 0, _yres);        // remap averaged values to yres
        int yvalue=data_avgs[i];

        peaks[i] = peaks[i]-1;    // decay by one light
        if (yvalue > peaks[i])  
            peaks[i] = yvalue ;
        yvalue = peaks[i];    
        displaycolumn =  i;
        //Serial.printf("data_avgs[i] %d xres %d yres %d numleds %d max_value_yres %d yvalue %d displaycolumn %d \n"
        //              ,data_avgs[i],xres, _yres ,num_leds, max_value_yres, yvalue, displaycolumn);
        setColumn(_leds, displaycolumn, yvalue, num_leds);              //  for left to right
      }
      //apply_to_all_led_from_leds();
      _leds[0]= CRGB::Red;
      FastLED.show();
  }
}

// Other functions and main loop can be added below.

/*

//https://github.com/ohnoitsalobo/sound-reactive-esp32/blob/master/src/FFT.ino
#define SAMPLES 128            
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC
#define samplingFrequency 25000 // samples per second, not to be confused with Nyquist frequency which will be half of this
#define xres              16 //32     // number of bands = xres/2
#define yres              14
#define noise             1500
#define MAX               50000
#define samples           SAMPLES// must ALWAYS be a power of 2 // VERY IMPORTANT
double spectrum[3][samples/2];
double vReal[512];
double vImag[512];
int    peaks[xres];
char   data_avgs[xres];
int scale_vreal = 8;
int max_value_yres = 80;
int MY_ARRAY[]={0, 128, 192, 224, 240, 248, 252, 254, 255,8,16,32,64,80,90,100,110};
//int MY_ARRAY[]={0, 128, 192, 224, 240, 248, 252, 254, 255,0,0,0,0,0,0,0,0};
int MY_MODE_1[]={0, 128, 192, 224, 240, 248, 252, 254, 255};
int MY_MODE_2[]={0, 128, 64, 32, 16, 8, 4, 2, 1};
int MY_MODE_3[]={0, 128, 192, 160, 144, 136, 132, 130, 129};
int MY_MODE_4[]={0, 128, 192, 160, 208, 232, 244, 250, 253};
int MY_MODE_5[]={0, 1, 3, 7, 15, 31, 63, 127, 255};
//const int SAMPLING_FREQUENCY_MSEC= 5;
const int SAMPLE_RATE = 44100;  // 44.1 kHz sampling rate
unsigned long previousMillis = 0;
unsigned long interval = 1000 / SAMPLE_RATE;  // Sampling interval in milliseconds
unsigned int sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));

int yvalue;
int displaycolumn , displayvalue;
//arduinoFFT FFT = arduinoFFT();
arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);

void set_scale_vreal(int val){
  scale_vreal = val;
}
int get_scale_vreal(){
  return scale_vreal;
}

void set_max_value_yres(int val){
  max_value_yres = val;
}
int get_max_value_yres(){
  return max_value_yres;
}
void fftSetup(){
    sampling_period_us = round(1000000*(1.0/samplingFrequency));
    
    double exponent = 0.66;  //// this number will have to change for the best output on different numbers of LEDs and different numbers of samples.
    for (uint16_t i = 2; i < samples/2; i++){
        spectrum[0][i] = pow((i-2)/(samples/2.0-2), exponent) * NUM_LEDS_0; // **
        spectrum[1][i] = 0; // left  channel values
        spectrum[2][i] = 0; // right channel values
    }     
    for (uint16_t i = 0; i < samples; i++){
        vReal[i] = 0; //vReal[1][i] = 0;
        vImag[i] = 0; //vImag[1][i] = 0;
    }
}

void run_fft(){
   int scale_vreal = 8;
   for(int i=0; i<SAMPLES; i++)
    {
      microseconds = micros(); 
      int value = analogRead(34);               
      vReal[i]= value/scale_vreal;                      
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){  }
      microseconds += sampling_period_us;

     //while(micros() < (microseconds + sampling_period_us)){
      //}
    }
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
     // The frequency range for a standard six-string guitar is roughly 82 Hz (E2) to 1319 Hz (E6)
    //double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY_MSEC*1000);
    //int peakIndex = getMajorPeakIndex(vReal, SAMPLES);
    //if (peak >= 80 && peak <= 1400) {
        //Serial.println("Guitar sound detected!");
    //}
    int step = (SAMPLES/2)/xres; //=1
    int c=0;
    for(int i=0; i<(SAMPLES/2); i+=step)  // 32
    {
      data_avgs[c] = 0;
      for (int k=0 ; k< step ; k++) {
          data_avgs[c] = data_avgs[c] + vReal[i+k];
      }
      data_avgs[c] = data_avgs[c]/step; 
      c++;
      //Serial.print((int)data_avgs[c]);Serial.print(",");
      //Serial.print((int)vReal[i]);Serial.print(",");
    }
    //Serial.println(".");
}

void setColumn(CRGB *_leds, int _displaycolumn, int _displayvalue, int num_leds) {
   int position = _displaycolumn *  yres;
   //Serial.print("setColumn:");Serial.print(_displaycolumn); Serial.print(","); Serial.print(_displayvalue); Serial.println(";");
   CRGB color = CRGB::Black;
   int color_index = 0 ;
   for(int i = 0; i < _displayvalue; i++){
      //color = getLEDColorRGB(MY_ARRAY[i]);
      color = getLEDColor(MY_ARRAY[i]);
      if( i < num_leds){
        _leds[position + i] = color;
      }else{
        Serial.print(i);Serial.println("error;");
      }
   }
   for(int i = _displayvalue; i< yres; i++){
      //if (i < _displayvalue ){
      color = CRGB::Black;
      if( i < num_leds){
        _leds[i+position] = color;
      }else{
        Serial.print(i);Serial.println("error;");
      }
   }
}


void music(CRGB *_leds, CRGB baseColor, int num_leds) {
 // ++ send to display according measured value 
    int displaycolumn , displayvalue;
    int max_bands = 10;//was xres
    int start_band = 0;
    for(int i=start_band; i < start_band+8 && i < xres; i++)
    {
      
      data_avgs[i] = constrain(data_avgs[i],0,max_value_yres);            // set max & min values for buckets
      data_avgs[i] = map(data_avgs[i],  0, max_value_yres, 0, yres);        // remap averaged values to yres
      yvalue=data_avgs[i];

      peaks[i] = peaks[i]-1;    // decay by one light
      if (yvalue > peaks[i])  
          peaks[i] = yvalue ;
      yvalue = peaks[i];    
      displayvalue=MY_ARRAY[yvalue];
      displaycolumn= i;
      //setColumn(_leds, displaycolumn, displayvalue, num_leds);              //  for left to right
      setColumn(_leds, displaycolumn, yvalue, num_leds);              //  for left to right
     }
    apply_to_all_led_from_leds();
    FastLED.show();
}


*/
#endif
