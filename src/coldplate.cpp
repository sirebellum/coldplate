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

// Activation function (e.g., ReLU)
static int32_t relu(int32_t x) {
    return x > 0 ? x : 0;
}

// Function to compute dot product between inputs and weights
static int32_t dot_product(const uint8_t* input, const int32_t* weights, uint16_t length) {
    int32_t sum = 0;
    for (uint16_t i = 0; i < length; i++) {
        sum += input[i] * weights[i];
    }
    return sum;
}

void maxpool2x2(uint8_t input[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y],
                uint8_t output[MLX90640_RESOLUTION_X/2 * MLX90640_RESOLUTION_Y/2]) {
    for (uint8_t y = 0; y < MLX90640_RESOLUTION_Y/2; y++) {
        for (uint8_t x = 0; x < MLX90640_RESOLUTION_X/2; x++) {
            uint8_t max_val = 0;
            for (uint8_t j = 0; j < 2; j++) {
                for (uint8_t i = 0; i < 2; i++) {
                    uint8_t val = input[(y*2 + j) * MLX90640_RESOLUTION_X + x*2 + i];
                    if (val > max_val) {
                        max_val = val;
                    }
                }
            }
            output[y * MLX90640_RESOLUTION_X/2 + x] = max_val;
        }
    }
}

// Function to compute the neural network inference
uint8_t classify_point(uint8_t data_point[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y],
                       const int32_t weights_input[NN_INPUT_SIZE * NN_HIDDEN_SIZE],
                       const int32_t weights_hidden[NN_HIDDEN_SIZE * NN_OUTPUT_SIZE]) {
    // Downsample the data point
    uint8_t downsampled_data[MLX90640_RESOLUTION_X/2 * MLX90640_RESOLUTION_Y/2] = {0};
    maxpool2x2(data_point, downsampled_data);
    
    // Allocate hidden layer
    int32_t hidden_layer[NN_HIDDEN_SIZE] = {0};

    // Forward pass for the hidden layer
    for (uint16_t i = 0; i < NN_HIDDEN_SIZE; i++) {
        int32_t weights[NN_INPUT_SIZE] = {0};
        for (uint16_t j = 0; j < NN_INPUT_SIZE; j++) {
            weights[j] = pgm_read_dword(&weights_input[i * NN_INPUT_SIZE + j]);
        }
        hidden_layer[i] = dot_product(downsampled_data, weights, NN_INPUT_SIZE);
        hidden_layer[i] = relu(hidden_layer[i]);
    }

    // Allocate output layer
    int32_t output_layer[NN_OUTPUT_SIZE] = {0};

    // Forward pass for the output layer
    for (uint16_t i = 0; i < NN_OUTPUT_SIZE; i++) {
        int32_t weights[NN_HIDDEN_SIZE] = {0};
        for (uint16_t j = 0; j < NN_HIDDEN_SIZE; j++) {
            weights[j] = pgm_read_dword(&weights_hidden[i * NN_HIDDEN_SIZE + j]);
        }
        output_layer[i] = dot_product((uint8_t*)hidden_layer, weights, NN_HIDDEN_SIZE);
    }

    // Find the class with the highest score
    uint8_t predicted_class = 0;
    int32_t max_score = output_layer[0];
    for (uint8_t i = 1; i < NN_OUTPUT_SIZE; i++) {
        if (output_layer[i] > max_score) {
            max_score = output_layer[i];
            predicted_class = i;
        }
    }

    return predicted_class;
}

String detect_cats(uint8_t data[MLX90640_RESOLUTION_X*MLX90640_RESOLUTION_Y],
                   const int32_t weights_input[NN_INPUT_SIZE*NN_HIDDEN_SIZE],
                   const int32_t weights_hidden[NN_HIDDEN_SIZE*NN_OUTPUT_SIZE]) {
    // Use NN to detect cats
    String cat_detected;

    // Classify the data point
    uint8_t data_class = classify_point(data, weights_input, weights_hidden);

    // Check the class
    if (data_class == 0) { // No cats detected
        cat_detected = "None";
    } else if (data_class == 1) { // Zephyr detected
        cat_detected = "Zephyr";
    } else if (data_class == 2) { // Flea detected
        cat_detected = "Flea";
    } else if (data_class == 3) { // Both cats detected
        cat_detected = "Both";
    } else { // Unknown cat detected
        cat_detected = "Unknown";
    }

    return cat_detected;
}
