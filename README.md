# 🤖 Autonomous Radar Mapping Robot 

> **Real-time object detection & room mapping** powered by **embedded systems + Python visualization**.  
> Built with C (CyBot firmware), Python (Tkinter + Matplotlib GUI), and Wi-Fi sockets.

---

## 🚀 Project Overview
This robot scans its surroundings with infrared (IR) and ultrasonic sensors,  
detects objects, and plots a **live 360° radar map** on a custom Python GUI.

Key Features:
- 🛰️ **Autonomous Radar Mapping** – 360° object scanning and distance calculation
- 🎯 **Target Detection** – Identifies the largest object (≥17 cm) as a final target
- ⚡ **Real-time Visualization** – Interactive Tkinter/Matplotlib radar interface
- 🧠 **Smart Pathfinding** – Navigates toward smallest-width openings while avoiding obstacles
- 🌐 **Wireless Control** – TCP socket connection to the CyBot at `192.168.1.1:288`

---

## 🏗️ Tech Stack

| Layer            | Tools / Languages |
|------------------|--------------------|
| **Firmware**     | C for sensor control, UART, and motor logic |
| **Hardware**     | CyBot, IR & PING sensors, servo motor |
| **UI / Control** | Python, Tkinter GUI, Matplotlib (polar plots), keyboard module |
| **Networking**   | Wi-Fi TCP socket for bidirectional commands & data streaming |

---

## ⚙️ How It Works
1. **Scan** – Robot sweeps IR & PING sensors through 180–360°  
2. **Measure** – Calculates object width using angle and distance data  
3. **Transmit** – Sends formatted packets `!theta!r!size` over TCP  
4. **Visualize** – Python GUI converts packets into a live radar map  
5. **Navigate** – Algorithm drives toward safe paths or final target

---

## 🖥️ Running the Project

### 1️⃣ Firmware
- Flash the CyBot with the embedded C code (IR + PING scanning and UART transmission)

### 2️⃣ Python Radar UI
```bash
python radar_ui.py
