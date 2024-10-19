#include "coldplate.h"

// Function to time the ultrasonic cleaning process
void check_ultrasonic_cleaning(String cat_detected, unsigned long ultrasonic_start_time, unsigned long ultrasonic_end_time) {
    // Get time since last ultrasonic cleaning
    unsigned long current_time = millis();
    unsigned long time_since_stop = current_time - ultrasonic_end_time;
    unsigned long time_since_start = current_time - ultrasonic_start_time;

    // Stop if Zephyr is detected. Screw Flea
    if (cat_detected == "Zephyr") {
        ultrasonic_stop();
        ultrasonic_end_time = current_time;
    } else if (cat_detected == "Flea") {
        ultrasonic_start();
        ultrasonic_start_time = current_time;
    } else if (cat_detected == "Both") {
        ultrasonic_stop();
        ultrasonic_end_time = current_time;
    } else {
        // Do nothing
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
void drawSplashScreen(const uint16_t splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16], Adafruit_SSD1306 display) {
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
    // Set up PWM on ESP8266 pins using analogWrite, which uses software PWM with a default frequency of ~1kHz
    pinMode(PWM_FAN_PIN, OUTPUT);
    pinMode(PWM_PUMP_PIN, OUTPUT);
    pinMode(PWM_ULTRASONIC_PIN, OUTPUT);
    pinMode(TEG_PIN, OUTPUT);
    pinMode(TEG_AUX_PIN, OUTPUT);

    // Set initial PWM duty cycles to 0 (off initially)
    analogWrite(PWM_FAN_PIN, 0);  // Duty cycle for FAN
    analogWrite(PWM_PUMP_PIN, 0); // Duty cycle for PUMP
    analogWrite(PWM_ULTRASONIC_PIN, 0); // Duty cycle for ULTRASONIC

    // Change PWM frequency if needed (default is ~1kHz). ESP8266 supports changing the frequency:
    analogWriteFreq(ULTRASONIC_FREQ); // Set PWM frequency to 40kHz for all pins
}

void ultrasonic_stop() {
    // Set pwm to 0
    analogWrite(PWM_ULTRASONIC_PIN, 0);
}

void ultrasonic_start() {
    // Set pwm to 50%
    analogWrite(PWM_ULTRASONIC_PIN, 128);
}

bool adjust_teg_power(int32_t cold_temp) {
    // Turn TEG on and off based on the cold side temperature
    if (cold_temp < TEMP_MIN) {
        digitalWrite(TEG_PIN, LOW); // Turn off the TEG
        return true;
    } else {
        digitalWrite(TEG_PIN, HIGH); // Turn on the TEG
        return false;
    }
}

bool adjust_aux_teg_power(int32_t cat_temp) {
    // Turn TEG on and off based on the cat pad temperature
    if (cat_temp > CAT_PAD_TEMP_MAX) {
        digitalWrite(TEG_AUX_PIN, LOW); // Turn off the TEG
        return true;
    } else {
        digitalWrite(TEG_AUX_PIN, HIGH); // Turn on the TEG
        return false;
    }
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

    analogWrite(PWM_FAN_PIN, pwm_value); // Set PWM value for the pump (Pin 5)
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

    analogWrite(PWM_PUMP_PIN, pwm_value); // Set PWM value for the fan (Pin 6)
    return pwm_value;
}

// Function to classify a data point to the nearest centroid
uint8_t classify_point(uint8_t data_point[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y], const uint32_t centroids[KMEANS_NUM_CENTEROIDS][KMEANS_DIMENSIONALITY]) {
    uint32_t min_distance = UINT32_MAX;
    uint8_t closest_centroid = 0;

    // Iterate over each centroid
    for (uint8_t i = 0; i < KMEANS_NUM_CENTEROIDS; ++i) {
        uint32_t distance = 0;

        // Compute the Manhattan distance
        for (uint16_t j = 0; j < KMEANS_NUM_CENTEROIDS; ++j) {
            distance += (data_point[j] > centroids[i][j])
                            ? (data_point[j] - centroids[i][j])
                            : (centroids[i][j] - data_point[j]);
        }

        // Track the centroid with the smallest distance
        if (distance < min_distance) {
            min_distance = distance;
            closest_centroid = i;
        }
    }

    return closest_centroid;
}

String detect_cats(uint8_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y], const uint32_t centroids[KMEANS_NUM_CENTEROIDS][KMEANS_DIMENSIONALITY]) {
    // Use KMeans clustering to detect cats
    String cat_detected;

    // Classify the data point
    uint8_t centroid = classify_point(data, centroids);

    // Check if the data point is close to a centroid
    if (centroid == 0) { // No cats detected
        cat_detected = "None";
    } else if (centroid == 1) { // Zephyr detected
        cat_detected = "Zephyr";
    } else if (centroid == 2) { // Flea detected
        cat_detected = "Flea";
    } else if (centroid == 3) { // Both cats detected
        cat_detected = "Both";
    } else { // Unknown cat detected
        cat_detected = "Unknown";
    }

    return cat_detected;
}
