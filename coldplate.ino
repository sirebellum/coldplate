#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

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
#define PWM_TEG_PIN PB2 // (pin 10)
#define PWM_PUMP_PIN PD5 // (pin 5)
#define PWM_FAN_PIN PD6 // (pin 6)
#define PWM_ULTRASONIC_PIN PB1 // (pin 9)

// GPIO output pins
#define TEG_AUX_PIN PB3 // (pin 11)

// Ultrasonic cleaning config
#define ULTRASONIC_FREQ 40 // Ultrasonic frequency in kHz
#define ULTRASONIC_DURATION (1000UL * 60UL * 5UL) // 5 minutes
#define ULTRASONIC_INTERVAL (1000UL * 60UL * 50UL) // 1 hour

// Adjusted Voltage Divider Constants
// Use scaled integer math for voltage calculations
#define VOLTAGE_DIVIDER_R1 15000  // Resistor R1 value in ohms
#define VOLTAGE_DIVIDER_R2 10000  // Resistor R2 value in ohms
// The above values create a voltage divider that scales 12V down to 4.8V

// OLED display config
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Create an SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Create BME280 objects for two temperature sensors
Adafruit_BME280 bme_cold_side; // Cold side sensor (I2C)
Adafruit_BME280 bme_cat_pad;   // Cat pad temperature sensor (I2C)

// Timing variables
unsigned long last_detection_time = 0;
unsigned long last_update_time = millis();
unsigned long refresh_rate = 0;

