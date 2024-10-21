#include "coldplate.h"

// Function to time the ultrasonic cleaning process
void check_ultrasonic_cleaning(bool cat_detected, unsigned long *ultrasonic_start_time, unsigned long *ultrasonic_end_time) {
    // Get time since last ultrasonic cleaning
    unsigned long current_time = millis();
    unsigned long time_since_stop = current_time - *ultrasonic_end_time;
    unsigned long time_since_start = current_time - *ultrasonic_start_time;

    DEBUG_PRINT("Current time: "); DEBUG_PRINTLN(current_time);
    DEBUG_PRINT("Time since last stop: "); DEBUG_PRINTLN(time_since_stop);
    DEBUG_PRINT("Time since last start: "); DEBUG_PRINTLN(time_since_start);

    // Stop if Zephyr is detected. Screw Flea
    if (cat_detected) {
        ultrasonic_stop();
        *ultrasonic_end_time = current_time;
        DEBUG_PRINTLN("Cat detected, stopping ultrasonic cleaning.");
    }

    // Check if ultrasonic cleaning is needed
    if (time_since_stop >= ULTRASONIC_INTERVAL) {
        ultrasonic_start();
        *ultrasonic_start_time = current_time;
        DEBUG_PRINTLN("Ultrasonic cleaning started.");
    } else if (time_since_start >= ULTRASONIC_DURATION) {
        ultrasonic_stop();
        *ultrasonic_end_time = current_time;
        DEBUG_PRINTLN("Ultrasonic cleaning stopped after duration.");
    }
}

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
            uint16_t bits = pgm_read_word(&splashscreen[y][x_sub]);
            for (int x = 0; x < 16; x++) {
                // Get pixels from left to right
                int pixel = (bits >> (16 - x)) & 0x01;
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

// Function to initialize PWM pins for the fan, pump, ultrasonic transducer, and TEG module
void pwm_init() {
    // Set up PWM on ESP8266 pins using analogWrite, which uses software PWM with a default frequency of ~1kHz
    pinMode(PWM_FAN_PIN, OUTPUT);
    pinMode(PWM_PUMP_PIN, OUTPUT);

    DEBUG_PRINTLN("Initializing PWM pins...");
    // Set initial PWM duty cycles
    analogWrite(PWM_FAN_PIN, 255);  // Duty cycle for FAN
    analogWrite(PWM_PUMP_PIN, 255); // Duty cycle for PUMP

    analogWriteFreq(PWM_FREQ); // Set PWM frequency
    DEBUG_PRINTLN("PWM initialization complete.");
}

void ultrasonic_init() {
    pinMode(PWM_ULTRASONIC_PIN, OUTPUT);
}

void ultrasonic_stop() {
    digitalWrite(PWM_ULTRASONIC_PIN, LOW);
}

void ultrasonic_start() {
    digitalWrite(PWM_ULTRASONIC_PIN, HIGH);
}

// Turn the power on or off of the teg module cooling the plate
bool adjust_teg_power(int32_t hot_temp, int32_t cold_temp, int32_t temp_diff) {
    DEBUG_PRINT("Adjusting TEG power. Hot temp: "); DEBUG_PRINT(hot_temp);
    DEBUG_PRINT(" Cold temp: "); DEBUG_PRINT(cold_temp);
    DEBUG_PRINT(" Temp diff: "); DEBUG_PRINTLN(temp_diff);

    if (cold_temp > TEMP_MAX) {
        digitalWrite(TEG_PIN, HIGH);
        DEBUG_PRINTLN("TEG power ON.");
        return true;
    } else if (cold_temp < TEMP_MIN) {
        digitalWrite(TEG_PIN, LOW);
        DEBUG_PRINTLN("TEG power OFF.");
        return false;
    } else {
        DEBUG_PRINTLN("TEG power unchanged.");
        return false;
    }
}

// Turn the power on or off of the auxiliary teg module cooling something else
bool adjust_aux_teg_power(int32_t hot_temp, int32_t cold_temp, int32_t temp_diff) {
    DEBUG_PRINT("Adjusting auxiliary TEG power. Hot temp: "); DEBUG_PRINT(hot_temp);
    DEBUG_PRINT(" Cold temp: "); DEBUG_PRINT(cold_temp);
    DEBUG_PRINT(" Temp diff: "); DEBUG_PRINTLN(temp_diff);

    // Prioritize main teg over aux teg
    if (hot_temp > CAT_PAD_TEMP_MAX) {
        digitalWrite(TEG_AUX_PIN, LOW);
        DEBUG_PRINTLN("Auxiliary TEG power OFF.");
        return false;
    } else {
        digitalWrite(TEG_AUX_PIN, HIGH);
        DEBUG_PRINTLN("Auxiliary TEG power ON.");
        return true;
    }
}

// Adjust speed of the fan
uint8_t adjust_fan_speed(int32_t hot_temp) {
    DEBUG_PRINT("Adjusting fan speed. Hot temp: "); DEBUG_PRINT(hot_temp);

    uint8_t fan_speed = 0;
    // Set fan speed based on CAT_PAD temps
    if (hot_temp > CAT_PAD_TEMP_MAX) {
        fan_speed = 255;
    } else if (hot_temp < CAT_PAD_TEMP_MIN) {
        fan_speed = 0;
    } else {
        int32_t scaled_speed = ((hot_temp - CAT_PAD_TEMP_MIN) * 255) / (CAT_PAD_TEMP_MAX - CAT_PAD_TEMP_MIN);
        fan_speed = (uint8_t)(scaled_speed > 255 ? 255 : scaled_speed);
    }

    analogWrite(PWM_FAN_PIN, fan_speed);
    DEBUG_PRINT("Fan speed set to: "); DEBUG_PRINTLN(fan_speed);
    return fan_speed;
}

// Adjust speed of the pump
uint8_t adjust_pump_speed(int32_t temp_diff) {
    DEBUG_PRINT("Adjusting pump speed. Temp diff: "); DEBUG_PRINT(temp_diff);

    uint8_t pump_speed = 0;
    // Set pump to maintain a maximum and minimum temperature difference
    if (temp_diff > TEMP_DIFF_MAX) {
        pump_speed = 255;
    } else if (temp_diff < TEMP_DIFF_MIN) {
        pump_speed = 0;
    } else {
        int32_t scaled_speed = ((temp_diff - TEMP_DIFF_MIN) * 255) / (TEMP_DIFF_MAX - TEMP_DIFF_MIN);
        pump_speed = (uint8_t)(scaled_speed > 255 ? 255 : scaled_speed);
    }

    analogWrite(PWM_PUMP_PIN, pump_speed);
    DEBUG_PRINT("Pump speed set to: "); DEBUG_PRINTLN(pump_speed);
    return pump_speed;
}

// Calculate max of an array
int32_t calculate_max_temp(uint8_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]) {
    int32_t max_temp = 0;
    DEBUG_PRINTLN("Calculating max temperature...");
    for (uint16_t i = 0; i < MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y; i++) {
        if (data[i] > max_temp) {
            max_temp = data[i];
        }
        // DEBUG_PRINT("Data["); DEBUG_PRINT(i); DEBUG_PRINT("]: "); DEBUG_PRINT(data[i]);
        // DEBUG_PRINT(", Current max: "); DEBUG_PRINTLN(max_temp);
    }
    DEBUG_PRINT("Max temperature: "); DEBUG_PRINTLN(max_temp);
    return max_temp;
}

