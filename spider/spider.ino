
// Includes
#include <ArduinoOTA.h>
#include <FastLED.h>

#ifdef ESP32
    #include <WiFi.h>
    #include <ESPmDNS.h>
    #include <AsyncTCP.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESPAsyncTCP.h>
    #include <ESP8266mDNS.h>
#endif

#include <ESPAsyncWebSrv.h>
#include "leds.h"

// Definitions
#define FASTLED_ESP8266_D1_PIN_ORDER
#define TOTAL_NUM_LEDS 1200
#define BRIGHTNESS 250
#define SPEED 10
#define SPIDER
#ifdef SPIDER
    #define NUM_LEDS_0 240
    #define NUM_LEDS_1 50
    #define NUM_LEDS_2 50
    #define NUM_LEDS_3 50
    #define NUM_LEDS_C 10
    #define LED_0_PIN  2 // Amudim
    #define LED_1_PIN  4  
    #define LED_2_PIN  22
    #define LED_3_PIN  23
    #define MAX_POWER_MILLIAMPS  (60 * TOTAL_NUM_LEDS)
bool just_dance = false;
#else // JUST_DANCE
    #define NUM_LEDS_0 150 //Amudim
    #define NUM_LEDS_1 150 //Face 130
    #define NUM_LEDS_2 150 // Mishgeret 170
    #define NUM_LEDS_3 150 //Oznaim
    #define NUM_LEDS_C 10
    #define LED_0_PIN  2  //Amudim
    #define LED_1_PIN  19 // face 
    #define LED_2_PIN  5  // Mishgeret 23 right 30 buttom 24 lef 30 top 107
    #define LED_3_PIN  18  // Oznaim 150
    #define LED_PIN_C  23
    #define MAX_POWER_MILLIAMPS (30 * (NUM_LEDS_1+NUM_LEDS_2 + NUM_LEDS_3 ))
bool just_dance = true;
#endif
const int press_button_pin = 21;
const int mode_button_pin = 23;

// air pump
extern void setup_2_relay();
extern void relay_loop();

// Forward declarations
extern void run_just_dance();
extern void notify_button_pressed();
void just_dance_led_test();
void music(CRGB *_leds, int num_leds, CRGB baseColor);
void run_fft();
void fftSetup();
void handleRoot(AsyncWebServerRequest *request);
void handleUpdate(AsyncWebServerRequest *request);
void fadeOutLeds(int durationMillis);
void set_max_value_yres(int val);
int  get_max_value_yres();
void set_scale_vreal(int val);
int  get_scale_vreal();
uint8_t get_gCurrentPatternNumber();
void set_gCurrentPatternNumber(uint8_t val);

// Global variables
enum State { TEST, SHOW_UP, SHOW_DOWN, WAVE, MUSIC, CONFFETI, PRIDE };
State currentState = TEST;
unsigned long stateChangeTime = 0;
const char* apSSID = "spider";
const char* apPassword = "1111111111";
AsyncWebServer server(80);
uint8_t gHue = 0, gHue1 = 0, gHue2 = 0;
int opt1 = 0, opt2 = 0, opt3 = 3, opt4 = 0, opt5 = 0;
bool pota = false;

CRGBPalette16 currentPalette, targetPalette, randomPalette1, randomPalette2;
TBlendType currentBlending;
uint8_t maxChanges = 24;
TaskHandle_t Core0Task, Core1Task;
CRGB leds_0[NUM_LEDS_0], leds_1[NUM_LEDS_1], leds_2[NUM_LEDS_2], leds_3[NUM_LEDS_3], leds_c[NUM_LEDS_C];

void setup() {
  fftSetup();
  Serial.begin(9600);   // Initialize serial communication at 9600 baud rate
  Serial.println("Starting");
  pinMode(LED_0_PIN, OUTPUT);  // Set the button as an input with an internal pull-up resistor
  pinMode(LED_1_PIN, OUTPUT);  // Set the button as an input with an internal pull-up resistor
  pinMode(LED_2_PIN, OUTPUT);  // Set the button as an input with an internal pull-up resistor
  pinMode(LED_3_PIN, OUTPUT);  // Set the button as an input with an internal pull-up resistor
  pinMode(press_button_pin, INPUT_PULLUP);
  pinMode(mode_button_pin, INPUT_PULLUP);

  //pinMode(buttonPin, INPUT_PULLUP);  // Set the button as an input with an internal pull-up resistor
  FastLED.addLeds<WS2812B, LED_0_PIN, GRB>(leds_0, sizeof(leds_0)/3);
  //FastLED.addLeds<WS2811, LED_1_PIN, GRB>(leds_1, sizeof(leds_1)/3);
  //FastLED.addLeds<WS2811, LED_2_PIN, GRB>(leds_2, sizeof(leds_2)/3);
  //FastLED.addLeds<WS2811, LED_3_PIN, GRB>(leds_3, sizeof(leds_3)/3);
  //FastLED.setMaxRefreshRate(10000000,false);
  FastLED.clear();
  //FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,MAX_POWER_MILLIAMPS);
  //FastLED.show(0);
  WiFi.softAP(apSSID, apPassword);
  IPAddress apIP = WiFi.softAPIP();
  Serial.println("AP IP address: " + apIP.toString());
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, handleUpdate);
  server.begin();
  // air pump
  setup_2_relay();
  //led_test();
  delay(5000);
  setup_tasks();

}