// Ultrasonic cleaning variables
unsigned long ultrasonic_start_time = millis();
unsigned long ultrasonic_end_time = 0;
bool ultrasonic_cleaning = true;
#define SINE_TABLE_SIZE 1024
const uint16_t sine_wave[SINE_TABLE_SIZE] PROGMEM = {
    32767, 32968, 33169, 33370, 33571, 33772, 33973, 34174,
    34375, 34576, 34776, 34977, 35178, 35378, 35578, 35779,
    35979, 36179, 36379, 36578, 36778, 36978, 37177, 37376,
    37575, 37774, 37972, 38171, 38369, 38567, 38765, 38962,
    39160, 39357, 39554, 39750, 39946, 40142, 40338, 40534,
    40729, 40924, 41118, 41313, 41507, 41700, 41893, 42086,
    42279, 42471, 42663, 42854, 43046, 43236, 43427, 43617,
    43806, 43995, 44184, 44372, 44560, 44747, 44934, 45121,
    45307, 45492, 45677, 45862, 46046, 46229, 46412, 46595,
    46777, 46958, 47139, 47320, 47500, 47679, 47858, 48036,
    48213, 48391, 48567, 48743, 48918, 49093, 49267, 49440,
    49613, 49785, 49956, 50127, 50298, 50467, 50636, 50804,
    50972, 51138, 51305, 51470, 51635, 51799, 51962, 52125,
    52287, 52448, 52608, 52768, 52927, 53085, 53242, 53399,
    53554, 53710, 53864, 54017, 54170, 54322, 54473, 54623,
    54772, 54921, 55069, 55215, 55362, 55507, 55651, 55795,
    55937, 56079, 56220, 56360, 56499, 56637, 56774, 56911,
    57046, 57181, 57314, 57447, 57579, 57710, 57840, 57969,
    58097, 58224, 58350, 58475, 58599, 58722, 58845, 58966,
    59086, 59205, 59324, 59441, 59557, 59672, 59787, 59900,
    60012, 60123, 60234, 60343, 60451, 60558, 60664, 60769,
    60873, 60975, 61077, 61178, 61278, 61376, 61474, 61570,
    61665, 61760, 61853, 61945, 62036, 62126, 62214, 62302,
    62388, 62474, 62558, 62641, 62723, 62804, 62884, 62963,
    63040, 63117, 63192, 63266, 63339, 63411, 63481, 63551,
    63619, 63686, 63752, 63817, 63881, 63943, 64004, 64065,
    64124, 64181, 64238, 64293, 64348, 64401, 64452, 64503,
    64552, 64601, 64648, 64694, 64738, 64782, 64824, 64865,
    64905, 64944, 64981, 65017, 65052, 65086, 65118, 65150,
    65180, 65209, 65236, 65263, 65288, 65312, 65335, 65356,
    65377, 65396, 65414, 65430, 65446, 65460, 65473, 65485,
    65495, 65504, 65512, 65519, 65525, 65529, 65532, 65534,
    65535, 65534, 65532, 65529, 65525, 65519, 65512, 65504,
    65495, 65485, 65473, 65460, 65446, 65430, 65414, 65396,
    65377, 65356, 65335, 65312, 65288, 65263, 65236, 65209,
    65180, 65150, 65118, 65086, 65052, 65017, 64981, 64944,
    64905, 64865, 64824, 64782, 64738, 64694, 64648, 64601,
    64552, 64503, 64452, 64401, 64348, 64293, 64238, 64181,
    64124, 64065, 64004, 63943, 63881, 63817, 63752, 63686,
    63619, 63551, 63481, 63411, 63339, 63266, 63192, 63117,
    63040, 62963, 62884, 62804, 62723, 62641, 62558, 62474,
    62388, 62302, 62214, 62126, 62036, 61945, 61853, 61760,
    61665, 61570, 61474, 61376, 61278, 61178, 61077, 60975,
    60873, 60769, 60664, 60558, 60451, 60343, 60234, 60123,
    60012, 59900, 59787, 59672, 59557, 59441, 59324, 59205,
    59086, 58966, 58845, 58722, 58599, 58475, 58350, 58224,
    58097, 57969, 57840, 57710, 57579, 57447, 57314, 57181,
    57046, 56911, 56774, 56637, 56499, 56360, 56220, 56079,
    55937, 55795, 55651, 55507, 55362, 55215, 55069, 54921,
    54772, 54623, 54473, 54322, 54170, 54017, 53864, 53710,
    53554, 53399, 53242, 53085, 52927, 52768, 52608, 52448,
    52287, 52125, 51962, 51799, 51635, 51470, 51305, 51138,
    50972, 50804, 50636, 50467, 50298, 50127, 49956, 49785,
    49613, 49440, 49267, 49093, 48918, 48743, 48567, 48391,
    48213, 48036, 47858, 47679, 47500, 47320, 47139, 46958,
    46777, 46595, 46412, 46229, 46046, 45862, 45677, 45492,
    45307, 45121, 44934, 44747, 44560, 44372, 44184, 43995,
    43806, 43617, 43427, 43236, 43046, 42854, 42663, 42471,
    42279, 42086, 41893, 41700, 41507, 41313, 41118, 40924,
    40729, 40534, 40338, 40142, 39946, 39750, 39554, 39357,
    39160, 38962, 38765, 38567, 38369, 38171, 37972, 37774,
    37575, 37376, 37177, 36978, 36778, 36578, 36379, 36179,
    35979, 35779, 35578, 35378, 35178, 34977, 34776, 34576,
    34375, 34174, 33973, 33772, 33571, 33370, 33169, 32968,
    32767, 32566, 32365, 32164, 31963, 31762, 31561, 31360,
    31159, 30958, 30758, 30557, 30356, 30156, 29956, 29755,
    29555, 29355, 29155, 28956, 28756, 28556, 28357, 28158,
    27959, 27760, 27562, 27363, 27165, 26967, 26769, 26572,
    26374, 26177, 25980, 25784, 25588, 25392, 25196, 25000,
    24805, 24610, 24416, 24221, 24027, 23834, 23641, 23448,
    23255, 23063, 22871, 22680, 22488, 22298, 22107, 21917,
    21728, 21539, 21350, 21162, 20974, 20787, 20600, 20413,
    20227, 20042, 19857, 19672, 19488, 19305, 19122, 18939,
    18757, 18576, 18395, 18214, 18034, 17855, 17676, 17498,
    17321, 17143, 16967, 16791, 16616, 16441, 16267, 16094,
    15921, 15749, 15578, 15407, 15236, 15067, 14898, 14730,
    14562, 14396, 14229, 14064, 13899, 13735, 13572, 13409,
    13247, 13086, 12926, 12766, 12607, 12449, 12292, 12135,
    11980, 11824, 11670, 11517, 11364, 11212, 11061, 10911,
    10762, 10613, 10465, 10319, 10172, 10027,  9883,  9739,
     9597,  9455,  9314,  9174,  9035,  8897,  8760,  8623,
     8488,  8353,  8220,  8087,  7955,  7824,  7694,  7565,
     7437,  7310,  7184,  7059,  6935,  6812,  6689,  6568,
     6448,  6329,  6210,  6093,  5977,  5862,  5747,  5634,
     5522,  5411,  5300,  5191,  5083,  4976,  4870,  4765,
     4661,  4559,  4457,  4356,  4256,  4158,  4060,  3964,
     3869,  3774,  3681,  3589,  3498,  3408,  3320,  3232,
     3146,  3060,  2976,  2893,  2811,  2730,  2650,  2571,
     2494,  2417,  2342,  2268,  2195,  2123,  2053,  1983,
     1915,  1848,  1782,  1717,  1653,  1591,  1530,  1469,
     1410,  1353,  1296,  1241,  1186,  1133,  1082,  1031,
      982,   933,   886,   840,   796,   752,   710,   669,
      629,   590,   553,   517,   482,   448,   416,   384,
      354,   325,   298,   271,   246,   222,   199,   178,
      157,   138,   120,   104,    88,    74,    61,    49,
       39,    30,    22,    15,     9,     5,     2,     0,
        0,     0,     2,     5,     9,    15,    22,    30,
       39,    49,    61,    74,    88,   104,   120,   138,
      157,   178,   199,   222,   246,   271,   298,   325,
      354,   384,   416,   448,   482,   517,   553,   590,
      629,   669,   710,   752,   796,   840,   886,   933,
      982,  1031,  1082,  1133,  1186,  1241,  1296,  1353,
     1410,  1469,  1530,  1591,  1653,  1717,  1782,  1848,
     1915,  1983,  2053,  2123,  2195,  2268,  2342,  2417,
     2494,  2571,  2650,  2730,  2811,  2893,  2976,  3060,
     3146,  3232,  3320,  3408,  3498,  3589,  3681,  3774,
     3869,  3964,  4060,  4158,  4256,  4356,  4457,  4559,
     4661,  4765,  4870,  4976,  5083,  5191,  5300,  5411,
     5522,  5634,  5747,  5862,  5977,  6093,  6210,  6329,
     6448,  6568,  6689,  6812,  6935,  7059,  7184,  7310,
     7437,  7565,  7694,  7824,  7955,  8087,  8220,  8353,
     8488,  8623,  8760,  8897,  9035,  9174,  9314,  9455,
     9597,  9739,  9883, 10027, 10172, 10319, 10465, 10613,
    10762, 10911, 11061, 11212, 11364, 11517, 11670, 11824,
    11980, 12135, 12292, 12449, 12607, 12766, 12926, 13086,
    13247, 13409, 13572, 13735, 13899, 14064, 14229, 14396,
    14562, 14730, 14898, 15067, 15236, 15407, 15578, 15749,
    15921, 16094, 16267, 16441, 16616, 16791, 16967, 17143,
    17321, 17498, 17676, 17855, 18034, 18214, 18395, 18576,
    18757, 18939, 19122, 19305, 19488, 19672, 19857, 20042,
    20227, 20413, 20600, 20787, 20974, 21162, 21350, 21539,
    21728, 21917, 22107, 22298, 22488, 22680, 22871, 23063,
    23255, 23448, 23641, 23834, 24027, 24221, 24416, 24610,
    24805, 25000, 25196, 25392, 25588, 25784, 25980, 26177,
    26374, 26572, 26769, 26967, 27165, 27363, 27562, 27760,
    27959, 28158, 28357, 28556, 28756, 28956, 29155, 29355,
    29555, 29755, 29956, 30156, 30356, 30557, 30758, 30958,
    31159, 31360, 31561, 31762, 31963, 32164, 32365, 32566
};
uint8_t sine_index = 0;

