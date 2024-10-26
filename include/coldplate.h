#ifndef COLDPLATE_H
#define COLDPLATE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SHT31.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define TEMP_SCALE 100.0  // Temperature scale factor

// Cat pad temperature ranges
#define CAT_PAD_TEMP_MIN (42 * TEMP_SCALE)  // 38.00°C
#define CAT_PAD_TEMP_MAX (55 * TEMP_SCALE)  // 50.00°C

// Define PWM pins
#define PWM_FAN_PIN D3
#define PWM_PUMP_PIN D4

// Define GPIO pins
#define TEG_PIN D1
#define TEG_AUX_PIN D2
#define BUTTON_DETECT_PIN D8
#define ULTRA_SONIC_PIN D5

// Define i2c pins
#define SDA_PIN D6
#define SCL_PIN D7

#define PWM_FREQ 1000 // PWM frequency in Hz

// OLED display config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define YELLOW_HEIGHT 16 // Height of the yellow bar on the display
#define BLUE_HEIGHT (SCREEN_HEIGHT - YELLOW_HEIGHT) // Height of the blue bar on the display

// Splash screen stuff
#define SPLASH_HEIGHT 48
#define SPLASH_WIDTH 48

// Ultrasonic transducer stuff
#define ULTRASONIC_ON_TIME (60UL * 1000UL * 5UL) // Stay on for 15 minutes
#define ULTRASONIC_OFF_TIME (60UL * 1000UL * 60UL * 12UL) // Stay off for 12 hours

// Object detection stuff
#define OBJ_NONE 0
#define OBJ_CAT 1
#define OBJ_ZEPHYR 2
#define OBJ_FLEA 3

#define DEBUG 0 // Set to 0 to disable Serial prints

#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Function prototypes
bool pwm_init();
bool pin_init();
void button_detect_isr();
int adjust_coldplate(int16_t water_temp);
int adjust_water_loop(int16_t water_temp);
bool adjust_ultrasonic(unsigned int *ultrasonic_on_time, unsigned int *ultrasonic_off_time, bool zephyr, bool flea);
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 *display);
int obj_detect_via_web(uint8_t *obj_buffer);
bool detect_object(uint8_t *obj_buffer, uint8_t obj_buffer_len, uint8_t obj_class);

#endif