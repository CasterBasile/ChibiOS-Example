import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Adjust to your board’s COM port and baudrate
PORT = "/dev/ttyACM0"    # or "/dev/ttyUSB0"
BAUD = 38400

ser = serial.Serial(PORT, BAUD, timeout=1)

fig, ax = plt.subplots()
heatmap = ax.imshow(np.zeros((8,8)), vmin=0, vmax=2000, cmap="plasma")
fig.colorbar(heatmap)

def update(_):
    line = ser.readline().decode(errors="ignore").strip()
    if not line.startswith("FRAME,"):
        return heatmap

    try:
        values = list(map(int, line.split(",")[1:]))
        if len(values) == 64:
            frame = np.array(values).reshape((8,8))
            heatmap.set_data(frame)
    except ValueError:
        pass

    return heatmap

ani = animation.FuncAnimation(fig, update, interval=100, blit=False)
plt.show()