// Moving Average variables
#define MOVING_AVG_WINDOW 48 // Adjust this for how many readings you want to average
int32_t cold_temp_history[MOVING_AVG_WINDOW] = {0};
int32_t fan_speed_history[MOVING_AVG_WINDOW] = {0};
int32_t pump_speed_history[MOVING_AVG_WINDOW] = {0};
int32_t teg_power_history[MOVING_AVG_WINDOW] = {0};
int32_t moving_avg_temp = 0;
int history_index = 0;
int num_readings = 0;
bool food_detected = false;

// Splash screen stuff
#define SPLASH_HEIGHT 64
#define SPLASH_WIDTH 64
const bool snowflake_splashscreen[SPLASH_HEIGHT][SPLASH_WIDTH] PROGMEM = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// Flags for whether I2C devices are available
bool bme_cold_side_available = false;
bool bme_cat_pad_available = false;
bool display_available = false;

// Function prototypes
void pwm_init();
void adc_init();
uint16_t adc_read(uint8_t channel);
uint8_t adjust_teg_power(int32_t current_temp, int32_t cat_pad_temp, bool food_detected);
uint8_t adjust_teg_aux_power(int32_t hot_temp);
uint8_t adjust_pump_speed(int32_t temp_diff);
uint8_t adjust_fan_speed(int32_t hot_temp);
bool check_ultrasonic_cleaning();
void set_timer2_prescaler(uint8_t prescaler);
void display_splash_screen(String message, const bool *splashscreen);
void drawBanner(String message);
void drawSplashScreen(const bool *splashscreen);
void draw_graph(int cursor_y, const int32_t* data_history, int graph_height, int graph_width, float scale_factor, int base_y);
void update_display(int32_t voltage, int32_t hot_temp, int32_t cold_temp, int32_t temp_diff, int32_t cat_pad_temp, bool food_detected);
int32_t calculate_percentile_threshold(int32_t moving_avg_temp, int32_t percentile);
bool detect_food(int32_t current_temp);

