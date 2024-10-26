#ifndef COLDPLATE_H
#define COLDPLATE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90640.h>
#include <Adafruit_SHT31.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define TEMP_SCALE 100.0  // Temperature scale factor

// Define TEG coldplate temperature ranges
#define TEMP_MIN (4 * TEMP_SCALE)         // 4.00°C
#define TEMP_MAX (7 * TEMP_SCALE)         // 7.00°C
#define TEMP_IDLE_MIN (12 * TEMP_SCALE)       // 12.00°C
#define TEMP_IDLE_MAX (15 * TEMP_SCALE)       // 15.00°C
#define TEMP_EMERGENCY (69 * TEMP_SCALE)  // 50.00°C

// Cat pad temperature ranges
#define CAT_PAD_TEMP_MIN (38 * TEMP_SCALE)  // 38.00°C
#define CAT_PAD_TEMP_MAX (50 * TEMP_SCALE)  // 50.00°C

// Define PWM pins
#define PWM_FAN_PIN D3
#define PWM_PUMP_PIN D4

// Define GPIO pins
#define TEG_PIN D1
#define TEG_AUX_PIN D2
#define FOOD_DETECT_PIN D8
#define ULTRA_SONIC_PIN D5

// Define i2c pins
#define SDA_PIN D6
#define SCL_PIN D7

#define PWM_FREQ 1000 // PWM frequency in Hz

// Food stuff
#define FOOD_STAY_ON_TIME (60UL * 1000UL * 120UL) // Stay on for 2 hours

// OLED display config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define YELLOW_HEIGHT 16 // Height of the yellow bar on the display
#define BLUE_HEIGHT (SCREEN_HEIGHT - YELLOW_HEIGHT) // Height of the blue bar on the display

// Splash screen stuff
#define SPLASH_HEIGHT 48
#define SPLASH_WIDTH 48

// IR sensor stuff
#define MLX90640_RESOLUTION_Y 24
#define MLX90640_RESOLUTION_X 32
#define MLX90640_I2C_ADDR 0x33

// Ultrasonic transducer stuff
#define ULTRASONIC_ON_TIME (60UL * 1000UL * 5UL) // Stay on for 5 minutes
#define ULTRASONIC_OFF_TIME (60UL * 1000UL * 240UL) // Stay off for 4 hours

// KMeans stuff
#define KMEANS_DIMENSIONALITY (MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y)
#define KMEANS_PULL_INTERVAL (60UL * 1000UL * 5UL) // Pull centroids every 5 minutes
#define KMEANS_CLASS_NOTHING 0
#define KMEANS_CLASS_ZEPHYR 1
#define KMEANS_CLASS_FLEA 2
#define KMEANS_CLASS_OTHER 3

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
void food_detect_isr();

int read_ir_sensor(int16_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y], Adafruit_MLX90640 *mlx);
int upload_thermal_data(const int16_t data[MLX90640_RESOLUTION_Y*MLX90640_RESOLUTION_X]);
int pull_centroids(int16_t **centroids);
bool detect_food(int *adc_samples);

int16_t calculate_max_temp(const int16_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]);
int16_t calculate_min_temp(const int16_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]);
int16_t calculate_kmeans_cluster(int16_t data[KMEANS_DIMENSIONALITY], int16_t *centroids, uint8_t num_clusters);

bool adjust_ultra_sonic(unsigned int *ultrasonic_on_time, unsigned int *ultrasonic_off_time, bool zephyr, bool flea);
bool adjust_teg_power(int32_t cold_temp, bool food_detected);
bool adjust_aux_teg_power(int32_t water_temp);
uint8_t adjust_pump_speed(int32_t temp_diff);
uint8_t adjust_fan_speed(int32_t hot_temp);

void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 *display);

#endif