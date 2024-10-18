#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
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

// PWM output pins
#define PWM_FAN_PIN DDD6 // (pin 6)
#define PWM_PUMP_PIN DDD5 // (pin 5)
#define PWM_TEG_PIN DDB2 // (pin 10)
#define PWM_ULTRASONIC_PIN DDB1 // (pin 9)

// GPIO output pins
#define TEG_AUX_PIN 8 // (pin 8)
#define TEG_NEGATIVE_CHANNEL 0
#define TEG_POSITIVE_CHANNEL 1
#define TEG_SAMPLES 16

// Ultrasonic generator config
#define ULTRASONIC_FREQ 40000 // 40 kHz
#define ULTRASONIC_DURATION (1000 * 60 * 5) // 5 minutes
#define ULTRASONIC_INTERVAL (1000 * 60 * 60 * 2) // 2 hours

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

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create objects for two temperature sensors
Adafruit_BME280 bme; // Cold side sensor (I2C)
Adafruit_SHT31 sht31; // Hot side sensor (I2C)

// Moving Average variables
#define MOVING_AVG_WINDOW 48 // Adjust this for how many readings you want to average
int32_t cold_temp_history[MOVING_AVG_WINDOW] = {0};
int32_t teg_power_history[MOVING_AVG_WINDOW] = {0};
int history_index = 0;

// Maintain a buffer of recent derivatives to calculate a dynamic threshold
int32_t derivative_history[MOVING_AVG_WINDOW] = {0};
int num_derivatives = 0;

// Ultrasonic time tracking
unsigned long ultrasonic_start_time = 0;
unsigned long ultrasonic_end_time = 0;