void setup() {
    // Initialize PWM
    pwm_init();

    // Initialize ADC
    adc_init();

    // Set up Timer2 for interrupt (updates the sine wave)
    TCCR2A = 0;                             // Normal mode
    TIMSK2 = (1 << TOIE2);                  // Enable Timer2 overflow interrupt

    // Set Timer2 prescaler to 1 (no prescaler)
    set_timer2_prescaler(1);

    sei(); // Enable global interrupts

    // Initialize SSD1306 display
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        display_available = true; // Set flag if display is available
        // Display splash screen
        display_splash_screen("Cooling down...", &snowflake_splashscreen[0][0]);
    }

    // Initialize BME280 sensors
    if (bme_cold_side.begin(0x76)) {
        bme_cold_side_available = true; // Set flag if cold side sensor is available
    }

    if (bme_cat_pad.begin(0x77)) {
        bme_cat_pad_available = true; // Set flag if cat pad sensor is available
    }
    
    // Turn on TEG at full blast till cold side reaches idle temp
    // Run ultrasonic cleaning while we're at it
    OCR2B = 255; // Set TEG to full power
    OCR1B = 255 * 255; // Set pump to full power
    OCR0B = 255; // Set fan to full power
    OCR2A = 255; // Set auxillary TEG to full power
    int32_t cold_temp = bme_cold_side_available ? bme_cold_side.readTemperature() : 0.0;
    while (cold_temp > TEMP_IDLE) {
        cold_temp = bme_cold_side_available ? bme_cold_side.readTemperature() : 0.0;
        if (!ultrasonic_cleaning)
            ultrasonic_cleaning = true; // Start ultrasonic cleaning again if it was stopped
    }
}

// Timer2 overflow interrupt service routine
ISR(TIMER2_OVF_vect) {
    if (!ultrasonic_cleaning) return; // Skip if ultrasonic cleaning is not active
    uint16_t sine_value = pgm_read_word(&sine_wave[sine_index]); // Read sine value from PROGMEM
    OCR1A = sine_value; // Update PWM duty cycle
    sine_index = (sine_index + 1) % SINE_TABLE_SIZE;  // Move to the next sine value
}

