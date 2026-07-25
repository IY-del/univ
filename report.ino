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
const uint8_t LCD_ADDR = 0x27; // モジュールに合わせて調整
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

// 距離・時間関連
const uint16_t DIST_MIN_CM = 20;      // これより近いと危険域扱い
const uint16_t DIST_MAX_CM = 200;     // これ以上は「遠い」とみなす
const uint16_t DURATION_MIN_MS = 40;  // パターン更新間隔の最小（約25Hz相当）
const uint16_t DURATION_MAX_MS = 500; // パターン更新間隔の最大（2Hz相当）

// 状態空間：ここでは「位置系」なので0〜23
const uint8_t STATE_COUNT = NEOPIXEL_NUM;

// ジェネレータの種類
enum StateLogic : uint16_t
{
    STATE_LOGIC_INC,
    STATE_LOGIC_LFSR,
    STATE_LOGIC_MOD
};

// generatorを選ぶ
const uint8_t state_logic_num = STATE_LOGIC_INC;

// ==============================
// ライブラリオブジェクト
// ==============================

Adafruit_NeoPixel ring(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ==============================
// ジェネレータ (state -> next_state)
// ==============================
typedef int (*Generator)(int);

Generator gens[] = {
    [](int current) // シンプルインクリメント (ランナー)
    {
        return (current + 1) % STATE_COUNT;
    },
    [](int current) // 簡易LFSR的 (擬似ランダム巡回)
    {
        if (current == 0)
            current = 1;

        int x = current & 0x1F; // 5bit
        bool b = ((x >> 0) ^ (x >> 2)) & 1;
        x = (x >> 1) | (uint8_t(b) << 4);

        if (x == 0)
            x = 1;
        int next = x % STATE_COUNT;
        return next;
    },
    [](int current) // モジュロ乗算的ステップ
    {
        const uint8_t a = 5; // N=24に対してそれなりに散る値
        int next = (current * a + 1) % STATE_COUNT;
        return next;
    }};

const auto get_next = gens[state_logic_num];

// ==============================
// グローバル状態
// ==============================

int current_state = 0;
unsigned long last_update_ms = 0;

// ==============================
// 距離測定 (HC-SR04)
// ==============================

int measure_distance_cm()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // タイムアウト30ms
    if (duration == 0)
    {
        return DIST_MAX_CM; // 失敗時は「遠い」
    }
    int distance = (int)(duration * 0.034 / 2.0); // cm
    return distance;
}

// ==============================
// 距離 -> duration (ms)
// ==============================

int map_distance_to_duration(int distance_cm)
{
    if (distance_cm <= (int)DIST_MIN_CM)
        return DURATION_MIN_MS;
    if (distance_cm >= (int)DIST_MAX_CM)
        return DURATION_MAX_MS;

    float x = (float)(distance_cm - DIST_MIN_CM) /
              (float)(DIST_MAX_CM - DIST_MIN_CM);

    // 近距離を強調する二乗カーブ
    float t = x * x;

    float dura = DURATION_MIN_MS +
                 (DURATION_MAX_MS - DURATION_MIN_MS) * t;

    return (int)dura;
}

// ==============================
// NeoPixel ring24 のrender
// ==============================

uint32_t color_from_distance(int distance_cm)
{
    if (distance_cm <= 50)
    {
        return ring.Color(50, 0, 0); // 近: 赤
    }
    else if (distance_cm <= 100)
    {
        return ring.Color(50, 25, 0); // 中: 黄
    }
    else
    {
        return ring.Color(0, 50, 0); // 遠: 緑
    }
}

void render_state(uint8_t state, int distance_cm)
{
    ring.clear();

    uint32_t base = color_from_distance(distance_cm);

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

unsigned long last_beep_ms = 0;
bool buzzer_on = false;

void update_buzzer(int distance_cm)
{
    int dura = map_distance_to_duration(distance_cm);
    int period_ms = dura * 2;

    unsigned long now = millis();
    if (now - last_beep_ms >= (unsigned long)period_ms)
    {
        last_beep_ms = now;
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
    lcd.print("cm     ");

    lcd.setCursor(0, 1);
    if (distance_cm <= 50)
    {
        lcd.print("DANGER             ");
    }
    else if (distance_cm <= 100)
    {
        lcd.print("CAUTION            ");
    }
    else
    {
        lcd.print("SAFE                 ");
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
    lcd.print("Distance UI");

    current_state = 0;
    last_update_ms = 0;
    last_beep_ms = 0;
}

// ==============================
// loop
// ==============================

void loop()
{
    int distance_cm = measure_distance_cm();
    int duration_ms = map_distance_to_duration(distance_cm);

    unsigned long now = millis();
    if (now - last_update_ms >= (unsigned long)duration_ms)
    {
        last_update_ms = now;

        current_state = get_next(current_state);
        render_state(current_state, distance_cm);
    }

    update_buzzer(distance_cm);
    update_lcd(distance_cm);
}
