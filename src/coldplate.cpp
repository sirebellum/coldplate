#include "coldplate.h"

// Function to time the ultrasonic cleaning process
void check_ultrasonic_cleaning(bool cat_detected, unsigned long ultrasonic_start_time, unsigned long ultrasonic_end_time) {
    // Get time since last ultrasonic cleaning
    unsigned long current_time = millis();
    unsigned long time_since_stop = current_time - ultrasonic_end_time;
    unsigned long time_since_start = current_time - ultrasonic_start_time;

    // Stop until food not detected
    if (cat_detected) {
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
void drawSplashScreen(uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 display) {
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
void drawBanner(String message, Adafruit_SSD1306 display) {
    display.setTextColor(SSD1306_BLACK);
    // Fill the banner with white
    display.fillRect(0, 0, SCREEN_WIDTH, YELLOW_HEIGHT, SSD1306_WHITE);

    // Draw the banner text
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH / 2 - message.length() * 3, 2);
    display.print(message);
}

// Main function to draw splash screen
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 display) {
    display.clearDisplay();
    drawBanner(message, display);
    drawSplashScreen(splashscreen, display);
    display.display();
}

// Function to initialize PWM pins for the fan, pump, ultrasonic transducer, and TEG module
void pwm_init() {
    // Set PWM pins as output
    DDRB |= (1 << PWM_ULTRASONIC_PIN);  // Set PB1 and PB2 as output (OC1A, OC1B)
    DDRD |= (1 << PWM_PUMP_PIN) | (1 << PWM_FAN_PIN);        // Set PD5 and PD6 as output (OC0B, OC0A)
    
    // Set up Timer0 for Fast PWM mode (8-bit) for PUMP_PIN (PD5/OC0B) and FAN_PIN (PD6/OC0A)
    // Fast PWM mode, with 8-bit resolution (TOP = 255)
    TCCR0A = (1 << WGM01) | (1 << WGM00);         // Fast PWM Mode, WGM00 and WGM01 set
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1);      // Non-inverted PWM on OC0A (PD6) and OC0B (PD5)
    TCCR0B = (1 << CS01);                         // Prescaler set to 8 (PWM frequency = 16MHz / (8 * 256) = 7.8125 kHz)

    // Set up Timer1 for Fast PWM mode (16-bit) for ULTRASONIC_PIN (PB1/OC1A)
    // Fast PWM with ICR1 as TOP for full 16-bit resolution
    TCCR1A = (1 << WGM11);                        // Fast PWM mode, WGM11 set (WGM13:10 = 1110)
    TCCR1A |= (1 << COM1A1);                      // Non-inverted PWM on OC1A (PB1)
    TCCR1B = (1 << WGM12) | (1 << WGM13);         // Set WGM13 and WGM12 for Fast PWM using ICR1 as TOP
    TCCR1B |= (1 << CS11);                        // Prescaler set to 8 (PWM frequency = 16MHz / (8 * ICR1))

    // Set ICR1 (TOP value) for Timer1, allowing full 16-bit resolution
    ICR1 = 49; // Set ICR1 to 49 for 40kHz PWM frequency

    // Initialize PWM duty cycles to 0 (PWM off initially)
    OCR0A = 0;  // PWM duty cycle for FAN_PIN (PD6)
    OCR0B = 0;  // PWM duty cycle for PUMP_PIN (PD5)
    OCR1A = 0;  // PWM duty cycle for ULTRASONIC_PIN (PB1)
}

void ultrasonic_stop() {
    // Set pwm to 0
    OCR1A = 0;
}

void ultrasonic_start() {
    // Set pwm to 50%
    OCR1A = 25;
}

bool adjust_teg_power(int32_t cold_temp, bool cat_detected) {

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