void loop(void) {
    // Read ADC value from filtered voltage
    uint16_t adc_value = adc_read(0); // ADC0 connected to the RC filter output

    // Calculate the actual voltage
    int32_t measured_voltage = (adc_value * 5000) / 1023; // Voltage in millivolts (scaling by 1000)
    // Adjust for voltage divider if necessary
    int32_t voltage = measured_voltage * ((VOLTAGE_DIVIDER_R1 + VOLTAGE_DIVIDER_R2) / VOLTAGE_DIVIDER_R2);

    // Ensure voltage is within expected range
    if (voltage < 0) voltage = 0;

    // Convert voltage to temperature difference using Seebeck coefficient
    int32_t temp_diff = voltage / SEEBECK_COEFFICIENT; // Temperature difference in Kelvin

    // Read temperature from BME280 sensors
    int32_t cold_temp = bme_cold_side_available ? (int32_t)(bme_cold_side.readTemperature() * 100) : 0;
    int32_t cat_pad_temp = bme_cat_pad_available ? (int32_t)(bme_cat_pad.readTemperature() * 100) : 0;

    // Calculate hot side temperature
    int32_t hot_temp = cold_temp + temp_diff; // Calculate hot side temperature

    // Check for emergency temperature conditions
    while (cold_temp > TEMP_EMERGENCY || cat_pad_temp > TEMP_EMERGENCY) {
        // Emergency hold if temperature exceeds threshold
        OCR2B = 0; // Turn off TEG
        OCR1B = 0; // Turn off pump
        OCR0B = 0; // Turn off fan
        OCR2A = 0; // Turn off auxillary TEG
        OCR1A = 0; // Turn off ultrasonic transducer
        // Display emergency message if available
        if (display_available) {
            display_splash_screen("EMERGENCY SHUTDOWN", &snowflake_splashscreen[0][0]);
        }
        // Delay for emergency shutdown
        _delay_ms(1000);
    }

    // Update the temp history array with the latest readings
    cold_temp_history[history_index] = cold_temp;

    // Update the number of readings
    if (num_readings < MOVING_AVG_WINDOW) {
        num_readings++;
    }

    // Detect food presence based on deviation from moving average
    food_detected = detect_food(cold_temp);

    // Adjust TEG power, pump speed, and fan speed based on temperature readings
    uint8_t teg_pwm_value = adjust_teg_power(cold_temp, cat_pad_temp, food_detected);
    uint8_t pump_pwm_value = adjust_pump_speed(temp_diff);
    uint8_t fan_pwm_value = adjust_fan_speed(cat_pad_temp);
    uint8_t teg_aux_pwm_value = adjust_teg_aux_power(hot_temp);
    bool ultrasonic_cleaning = check_ultrasonic_cleaning();

    // Update the history
    fan_speed_history[history_index] = fan_pwm_value;
    pump_speed_history[history_index] = pump_pwm_value;
    teg_power_history[history_index] = teg_pwm_value;

    // Update the history index
    history_index = (history_index + 1) % MOVING_AVG_WINDOW;

    // Update refresh rate in hz
    refresh_rate = 1000 / (millis() - last_update_time);

    // Update last update time
    last_update_time = millis();

    // Update display if available
    if (display_available) {
        update_display(voltage, hot_temp, cold_temp, cat_pad_temp, food_detected, refresh_rate);
    }
}

// Function to draw caution ribbon in the yellow section (top 16 pixels)
void drawBanner(String message) {
    display.setTextColor(SSD1306_WHITE);
    // Border
    display.drawRect(0, 0, SCREEN_WIDTH, 16, SSD1306_WHITE);

    // Draw the banner text
    display.setTextSize(1);
    display.setCursor(SCREEN_WIDTH / 2 - message.length() * 3, 2);
    display.print(message);
}

// Function to draw a detailed splash screen image in the blue section (below 16 pixels)
// Pixel values are hardcoded in splashscreen
void drawSplashScreen(const bool *splashscreen) {
    // Center the splashscreen image
    int x_offset = (SCREEN_WIDTH - SPLASH_WIDTH) / 2;
    int y_offset = (SCREEN_HEIGHT - SPLASH_HEIGHT) / 2;

    // Draw the splashscreen image
    for (int y = 0; y < SPLASH_HEIGHT; y++) {
        for (int x = 0; x < SPLASH_WIDTH; x++) {
            bool pixel = pgm_read_byte(&splashscreen[y * SPLASH_WIDTH + x]);
            display.drawPixel(x + x_offset, y + y_offset, pixel);
        }
    }
}

