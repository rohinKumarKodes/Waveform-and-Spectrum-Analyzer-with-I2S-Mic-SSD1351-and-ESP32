# Remote Waveform and Spectrum Analyzer with I2S Mic SSD1351 and ESP32

A wireless audio visualization system using two ESP32 boards. An **ESP32 + INMP441 MEMS microphone** captures environmental audio, transmits it over **ESP-NOW**, and a second **ESP32 + SSD1351 OLED display** receives and renders the waveform or FFT-based spectrum in real time.

---

## Description

This project is a remote audio sensing and visualization system. It acts as a wireless "microphone + display" pair:

- The **transmitter** ESP32 reads audio data from the INMP441 via I2S.
- Audio data is sent via **ESP-NOW** (low-latency WiFi protocol).
- The **receiver** ESP32 processes the incoming audio data using FFT and renders either a time-domain waveform or frequency-domain spectrum on the SSD1351 OLED display.

---

## Purpose

A hobby project and proof-of-concept for future embedded audio systems. The architecture can be adapted for:

- Wireless **audio classification** systems
- **Signal monitoring** tools in remote locations
- Audio visualizers paired with music players or radios
This project also happens to be aesthetic and addicting to look at in my opinion
---

## How to Run

1. **Clone this repository** to your local machine.
2. **Run `I2S_Mic_Transmitter` using Arduino IDE** for best results (avoids certain I2S/ESP-NOW bugs in PlatformIO).
3. Upload `Display_Reciever` code using either **PlatformIO** or **Arduino IDE**.
   - Make sure to properly select your setup based on your display in the TFT eSPI library.
     1. Go to Display_Reciever\.pio\libdeps\esp32dev\TFT_eSPI\User_Setup_Select.h
     2. Comment Out #include <User_Setup.h>
     3. Uncomment the include statement that corresponds to your display.
     4. Go to the definition of your display and configure your pins
5. Connect hardware:
   - **Microphone (INMP441)** to ESP32 using I2S pins.
   - **OLED (SSD1351)** to ESP32 using SPI pins.
   - Make sure to configure the I2S and SPI pin assignments in the respective source files.
6. Power both ESP32s and observe real-time waveform/spectrum output.

---

## Technologies Used

- **ESP32**
- **INMP441 MEMS Microphone (I2S)**
- **SSD1351 OLED Display (SPI)**
- **ESP-NOW** for wireless communication
- **ArduinoFFT** for spectral analysis
- **PlatformIO & Arduino IDE**

---

## Future Improvements

- Add audio classification support (e.g., keyword spotting)
- Add support for dynamic range compression or smoothing
- Implement stereo input/output
- Support additional display types or higher resolutions

---

## License

MIT License (or specify your preferred license)

---

## Credits

- ESP-NOW implementation based on Random Nerd Tutorials code for ESP-NOW: https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/
