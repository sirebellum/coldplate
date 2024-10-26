#include "coldplate.h"

// Function to draw a detailed splash screen image in center of the blue section (below 16 pixels)
void drawSplashScreen(const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 *display) {
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    
    // Calculate Offsets
    int xOffset = (SCREEN_WIDTH - SPLASH_WIDTH) / 2;
    int yOffset = YELLOW_HEIGHT + (BLUE_HEIGHT - SPLASH_HEIGHT) / 2;

    DEBUG_PRINTLN("Drawing splash screen...");
    // Draw the splash screen
    for (int y = 0; y < SPLASH_HEIGHT; y++) {
        for (int x_sub = 0; x_sub < SPLASH_WIDTH/16; x_sub++) {
            uint16_t bits = splashscreen[y][x_sub];
            for (int x = 0; x < 16; x++) {
                // Get pixels from left to right
                int pixel = (bits >> (15 - x)) & 1;
                if (pixel) {
                    display->drawPixel(x + xOffset + x_sub*16, y + yOffset, SSD1306_WHITE);
                }
            }
        }
    }
    DEBUG_PRINTLN("Splash screen drawn.");
}

// Function to draw banner in the yellow section (top 16 pixels)
void drawBanner(String message, Adafruit_SSD1306 *display) {
    display->setTextColor(SSD1306_BLACK);
    // Fill the banner with white
    display->fillRect(0, 0, SCREEN_WIDTH, YELLOW_HEIGHT, SSD1306_WHITE);

    DEBUG_PRINT("Drawing banner: "); DEBUG_PRINTLN(message);
    // Draw the banner text
    display->setTextSize(1);
    display->setCursor(SCREEN_WIDTH / 2 - message.length() * 3, 2);
    display->print(message);
}

// Main function to draw splash screen
void display_splash_screen(String message, const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 *display) {
    DEBUG_PRINTLN("Displaying splash screen...");
    display->clearDisplay();
    drawBanner(message, display);
    drawSplashScreen(splashscreen, display);
    display->display();
    DEBUG_PRINTLN("Splash screen display complete.");
}

// Function to initialize PWM pins for the fan, pump, and capacitive plate
bool pwm_init() {
    // Set up PWM on ESP8266 pins using analogWrite
    pinMode(PWM_FAN_PIN, OUTPUT);
    pinMode(PWM_PUMP_PIN, OUTPUT);

    DEBUG_PRINTLN("Initializing PWM pins...");
    // Set initial PWM duty cycles
    analogWrite(PWM_FAN_PIN, 255);  // Duty cycle for FAN
    analogWrite(PWM_PUMP_PIN, 255); // Duty cycle for PUMP

    analogWriteFreq(PWM_FREQ); // Set PWM frequency
    DEBUG_PRINTLN("PWM initialization complete.");

    return true;
}

// Function to initialize the gpio pins
bool pin_init() {
    pinMode(TEG_PIN, OUTPUT);
    pinMode(TEG_AUX_PIN, OUTPUT);
    pinMode(ULTRA_SONIC_PIN, OUTPUT);
    pinMode(BUTTON_DETECT_PIN, INPUT);

    digitalWrite(TEG_PIN, LOW);
    digitalWrite(TEG_AUX_PIN, LOW);
    digitalWrite(ULTRA_SONIC_PIN, LOW);

    attachInterrupt(digitalPinToInterrupt(BUTTON_DETECT_PIN), button_detect_isr, RISING);

    return true;
}

// Function to adjust the coldplate components based on the water temperature
int adjust_coldplate(int16_t water_temp) {
    // Adjust the coldplate based on the water temperature
    if (water_temp < CAT_PAD_TEMP_MIN) {
        // Turn on the TEG
        digitalWrite(TEG_PIN, HIGH);
        digitalWrite(TEG_AUX_PIN, HIGH);
        return 1;
    } else if (water_temp > CAT_PAD_TEMP_MAX) {
        // Turn off the TEG
        digitalWrite(TEG_PIN, LOW);
        digitalWrite(TEG_AUX_PIN, LOW);
        return -1;
    } else {
        // Do nothing
        return 0;
    }
}

// Function to adjust the water loop componenets based on the water temperature
int adjust_water_loop(int16_t water_temp) {
    // Vary the pump and fan speed based on the water temperature
    uint8_t fan_speed, pump_speed;
    if (water_temp < CAT_PAD_TEMP_MIN) {
        fan_speed = 0;
        pump_speed = 0;
    } else if (water_temp > CAT_PAD_TEMP_MAX) {
        fan_speed = 255;
        pump_speed = 255;
    } else {
        // Linearly interpolate the fan and pump speeds
        fan_speed = (uint8_t)(255 * (water_temp - CAT_PAD_TEMP_MIN) / (CAT_PAD_TEMP_MAX - CAT_PAD_TEMP_MIN));
        pump_speed = (uint8_t)(255 * (water_temp - CAT_PAD_TEMP_MIN) / (CAT_PAD_TEMP_MAX - CAT_PAD_TEMP_MIN));
    }

    // Set minimum for pump to 10%
    if (pump_speed < 25) {
        pump_speed = 25;
    }

    // Set the fan and pump speeds
    analogWrite(PWM_FAN_PIN, fan_speed);
    analogWrite(PWM_PUMP_PIN, pump_speed);

    return 0;
}

// Function to adjust the ultrasonic transducer based on time, zephyr, and flea status
bool adjust_ultrasonic(unsigned int *ultrasonic_on_time, unsigned int *ultrasonic_off_time, bool zephyr, bool flea) {
    unsigned long current_time = millis();

    // If zephyr is present, turn off
    if (zephyr) {
        digitalWrite(ULTRA_SONIC_PIN, LOW);
        return false;
    } else if (flea) {
        // If flea is present, turn on
        digitalWrite(ULTRA_SONIC_PIN, HIGH);
        return true;
    }

    // If it is time to turn on the ultrasonic transducer
    if (current_time - *ultrasonic_off_time > ULTRASONIC_OFF_TIME) {
        digitalWrite(ULTRA_SONIC_PIN, HIGH);
        *ultrasonic_on_time = current_time;
        return true;
    } else if (current_time - *ultrasonic_on_time > ULTRASONIC_ON_TIME) {
        // If it is time to turn off
        digitalWrite(ULTRA_SONIC_PIN, LOW);
        *ultrasonic_off_time = current_time;
        return false;
    }

    return digitalRead(ULTRA_SONIC_PIN);
}

// Function to detect class of object in object detection data
bool detect_object(uint8_t *obj_buffer, uint8_t obj_buffer_len, uint8_t obj_class) {
    // Check if the object is present in the object detection data
    for (int i = 0; i < obj_buffer_len; i++) {
        if (obj_buffer[i] == obj_class) {
            return true;
        }
    }

    return false;
}