// Splash screen stuff
#define SPLASH_HEIGHT 48
#define SPLASH_WIDTH 48
const uint16_t food_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0001, 0xF00F, 0x8000},
    {0x0003, 0xF81F, 0xC000},
    {0x0007, 0xFC3F, 0xC000},
    {0x0007, 0xFC3F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0007, 0xFE7F, 0xE000},
    {0x0003, 0xFE7F, 0xC000},
    {0x03E3, 0xFC3F, 0xC7C0},
    {0x07F1, 0xF83F, 0x8FE0},
    {0x07F8, 0x700E, 0x1FE0},
    {0x07F8, 0x0000, 0x3FE0},
    {0x0FFC, 0x07C0, 0x3FE0},
    {0x07FC, 0x1FF8, 0x3FE0},
    {0x07FC, 0x3FFC, 0x3FE0},
    {0x07FC, 0x7FFE, 0x3FE0},
    {0x03F8, 0xFFFF, 0x3FC0},
    {0x01F9, 0xFFFF, 0x9F80},
    {0x00F3, 0xFFFF, 0xCF00},
    {0x0007, 0xFFFF, 0xE000},
    {0x000F, 0xFFFF, 0xE000},
    {0x000F, 0xFFFF, 0xF000},
    {0x001F, 0xFFFF, 0xF000},
    {0x001F, 0xFFFF, 0xF800},
    {0x001F, 0xFFFF, 0xF800},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x003F, 0xFFFF, 0xFC00},
    {0x007F, 0xFFFF, 0xFC00},
    {0x007F, 0xF00F, 0xFC00},
    {0x003F, 0xC003, 0xFC00},
    {0x003F, 0x0000, 0xFC00},
    {0x003E, 0x0000, 0x7C00},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
};
const uint16_t caution_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x0FF0, 0x0000},
    {0x0000, 0x0FF0, 0x0000},
    {0x0000, 0x1E78, 0x0000},
    {0x0000, 0x1E78, 0x0000},
    {0x0000, 0x3C3C, 0x0000},
    {0x0000, 0x3C3C, 0x0000},
    {0x0000, 0x781E, 0x0000},
    {0x0000, 0x781E, 0x0000},
    {0x0000, 0xF00F, 0x0000},
    {0x0000, 0xF00F, 0x0000},
    {0x0001, 0xE007, 0x8000},
    {0x0001, 0xE007, 0x8000},
    {0x0003, 0xC003, 0xC000},
    {0x0003, 0xC3C3, 0xC000},
    {0x0007, 0x87E1, 0xE000},
    {0x0007, 0x87E1, 0xE000},
    {0x000F, 0x07E0, 0xF000},
    {0x000F, 0x07E0, 0x7800},
    {0x001E, 0x07E0, 0x7800},
    {0x003C, 0x07E0, 0x3C00},
    {0x003C, 0x07E0, 0x3C00},
    {0x0078, 0x07E0, 0x1E00},
    {0x0078, 0x03C0, 0x1E00},
    {0x00F0, 0x03C0, 0x0F00},
    {0x00F0, 0x03C0, 0x0F00},
    {0x01E0, 0x03C0, 0x0780},
    {0x01E0, 0x03C0, 0x0780},
    {0x03C0, 0x0180, 0x03C0},
    {0x03C0, 0x0180, 0x03C0},
    {0x0780, 0x0180, 0x01E0},
    {0x0780, 0x0180, 0x01E0},
    {0x0F00, 0x0000, 0x00F0},
    {0x0F00, 0x03C0, 0x00F0},
    {0x1E00, 0x07E0, 0x0078},
    {0x1E00, 0x07E0, 0x0078},
    {0x3C00, 0x07E0, 0x003C},
    {0x3C00, 0x07E0, 0x003C},
    {0x7800, 0x07E0, 0x001E},
    {0x7800, 0x03C0, 0x001E},
    {0xF000, 0x0000, 0x000F},
    {0xF000, 0x0000, 0x000F},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0xFFFF, 0xFFFF, 0xFFFF},
    {0x7FFF, 0xFFFF, 0xFFFE},
};
const uint16_t snowflake_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0180, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x1188, 0x0000},
    {0x0000, 0x1DB8, 0x0000},
    {0x0000, 0x07E0, 0x0000},
    {0x00E4, 0x03C0, 0x2780},
    {0x01E4, 0x0180, 0x2780},
    {0x01EC, 0x0DB0, 0x3780},
    {0x01EC, 0x07E0, 0x3780},
    {0x001D, 0x83C1, 0xB800},
    {0x007D, 0x8181, 0xBE00},
    {0x01FF, 0x8001, 0xFF80},
    {0x0007, 0x9CB9, 0xE000},
    {0x001F, 0x9FF9, 0xF800},
    {0x001F, 0x9FF9, 0xF800},
    {0x0000, 0x7BDE, 0x0000},
    {0x0000, 0x799E, 0x0000},
    {0x0C03, 0xF99F, 0xC030},
    {0x0443, 0xFDBF, 0xC220},
    {0x0663, 0x8FF1, 0xC660},
    {0x3331, 0xC7E3, 0x8CCC},
    {0x7FF9, 0xFFFF, 0x9FFE},
    {0x7FFB, 0xFFFF, 0xDFFE},
    {0x3331, 0xC7E3, 0x8CCC},
    {0x0663, 0x8FF1, 0xC660},
    {0x0443, 0xFDBF, 0xC220},
    {0x0C03, 0xF99F, 0xC030},
    {0x0000, 0x799E, 0x0000},
    {0x0000, 0x7BDE, 0x0000},
    {0x001F, 0x9FF9, 0xF800},
    {0x001F, 0x9FF9, 0xF800},
    {0x0007, 0x9CB9, 0xE000},
    {0x01FF, 0x8001, 0xFF80},
    {0x007D, 0x8181, 0xBE00},
    {0x001D, 0x83C1, 0xB800},
    {0x01EC, 0x07E0, 0x3780},
    {0x01EC, 0x0DB0, 0x3780},
    {0x01E4, 0x0180, 0x2780},
    {0x01E4, 0x03C0, 0x2780},
    {0x0000, 0x07E0, 0x0000},
    {0x0000, 0x1DB8, 0x0000},
    {0x0000, 0x1188, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x03C0, 0x0000},
    {0x0000, 0x0180, 0x0000},
    {0x0000, 0x0000, 0x0000},
};
const uint16_t ultrasonic_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0003, 0x8000},
    {0x0000, 0x0003, 0xE000},
    {0x0000, 0x0001, 0xF800},
    {0x0000, 0x0000, 0x3C00},
    {0x0000, 0x0004, 0x0F00},
    {0x0000, 0x000F, 0x0780},
    {0x0000, 0x0007, 0xC3C0},
    {0x0000, 0x0001, 0xE1C0},
    {0x0000, 0x0000, 0xF0E0},
    {0x0000, 0x000C, 0x3870},
    {0x0000, 0x001E, 0x1C70},
    {0x0000, 0x3C0F, 0x8E38},
    {0x0000, 0x7F83, 0xC718},
    {0x0000, 0xFFC1, 0xC71C},
    {0x0000, 0xE1E0, 0xE398},
    {0x0000, 0xE078, 0x7380},
    {0x0000, 0xE03C, 0x7000},
    {0x0000, 0x601E, 0x0000},
    {0x0000, 0x600E, 0x0000},
    {0x0000, 0x6007, 0x0000},
    {0x0000, 0xE003, 0x8000},
    {0x0001, 0xC003, 0x8000},
    {0x0003, 0xC001, 0xC000},
    {0x0007, 0x9801, 0xC000},
    {0x000F, 0x3801, 0xC000},
    {0x001E, 0x79FB, 0xC000},
    {0x003C, 0x63FF, 0x8000},
    {0x0078, 0x079E, 0x0000},
    {0x00F0, 0x0E00, 0x0000},
    {0x01C0, 0x1C00, 0x0000},
    {0x0180, 0x3800, 0x0000},
    {0x0380, 0x7000, 0x0000},
    {0x0381, 0xE000, 0x0000},
    {0x0383, 0xC000, 0x0000},
    {0x0387, 0x8000, 0x0000},
    {0x03FF, 0x0000, 0x0000},
    {0x03FE, 0x0000, 0x0000},
    {0x0FE0, 0x0000, 0x0000},
    {0x1E00, 0x0000, 0x0000},
    {0x1C00, 0x0000, 0x0000},
    {0x1800, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000},
};

