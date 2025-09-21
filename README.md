# 🤖 Autonomous Radar-Mapping Robot

> An embedded-systems robot that scans its environment, detects objects with **infrared and ultrasonic sensors**, and renders a **real-time radar map** on a custom Python GUI.

---

## 🛰️ Overview
This project demonstrates a full embedded + software stack:
- **C firmware** running on a CyBot microcontroller gathers distance and angle data from IR and PING (ultrasonic) sensors.
- Data streams live over **TCP sockets** to a **Python GUI** built with Tkinter and Matplotlib.
- The GUI displays a **dynamic 180° radar plot** of detected objects and highlights the **largest target**.

---

## ⚙️ Tech Stack

| Layer | Technologies | Role |
|-------|-------------|------|
| **Embedded Firmware** | C, ADC drivers, sensor fusion | Controls IR & ultrasonic sensors, calculates object width and distance |
| **Communication** | TCP sockets | Sends sensor data (`!theta!r!size`) to the PC in real time |
| **Desktop Application** | Python, Tkinter, Matplotlib | Plots objects on a polar radar map and provides live control interface |
| **Hardware** | CyBot robot, IR and PING sensors | Scans surroundings and navigates autonomously |

---

## 📸 Live Radar Screenshot
Below is an actual scan captured from the running system:

![R2D2 Radar System](IMG_6266.jpg)

This image shows multiple detected objects plotted at their true angles and distances.  
Objects are dynamically updated as the robot scans.

---

## 🎥 Demo Video (Optional)
Add your recorded demo for recruiters and collaborators:

> 👉 [Download/Watch Demo](IMG_0206.MOV)

*(The link will work when the `.MOV` file is committed to the repository.)*

---

## 🏗️ How It Works
1. **Sensor Sweep** – The robot rotates a servo-mounted sensor array to collect distance readings across a 180° arc.  
2. **Real-Time Processing** – Embedded C code filters raw ADC readings, computes object width, and formats packets.  
3. **Live Visualization** – A Python Tkinter/Matplotlib GUI plots each object on a radar grid and highlights the largest target.  
4. **Autonomous Navigation** – The system can steer toward or avoid objects based on the mapped environment.

---
