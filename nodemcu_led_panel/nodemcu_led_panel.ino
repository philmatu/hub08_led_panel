/**
 * LED Matrix library for HUB08 led panels
 * 
 * The LED Matrix panel that I used has 64x16 pixels from aliexpress.com lysonled:
 * https://www.aliexpress.com/item/Indoor-P4-75-RG-SMD-LED-Module-304-76mm-1-16-Scan-Hub08-Store-LED-Moving/32844347325.html?spm=a2g0s.9042311.0.0.KQXW2u
 * "Indoor P4.75 RG SMD LED Module 304*76mm 1/16 Scan Hub08 Store LED Moving Display Module Replace F3.75 Dot Matrix LED Panel"
 * 
 * Base from: https://github.com/dpavlin/Arduino-projects/tree/master/Hub08_LedMatrix
 * Inspired by https://github.com/Seeed-Studio/Ultrathin_LED_Matrix/
 */

#include "LEDMatrix.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Comment/uncomment the board you want to flash onto.
#define WIDTH   64
#define HEIGHT  16

const char* ssid     = "";
const char* password = "";
const char* host = "http://domain.com/sign.php?sign=AG1";

//pin assignments can be changed here to make wiring easier
  #define PIN_A D0
  #define PIN_B D1
  #define PIN_C D2
  #define PIN_D D3
  #define PIN_R1 D6
  #define PIN_G1 D4
  #define PIN_STB D7  // or LAT
  #define PIN_OE D5
  #define PIN_CLK D8

unsigned char font5_8[][5] PROGMEM =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00 },   // [space]
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // - -> /
};

int ptrChar = 0;

LEDMatrix matrix(PIN_A, PIN_B, PIN_C, PIN_D, PIN_OE, PIN_R1, PIN_G1, PIN_STB, PIN_CLK);
uint8_t displaybuf[WIDTH * HEIGHT]; // Display Buffer
uint16_t buf_index;
uint8_t temp;
uint32_t now;
uint32_t last_scrolled = 0;
uint8_t whichPos = 0;
int posA = 0;
int posB = 0;
String payload;
String line1;
String line2;
String line3;
String line4;

HTTPClient http;


void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  matrix.begin(displaybuf, WIDTH, HEIGHT);
  clearDisplay();
  Serial.println("Started!");


}

void writeString(int ln, String data, String colors){//7=yellow,6=green,5=red
  int cpos = 0;
  char colorint = getSubValue(colors, ' ', cpos).toInt(); 
  
  posA=0;
    for(int i = 0; i<data.length(); i++){
      char c = data.charAt(i);
      if(c == ' '){
        cpos = cpos + 1;
        colorint = getSubValue(colors, ' ', cpos).toInt(); 
        drawChar(posA, (ln * 8), c, colorint);
        posA = posA + 2;
      }else if(c == ':'){
        drawChar(posA, (ln * 8), c, colorint);
        posA = posA + 4;
      }else if(c == '1'){
        drawChar(posA, (ln * 8), c, colorint);
        posA = posA + 5;
      }else if(c == '-'){
        drawChar(posA, (ln * 8), c, colorint);
        posA = posA + 5;
      }else{
        drawChar(posA, (ln * 8), c, colorint);
        posA = posA + 6;
      }
    }
}

void clearDisplay(){
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      matrix.drawPoint(x, y, 0);
    }
  }
  posA = 0;
  posB = 0;
}

void drawChar(int i, int j, char c, int color){ //7=yellow,6=green,5=red
  for (int x = 0; x < 5; x++) {
    byte bitMask = 0x01;
    if(c == 32){
      c = 47;
    }
    if(c == 45){
      c = 91;
    }
    byte f = pgm_read_byte(&(font5_8[c-47][x]));
    for (int y = 0; y < 7; y++) {
      if (f & bitMask){
        
        matrix.drawPoint(x + i, y + j, color);
      }
      bitMask = bitMask << 1;
    }
  }  
}

String getSubValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int i = 4;
void loop() {
  
    matrix.scan();
    now = millis();
  
  if ((now - last_scrolled) < 10000) return;
  
  last_scrolled = now;
  

  //update every 20 seconds from API ( 2 rounds)
  if(whichPos == 0){
    digitalWrite(PIN_OE, HIGH);//disable display momentarily
    whichPos = 1;
    http.begin(host);
    int httpCode = http.GET();

    if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
        payload = http.getString();
        payload.toUpperCase();
        Serial.println(payload);
      }
    }
    http.end();
    line1 = getSubValue(payload, '\n', 0);
    line2 = getSubValue(payload, '\n', 1);
    line3 = getSubValue(payload, '\n', 2);
    line4 = getSubValue(payload, '\n', 3);
    
  }else{
    whichPos = 0;
    line1 = getSubValue(payload, '\n', 4);
    line2 = getSubValue(payload, '\n', 5);
    line3 = getSubValue(payload, '\n', 6);
    line4 = getSubValue(payload, '\n', 7);
  }
  
  clearDisplay();
  line1.trim();
  line2.trim();
  line3.trim();
  line4.trim();
  
  
  writeString(0, line1, line2);
  writeString(1, line3, line4);
  
}
