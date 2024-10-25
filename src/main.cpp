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

// Kmeans centroids
uint16_t *_centroids = nullptr;
uint16_t **centroids = &_centroids;
uint8_t centroids_count = 0;
unsigned int centroids_pull_time = 0;

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
const char* uploadUrl = "http://192.168.69.9:6969/upload";
const char* centroidscountUrl = "http://192.168.69.9:6969/centroids_count";
const char* centroidsUrl = "http://192.168.69.9:6969/serve_centroids";

// Create a client
WiFiClient client;
HTTPClient http;

// IR buffers
float ir_data[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];
uint16_t mlx_buffer[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];

void setup() {
    Serial.begin(9600);

    system_update_cpu_freq(160);

    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    pinMode(LED_BUILTIN, OUTPUT);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display_splash_screen("Initializing...", caution_splashscreen, &display);

    pwm_init();
    adc_init();

    pinMode(TEG_PIN, OUTPUT);
    pinMode(TEG_AUX_PIN, OUTPUT);
    digitalWrite(TEG_PIN, LOW);
    digitalWrite(TEG_AUX_PIN, LOW);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        display_splash_screen("Connecting to WiFi...", caution_splashscreen, &display);
        delay(500);
    }

    centroids_count = pull_centroids(centroids);
    centroids_pull_time = millis();

    mlx.begin(MLX90640_I2C_ADDR, &Wire);
    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_16BIT);
    mlx.setRefreshRate(MLX90640_1_HZ);

    sht35.begin(0x44);
}

void loop(void) {
    // Get the IR data
    mlx.getFrame(ir_data);

    // Convert the IR data to a buffer
    for (int i = 0; i < MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y; i++) {
        mlx_buffer[i] = ir_data[i] * 100;
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
    } else {
        mlx_detected = true;
    }

    // If the sensor is detected, upload the data
    if (mlx_detected) {
        uploadThermalData(ir_data);
    }

    // Adjust TEG power, pump speed, and fan speed based on temperature readings
    adjust_teg_power(cold_temp);
    adjust_aux_teg_power(water_temp);
    adjust_pump_speed(water_temp);
    adjust_fan_speed(water_temp);

    // Perform kmeans clustering
    bool cat_detected;
    if (mlx_detected && *centroids != nullptr) {
        cat_detected = kmeans_cluster(mlx_buffer, *centroids, centroids_count) > 0;
    } else {
        cat_detected = false;
    }

    // Update the display (1 decimal point)
    char hot_temp_str[6];
    char cold_temp_str[6];
    char water_temp_str[6];
    dtostrf(hot_temp / 100.0, 4, 1, hot_temp_str);
    dtostrf(cold_temp / 100.0, 4, 1, cold_temp_str);
    dtostrf(water_temp / 100.0, 4, 1, water_temp_str);
    String message = String(hot_temp_str) + " " + String(cold_temp_str) + " " + String(water_temp_str);
    if (cat_detected) {
        display_splash_screen(message, cat_splashscreen, &display);
    } else {
        display_splash_screen(message, snowflake_splashscreen, &display);
    }

    // Pull the centroids every 2 hours
    if (millis() - centroids_pull_time > 7200000) {
        centroids_count = pull_centroids(centroids);
        centroids_pull_time = millis();
    }
}

// Function to pull the latest centroids from the server
uint8_t pull_centroids(uint16_t **centroids) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return -1;
    }

    // Get the number of centroids
    http.begin(client, centroidscountUrl);
    DEBUG_PRINTLN("Getting centroids count...");
    int httpResponseCode = http.GET();
    uint8_t centroids_count;
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        centroids_count = (uint8_t)http.getString().toInt();
        DEBUG_PRINT("Centroids count: "); DEBUG_PRINTLN(centroids_count);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        http.end();
        return -1;
    }
    http.end();

    // Deallocate memory if it exists
    if (*centroids != nullptr) {
        free(*centroids);
    }

    // Allocate memory for the centroids
    int centroids_size = centroids_count * KMEANS_DIMENSIONALITY * sizeof(uint16_t);
    *centroids = (uint16_t *)malloc(centroids_size);

    // Get the centroids
    http.begin(client, centroidsUrl);
    DEBUG_PRINTLN("Getting centroids...");
    httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        DEBUG_PRINTLN("Centroids pulled successfully.");
        DEBUG_PRINT("Centroids count: "); DEBUG_PRINTLN(centroids_size);
    } else {
        DEBUG_PRINT("Error on sending GET: ");
        DEBUG_PRINTLN(httpResponseCode);
        http.end();
        return -1;
    }
    http.end();

    return centroids_count;
}

// Function to send MLX90640 data array
bool uploadThermalData(const float data[MLX90640_RESOLUTION_Y * MLX90640_RESOLUTION_X]) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return false;
    }

    // Prepare data for POST
    String postUrl = uploadUrl;
    String payload;
    payload += "{\"data\":\"";

    // Encode the data to base64
    size_t byte_size = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y * sizeof(float);
    size_t base64_size = 4 * ((byte_size + 2) / 3);
    if (base64_size % 4) {
        base64_size += 4 - (base64_size % 4);
    }
    unsigned char *encodedData = (unsigned char *)malloc(base64_size + 1);
    encode_base64((unsigned char*)data, byte_size, encodedData);

    // Add encoded data to the payload
    payload += String((char*)encodedData);
    payload += "\"}";

    // Send POST request
    http.begin(client, postUrl);
    http.addHeader("Content-Type", "application/json");
    DEBUG_PRINTLN("Uploading thermal data...");
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
    } else {
        DEBUG_PRINT("Error on sending POST: ");
        DEBUG_PRINTLN(httpResponseCode);
        // Deallocate
        free(encodedData);
        http.end();
        return false;
    }

    DEBUG_PRINTLN("Thermal data upload complete.");
    // Deallocate
    free(encodedData);
    http.end();
    return true;
}

