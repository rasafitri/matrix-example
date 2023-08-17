/*******************************************************************
    Beispiel für die Verwendung von ESP32 oder ESP8266 zur Steuerung
    der HUB75 LED-Matrixanzeigetafel mit PxMatrix.
 *******************************************************************/

// Lib für die Steuerung der Anzeigetafel
#include <PxMatrix.h>

// ----------------------------------------
// Einstellungen der LED-Matrixanzeigetafel
// ----------------------------------------
#define matrix_width 64   // Breite der Anzeige, anzupassen
#define matrix_height 32  // Höhe der Anzeige, anzupassen

// Pin Belegungen der Anzeige, anzupassen
#ifdef ESP32
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E -1  // anzupassen bei Verbindung
#define P_LAT 22
#define P_OE 16
#endif

#ifdef ESP8266
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E -1  // anzupassen bei Verbindung
#define P_LAT 16
#define P_OE 2
#endif

PxMATRIX display(matrix_width, matrix_height, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// die Helligkeit der Anzeige, 30-70, je nach Bedarf anpassen
uint8_t display_draw_time = 60;

// ----------------------------------------
// Timer für Callbacks
// ----------------------------------------
#ifdef ESP32
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif
#ifdef ESP8266
#include <Ticker.h>
Ticker display_ticker;
#endif

// ----------------------------------------
// Standard-Farben in RGB565 für Texte
// ----------------------------------------
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

// ----------------------------------------
// Beispielbild
// ----------------------------------------
// ./images/yt-logo.png umgewandelt in .c Datei mit ImageConverter (UTFT) von http://www.rinkydinkelectronics.com/t_imageconverter565.php
uint16_t static youtube_logo[] = {
  0x2800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0010 (16) pixels
  0xF800, 0xF800, 0xF800, 0x0000, 0x2800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0020 (32) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0030 (48) pixels
  0xF800, 0xF800, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0040 (64) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0050 (80) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800,  // 0x0060 (96) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0070 (112) pixels
  0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0080 (128) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0090 (144) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,  // 0x00A0 (160) pixels
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x00B0 (176) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x00C0 (192) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x00D0 (208) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800,  // 0x00E0 (224) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF,  // 0x00F0 (240) pixels
  0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0100 (256) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0110 (272) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0120 (288) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,  // 0x0130 (304) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x2800, 0x0000, 0xF800, 0xF800, 0xF800,  // 0x0140 (320) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x2800,  // 0x0150 (336) pixels
};
// Größe des Bilds in px
int imageWidth = 21;
int imageHeight = 16;

// ----------------------------------------
// Funktionen für Anzeige Update
// ----------------------------------------
#ifdef ESP32
void IRAM_ATTR display_updater() {
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif
#ifdef ESP8266
void display_updater() {
  display.display(display_draw_time);
}
#endif

void display_update_enable(bool is_enable) {
#ifdef ESP32
  if (is_enable) {
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  } else {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
#endif
#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
#endif
}

// ----------------------------------------
// LED-Matrixanzeige Funktionen
// ----------------------------------------
// zeige einmalig einen Lauftext in bestimmter Farbe
void scrollText(String text, uint16_t colorRGB) {
  unsigned long scroll_speed = 50;       // Geschwindigkeit des Scrollens
  uint16_t text_length = text.length();  // Länge des Texts für die Berechnung

  // Einstellung der Display
  display.setTextWrap(false);      // Lauftext --> kein TextWrap nötig
  display.setTextSize(1);          // Standardgröße, 8px
  display.setRotation(0);          // keine Rotation
  display.setTextColor(colorRGB);  // Farbe des Texts wie angegeben

  // Schritt für Schritt Text Anzeigen
  for (int xpos = matrix_width; xpos > -(matrix_width + text_length * 5); xpos--) {
    display.clearDisplay();      // immer Anzeige zurücksetzen, bevor etwas Neues angezeigt wird
    display.setCursor(xpos, 0);  // Setze den Text ganz oben (ypos=0), aber bei variablen xpos
    display.println(text);
    delay(scroll_speed);
    yield();

    delay(scroll_speed / 5);
    yield();
  }
}

// zeige den stehenden Text in bestimmter Farbe
void drawText(String text, uint16_t colorRGB) {
  display.clearDisplay();          // immer Anzeige zurücksetzen, bevor etwas Neues angezeigt wird
  display.setTextWrap(true);       // stehender Text --> TextWrap nötig für längere Texte
  display.setTextSize(1);          // Standardgröße, 8px
  display.setRotation(0);          // keine Rotation
  display.setCursor(0, 0);         // Text fängt bei der Position oben links
  display.setTextColor(colorRGB);  // Farbe des Texts wie angegeben
  display.println(text);
}

// zeige das skalierte Bild
void drawImage(uint16_t image[], int imageWidth, int imageHeight) {
  display.clearDisplay();  // immer Anzeige zurücksetzen, bevor etwas Neues angezeigt wird
  int counter = 0;
  for (int y = 0; y < imageHeight; y++) {
    for (int x = 0; x < imageWidth; x++) {
      display.drawPixel(x, y, image[counter]);
      counter++;
    }
  }
}

// ----------------------------------------
// Setup
// ----------------------------------------
void setup() {

  Serial.begin(115200);  // Serial baudrate 115200

  // LED-Matrixanzeige einstellen
  display.begin(16);  // display rows-scan pattern 1/16
  display.setFastUpdate(true);

  // ----------------------------------------
  // weitere Einstellungen laut PxMatrix Example
  // anzupassen wenn Grundeinstellungen nicht ausreichend sind
  // Für weitere Infos siehe https://github.com/2dom/PxMatrix/tree/master
  // ----------------------------------------

  // multiplex pattern {BINARY (def), STRAIGHT, SHIFTREG_ABC, SHIFTREG_SPI_SE}
  //display.setMuxPattern(BINARY);

  // scan pattern {LINE (def), ZIGZAG,ZZAGG, ZAGGIZ, WZAGZIG, VZAG, ZAGZIG, WZAGZIG2}
  //display.setScanPattern(LINE);

  // Rotate display
  //display.setRotate(true);

  // Flip display
  //display.setFlip(true);

  // minimum color values that result in an active pixel
  //display.setColorOffset(5, 5,5);

  // color order {RRGGBB (def), RRBBGG, GGRRBB, GGBBRR, BBRRGG, BBGGRR}
  //display.setColorOrder(RRGGBB);

  // time in microseconds that we pause after selecting each mux channel
  // (May help if some rows are missing / the mux chip is too slow)
  //display.setMuxDelay(0,1,0,0,0);

  // Set the number of panels that make up the display area width (default is 1)
  //display.setPanelsWidth(2);

  // Set the brightness of the panels (default is 255)
  //display.setBrightness(50);

  // Set driver chip type
  //display.setDriverChip(FM6124);

  display.clearDisplay();  // immer Anzeige zurücksetzen, bevor etwas Neues angezeigt wird
  display_update_enable(true);
}

// ----------------------------------------
// Loop
// ----------------------------------------
void loop() {
  scrollText("Herzlich Willkommen zur Summer School 2023!", myBLUE);  // Willkommenstext (laufend) in blau
  delay(500);                                                         // kurze Pause
  drawText("Bald kommt Youtube Logo!", myGREEN);                      // Text (stehend) in grün
  delay(2000);                                                        // 2s Pause
  drawImage(youtube_logo, imageWidth, imageHeight);                   // Youtube Logo als Bild
  delay(5000);                                                        // 5s Pause
}