void led_test(){
  for (int i = 0; i < sizeof(leds_0)/3; i++) 
      leds_0[i] = CRGB::Blue;
  /*for (int i = 0; i < sizeof(leds_1)/3; i++) 
      leds_1[i] = CRGB::Yellow;
  for (int i = 0; i < sizeof(leds_2)/3; i++) 
      leds_2[i] = CRGB::Red;
  for (int i = 0; i < sizeof(leds_3)/3; i++) 
      leds_3[i] = CRGB::Green;*/
  FastLED.show();
}


CRGB *get_leds(int led_index, size_t &num_leds){
  CRGB* _leds;
  switch(led_index){
    case 0: num_leds = sizeof(leds_0)/3, _leds = leds_0; break;
    case 1: num_leds = sizeof(leds_1)/3, _leds = leds_1; break;
    case 2: num_leds = sizeof(leds_2)/3, _leds = leds_2; break;
    case 3: num_leds = sizeof(leds_3)/3, _leds = leds_3; break;
    default: num_leds = sizeof(leds_0)/3, _leds = leds_0; break;
  }
  return _leds;
}



void codeForCore0Task(void *pvParameters) {
  Serial.print("Task fft ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    fftLoop();
    //run_fft(); 
    //fftLoop();
    //Serial.println();       
    //delayMicroseconds(2);
    delay(20);
  }
}

void codeForCore1Task(void *pvParameters) {
  for (;;)
  {
    relay_loop();
    if (just_dance)
    {    
      if(digitalRead(press_button_pin) == LOW){
        notify_button_pressed();
      }      
      run_just_dance();
      continue;
    }
    //Serial.print("Task 1 loop on core ");
    //Serial.println(xPortGetCoreID());
    //delay(100);
    switch (currentState) {
      case TEST:
        EVERY_N_MILLISECONDS(30){  
          FastLED.show();
          led_test();
        };
        if (millis() - stateChangeTime >= 5000) {
          stateChangeTime = millis();
          changeState(WAVE);
        }
        break;

      case SHOW_UP:
        FastLED.clear();
        show_grow(leds_0, sizeof(leds_0)/3, true, 5,255, 10);
        show_grow(leds_1, sizeof(leds_1)/3, true, 170,255 ,10);
        show_grow(leds_2, sizeof(leds_2)/3, true, 85,255, 10);
        show_grow(leds_3, sizeof(leds_3)/3, true, 42, 255, 10);
        //fadeOutLeds(2000);
        FastLED.show();
        //changeState(SHOW_DOWN);        
        changeState(CONFFETI);        
        break;
      case SHOW_DOWN:
        show_grow(leds_3, sizeof(leds_3)/3, false, 42, 0, 10);
        show_grow(leds_2, sizeof(leds_2)/3, false, 85,0, 10);
        show_grow(leds_1, sizeof(leds_1)/3, false, 170,0, 10);
        show_grow(leds_0, sizeof(leds_0)/3, false, 5,0, 10);
        changeState(SHOW_UP);
        break;
      case WAVE:
        //wave(leds_0, sizeof(leds_0)/3, false, 5,0, 10);
        wave2(leds_0, sizeof(leds_0)/3);
        break;
      case CONFFETI:        
        // random colored speckles that blink in and fade smoothly
        fadeToBlackBy( leds_0, sizeof(leds_0)/3, 10);
        static unsigned int led_pos = random16(sizeof(leds_0)/3);
        leds_0[led_pos] += CHSV( 80 + random8(64), 200, 255);
                FastLED.show();

        if (millis() - stateChangeTime >= 10000) {
            //FastLED.clear();
            fadeOutLeds(2000);
            FastLED.show();
            //changeState(CONFFETI);
            changeState(PRIDE);
          }
      break;
      case PRIDE:
        pride(leds_0, sizeof(leds_0)/3);
        if (millis() - stateChangeTime >= 10000) {
            //FastLED.clear();
            FastLED.show();
            //changeState(PRIDE);
            changeState(SHOW_DOWN);
          }
      break;
      case MUSIC:       
        music(leds_0,sizeof(leds_0)/3, CRGB::Red);
        break;
    }
  }
}