// Function prototypes
void pwm_init();
void adc_init();
uint16_t adc_read(uint8_t channel);
uint16_t adjust_teg_power(int32_t current_temp, bool food_detected);
uint8_t adjust_pump_speed(int32_t temp_diff);
uint8_t adjust_fan_speed(int32_t hot_temp);
void ultrasonic_stop();
void ultrasonic_start();
void check_ultrasonic_cleaning(bool food_detected);
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16]);
void drawBanner(String message);
void drawSplashScreen(uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16]);

bool sht31_detected = false;
void setup() {
    Wire.begin();
    Wire.setClock(100000);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // Wait for the display to initialize
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
    }
    display_splash_screen("Initializing...", caution_splashscreen);

    bme.begin(0x76);
    // sht31.begin(0x44);
    sht31_detected = false;

    pwm_init();
    adc_init();

    pinMode(TEG_AUX_PIN, OUTPUT);
    digitalWrite(TEG_AUX_PIN, HIGH);

    ultrasonic_start_time = millis();
}

void loop(void) {
    // Read the temp sensors
    int32_t cold_temp = bme.readTemperature() * 100;
    int32_t hot_temp = sht31_detected ? sht31.readTemperature() * 100 : 0;

    // Enter emergency mode if the cat pad is too hot
    while (hot_temp > TEMP_EMERGENCY) {
        display_splash_screen("EMERGENCY MODE", caution_splashscreen);
        OCR1B = 0;
        digitalWrite(TEG_AUX_PIN, LOW);
        OCR0A = 255;
        OCR0B = 255;
        ultrasonic_stop();
        _delay_ms(1000);
        hot_temp = sht31_detected ? sht31.readTemperature() * 100 : 0;
        cold_temp = bme.readTemperature() * 100;
    }

    // Read the TEG voltages (positive and negative), take an average because PWM
    int32_t positive_voltages[TEG_SAMPLES];
    int32_t negative_voltages[TEG_SAMPLES];
    for (int i = 0; i < TEG_SAMPLES; i++) {
        positive_voltages[i] = adc_read(TEG_POSITIVE_CHANNEL);
        negative_voltages[i] = adc_read(TEG_NEGATIVE_CHANNEL);
    }
    int32_t positive_voltage = 0;
    int32_t negative_voltage = 0;
    for (int i = 0; i < TEG_SAMPLES; i++) {
        positive_voltage += positive_voltages[i];
        negative_voltage += negative_voltages[i];
    }
    positive_voltage /= TEG_SAMPLES;
    negative_voltage /= TEG_SAMPLES;
    int32_t teg_voltage = positive_voltage - negative_voltage;

    // Detect Food
    bool food_detected = true;

    // Adjust TEG power, pump speed, and fan speed based on temperature readings
    uint8_t teg_pwm_value = adjust_teg_power(cold_temp, food_detected);
    uint8_t pump_pwm_value = adjust_pump_speed(hot_temp - cold_temp);
    uint8_t fan_pwm_value = adjust_fan_speed(hot_temp);

    // Update the history
    teg_power_history[history_index % MOVING_AVG_WINDOW] = teg_pwm_value;
    cold_temp_history[history_index % MOVING_AVG_WINDOW] = cold_temp;

    // Update the history index
    history_index = (history_index + 1) % MOVING_AVG_WINDOW;

    // Update the display
    // if (food_detected) {
    //     display_splash_screen("FOOD DETECTED", food_splashscreen);
    // } else if (millis() - ultrasonic_start_time < ULTRASONIC_DURATION) {
    //     display_splash_screen("ULTRASONIC CLEANING", ultrasonic_splashscreen);
    // } else {
    //     display_splash_screen("COOLING", snowflake_splashscreen);
    // }
    display_splash_screen("COOLING", snowflake_splashscreen);

    // Check ultrasonic cleaning status
    check_ultrasonic_cleaning(food_detected);
}

