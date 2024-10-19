#ifndef COLDPLATE_H
#define COLDPLATE_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SHT31.h>

#define TEMP_MIN 200         // 2.00°C -> multiply by 100 to scale
#define TEMP_MAX 700         // 7.00°C -> multiply by 100
#define TEMP_IDLE 1000       // 10.00°C
#define TEMP_THRESHOLD 1500  // 15.00°C
#define TEMP_EMERGENCY 5000  // 50.00°C
#define SEEBECK_COEFFICIENT 5000 // (Fixed-point math: 0.05 V per K * 100000)

// Cat pad temperature ranges (scaled by 100)
#define CAT_PAD_TEMP_MIN 2900  // 29.00°C
#define CAT_PAD_TEMP_MAX 3900  // 39.00°C

// Define PWM output pins using GPIO numbers instead of DDR and port registers
#define PWM_FAN_PIN 12 // GPIO12 (D6)
#define PWM_PUMP_PIN 14 // GPIO14 (D5)
#define PWM_ULTRASONIC_PIN 13 // GPIO13 (D7)

// Define GPIO output pins
#define TEG_PIN 10 // GPIO10 (SD3)
#define TEG_AUX_PIN 15 // GPIO15 (D8)

// Ultrasonic generator config
#define ULTRASONIC_FREQ 40000 // 40 kHz
#define ULTRASONIC_DURATION (1000UL * 60UL * 5UL) // 5 minutes
#define ULTRASONIC_INTERVAL (1000UL * 60UL * 60UL * 2UL) // 2 hours

// Adjusted Voltage Divider Constants
// Use scaled integer math for voltage calculations
#define VOLTAGE_DIVIDER_R1 15000  // Resistor R1 value in ohms
#define VOLTAGE_DIVIDER_R2 10000  // Resistor R2 value in ohms
// The above values create a voltage divider that scales 12V down to 4.8V

// OLED display config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define YELLOW_HEIGHT 16 // Height of the yellow bar on the display
#define BLUE_HEIGHT SCREEN_HEIGHT - YELLOW_HEIGHT // Height of the blue bar on the display

#define START_BYTE 0x02
#define END_BYTE 0x03

// Splash screen stuff
#define SPLASH_HEIGHT 48
#define SPLASH_WIDTH 48

// IR sensor stuff
#define MLX90640_RESOLUTION_Y 24
#define MLX90640_RESOLUTION_X 32
#define MLX90640_DOWMSAMPLE 4
#define MLX90640_I2C_ADDR 0x33
#define MLX90640_REFRESH_RATE 3  // 3 Hz
#define IR_MAX_TEMP 39  // 39.00°C
#define IR_MIN_TEMP 20  // 20.00°C

// NN stuff
#define NN_INPUT_SIZE 768
#define NN_OUTPUT_SIZE 5
#define NN_HIDDEN_SIZE 128

// Function prototypes
void pwm_init();
bool adjust_teg_power(int32_t current_temp);
uint8_t adjust_pump_speed(int32_t temp_diff);
uint8_t adjust_fan_speed(int32_t hot_temp);
void check_ultrasonic_cleaning(String cat_detected, unsigned long ultrasonic_start_time, unsigned long ultrasonic_end_time);
void ultrasonic_start();
void ultrasonic_stop();
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 display);
void update_background(uint8_t *frame, uint8_t *background, uint8_t *persistence);
String detect_cats(uint8_t frame[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y], const int32_t weights_input[NN_INPUT_SIZE*NN_HIDDEN_SIZE], const int32_t weights_hidden[NN_HIDDEN_SIZE*NN_OUTPUT_SIZE]);

#endif