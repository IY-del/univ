#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

// ==============================
// 定数定義
// ==============================

// 距離センサー (HC-SR04)
const uint8_t TRIG_PIN = 8;
const uint8_t ECHO_PIN = 9;

// NeoPixel ring
const uint8_t NEOPIXEL_PIN = 2;
const uint8_t NEOPIXEL_NUM = 24;

// 圧電ブザー
const uint8_t BUZZER_PIN = 7;

// LCD (16x2 I2C)
const uint8_t LCD_ADDR = 0x27;
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

// 距離閾値
const uint16_t DIST_MIN_CM = 2;       // 最短扱い
const uint16_t DIST_MAX_CM = 335;     // 最長扱い
const uint16_t DIST_DANGER_CM = 50;   // 危険
const uint16_t DIST_CAUTION_CM = 100; // 注意

// HC-SR04計測制御
const uint32_t SENSOR_INTERVAL_MS = 35; // 約29ms以上を確保

// UI更新間隔
const uint16_t DURATION_MIN_MS = 40;
const uint16_t DURATION_MAX_MS = 500;
const uint16_t LCD_UPDATE_INTERVAL_MS = 200;

// 状態空間
const uint8_t STATE_COUNT = NEOPIXEL_NUM;

// ジェネレータの種類
enum StateLogic : uint8_t
{
    STATE_LOGIC_INC,
    STATE_LOGIC_LFSR,
    STATE_LOGIC_MOD
};

// 使用するgenerator
const StateLogic state_logic_num = STATE_LOGIC_INC;

// 危険度
enum DangerLevel : uint8_t
{
    LEVEL_SAFE,
    LEVEL_CAUTION,
    LEVEL_DANGER
};

// 型定義エラー対策のプロトタイプ
DangerLevel get_danger_level(int distance_cm);
uint32_t color_from_level(DangerLevel level);
void render_state(uint8_t state, int distance_cm);
void update_buzzer(int distance_cm);
void update_lcd(int distance_cm);

// ==============================
// ライブラリオブジェクト
// ==============================

Adafruit_NeoPixel ring(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ==============================
// ジェネレータ (state -> next_state)
// ==============================

typedef uint8_t (*Generator)(uint8_t);

constexpr uint8_t LFSR_BITS = 0x1F;

Generator gens[] = {
    [](uint8_t current) -> uint8_t
    {
        return (current + 1) % STATE_COUNT;
    },
    [](uint8_t current) -> uint8_t
    {
        uint8_t x = (current == 0 || current >= STATE_COUNT) ? 1 : current;
        for (;;)
        {
            const bool feedback = ((x >> 0) ^ (x >> 2)) & 0x01;
            x = ((x >> 1) | (uint8_t(feedback) << 4)) & LFSR_BITS;

            if (x != 0 && x < STATE_COUNT)
            {
                return x;
            }
        }
    },
    [](uint8_t current) -> uint8_t
    {
        constexpr uint8_t a = 5;
        return (current * a + 1) % STATE_COUNT;
    }};

Generator get_next = gens[state_logic_num];

// ==============================
// グローバル状態
// ==============================

uint8_t current_state = 0;

unsigned long last_sensor_ms = 0;
unsigned long last_render_ms = 0;
unsigned long last_lcd_ms = 0;
unsigned long last_beep_toggle_ms = 0;

int measured_distance_cm = DIST_MAX_CM;
int filtered_distance_cm = DIST_MAX_CM;

bool buzzer_on = false;

// ==============================
// ヘルパ
// ==============================

uint32_t get_echo_timeout_us()
{
    // 距離cm -> 往復時間us
    // time_us = distance_cm * 2 / 0.0343
    // 少し余裕を持たせて +1000us
    const float timeout_us = (2.0f * DIST_MAX_CM / 0.0343f) + 1000.0f;
    return (uint32_t)timeout_us;
}

DangerLevel get_danger_level(int distance_cm)
{
    if (distance_cm <= (int)DIST_DANGER_CM)
    {
        return LEVEL_DANGER;
    }
    if (distance_cm <= (int)DIST_CAUTION_CM)
    {
        return LEVEL_CAUTION;
    }
    return LEVEL_SAFE;
}

int clamp_distance_cm(int distance_cm)
{
    if (distance_cm < (int)DIST_MIN_CM)
    {
        return DIST_MIN_CM;
    }
    if (distance_cm > (int)DIST_MAX_CM)
    {
        return DIST_MAX_CM;
    }
    return distance_cm;
}

void lcd_print_padded(const char *text)
{
    lcd.print(text);
    size_t len = strlen(text);
    for (size_t i = len; i < LCD_COLS; ++i)
    {
        lcd.print(' ');
    }
}

// ==============================
// 距離測定 (HC-SR04)
// ==============================

int measure_distance_cm_raw()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const uint32_t echo_timeout_us = get_echo_timeout_us();
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, echo_timeout_us);

    if (duration == 0)
    {
        return DIST_MAX_CM;
    }

    int distance = (int)(duration * 0.0343f / 2.0f);
    return clamp_distance_cm(distance);
}

// 軽い平滑化: EMA
int smooth_distance_cm(int prev_cm, int new_cm)
{
    // 0.7 * prev + 0.3 * new
    return (prev_cm * 7 + new_cm * 3) / 10;
}

// ==============================
// 距離 -> duration (ms)
// ==============================