// Calculate min of an array
int32_t calculate_min_temp(uint8_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y]) {
    int32_t min_temp = 1000;
    DEBUG_PRINTLN("Calculating min temperature...");
    for (uint16_t i = 0; i < MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y; i++) {
        if (data[i] < min_temp) {
            min_temp = data[i];
        }
        // DEBUG_PRINT("Data["); DEBUG_PRINT(i); DEBUG_PRINT("]: "); DEBUG_PRINT(data[i]);
        // DEBUG_PRINT(", Current min: "); DEBUG_PRINTLN(min_temp);
    }
    DEBUG_PRINT("Min temperature: "); DEBUG_PRINTLN(min_temp);
    return min_temp;
}

uint8_t kmeans_cluster(uint8_t data[KMEANS_DIMENSIONALITY], const uint32_t centroids[KMEANS_CENTROIDS][KMEANS_DIMENSIONALITY]) {
    uint8_t closest_centroid = 0;
    uint64_t min_distance = UINT64_MAX;

    // Precompute a multiplier to avoid floating-point arithmetic
    uint32_t scale_multiplier = KMEANS_SCALE_FACTOR / KMEANS_SCALE_FACTOR;

    DEBUG_PRINTLN("Starting k-means clustering...");

    // Loop through each centroid
    for (uint8_t i = 0; i < KMEANS_CENTROIDS; i++) {
        uint64_t distance = 0;

        // DEBUG_PRINT("Centroid "); DEBUG_PRINT(i); DEBUG_PRINTLN(":");

        // Calculate Euclidean distance squared (to avoid the cost of sqrt)
        for (uint16_t j = 0; j < KMEANS_DIMENSIONALITY; j++) {
            // Scale the input data to match the scaled centroids using the multiplier
            uint32_t scaled_data = (uint32_t)data[j] * scale_multiplier;
            // Pull from PROGMEM
            uint32_t centroid = pgm_read_dword(&centroids[i][j]);
            // Handle underflow
            uint32_t diff;
            if (scaled_data > centroid) {
                diff = scaled_data - centroid;
            } else {
                diff = centroid - scaled_data;
            }
            distance += (uint64_t)(diff * diff);

            // DEBUG_PRINT("  Dimension "); DEBUG_PRINT(j); DEBUG_PRINT(": data="); DEBUG_PRINT(data[j]);
            // DEBUG_PRINT(", scaled_data="); DEBUG_PRINT(scaled_data);
            // DEBUG_PRINT(", centroid="); DEBUG_PRINT(centroids[i][j]);
            // DEBUG_PRINT(", diff="); DEBUG_PRINT(diff);
            // DEBUG_PRINT(", distance="); DEBUG_PRINTLN(distance);
        }

        // Track the centroid with the minimum distance
        if (distance < min_distance) {
            min_distance = distance;
            closest_centroid = i;
        }

        DEBUG_PRINT("Distance to centroid "); DEBUG_PRINT(i); DEBUG_PRINT(": "); DEBUG_PRINTLN(distance);
    }

    DEBUG_PRINT("Closest centroid: "); DEBUG_PRINTLN(closest_centroid);

    return closest_centroid;
}