// Function to time the ultrasonic cleaning process
void check_ultrasonic_cleaning(bool food_detected) {
    // Get time since last ultrasonic cleaning
    unsigned long current_time = millis();
    unsigned long time_since_stop = current_time - ultrasonic_end_time;
    unsigned long time_since_start = current_time - ultrasonic_start_time;

    // Stop until food not detected
    if (food_detected) {
        ultrasonic_stop();
        ultrasonic_end_time = current_time;
    }

    // Check if ultrasonic cleaning is needed
    if (time_since_stop >= ULTRASONIC_INTERVAL) {
        ultrasonic_start();
        ultrasonic_start_time = current_time;
    } else if (time_since_start >= ULTRASONIC_DURATION) {
        ultrasonic_stop();
        ultrasonic_end_time = current_time;
    }
}

// Function to draw a detailed splash screen image in center of the blue section (below 16 pixels)
void drawSplashScreen(uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16]) {
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    
    // Calculate Offsets
    int xOffset = (SCREEN_WIDTH - SPLASH_WIDTH) / 2;
    int yOffset = YELLOW_HEIGHT + (BLUE_HEIGHT - SPLASH_HEIGHT) / 2;

    // Draw the splash screen
    for (int y = 0; y < SPLASH_HEIGHT; y++) {
        for (int x_sub = 0; x_sub < SPLASH_WIDTH/16; x_sub++) {
            uint16_t bits = pgm_read_word(&splashscreen[y][x_sub]);
            for (int x = 0; x < 16; x++) {
                // Get pixels from left to right
                int pixel = (bits >> (16 - x)) & 0x01;
                if (pixel) {
                    display.drawPixel(x + xOffset + x_sub*16, y + yOffset, SSD1306_WHITE);
                }
            }
        }
    }

    // Draw noise on the rest of the blue area
    for (int y = YELLOW_HEIGHT; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (random(0, 100) < 3) {
                display.drawPixel(x, y, SSD1306_WHITE);
            } else if (random(0, 100) < 3) {
                display.drawPixel(x, y, SSD1306_BLACK);
            }
        }
    }
}

// Function to draw banner in the yellow section (top 16 pixels)
void drawBanner(String message) {
    display.setTextColor(SSD1306_BLACK);
    // Fill the banner with white
    display.fillRect(0, 0, SCREEN_WIDTH, YELLOW_HEIGHT, SSD1306_WHITE);

    // Draw the banner text
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH / 2 - message.length() * 3, 2);
    display.print(message);
}

// Main function to draw splash screen
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16]) {
    display.clearDisplay();
    drawBanner(message);
    drawSplashScreen(splashscreen);
    display.display();
}

// Function to initialize PWM pins for the fan, pump, ultrasonic transducer, and TEG module
void pwm_init() {
    // Set PWM pins as output
    DDRB |= (1 << PWM_TEG_PIN) | (1 << PWM_ULTRASONIC_PIN);  // Set PB1 and PB2 as output (OC1A, OC1B)
    DDRD |= (1 << PWM_PUMP_PIN) | (1 << PWM_FAN_PIN);        // Set PD5 and PD6 as output (OC0B, OC0A)
    
    // Set up Timer0 for Fast PWM mode (8-bit) for PUMP_PIN (PD5/OC0B) and FAN_PIN (PD6/OC0A)
    // Fast PWM mode, with 8-bit resolution (TOP = 255)
    TCCR0A = (1 << WGM01) | (1 << WGM00);         // Fast PWM Mode, WGM00 and WGM01 set
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);      // Non-inverted PWM on OC0A (PD6) and OC0B (PD5)
    TCCR0B = (1 << CS01);                         // Prescaler set to 8 (PWM frequency = 16MHz / (8 * 256) = 7.8125 kHz)

    // Set up Timer1 for Fast PWM mode (16-bit) for TEG_PIN (PB2/OC1B) and ULTRASONIC_PIN (PB1/OC1A)
    // Fast PWM with ICR1 as TOP for full 16-bit resolution
    TCCR1A = (1 << WGM11);                        // Fast PWM mode, WGM11 set (WGM13:10 = 1110)
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);      // Non-inverted PWM on OC1A (PB1) and OC1B (PB2)
    TCCR1B = (1 << WGM12) | (1 << WGM13);         // Set WGM13 and WGM12 for Fast PWM using ICR1 as TOP
    TCCR1B |= (1 << CS11);                        // Prescaler set to 8 (PWM frequency = 16MHz / (8 * ICR1))

    // Set ICR1 (TOP value) for Timer1, allowing full 16-bit resolution
    ICR1 = 49; // Set ICR1 to 49 for 40kHz PWM frequency

    // Initialize PWM duty cycles to 0 (PWM off initially)
    OCR0A = 0;  // PWM duty cycle for FAN_PIN (PD6)
    OCR0B = 0;  // PWM duty cycle for PUMP_PIN (PD5)
    OCR1A = 0;  // PWM duty cycle for ULTRASONIC_PIN (PB1)
    OCR1B = 0;  // PWM duty cycle for TEG_PIN (PB2)
}

