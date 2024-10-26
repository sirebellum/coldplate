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

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create objects for the sht35 sensor
Adafruit_SHT31 sht35 = Adafruit_SHT31();

// Vars for button press
bool button_detected = false;
unsigned int button_detect_time = 0;

// Vars for ultrasonic transducer
unsigned int ultrasonic_on_time = 0;
unsigned int ultrasonic_off_time = 0;

// Vars for object detection
uint8_t *_obj_buffer = nullptr;
uint8_t **obj_buffer = &_obj_buffer;
uint8_t obj_buffer_len = 0;

// Network stuff
const char* ssid = "Mordor";
const char* password = "0ned0esn0tsimplyl0gint0m0rd0r";
const char* od_endpoint = "http://192.168.69.11:5000/object_detection";
const char* od_len_endpoint = "http://192.168.69.11:5000/object_detection_len";

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

    if (!sht35.begin(0x44)) {
        DEBUG_PRINTLN("Error: SHT35 sensor not found.");
        display_splash_screen("Error: SHT35 Init Fail", caution_splashscreen, &display);
        while (1); // Halt execution
    }
}

ICACHE_RAM_ATTR void button_detect_isr() {
    button_detected = true;
    button_detect_time = millis();
    DEBUG_PRINTLN("Button pressed");
}

void loop(void) {
    // Get temperatures
    float temp = sht35.readTemperature();
    if (isnan(temp)) {
        DEBUG_PRINTLN("Error: Failed to read temperature from SHT35.");
        display_splash_screen("Error: Water Temp Read Fail", caution_splashscreen, &display);
        delay(1000);
        return; // Skip this loop iteration
    }
    int16_t water_temp = (int16_t)(temp * TEMP_SCALE);

    // Perform object detection
    int obj_detect_status = obj_detect_via_web(*obj_buffer);
    if (obj_detect_status != 0) {
        DEBUG_PRINTLN("Error: Failed to get object detection data.");
        display_splash_screen("Error: Object Detect Fail", caution_splashscreen, &display);
        delay(1000);
        return; // Skip this loop iteration
    }
    bool zephyr_present = detect_object(*obj_buffer, obj_buffer_len, OBJ_ZEPHYR);
    bool flea_present = detect_object(*obj_buffer, obj_buffer_len, OBJ_FLEA);

    // Update coldplate components
    adjust_coldplate(water_temp);
    adjust_water_loop(water_temp);
    bool ultrasonic_on = adjust_ultrasonic(&ultrasonic_on_time, &ultrasonic_off_time, zephyr_present, flea_present);

    // Message to display
    char water_temp_str[6];
    dtostrf(water_temp / TEMP_SCALE, 4, 1, water_temp_str);
    String message = "Water Temp: " + String(water_temp_str) + "C";

    // Display the message
    if (ultrasonic_on) {
        display_splash_screen(message, ultra_splashcreen, &display);
    } else {
        display_splash_screen(message, snowflake_splashscreen, &display);
    }
}

// Function to pull object detection data from the web
int obj_detect_via_web(uint8_t *obj_buffer) {
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("Error: WiFi not connected.");
        return -1;
    }

    // Get the len of the object detection data
    http.begin(client, od_len_endpoint);
    int httpCode = http.GET();
    if (httpCode != 200) {
        DEBUG_PRINT("Error: HTTP GET failed with code ");
        DEBUG_PRINTLN(httpCode);
        http.end();
        return -1;
    } else {
        DEBUG_PRINTLN("HTTP GET successful.");
    }
    String payload = http.getString();
    obj_buffer_len = payload.toInt();
    http.end();

    if (obj_buffer_len <= 0) {
        DEBUG_PRINTLN("Error: Invalid object detection data length.");
        return -1;
    }

    // Allocate memory for the base64-encoded object detection data
    unsigned int base64_len = encode_base64_length(obj_buffer_len);
    unsigned char *base64_obj_buffer = (unsigned char *)malloc(base64_len);

    // Get the object detection data
    http.begin(client, od_endpoint);
    httpCode = http.GET();
    if (httpCode != 200) {
        DEBUG_PRINT("Error: HTTP GET failed with code ");
        DEBUG_PRINTLN(httpCode);
        http.end();
        return -1;
    } else {
        DEBUG_PRINTLN("HTTP GET successful.");
    }
    http.getStreamPtr()->readBytes(base64_obj_buffer, base64_len);
    http.end();

    // Decode the base64-encoded object detection data
    unsigned int decoded_len = decode_base64(base64_obj_buffer, base64_len, obj_buffer);
    free(base64_obj_buffer);

    // Make sure the decoded length matches the expected length
    if (decoded_len != obj_buffer_len) {
        DEBUG_PRINTLN("Error: Decoded object detection data length mismatch.");
        return -1;
    }

    return 0;
}