// Main function to draw splash screen
void display_splash_screen(String message, const bool *splashscreen) {
  display.clearDisplay();

  // Draw the banner in the top yellow section
  drawBanner(message);

  // Draw a large splash screen in the lower blue section
  drawSplashScreen(splashscreen);

  display.display(); // Push the changes to the display
}

void pwm_init() {
    // Set PWM pins as output (exclude TEG auxiliary module which is now GPIO)
    DDRB |= (1 << PWM_TEG_PIN) | (1 << PWM_PUMP_PIN); // PB1 (Ultrasonic) and PB2 (Pump)
    DDRD |= (1 << PWM_FAN_PIN) | (1 << PWM_ULTRASONIC_PIN); // PD6 (Fan), PD5 (Pump)
    
    // Set the TEG auxiliary pin as GPIO output
    DDRB |= (1 << TEG_AUX_PIN);  // Use GPIO for TEG auxiliary module

    // Set Timer0 for 8-bit Fast PWM mode: Fan (OC0A), Pump (OC0B)
    TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1); // Fast PWM, non-inverting
    TCCR0B = (1 << CS00); // No prescaler

    // Set Timer1 for 16-bit Fast PWM mode: Ultrasonic Transducer (OC1A), Pump (OC1B)
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);  // Fast PWM, non-inverting on OC1A (Ultrasonic) and OC1B (Pump)
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10);  // Fast PWM with ICR1 as TOP, no prescaler

    // Set ICR1 for the desired PWM frequency (e.g., 400 kHz)
    ICR1 = 39;  // This value sets the PWM frequency based on the desired sine wave resolution

    // Set initial PWM values
    OCR0A = 0; // Fan speed (OC0A, PD6)
    OCR0B = 0; // Pump speed (OC0B, PD5)
    OCR1A = 0; // Ultrasonic transducer (OC1A, PB1)
    OCR1B = 0; // Pump speed (OC1B, PB2)
    
    // Set the TEG auxiliary module GPIO to a default state (low or high)
    PORTB &= ~(1 << TEG_AUX_PIN);  // Set to low (or high if needed)
}


void adc_init() {
    // AVcc with external capacitor at AREF pin, right adjust result
    ADMUX = (1 << REFS0);
    // Enable ADC, prescaler = 128
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    // Select ADC channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    // Start conversion
    ADCSRA |= (1 << ADSC);
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC))
        ;
    // Return the ADC value
    return ADC;
}

void set_timer2_prescaler(uint8_t prescaler) {
    // Clear the prescaler bits (CS22, CS21, CS20)
    TCCR2B &= ~(1 << CS22) & ~(1 << CS21) & ~(1 << CS20);
    
    // Set the new prescaler value
    switch (prescaler) {
        case 1:
            TCCR2B |= (1 << CS20); // No prescaler
            break;
        case 8:
            TCCR2B |= (1 << CS21); // Prescaler = 8
            break;
        case 64:
            TCCR2B |= (1 << CS22); // Prescaler = 64
            break;
        case 256:
            TCCR2B |= (1 << CS22) | (1 << CS21); // Prescaler = 256
            break;
        case 1024:
            TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler = 1024
            break;
        default:
            TCCR2B |= (1 << CS20); // Default to no prescaler if invalid input
            break;
    }
}

bool check_ultrasonic_cleaning() {
    // Check if ultrasonic cleaning is needed
    unsigned long current_time = millis();
    if (!ultrasonic_cleaning && (current_time - ultrasonic_end_time) >= ULTRASONIC_INTERVAL) {
        ultrasonic_cleaning = true;
        ultrasonic_start_time = current_time;
        set_timer2_prescaler(1); // Set Timer2 prescaler to 1 (no prescaler)
    } else if (ultrasonic_cleaning && (current_time - ultrasonic_start_time) >= ULTRASONIC_DURATION) {
        ultrasonic_cleaning = false;
        ultrasonic_end_time = current_time;
        set_timer2_prescaler(1024); // Set Timer2 prescaler to 1024
    }

    return ultrasonic_cleaning;
}

