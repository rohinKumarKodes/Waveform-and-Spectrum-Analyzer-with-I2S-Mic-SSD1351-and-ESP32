import time
import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import struct

# Track script start time (optional; useful for timing-based logic)
start_time = time.time()

# Configure serial port for incoming data from ESP device
ser = serial.Serial("COM4", 115200, timeout=None)

# Buffers for incoming waveform and FFT data
y1 = np.zeros(128)  # time-domain samples
y2 = np.zeros(64)   # frequency-domain magnitudes
x1 = np.linspace(0, 128, 128)  # X-axis for waveform plot
x2 = np.linspace(0, (64 * 8000) / 128, 64)  # X-axis for FFT plot in Hz

# Buffers for reconstructed top 3 sine waves from FFT
xSin1 = np.linspace(0, 50, 50)
ySin1 = np.zeros(50)
xSin2 = np.linspace(0, 50, 50)
ySin2 = np.zeros(50)
xSin3 = np.linspace(0, 50, 50)
ySin3 = np.zeros(50)

# Create 2x3 subplot grid for live data visualization
fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(12, 8))

# Plot objects for updating during animation
line1, = axes[0, 0].plot(x1, y1)  # Time-domain waveform
line2, = axes[0, 1].plot(x2, y2)  # FFT magnitude spectrum
sin1, = axes[0, 2].plot(xSin1, ySin1)  # Top FFT peak reconstructed as sine wave
sin2, = axes[1, 0].plot(xSin2, ySin2)
sin3, = axes[1, 1].plot(xSin3, ySin3)

# Set Y-axis limits for each plot
axes[0, 0].set_ylim(-250, 250)  # Waveform amplitude range
axes[0, 1].set_ylim(0, 10)      # FFT magnitude range
axes[0, 2].set_ylim(-75, 75)
axes[1, 0].set_ylim(-75, 75)
axes[1, 1].set_ylim(-75, 75)

# Label axes
axes[0, 0].set_xlabel("Samples")
axes[0, 0].set_ylabel("Amplitude")
axes[0, 1].set_xlabel("Frequencies (Hz)")
axes[0, 1].set_ylabel("|FFT(x)|")
axes[0, 2].set_xlabel("Samples")
axes[0, 2].set_ylabel("Amplitude")
axes[1, 0].set_xlabel("Samples")
axes[1, 0].set_ylabel("Amplitude")
axes[1, 1].set_xlabel("Samples")
axes[1, 1].set_ylabel("Amplitude")


def main():
    """Start live updating plots."""
    ani = FuncAnimation(fig, animate, interval=10, blit=True, cache_frame_data=False)
    plt.show()


def animate(i):
    """Read packet from serial, update waveform, FFT, and top sine waves."""
    # Wait for sync header
    if ser.read(4) == b'\xAA\xBB\xCC\xDD':
        # Read remaining data: 64 FFT doubles + 128 waveform int16 samples
        bytestream = ser.read(768)
        data = list(struct.unpack("<64d128h", bytestream))

        # Update waveform plot (time domain)
        line1.set_ydata(data[64:192])

        # Update FFT plot (frequency domain)
        line2.set_ydata(data[0:64])

        # Find top 3 frequency peaks
        topThree = topThreeVals(data)

        # Unpack top 3 amplitude-frequency pairs
        top1_amp, top1_freq = topThree[0]
        top2_amp, top2_freq = topThree[1]
        top3_amp, top3_freq = topThree[2]

        # Reconstruct sine waves from top FFT peaks
        sin1.set_ydata(np.sin(2 * np.pi * top1_freq * xSin1) * top1_amp)
        sin2.set_ydata(np.sin(2 * np.pi * top2_freq * xSin2) * top2_amp)
        sin3.set_ydata(np.sin(2 * np.pi * top3_freq * xSin3) * top3_amp)

        print(f"TOP 3 AMPLITUDES: {top1_amp}, {top2_amp}, {top3_amp}")

    return line1, line2, sin1, sin2, sin3,


def topThreeVals(arr):
    """Return top 3 (amplitude, frequency) pairs from FFT results."""
    i = 0
    topThree = []
    while i < 3:
        amp = max(arr[1:64 - i])  # Find highest remaining amplitude
        freq = arr.index(amp)     # Find its index
        topThree.append((amp, freq * 8000 / 128))  # Convert index to Hz
        arr.pop(freq)  # Remove so we can find next largest
        i += 1
    return topThree

main()