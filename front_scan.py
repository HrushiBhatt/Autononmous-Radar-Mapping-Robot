import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.gridspec as gridspec
import socket
import math
import threading
import keyboard

# ========== CONFIGURATION ========== #
HOST = '192.168.1.1'
PORT = 288
ROBOT_WIDTH = 35  # cm
RADAR_RADIUS = 70  # cm (IR max range)

# ========== DATA STORAGE ========== #
object_data = []  # (angle_deg, dist, size, width, min_deg, max_deg)
bump_objects = []  # (side, angle)
cliff_lines = []   # (angle_deg, side)

# ========== SOCKET SETUP ========== #
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))

# ========== KEYBOARD HANDLING ========== #
def on_key_press(event):
    global object_data, bump_objects, cliff_lines
    if event.name == 'esc':
        sock.close()
        print('🔌 Disconnected')
        exit()
    elif event.name == 'm':
        object_data.clear()
        bump_objects.clear()
        cliff_lines.clear()
        sock.send(b'm')
    else:
        sock.send(event.name.encode())

keyboard.on_press(on_key_press)

# ========== RECEIVE DATA ========== #
def receive_data():
    while True:
        try:
            data = sock.recv(2048)
            if not data:
                break
            lines = data.decode(errors="ignore").strip().split('\n')
            for line in lines:
                print(f"📥 Received: {line}")
                if line.startswith('Min Degree:'):
                    try:
                        parts = line.split(',')
                        min_deg = int(parts[0].split()[2])
                        max_deg = int(parts[0].split()[4])
                        ir_cm = float(parts[1].split(':')[1].strip())
                        midpoint = int(parts[2].split(':')[1].strip())
                        width = float(parts[3].split(':')[1].strip())

                        if ir_cm > RADAR_RADIUS:
                            continue

                        size = 'small' if width < 5 else 'medium' if width < 15 else 'large'
                        object_data.append((midpoint, ir_cm, size, width, min_deg, max_deg))
                    except Exception as e:
                        print("❌ Parse error:", line)
                        print("‼️ Exception:", e)
                elif "BUMP LEFT" in line:
                    bump_objects.append(("left", 90))
                elif "BUMP RIGHT" in line:
                    bump_objects.append(("right", 90))
                elif "CLIFF FRONT LEFT" in line:
                    cliff_lines.append((90, "front_left"))
                elif "CLIFF FRONT RIGHT" in line:
                    cliff_lines.append((90, "front_right"))
                elif "CLIFF LEFT" in line:
                    cliff_lines.append((45, "left"))
                elif "CLIFF RIGHT" in line:
                    cliff_lines.append((135, "right"))
        except Exception as e:
            print("⚠️ Socket error:", e)

threading.Thread(target=receive_data, daemon=True).start()

# ========== SETUP UI ========== #
fig = plt.figure(figsize=(20, 10), facecolor='#0e0e0e')
gs = gridspec.GridSpec(1, 2, width_ratios=[3, 1])
ax_radar = fig.add_subplot(gs[0], polar=True)
ax_info = fig.add_subplot(gs[1])

# ========== UTILS ========== #
def get_color(size):
    return {'small': 'lime', 'medium': 'orange', 'large': 'red'}.get(size, 'gray')

# ========== ANIMATION ========== #
def animate(i):
    ax_radar.clear()
    ax_info.clear()

    ax_radar.set_facecolor('#111')
    ax_radar.set_theta_zero_location('N')
    ax_radar.set_theta_direction(-1)
    ax_radar.set_thetamin(0)
    ax_radar.set_thetamax(180)
    ax_radar.set_rmax(RADAR_RADIUS)
    ax_radar.grid(True, color='gray', linestyle='--', alpha=0.3)

    ax_radar.plot([0], [0], marker='o', markersize=10, color='white')
    ax_radar.plot([math.radians(0), math.radians(180)], [ROBOT_WIDTH / 2, ROBOT_WIDTH / 2], color='white', linewidth=2)

    for angle, dist, size, width, min_d, max_d in object_data:
        theta = math.radians(angle)
        ax_radar.plot([0, theta], [0, dist], color='deepskyblue', linewidth=2.2, alpha=0.8)
        ax_radar.scatter(theta, dist, color=get_color(size), s=90)
        ax_radar.text(theta, dist + 5, f"{int(angle)}°\n{round(dist,1)}cm\nW:{round(width,1)}", color='white', fontsize=6, ha='center')

    for side, angle in bump_objects[-10:]:
        theta = math.radians(angle)
        ax_radar.scatter(theta, 10, color='magenta', s=120, marker='X', label='Rock')

    for angle, side in cliff_lines[-10:]:
        theta = math.radians(angle)
        ax_radar.plot([theta, theta], [0, RADAR_RADIUS], color='crimson', linewidth=2.5, linestyle='--', label='Cliff')

    ax_info.set_facecolor('#181818')
    ax_info.axis('off')
    ax_info.set_title("📋 Object Data", color='white', fontsize=12, pad=20)

    headers = ["#", "Angle", "Distance", "Width", "Size"]
    for col, text in enumerate(headers):
        ax_info.text(0.05 + 0.2 * col, 1.0, text, fontsize=10, color='cyan', weight='bold')

    for idx, (angle, dist, size, width, _, _) in enumerate(object_data[-12:]):
        y = 0.95 - idx * 0.065
        ax_info.text(0.05, y, f"{idx+1}", color='white', fontsize=9)
        ax_info.text(0.25, y, f"{int(angle)}°", color='white', fontsize=9)
        ax_info.text(0.45, y, f"{round(dist,1)} cm", color='white', fontsize=9)
        ax_info.text(0.65, y, f"{round(width,1)} cm", color='white', fontsize=9)
        ax_info.text(0.85, y, size, color=get_color(size), fontsize=9)

ani = animation.FuncAnimation(fig, animate, interval=100)
plt.tight_layout()
plt.show()
keyboard.wait()
receive_thread.join()
