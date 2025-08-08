import time
import serial
import matplotlib.pyplot as plt
# import matplotlib.animation as animation
from matplotlib.animation import FuncAnimation
import numpy as np

ser = serial.Serial("COM4", 115200,timeout=None)
y = np.zeros(512)
x = np.linspace(0, 512, 512)

# REMEMBER THIS WHEN WE SEND FFTed SIGNALS AS WELL
fig, ax = plt.subplots() # you create instances of the figure (the window), and the line of the graph itself (ax)
line, = ax.plot(x,y) # Initialize the line object (the line object is actually like a list; we add the comma in order to say "hey we only want the content of the list")
ax.set_ylim(-32768, 32767) # set limits within the range of a signed 16 bit integer

def main():
    ani = FuncAnimation(fig, animate, interval=10, blit=True)
    plt.show()
   
    
    
    # start_time = time.time()
    # s = ser.read(bytes_to_read)
    # print(len(s))
    # int_list = np.frombuffer(s, np.int16, count=-1) # the first 800 (or 600) bytes may be scuffed as they dont really have a correlation with whats going on
    # plt.plot(x, int_list)
    # plt.show()



def animate(i):
    data = ser.read(1024)
    y[:] = np.frombuffer(data, np.int16, count= -1)
    line.set_ydata(y)
    return line,
# We start by obtaining our bytes from the serial, then we turn those bytes
# into an integer list.
main()
#     print(y)

#     plt.plot(x,y)
#     plt.show()
# main()

# Basic stopwatch
#  start_time = time.time()

#     while((time.time() - start_time) < 10):
#         print("Hooray!!")
#         time.sleep

