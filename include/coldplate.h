#ifndef COLDPLATE_H
#define COLDPLATE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_SHT31.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define TEMP_MIN 200         // 2.00°C -> multiply by 100 to scale
#define TEMP_MAX 700         // 7.00°C -> multiply by 100
#define TEMP_IDLE 1000       // 10.00°C
#define TEMP_EMERGENCY 5000  // 50.00°C

#define TEMP_DIFF_MIN 1000  // 10.00°C
#define TEMP_DIFF_MAX 2000  // 20.00°C

// Cat pad temperature ranges (scaled by 100)
#define CAT_PAD_TEMP_MIN 2900  // 29.00°C
#define CAT_PAD_TEMP_MAX 4200  // 39.00°C

// Define PWM output pins using GPIO numbers instead of DDR and port registers
#define PWM_FAN_PIN 0
#define PWM_PUMP_PIN 2
#define PWM_PLATE_PIN 15
#define PWM_FREQ 1000

// Define GPIO output pins
#define TEG_PIN 13
#define TEG_AUX_PIN 12

// Define i2c pins
#define SDA_PIN 14
#define SCL_PIN 5

// ADC for capacitance sensing
#define ADC_PIN A0
#define ADC_SAMPLES 100
#define ADC_THRESHOLD 300

// OLED display config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define YELLOW_HEIGHT 16 // Height of the yellow bar on the display
#define BLUE_HEIGHT SCREEN_HEIGHT - YELLOW_HEIGHT // Height of the blue bar on the display

// Splash screen stuff
#define SPLASH_HEIGHT 48
#define SPLASH_WIDTH 48

// IR sensor stuff
#define MLX90640_RESOLUTION_Y 24
#define MLX90640_RESOLUTION_X 32
#define MLX90640_I2C_ADDR 0x33
#define IR_MAX_TEMP 39  // 39.00°C
#define IR_MIN_TEMP 20  // 20.00°C

// KMeans stuff
#define KMEANS_CENTROIDS 3
#define KMEANS_DIMENSIONALITY 768

#define DEBUG 1 // Set to 0 to disable Serial prints

#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Function prototypes
void pwm_init();
bool adjust_aux_teg_power(int32_t water_temp);
bool adjust_teg_power(int32_t cold_temp);
uint8_t adjust_pump_speed(int32_t temp_diff);
uint8_t adjust_fan_speed(int32_t hot_temp);
void adc_init();
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 *display);
int32_t calculate_max_temp(uint16_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]);
int32_t calculate_min_temp(uint16_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]);
uint8_t kmeans_cluster(uint16_t data[KMEANS_DIMENSIONALITY], uint16_t *centroids, uint8_t num_clusters);
bool uploadThermalData(const float data[MLX90640_RESOLUTION_Y*MLX90640_RESOLUTION_X]);
bool detect_food(int *adc_samples);
uint8_t pull_centroids(uint16_t **centroids);

#endif