# Remote Waveform and Spectrum Analyzer with I2S Mic SSD1351 and ESP32

A wireless audio visualization system using two ESP32 boards. An **ESP32 + INMP441 MEMS microphone** captures environmental audio, transmits it over **ESP-NOW**, and a second **ESP32 + SSD1351 OLED display** receives and renders the waveform or FFT-based spectrum in real time.

---

## 🔍 Description

This project is a remote audio sensing and visualization system. It acts as a wireless "microphone + display" pair:

- The **transmitter** ESP32 reads audio data from the INMP441 via I2S.
- Audio data is sent via **ESP-NOW** (low-latency WiFi protocol).
- The **receiver** ESP32 processes the incoming audio data using FFT and renders either a time-domain waveform or frequency-domain spectrum on the SSD1351 OLED display.

---

## 🎯 Purpose

A hobby project and proof-of-concept for future embedded audio systems. The architecture can be adapted for:

- Wireless **audio classification** systems
- **Signal monitoring** tools in remote locations
- Audio visualizers paired with music players or radios

---

## 🛠️ How to Run

1. **Clone this repository** to your local machine.
2. **Upload `mic_transmitter.ino` using Arduino IDE** for best results (avoids certain I2S/ESP-NOW bugs in PlatformIO).
3. Upload `oled_receiver` code using either **PlatformIO** or **Arduino IDE**.
4. Connect hardware:
   - **Microphone (INMP441)** to ESP32 using I2S pins.
   - **OLED (SSD1351)** to ESP32 using SPI pins.
   - Make sure to configure the I2S and SPI pin assignments in the respective source files.
5. Power both ESP32s and observe real-time waveform/spectrum output.

---

## 🔧 Technologies Used

- **ESP32**
- **INMP441 MEMS Microphone (I2S)**
- **SSD1351 OLED Display (SPI)**
- **ESP-NOW** for wireless communication
- **ArduinoFFT** for spectral analysis
- **PlatformIO & Arduino IDE**

---

## 📸 Screenshots *(Optional)*

*(You can add some waveform/spectrum display images here later)*

---

## 🚧 Future Improvements

- Add audio classification support (e.g., keyword spotting)
- Add support for dynamic range compression or smoothing
- Implement stereo input/output
- Support additional display types or higher resolutions

---

## 📄 License

MIT License (or specify your preferred license)

---

## 🙌 Credits

- Based on documentation and examples from Espressif and Adafruit