int map_distance_to_duration(int distance_cm)
{
    if (distance_cm <= (int)DIST_MIN_CM)
    {
        return DURATION_MIN_MS;
    }
    if (distance_cm >= (int)DIST_MAX_CM)
    {
        return DURATION_MAX_MS;
    }

    float x = (float)(distance_cm - DIST_MIN_CM) /
              (float)(DIST_MAX_CM - DIST_MIN_CM);

    float t = x * x;

    float dura = DURATION_MIN_MS +
                 (DURATION_MAX_MS - DURATION_MIN_MS) * t;

    return (int)dura;
}

// ==============================
// NeoPixel ring24 のrender
// ==============================

uint32_t color_from_level(DangerLevel level)
{
    switch (level)
    {
    case LEVEL_DANGER:
        return ring.Color(50, 0, 0);
    case LEVEL_CAUTION:
        return ring.Color(50, 25, 0);
    case LEVEL_SAFE:
    default:
        return ring.Color(0, 50, 0);
    }
}

void render_state(uint8_t state, int distance_cm)
{
    ring.clear();

    DangerLevel level = get_danger_level(distance_cm);
    uint32_t base = color_from_level(level);

    uint8_t idx = state % NEOPIXEL_NUM;
    ring.setPixelColor(idx, base);

    uint8_t idx1 = (idx + NEOPIXEL_NUM - 1) % NEOPIXEL_NUM;
    uint8_t idx2 = (idx + 1) % NEOPIXEL_NUM;
    uint8_t idx3 = (idx + NEOPIXEL_NUM - 2) % NEOPIXEL_NUM;
    uint8_t idx4 = (idx + 2) % NEOPIXEL_NUM;

    uint8_t r = (base >> 16) & 0xFF;
    uint8_t g = (base >> 8) & 0xFF;
    uint8_t b = base & 0xFF;

    uint32_t trail1 = ring.Color(r / 3, g / 3, b / 3);
    uint32_t trail2 = ring.Color(r / 6, g / 6, b / 6);

    ring.setPixelColor(idx1, trail1);
    ring.setPixelColor(idx2, trail1);
    ring.setPixelColor(idx3, trail2);
    ring.setPixelColor(idx4, trail2);

    ring.show();
}

// ==============================
// ブザー
// ==============================

void update_buzzer(int distance_cm)
{
    DangerLevel level = get_danger_level(distance_cm);
    unsigned long now = millis();

    if (level == LEVEL_SAFE)
    {
        buzzer_on = false;
        digitalWrite(BUZZER_PIN, LOW);
        return;
    }

    int duration_ms = map_distance_to_duration(distance_cm);
    int toggle_interval_ms = duration_ms;

    if (now - last_beep_toggle_ms >= (unsigned long)toggle_interval_ms)
    {
        last_beep_toggle_ms = now;
        buzzer_on = !buzzer_on;
        digitalWrite(BUZZER_PIN, buzzer_on ? HIGH : LOW);
    }
}

// ==============================
// LCD
// ==============================

void update_lcd(int distance_cm)
{
    lcd.setCursor(0, 0);
    lcd.print("Dist:");
    lcd.print(distance_cm);
    lcd.print("cm");

    int used = 5 + String(distance_cm).length() + 2;
    for (int i = used; i < LCD_COLS; ++i)
    {
        lcd.print(' ');
    }

    lcd.setCursor(0, 1);
    DangerLevel level = get_danger_level(distance_cm);
    switch (level)
    {
    case LEVEL_DANGER:
        lcd_print_padded("DANGER");
        break;
    case LEVEL_CAUTION:
        lcd_print_padded("CAUTION");
        break;
    case LEVEL_SAFE:
    default:
        lcd_print_padded("SAFE");
        break;
    }
}

// ==============================
// setup
// ==============================

void setup()
{
    Serial.begin(9600);

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    ring.begin();
    ring.setBrightness(40);
    ring.clear();
    ring.show();

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd_print_padded("Distance UI");
    lcd.setCursor(0, 1);
    lcd_print_padded("Initializing...");

    current_state = 0;
    measured_distance_cm = DIST_MAX_CM;
    filtered_distance_cm = DIST_MAX_CM;

    last_sensor_ms = 0;
    last_render_ms = 0;
    last_lcd_ms = 0;
    last_beep_toggle_ms = 0;
}

// ==============================
// loop
// ==============================

void loop()
{
    unsigned long now = millis();

    // 1. センサー更新
    if (now - last_sensor_ms >= SENSOR_INTERVAL_MS)
    {
        last_sensor_ms = now;
        measured_distance_cm = measure_distance_cm_raw();
        filtered_distance_cm = smooth_distance_cm(filtered_distance_cm, measured_distance_cm);

        Serial.print("raw=");
        Serial.print(measured_distance_cm);
        Serial.print("cm, filtered=");
        Serial.print(filtered_distance_cm);
        Serial.println("cm");
    }

    // 2. NeoPixel更新
    int duration_ms = map_distance_to_duration(filtered_distance_cm);
    if (now - last_render_ms >= (unsigned long)duration_ms)
    {
        last_render_ms = now;
        current_state = get_next(current_state);
        render_state(current_state, filtered_distance_cm);
    }

    // 3. ブザー更新
    update_buzzer(filtered_distance_cm);

    // 4. LCD更新
    if (now - last_lcd_ms >= LCD_UPDATE_INTERVAL_MS)
    {
        last_lcd_ms = now;
        update_lcd(filtered_distance_cm);
    }
}
