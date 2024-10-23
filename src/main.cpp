#include "coldplate.h"
#include <base64.hpp>


// Splash screens
const uint16_t cat_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] PROGMEM = {
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

// Kmeans centroids
uint32_t *centroids;
uint8_t centroids_count;

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create objects for the IR sensor
Adafruit_MLX90640 mlx;
bool mlx_detected = false;

// Create objects for the sht35 sensor
Adafruit_SHT31 sht35 = Adafruit_SHT31();

// Create objects for ADC sensing the capacitance of the plate
int adc_values[ADC_SAMPLES];

// Network stuff
const char* ssid = "Mordor";
const char* password = "0ned0esn0tsimplyl0gint0m0rd0r";
const char* filenameUrl = "http://192.168.69.9:6969/filename";
const char* uploadUrl = "http://192.168.69.9:6969/upload";
const char* centroidscountUrl = "http://192.168.69.9:6969/centroids_count";
const char* centroidsUrl = "http://192.168.69.9:6969/serve_centroids";

// Create a client
WiFiClient client;
HTTPClient http;

// Ultrasonic time tracking
unsigned long ultrasonic_start_time = 0;
unsigned long ultrasonic_end_time = 0;

// IR buffers
float ir_data[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];
uint8_t mlx_buffer[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];

void setup() {
    Serial.begin(9600);

    system_update_cpu_freq(160);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    pinMode(LED_BUILTIN, OUTPUT);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display_splash_screen("Initializing...", caution_splashscreen, &display);

    pwm_init();
    ultrasonic_init();
    adc_init();

    pinMode(TEG_PIN, OUTPUT);
    pinMode(TEG_AUX_PIN, OUTPUT);
    digitalWrite(TEG_PIN, LOW);
    digitalWrite(TEG_AUX_PIN, LOW);

    ultrasonic_start_time = millis();
    ultrasonic_start();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        display_splash_screen("Connecting to WiFi...", caution_splashscreen, &display);
        delay(500);
    }

    pull_centroids(centroids, &centroids_count);

    mlx.begin();
    mlx.setRefreshRate(MLX90640_16_HZ);
    mlx.setMode(MLX90640_INTERLEAVED);

    sht35.begin(0x44);
}

void loop(void) {
    // Get the IR data
    mlx.getFrame(ir_data);

    // Convert the IR data to a buffer
    for (int i = 0; i < MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y; i++) {
        mlx_buffer[i] = ir_data[i];
    }

    // Get temps
    int32_t water_temp = (int32_t)(sht35.readTemperature() * 100);
    int32_t hot_temp = calculate_max_temp(mlx_buffer);
    int32_t cold_temp = calculate_min_temp(mlx_buffer);
    int32_t temp_diff = hot_temp - cold_temp;

    // If no temperature gradient, assume no sensor
    // Use water temp
    if (temp_diff == 0) {
        mlx_detected = false;
        hot_temp = water_temp;
        cold_temp = TEMP_IDLE;
        temp_diff = hot_temp - cold_temp;
    }

    // If the sensor is detected, upload the data
    if (mlx_detected) {
        uploadThermalData(ir_data);
    }

    // Detect food
    bool food_detected = detect_food(adc_values);

    // Adjust TEG power, pump speed, and fan speed based on temperature readings
    adjust_teg_power(hot_temp, cold_temp, food_detected);
    adjust_aux_teg_power(hot_temp, cold_temp);
    adjust_pump_speed(temp_diff);
    adjust_fan_speed(hot_temp);

    // Perform kmeans clustering
    bool cat_detected;
    if (mlx_detected) {
        cat_detected = kmeans_cluster(mlx_buffer, centroids, centroids_count) > 0;
    } else {
        cat_detected = false;
    }

    // Update the display (1 decimal point)
    char hot_temp_str[6];
    char cold_temp_str[6];
    dtostrf(hot_temp / 100.0, 4, 1, hot_temp_str);
    dtostrf(cold_temp / 100.0, 4, 1, cold_temp_str);
    String message = "Hot: " + String(hot_temp_str) + " Cold: " + String(cold_temp_str);
    if (cat_detected) {
        display_splash_screen(message, cat_splashscreen, &display);
    } else if (digitalRead(PWM_ULTRASONIC_PIN) > 0) {
        display_splash_screen(message, ultrasonic_splashscreen, &display);
    } else {
        display_splash_screen(message, snowflake_splashscreen, &display);
    }

    // Check ultrasonic cleaning status
    check_ultrasonic_cleaning(cat_detected, &ultrasonic_start_time, &ultrasonic_end_time);
}

// Function to pull the latest centroids from the server
void pull_centroids(uint32_t *centroids, uint8_t *centroids_count) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return;
    }

    // Get the number of centroids
    http.begin(client, centroidscountUrl);
    DEBUG_PRINTLN("Getting centroids count...");
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        *centroids_count = (uint8_t)http.getString().toInt();
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        return;
    }

    // Allocate memory for the centroids
    centroids = (uint32_t *)malloc(*centroids_count * KMEANS_DIMENSIONALITY * sizeof(uint32_t));
    int size_centroids_base64 = 4 * ((*centroids_count * KMEANS_DIMENSIONALITY * sizeof(uint32_t) + 2) / 3);
    if (size_centroids_base64 % 4) {
        size_centroids_base64 += 4 - (size_centroids_base64 % 4);
    }
    unsigned char *centroids_base64 = (unsigned char *)malloc(size_centroids_base64 + 1);

    // Get the centroids
    http.begin(client, centroidsUrl);
    DEBUG_PRINTLN("Getting centroids...");
    httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        String centroids_str = http.getString();
        DEBUG_PRINTLN(centroids_str);
        encode_base64((unsigned char*)centroids_str.c_str(), centroids_str.length(), centroids_base64);
        for (int i = 0; i < *centroids_count * KMEANS_DIMENSIONALITY; i++) {
            centroids[i] = (uint32_t)centroids_base64[i];
        }
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        return;
    }
}

// Function to send MLX90640 data array
bool uploadThermalData(const float data[MLX90640_RESOLUTION_Y * MLX90640_RESOLUTION_X]) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return false;
    }

    // Get filename
    String filename;
    http.begin(client, filenameUrl);
    DEBUG_PRINTLN("Getting filename...");
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        filename = http.getString();
        DEBUG_PRINTLN(filename);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        return false;
    }
    http.end();

    String base_url = uploadUrl;
    base_url += "?filename=";
    base_url += filename;
    base_url += "&data=";
    size_t base_url_size = base_url.length();

    // get URL buffer
    size_t byte_size, base64_size;
    byte_size = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y * sizeof(float);
    base64_size = 4 * ((byte_size + 2) / 3);
    if (base64_size % 4) {
        base64_size += 4 - (base64_size % 4);
    }
    unsigned char *getUrl = (unsigned char *)malloc(base_url_size + base64_size + 1);
    
    // Zero out the buffer
    memset(getUrl, 0, base_url_size + base64_size + 1);

    // Copy the base url
    memcpy(getUrl, base_url.c_str(), base_url_size);

    // Encode the data
    encode_base64((unsigned char*)data, byte_size, getUrl + base_url_size);

    // Send the data
    http.begin(client, (const char*)getUrl);
    DEBUG_PRINTLN("Uploading thermal data...");
    httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        // Deallocate
        free(getUrl);
        return false;
    }

    DEBUG_PRINTLN("Thermal data upload complete.");
    // Deallocate
    free(getUrl);
    return true;
}

