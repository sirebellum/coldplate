#include "coldplate.h"
#include <base64.hpp>


// Splash screens
const uint16_t cat_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] = {
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
const uint16_t caution_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] = {
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
const uint16_t snowflake_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] = {
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
const uint16_t ultra_splashcreen[SPLASH_HEIGHT][SPLASH_WIDTH/16] = {
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
int16_t *_centroids = nullptr;
int16_t **centroids = &_centroids;
uint8_t centroids_count = 0;
unsigned int centroids_pull_time = 0;

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create objects for the IR sensor
Adafruit_MLX90640 mlx;

// IR buffers
int16_t ir_data[MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y];

// Create objects for the sht35 sensor
Adafruit_SHT31 sht35 = Adafruit_SHT31();

// Vars for food detection
bool food_detected = false;
unsigned int food_detect_time = 0;

// Vars for ultrasonic transducer
unsigned int ultrasonic_on_time = 0;
unsigned int ultrasonic_off_time = 0;

// Network stuff
const char* ssid = "Mordor";
const char* password = "0ned0esn0tsimplyl0gint0m0rd0r";
const char* uploadUrl = "http://192.168.69.9:6969/upload";
const char* centroidscountUrl = "http://192.168.69.9:6969/centroids_count";
const char* centroidsUrl = "http://192.168.69.9:6969/serve_centroids";

// Create a client
WiFiClient client;
HTTPClient http;

void setup() {
    #if DEBUG
        Serial.begin(9600);
    #endif

    system_update_cpu_freq(160);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(4000000);
    pinMode(LED_BUILTIN, OUTPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        DEBUG_PRINTLN("Error: SSD1306 allocation failed.");
        display_splash_screen("Error: Display Init Fail", caution_splashscreen, &display);
        while (1){
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }; // Halt execution
    }
    display_splash_screen("Initializing...", snowflake_splashscreen, &display);

    pwm_init();
    pin_init();

    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) { // 30-second timeout
        display_splash_screen("Connecting to WiFi...", snowflake_splashscreen, &display);
        delay(500);
    }
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("Error: Failed to connect to WiFi.");
        display_splash_screen("Error: WiFi Connect Fail", caution_splashscreen, &display);
        // Handle WiFi connection failure (e.g., retry or enter offline mode)
    }

    centroids_count = pull_centroids(centroids);
    if (centroids_count < 0) {
        DEBUG_PRINTLN("Error: Failed to pull centroids.");
        display_splash_screen("Error: Centroids Pull Fail", caution_splashscreen, &display);
        centroids_count = 0; // Set to zero to prevent null pointer access
    }
    centroids_pull_time = millis();

    if (!mlx.begin(MLX90640_I2C_ADDR, &Wire)) {
        DEBUG_PRINTLN("Error: MLX90640 sensor not found.");
        display_splash_screen("Error: MLX90640 Init Fail", caution_splashscreen, &display);
        while (1); // Halt execution
    }
    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_16BIT);
    mlx.setRefreshRate(MLX90640_2_HZ);

    if (!sht35.begin(0x44)) {
        DEBUG_PRINTLN("Error: SHT35 sensor not found.");
        display_splash_screen("Error: SHT35 Init Fail", caution_splashscreen, &display);
        while (1); // Halt execution
    }
}

ICACHE_RAM_ATTR void food_detect_isr() {
    food_detected = true;
    food_detect_time = millis();
    DEBUG_PRINTLN("Button pressed");
}