void ultrasonic_stop() {
    // Set pwm to 0
    OCR1A = 0;
}

void ultrasonic_start() {
    // Set pwm to 50%
    OCR1A = 25;
}

void adc_init() {
    // Set the analog reference to AVcc (default is AVcc)
    ADMUX |= (1 << REFS0);  
    // Set the ADC prescaler to 128 for maximum accuracy (16 MHz / 128 = 125 kHz ADC clock)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Enable the ADC
    ADCSRA |= (1 << ADEN);  
}

uint16_t adc_read(uint8_t channel) {
    // Set the ADC channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    // Start the conversion
    ADCSRA |= (1 << ADSC);
    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));
    // Return the ADC value
    return ADC;
}

uint16_t adjust_teg_power(int32_t cold_temp, bool food_detected) {
    uint16_t pwm_value = 0;

    // Calculate the dew point
    float humidity = bme.readHumidity();
    float dew_point = bme.readTemperature() - ((100 - humidity) / 5);

    // Scale the dew_point to match the cold_temp scale
    dew_point *= 100;

    // Adjust TEG power based on cold side temperature
    // If food is not detected, focus on keeping the cold side temperature right above the dew point
    if (!food_detected) {
        if (cold_temp < dew_point) {
            pwm_value = 0; // If the cold side temperature is below the dew point, turn off the TEG
        } else {
            // Adjust TEG power based on the proximity to dew point
            pwm_value = (uint16_t)((cold_temp - dew_point) / (TEMP_MAX - dew_point) * 49);
        }
    } else {
        // If food is detected, focus on keeping the cold side temperature below the hot side temperature
        if (cold_temp < TEMP_MIN) {
            pwm_value = 0; // If the cold side temperature is above the hot side temperature, turn off the TEG
        } else if (cold_temp > TEMP_MAX) {
            pwm_value = 49; // If the cold side temperature is too high, set TEG to full power
        } else {
            // Adjust TEG power based on the cold side temperature
            pwm_value = (uint16_t)((cold_temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN) * 49);

        }
    }

    // Temporarily step down the pwm
    pwm_value *= 0.69;

    OCR1B = pwm_value; // Set PWM value for the TEG (Pin 10)
    return pwm_value;
}

uint8_t adjust_pump_speed(int32_t temp_diff) {
    uint8_t pwm_value = 0;

    // Adjust pump speed based on TEMP_THRESHOLD
    if (temp_diff < TEMP_THRESHOLD) {
        pwm_value = 0; // If the temperature difference is too low, turn off the pump
    } else if (temp_diff > TEMP_THRESHOLD + 10) {
        pwm_value = 255; // If the temperature difference is too high, set pump to full speed
    } else {
        // Adjust pump speed based on the temperature difference
        pwm_value = (uint8_t)((temp_diff - TEMP_THRESHOLD) / 10.0 * 255);
    }

    OCR0B = pwm_value; // Set PWM value for the pump (Pin 5)
    return pwm_value;
}

uint8_t adjust_fan_speed(int32_t hot_temp) {
    uint8_t pwm_value = 0;

    // Adjust fan speed based on hot side temperature
    if (hot_temp < CAT_PAD_TEMP_MIN) {
        pwm_value = 0; // If the hot side temperature is too low, turn off the fan
    } else if (hot_temp > CAT_PAD_TEMP_MAX) {
        pwm_value = 255; // If the hot side temperature is too high, set fan to full speed
    } else {
        // Adjust fan speed based on the hot side temperature
        pwm_value = (uint8_t)((hot_temp - CAT_PAD_TEMP_MIN) / (CAT_PAD_TEMP_MAX - CAT_PAD_TEMP_MIN) * 255);
    }

    OCR0A = pwm_value; // Set PWM value for the fan (Pin 6)
    return pwm_value;
}
