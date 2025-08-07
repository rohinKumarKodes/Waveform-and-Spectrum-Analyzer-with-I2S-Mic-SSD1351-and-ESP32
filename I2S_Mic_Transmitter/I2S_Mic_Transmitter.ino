#include <esp_now.h>
#include <WiFi.h>
#include <driver/i2s.h>

// =============================
// ===== I2S CONFIGURATION =====
// =============================

#define SAMPLE_BUFFER_SIZE 128        // Number of samples per transmission
#define SAMPLE_RATE 8000              // Audio sample rate in Hz
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT

// Pin assignments for INMP441 microphone
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26     // Bit clock (BCLK)
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22 // Left/Right clock (LRCLK / WS)
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21      // Serial data (SD)
#define LED_PIN 2                            // Onboard LED

// I2S peripheral configuration
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Master receive mode
    .sample_rate = SAMPLE_RATE,                          // Sampling frequency
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,        // Microphone outputs 24 bits, aligned in 32-bit frames
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,         // Use left channel only
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,   // Standard I2S format
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,            // Interrupt priority
    .dma_buf_count = 4,                                  // Number of DMA buffers
    .dma_buf_len = 1024,                                 // Size of each DMA buffer
    .use_apll = false,                                   // Disable Audio PLL
    .tx_desc_auto_clear = false,                         // TX auto clear disabled
    .fixed_mclk = 0
};

// I2S pin mapping
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK, 
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE, // Not used (RX only)
    .data_in_num = I2S_MIC_SERIAL_DATA
};

// Structure to hold audio samples for transmission
#pragma pack(push, 1)
struct SoundData {
    int8_t samples[SAMPLE_BUFFER_SIZE];
};
#pragma pack(pop)

int32_t raw_samples[SAMPLE_BUFFER_SIZE];           // Raw 32-bit I2S samples

// =============================
// == ESP-NOW CONFIGURATION ====
// =============================

// Callback for ESP-NOW send status
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Receiver MAC address (update to match your device)
uint8_t broadcastAddress[] = {0x38, 0x18, 0x2B, 0x8B, 0xEE, 0x3C};
esp_now_peer_info_t peerInfo;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // Initialize I2S peripheral for microphone input
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);

    // Initialize Wi-Fi in station mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register send callback
    esp_now_register_send_cb(OnDataSent);

    // Register peer device
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    // Read samples from I2S microphone
    size_t bytes_read = 0;
    i2s_read(I2S_NUM_0, (void *) raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
    int samples_read = bytes_read / sizeof(int32_t);

    SoundData soundData;

    // Convert raw 32-bit samples to signed 8-bit
    for (int i = 0; i < samples_read; i++) {
        int8_t sample = raw_samples[i] >> 20; // Downscale to fit in int8_t
        soundData.samples[i] = sample;
    }

    // Send audio data via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &soundData, sizeof(soundData));
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }

    delay(17); // Adjust for desired frame rate / bandwidth. This is ~60 fps
}