void loop(void) {
    // Get the IR data
    int mlx_status = read_ir_sensor(ir_data, &mlx);
    if (mlx_status != 0) {
        DEBUG_PRINT("Error: Failed to get IR frame data. Status code: ");
        DEBUG_PRINTLN(status);
        display_splash_screen("Error: IR Data Fail", caution_splashscreen, &display);
        delay(1000);
        return; // Skip this loop iteration
    }

    // Get temperatures
    float temp = sht35.readTemperature();
    if (isnan(temp)) {
        DEBUG_PRINTLN("Error: Failed to read temperature from SHT35.");
        display_splash_screen("Error: Water Temp Read Fail", caution_splashscreen, &display);
        delay(1000);
        return; // Skip this loop iteration
    }
    int16_t water_temp = (int16_t)(temp * TEMP_SCALE);
    int16_t hot_temp = calculate_max_temp(ir_data);
    int16_t cold_temp = calculate_min_temp(ir_data);

    // Upload the data if the data has an actual temperature
    if (mlx_status == 0 && hot_temp != cold_temp) {
        if (!upload_thermal_data(ir_data)) {
            DEBUG_PRINTLN("Error: Failed to upload thermal data.");
            display_splash_screen("Error: Data Upload Fail", caution_splashscreen, &display);
            delay(1000);
        }
    }

    // Check if it's time to pull the centroids
    if (millis() - centroids_pull_time > KMEANS_PULL_INTERVAL) {
        centroids_count = pull_centroids(centroids);
        if (centroids_count < 0) {
            DEBUG_PRINTLN("Error: Failed to pull centroids.");
            display_splash_screen("Error: Centroids Pull Fail", caution_splashscreen, &display);
            centroids_count = 0; // Set to zero to prevent null pointer access
        }
        centroids_pull_time = millis();
    }

    // Use kmeans clustering to detect cat
    bool zephyr, flea;
    if (centroids_count > 0 && hot_temp != cold_temp && *centroids != nullptr) {
        uint8_t cluster = calculate_kmeans_cluster(ir_data, *centroids, centroids_count);
        zephyr = cluster == KMEANS_CLASS_ZEPHYR;
        flea = cluster == KMEANS_CLASS_FLEA;
    } else {
        zephyr = false;
        flea = false;
    }

    // Kmeans disable
    zephyr = false;
    flea = false;

    // Update various components
    // bool ultrasonic_on = adjust_ultra_sonic(&ultrasonic_on_time, &ultrasonic_off_time, zephyr, flea);
    bool ultrasonic_on = false;
    adjust_teg_power(cold_temp, food_detected);
    adjust_aux_teg_power(water_temp);
    adjust_fan_speed(water_temp);
    adjust_pump_speed(water_temp);

    // Message to display
    char hot_temp_str[6];
    char cold_temp_str[6];
    char water_temp_str[6];
    dtostrf(hot_temp / TEMP_SCALE, 4, 1, hot_temp_str);
    dtostrf(cold_temp / TEMP_SCALE, 4, 1, cold_temp_str);
    dtostrf(water_temp / TEMP_SCALE, 4, 1, water_temp_str);
    String message = String(hot_temp_str) + " " + String(cold_temp_str) + " " + String(water_temp_str);

    // Display the message
    if (zephyr) {
        display_splash_screen(message, cat_splashscreen, &display);
    } else if (ultrasonic_on) {
        display_splash_screen(message, ultra_splashcreen, &display);
    } else {
        display_splash_screen(message, snowflake_splashscreen, &display);
    }
}

// Function to pull the latest centroids from the server
int pull_centroids(int16_t **centroids) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return -1;
    }

    // Get the number of centroids
    http.begin(client, centroidscountUrl);
    DEBUG_PRINTLN("Getting centroids count...");
    int httpResponseCode = http.GET();
    int centroids_count;
    if (httpResponseCode == 200) {
        DEBUG_PRINT("HTTP Response: "); DEBUG_PRINTLN(httpResponseCode);
        centroids_count = (int)http.getString().toInt();
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
    int centroids_size = centroids_count * KMEANS_DIMENSIONALITY * sizeof(int16_t);
    *centroids = (int16_t *)malloc(centroids_size);

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
int upload_thermal_data(const int16_t data[MLX90640_RESOLUTION_Y * MLX90640_RESOLUTION_X]) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("WiFi not connected");
        return false;
    }

    // Prepare data for POST
    String postUrl = uploadUrl;
    String payload;
    payload += "{\"data\":\"";

    // Encode the data to base64
    size_t byte_size = MLX90640_RESOLUTION_X * MLX90640_RESOLUTION_Y * sizeof(int16_t);
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