void printStateEnum(State state) {
  switch (state) {
    case TEST:
      Serial.print("TEST");
      break;
    case SHOW_UP:
      Serial.print("SHOW_UP");
      break;
    case SHOW_DOWN:
      Serial.print("SHOW_DOWN");
      break;
    case WAVE:
      Serial.print("WAVE");
      break;
    case MUSIC:
      Serial.print("MUSIC");
      break;
    case CONFFETI:
      Serial.print("CONFFETI");
      break;
    case PRIDE:
      Serial.print("PRIDE");
      break;
    default:
      Serial.print("Unknown State");
      break;
  }
}



void setup_tasks() {
   // Set up Core 0 task handler
  xTaskCreatePinnedToCore(
    codeForCore0Task,
    "Core 0 task",
    10000,
    NULL,
    1,
    &Core0Task,
    0);


  // Set up Core 1 task handler
  xTaskCreatePinnedToCore(
    codeForCore1Task,
    "Core 1 task",
    10000,
    NULL,
    1,
    &Core1Task,
    1);
}

void apply_to_all_led_from_leds(){
  for (int i = 0; i < sizeof(leds_1)/3; i++) 
      leds_1[i] = leds_0[map(i,0,sizeof(leds_0)/3,0, sizeof(leds_1)/3)];
  for (int i = 0; i < sizeof(leds_2)/3; i++) 
      leds_2[i] = leds_0[map(i,0,sizeof(leds_0)/3,0, sizeof(leds_2)/3)];
  for (int i = 0; i < sizeof(leds_3)/3; i++) 
      leds_3[i] = leds_0[map(i,0,sizeof(leds_0)/3,0, sizeof(leds_3)/3)];
}
void changeState(State newState) {
  Serial.print("Changing state  "); printStateEnum(currentState); Serial.print("->"); printStateEnum(newState);Serial.println();
  currentState = newState;
  stateChangeTime = millis();
}
void handleRoot(AsyncWebServerRequest *request) {
  // Send the HTML form to set the options
  String html = "<html><body>";
  html += "<h1>Set Options</h1>";
  html += "<form action='/update' method='POST'>";
  html += "Option 1: <input type='number' name='opt1' min='0' max='255' value='" + String(opt1) + "'><br>";
  html += "Option 2: <input type='number' name='opt2' min='0' max='255' value='" + String(opt2) + "'><br>";
  html += "Option 3 max_value_yres 80: <input type='number' name='max_value_yres' min='1' max='255' value='" + String(get_max_value_yres()) + "'><br>";
  html += "Option 4 scale_vreal 8 : <input type='number' name='scale_vreal' min='1' max='255' value='" + String(get_scale_vreal()) + "'><br>";
  html += "Option 5: <input type='number' name='opt5' min='0' max='255' value='" + String(opt5) + "'><br>";
  html += "POTA: <input type='checkbox' name='pota' " + String(pota ? "checked" : "") + "><br>";
  html += "just_dance: <input type='checkbox' name='just_dance' " + String(just_dance ? "checked" : "") + "><br>";
  html += "<input type='submit' value='Submit'>";
  html += "</form>";
  html += "</body></html>";

  request->send(200, "text/html", html);
}
void handleUpdate(AsyncWebServerRequest *request) {
  // Extract the POST data
  if (request->args()) {
    for (size_t i = 0; i < request->args(); i++) {
      String paramName = request->argName(i);
      String paramValue = request->arg(i);
      
      if (paramName == "opt1") {
        opt1 = paramValue.toInt();
        set_gCurrentPatternNumber( opt1%16);
        switch(opt1){
          case 0 : changeState(TEST); break;
          case 1 : changeState(SHOW_UP); break;
          case 2 : changeState(CONFFETI); break;
          case 3 : changeState(PRIDE); break;
          case 4 : changeState(WAVE); break;
        }
      } else if (paramName == "opt2") {
        opt2 = paramValue.toInt();
      } else if (paramName == "max_value_yres") {
        set_max_value_yres( paramValue.toInt());
      } else if (paramName == "scale_vreal") {
        set_scale_vreal( paramValue.toInt() );
      } else if (paramName == "opt5") {
        opt5 = paramValue.toInt();
      } else if (paramName == "pota") {
        pota = (paramValue == "on");
      } else if (paramName == "just_dance") {
        just_dance = (paramValue == "on");
      }
    }
  }

  // Redirect back to the root page after setting the options
  request->redirect("/");
}

void loop() {
  Serial.print("Main loop on core ");
  delay(10000);
}