uint8_t adjust_teg_power(int32_t cold_temp, int32_t cat_pad_temp, bool food_detected) {
    uint8_t pwm_value = 0;

    // Adjust TEG power based on cold side temperature
    // Focus on maintaining idle temp if food is not detected
    if (!food_detected) {
        if (cold_temp < TEMP_IDLE) {
            pwm_value = 0; // If the cold side is too cold, turn off the TEG
        } else if (cold_temp > TEMP_MAX) {
            pwm_value = 255; // If the cold side is too hot, set TEG to full power
        } else {
            // Adjust TEG power based on the cold side temperature
            pwm_value = (uint8_t)((cold_temp - TEMP_IDLE) / (TEMP_MAX - TEMP_IDLE) * 255);
        }
    } else {
        if (cold_temp < TEMP_MIN) {
            pwm_value = 0; // If the cold side is too cold, turn off the TEG
        } else if (cold_temp > TEMP_MAX) {
            pwm_value = 255; // If the cold side is too hot, set TEG to full power
        } else {
            // Adjust TEG power based on the cold side temperature
            pwm_value = (uint8_t)((cold_temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN) * 255);
        }
    }

    OCR2B = pwm_value; // Set PWM value for the TEG (Pin 9)
    return pwm_value;
}

uint8_t adjust_teg_aux_power(int32_t hot_temp) {
    // Turn off the TEG auxillary gpio if the hot side temperature is too high
    if (hot_temp > TEMP_MAX) {
        PORTB &= ~(1 << TEG_AUX_PIN); // Turn off the TEG auxillary gpio
        return 0;
    } else {
        PORTB |= (1 << TEG_AUX_PIN); // Turn on the TEG auxillary gpio
        return 255;
    }
}

uint8_t adjust_pump_speed(int32_t temp_diff) {
    uint8_t pwm_value = 0;

    // Adjust pump speed based on temperature difference
    if (temp_diff < TEMP_THRESHOLD) {
        pwm_value = 0; // If the temperature difference is too low, turn off the pump
    } else {
        // Adjust pump speed based on the temperature difference
        pwm_value = (uint8_t)((temp_diff - TEMP_THRESHOLD) / (TEMP_MAX - TEMP_THRESHOLD) * 255);
    }

    OCR1B = pwm_value; // Set PWM value for the pump (Pin 10)
    return pwm_value;
}

uint8_t adjust_fan_speed(int32_t cat_pad_temp) {
    uint8_t pwm_value = 0;

    // Adjust fan speed based on cat pad temperature
    if (cat_pad_temp < CAT_PAD_TEMP_MIN) {
        pwm_value = 0; // If the cat pad is too cold, turn off the fan
    } else if (cat_pad_temp > CAT_PAD_TEMP_MAX) {
        pwm_value = 255; // If the cat pad is too hot, set fan to full power
    } else {
        // Adjust fan speed based on the cat pad temperature
        pwm_value = (uint8_t)((cat_pad_temp - CAT_PAD_TEMP_MIN) / (CAT_PAD_TEMP_MAX - CAT_PAD_TEMP_MIN) * 255);
    }

    OCR0B = pwm_value; // Set PWM value for the fan (Pin 3)
    return pwm_value;
}

void draw_graph(int cursor_y, const int32_t* data_history, int graph_height, int graph_width, float scale_factor, int base_y) {
    display.fillRect(0, cursor_y, graph_width, graph_height, SSD1306_BLACK);
    for (int i = 0; i < MOVING_AVG_WINDOW; i++) {
        int normalized_value = (int)(data_history[i] * scale_factor);
        normalized_value = constrain(normalized_value, 0, graph_height);
        display.drawPixel(i, base_y + graph_height - normalized_value, SSD1306_WHITE);
    }
}

