#include <Arduino.h>
#include <TFT_eSPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <arduinoFFT.h>

#define SAMPLE_BUFFER_SIZE 128     // Number of audio samples per packet
#define SAMPLE_RATE 8000           // Sampling frequency in Hz

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void displayData(int8_t[]);
void displayFFT(uint8_t[]);
uint16_t colorPicker(uint8_t);

/**
 * Struct: SoundData
 * Stores a buffer of signed 8-bit audio samples sent via ESP-NOW.
 */
struct SoundData {
    int8_t samples[SAMPLE_BUFFER_SIZE];
};
SoundData soundData;

uint8_t frequencies[SAMPLE_BUFFER_SIZE / 2];
double vReal[SAMPLE_BUFFER_SIZE];
double vImag[SAMPLE_BUFFER_SIZE];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLE_BUFFER_SIZE, SAMPLE_RATE);

/**
 * Callback: OnDataRecv
 * Purpose: Called when an ESP-NOW packet is received. Processes audio data,
 *          performs FFT, and updates the display with waveform and spectrum.
 * 
 * @param mac         MAC address of the sender (unused here)
 * @param incomingData Pointer to received byte array
 * @param len         Length of received data
 */
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&soundData, incomingData, sizeof(soundData));

    // Prepare FFT input buffers
    memset(vImag, 0, sizeof(vImag));
    memset(vReal, 0, sizeof(vReal));
    for (uint8_t i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
        vReal[i] = soundData.samples[i];
    }

    // Perform FFT
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();

    // Store frequency magnitudes
    for (int i = 0; i < SAMPLE_BUFFER_SIZE / 2; i++) {
        frequencies[i] = (uint8_t)vReal[i];
    }

    // Draw waveform and spectrum to sprite, then push to screen
    sprite.fillSprite(TFT_BLACK);
    displayData(soundData.samples);
    displayFFT(frequencies);
    sprite.pushSprite(0, 8);
}

void setup() {
    Serial.begin(115200);
    tft.init();
    tft.setRotation(0);

    // Basic screen test sequence
    tft.fillScreen(TFT_RED); delay(1000);
    tft.fillScreen(TFT_GREEN); delay(1000);
    tft.fillScreen(TFT_BLUE); delay(1000);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Speak or Play Music!", 7, 0);

    sprite.createSprite(128, 120);

    // Set device as Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;  
    }
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {
    // No continuous loop logic; all processing happens in OnDataRecv
}

/**
 * Function: displayData
 * Purpose: Draws the waveform from raw audio samples.
 * Note: The transformation is from standard cartesian to
 * cartesian pixel coordinates. 
 * 
 * @param y_vals Array of signed 8-bit samples
 */
void displayData(int8_t y_vals[]) {
    for (uint8_t t = 1; t < 128; t++) {
        sprite.drawLine(
            t - 1, (y_vals[t - 1] / -2) + 40,
            t,     (y_vals[t] / -2) + 40,
            TFT_BLUE
        );
    }
}

/**
 * Function: displayFFT
 * Purpose: Draws the frequency spectrum from FFT magnitudes.
 * 
 * @param y_vals Array of unsigned 8-bit magnitudes (half FFT size)
 */
void displayFFT(uint8_t y_vals[]) {
    for (uint8_t t = 0; t < 128; t++) {
        if (t % 2 == 0) { // Only draw on even pixels for spacing and aesthetic
            sprite.drawFastVLine(
                t, 
                (-y_vals[t / 2] / 8) + 119, 
                y_vals[t / 2], 
                TFT_RED
            );
        }
    }
}

/**
 * Function: colorPicker
 * Purpose: Returns a color based on position (x), creating a rainbow gradient.
 * Note: This uses more memory on top of the FFT calculations (ESP32 may crash).
 * 
 * @param x Pixel position (0–127)
 * @return 16-bit RGB565 color value
 */
uint16_t colorPicker(uint8_t x) {
    uint8_t region = 21;
    uint8_t red = 255, green = 0, blue = 0;

    uint8_t phase = x / region;
    switch (phase) {
        case 0: red = 255; green = 0;   blue = 0;   break;
        case 1: red = 255; green = 255; blue = 0;   break;
        case 2: red = 0;   green = 255; blue = 0;   break;
        case 3: red = 0;   green = 255; blue = 255; break;
        case 4: red = 0;   green = 0;   blue = 255; break;
        case 5: red = 255; green = 0;   blue = 255; break;
        default:red = 255; green = 0;   blue = 255; break;
    }
    return tft.color565(red, green, blue);
}