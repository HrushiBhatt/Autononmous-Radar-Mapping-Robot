# 🤖 Autonomous Radar-Mapping Robot

> A fully integrated embedded-systems and software project where a mobile robot **scans its surroundings**, senses obstacles with multiple sensors, and sends real-time data to a **live radar visualization**.  

This README explains **how the robot operates and how the radar plot displays object distance, width, and size** as the robot scans its environment.

---

## 🎯 Project Overview
The Autonomous Radar-Mapping Robot combines **embedded hardware, C firmware, sensor fusion, and a Python-based visualization pipeline**.  
Together these components create a self-scanning system that:

- Detects objects using **infrared (IR)** and **PING ultrasonic** sensors, with **cliff** and **bump** sensors for ground and collision safety.
- Calculates each object’s **angular position, distance, and linear width**.
- Streams those measurements over **TCP sockets** to a laptop running a live **radar-mapping GUI** built with **Tkinter and Matplotlib**.

The result is an accurate 180° environmental map rendered in real time.

---

## ⚙️ How the Robot Scans and Detects

### 1️⃣ Sensor Array & Motion
- **Infrared (IR) Sensor** – Measures short-range distances precisely by converting reflected infrared light into voltage (processed via the microcontroller’s ADC).  
- **PING Ultrasonic Sensor** – Emits ultrasonic pulses and measures echo time to confirm and extend distance readings.  
- **Cliff Sensors** – Detect sudden floor drops to prevent falls.  
- **Bump Sensors** – Trigger immediate stop on unexpected contact.

A servo motor **rotates the sensor assembly through a 180° sweep**, pausing at fixed angular increments. At each step, the firmware synchronously queries the sensors, ensuring precise distance–angle pairing.

---

### 2️⃣ Embedded Data Processing
- **Angle & Distance Calculation** – Every reading is timestamped and tagged with its servo angle. IR voltages and ultrasonic echo times are converted into accurate distances (cm) using calibration formulas.
- **Object Width Estimation** – The firmware monitors continuous detections across adjacent angles to compute an object’s **linear width**, factoring in the robot’s geometry.
- **Packet Formation** – Each detected object is encoded as:

and streamed via Wi-Fi TCP to the host computer.

This on-board computation reduces noise and bandwidth, sending only refined, high-value data.

---

## 📡 Real-Time Radar Visualization

### 1️⃣ Data Reception & Parsing
- A **Python Tkinter/Matplotlib application** listens on a TCP port.
- Incoming packets are parsed to extract **angle (θ)**, **distance (r)**, and **width**.

### 2️⃣ Polar Plot Mapping
- **Positioning:** Each object is plotted at its corresponding polar coordinate (θ, r).  
- **Size Rendering:** The object’s calculated **width** determines the arc length drawn on the radar.  
- **Visual Emphasis:** Larger objects appear as **thicker or wider arcs**, immediately conveying their scale relative to the robot.

This creates a **live, 180° radar map** where distance from the robot, object size, and position are intuitively visible.

---

## 🧠 Key Technical Features
- **Embedded C Firmware** – Controls servo motion, reads sensors, converts ADC values, and streams filtered data.
- **Sensor Fusion** – Combines IR precision and ultrasonic range for robust distance detection.
- **Real-Time Networking** – Uses TCP sockets for continuous, low-latency communication.
- **Dynamic Data Visualization** – Leverages Matplotlib polar plotting for instantaneous updates.

---

## 📸 Demonstration
- [🎥 Watch the robot scan and map its environment](https://drive.google.com/file/d/1QH72uYH43xJ-KvKbDiyjW5pgsUf5nxqJ/view?usp=drive_link)  
- [📄 View a captured radar scan](https://drive.google.com/file/d/16xv_A377L0o5gDUbLVbmbky7p9fTFUoK/view?usp=drive_link)

These media files show the robot performing a full sweep and the live radar interface rendering detected objects in real time.

---

## 🧩 Skills Demonstrated
- Embedded Systems Programming (C, ADC, servo control)
- Sensor Fusion & Object Detection
- Real-Time Networking and Data Streaming
- Python GUI & Scientific Visualization
- Systems Integration and Robotics

---

## 👤 Author
**Hrushi Bhatt**  
Junior, B.S. Computer Engineering – Iowa State University  

---
