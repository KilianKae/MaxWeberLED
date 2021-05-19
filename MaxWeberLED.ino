#include <FastLED.h>

#define DATA_PIN 3

#define NUM_LINES 10
#define LEDS_IN_LINE 30
const int NUM_LEDS = NUM_LINES * LEDS_IN_LINE;

#define BRIGHTNESS 100

int delayval = 50; // timing delay

CRGBArray<NUM_LEDS> leds;
CRGBPalette16 currentPalette = LavaColors_p;
TBlendType currentBlending = LINEARBLEND;

int count = 0;
CRGBSet leds0(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds1(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds2(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds3(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds4(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds5(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds6(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds7(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds8(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));
CRGBSet leds9(leds(LEDS_IN_LINE *count++, LEDS_IN_LINE *count));

struct CRGB *ledSets[] = {leds0, leds1, leds2, leds3, leds4, leds5, leds6, leds7, leds8, leds9};

static bool active = true;

struct Point
{
  uint8_t x;
  uint8_t y;
};

struct Line
{
  Point start;
  Point end;
};

struct Circle
{
  Point center;
  uint8_t radius;
};

Line lines[NUM_LINES] = {
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}},
    {{0, 0}, {0, 100}}};

//Point pixels[NUM_LINES][LEDS_IN_LINE];
//
//void linesToPixels() {
//  for(int i = 0; i < NUM_LINES; i++) {
//    Line l = lines[i];
//    Point vector = {l.end.x - l.start.x, l.end.y - l.start.y};
//    for(int j = 0; j < LEDS_IN_LINE; j++){
//      pixels[i][j] = {l.start.x + (1.0/LEDS_IN_LINE) * vector.x * j, l.start.y + (1.0/LEDS_IN_LINE) * vector.y * j};
//      Serial.print("pixels[i][j].x ");
//      Serial.print(pixels[i][j].x);
//      Serial.print(" pixels[i][j].y ");
//      Serial.println(pixels[i][j].y);
//    }
//  }
//};

Point indexToPixel(Line l, int i)
{
  Point vector = {l.end.x - l.start.x, l.end.y - l.start.y};
  return {l.start.x + (1.0 / LEDS_IN_LINE) * vector.x * i, l.start.y + (1.0 / LEDS_IN_LINE) * vector.y * i};
};

bool inCircle(Point p, Circle c)
{
  if (pow((p.x - c.center.x), 2) + pow((p.y - c.center.y), 2) < pow(c.radius, 2))
  {
    return true;
  }
  return false;
};

void inCircle(Circle c)
{
  for (int i = 0; i < NUM_LINES; i++)
  {
    for (int j = 0; j < LEDS_IN_LINE; j++)
    {
      if (inCircle(indexToPixel(lines[i], j), c))
      {
        ledSets[i][j] = ColorFromPalette(currentPalette, i % 239 + 1, 255, currentBlending);
      }
    }
  }
}

void walkSegments()
{
  static uint8_t i = 0;
  fill_solid(ledSets[i], LEDS_IN_LINE, ColorFromPalette(currentPalette, i % 239 + 1, 255, currentBlending));
  i = ++i % NUM_LINES;
};

void wanderSegments()
{
  static uint16_t i = 0;
  for (int j = 0; j < NUM_LINES; j++)
  {
    ledSets[j][i] = ColorFromPalette(currentPalette, i % 239 + 1, 255, currentBlending);
  }
  i = ++i % LEDS_IN_LINE;
};

void wander()
{
  static uint16_t i = 0;
  leds[i] = ColorFromPalette(currentPalette, i % 239 + 1, 255, currentBlending);
  i = ++i % NUM_LEDS;
};

void randomSegment()
{
  static uint8_t i = 0;
  uint16_t n = random(0, NUM_LINES);
  fill_solid(ledSets[n], LEDS_IN_LINE, ColorFromPalette(currentPalette, i % 239 + 1, 255, currentBlending));
  i = ++i % NUM_LINES;
}

typedef void (*function)(void);
static function state = randomSegment;
void read()
{
  const String incomingString = Serial.readString();
  Serial.print("Received ");
  Serial.println(incomingString);
  if (incomingString == "toggleOn")
  {
    active = !active;
  }
  if (incomingString == "test")
  {
    state = wander;
  }
}

void setup()
{
  delay(2000); // power-up safety delay
  Serial.begin(115200);
  Serial.println("Setup");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  set_max_power_indicator_LED(13);
  FastLED.setBrightness(BRIGHTNESS);
  //  linesToPixels();
}

void loop()
{
  static uint16_t i = 0;

  if (active)
  {
    FastLED.clear();
    state();
    FastLED.show();
    delay(delayval);
  }

  if (Serial.available() > 0)
  {
    read();
  }
}