void update_display(int32_t voltage, int32_t hot_temp, int32_t cold_temp, int32_t cat_pad_temp, bool food_detected, int32_t refresh_rate) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Construct the full text string for the discrete data
    String display_buffer = "";
    String labels[6] = {"Voltage: ", "Hot Temp: ", "Cold Temp: ", "Cat Pad Temp: ", "Food Detected: ", "Refresh Rate: "};
    int32_t values[6] = {voltage/1000, hot_temp/100, cold_temp/100, cat_pad_temp/100, food_detected, refresh_rate};
    String units[6] = {"V", "°C", "°C", "°C", "", "Hz"};

    for (int i = 0; i < 6; i++) {
        display_buffer += labels[i];
        display_buffer += String(values[i]);
        display_buffer += units[i];
        if (i % 3 == 2) { // Newline after 3 items to fit into the display rows
            display_buffer += "\n";
        } else {
            display_buffer += "  "; // Space between entries in the same line
        }
    }

    // Set cursor and print the constructed string all at once
    display.setCursor(0, 0);
    display.print(display_buffer);

    // Now handle the graphs separately (graphs cannot be optimized similarly due to pixel-level drawing)
    int graph_height = 28;
    int graph_width = 128;
    String graph_labels[4] = {"Fan Speed: ", "Pump Speed: ", "TEG Power: ", "Cold Temp: "};
    int cursors_graph[4][2] = {{16, 0}, {44, 0}, {72, 0}, {100, 0}};

    // Draw fan speed graph
    display.setCursor(cursors_graph[0][0], cursors_graph[0][1]);
    display.print(graph_labels[0]);
    draw_graph(16, fan_speed_history, graph_height, graph_width, 255.0 / graph_height, 16);

    // Draw pump speed graph
    display.setCursor(cursors_graph[1][0], cursors_graph[1][1]);
    display.print(graph_labels[1]);
    draw_graph(44, pump_speed_history, graph_height, graph_width, 255.0 / graph_height, 44);

    // Draw TEG power graph
    display.setCursor(cursors_graph[2][0], cursors_graph[2][1]);
    display.print(graph_labels[2]);
    draw_graph(72, teg_power_history, graph_height, graph_width, 255.0 / graph_height, 72);

    // Draw cold side temperature graph
    display.setCursor(cursors_graph[3][0], cursors_graph[3][1]);
    display.print(graph_labels[3]);
    draw_graph(100, cold_temp_history, graph_height, graph_width, 1.0 / (TEMP_MAX - TEMP_MIN), 100);

    display.display();
}

// Function to calculate the derivative (rate of temperature change)
int32_t calculate_derivative(int32_t current_temp, int32_t previous_temp) {
    return current_temp - previous_temp;
}

// Function to calculate the dynamic threshold based on recent derivatives (rate of temperature change)
int32_t calculate_derivative_threshold(int32_t derivatives[], int num_derivatives, int32_t percentile) {
    // Sort the derivatives in ascending order
    for (int i = 0; i < num_derivatives; i++) {
        for (int j = i + 1; j < num_derivatives; j++) {
            if (derivatives[i] > derivatives[j]) {
                int32_t temp = derivatives[i];
                derivatives[i] = derivatives[j];
                derivatives[j] = temp;
            }
        }
    }

    // Calculate the index for the desired percentile
    int index = (percentile * num_derivatives) / 100;
    
    // Return the derivative at the calculated percentile
    return derivatives[index];
}

// Function to detect food based on upward temperature rate of change (derivative)
bool detect_food(int32_t current_temp) {
    if (num_readings < 2) {
        // Not enough data to calculate a derivative
        return false;
    }

    // Get the previous temperature from the history buffer
    int32_t previous_temp = cold_temp_history[(num_readings - 1) % MOVING_AVG_WINDOW];

    // Calculate the derivative (rate of temperature change)
    int32_t derivative = calculate_derivative(current_temp, previous_temp);

    // Maintain a buffer of recent derivatives to calculate a dynamic threshold
    static int32_t derivative_history[MOVING_AVG_WINDOW] = {0};
    static int num_derivatives = 0;

    derivative_history[num_derivatives % MOVING_AVG_WINDOW] = derivative;
    num_derivatives++;

    // Calculate the dynamic threshold based on the 95th percentile of recent derivatives
    int32_t threshold = calculate_derivative_threshold(derivative_history, (num_derivatives < MOVING_AVG_WINDOW ? num_derivatives : MOVING_AVG_WINDOW), 95.0);

    // Trigger only if the rate of temperature increase exceeds the threshold
    if (derivative > threshold) {
        return true; // Food detected (significant increase in heat exchange)
    }

    return false;
}