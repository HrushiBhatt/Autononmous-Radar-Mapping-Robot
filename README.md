# 🤖 Autonomous Radar-Mapping Robot

> A microcontroller-driven robot that scans its surroundings with **IR + ultrasonic sensors**, streams measurements over **TCP**, and renders a **live 180° radar map** in a Python GUI.

---

## 📸 Demo
![Live radar scan](./IMG_6266.jpg)

> If the image doesn’t show: confirm the file name/case is `IMG_6266.jpg` and it lives in the **same folder** as this README (or update the path).

---

## ✨ What it does
- **Real-time scanning:** 180° sweep with angle + distance sampling  
- **Object detection:** width/size estimation and target highlighting  
- **Live visualization:** interactive radar plot on your laptop  
- **Autonomy hooks:** data pipeline ready for navigation/avoidance logic

---

## 🛠️ Tech Overview (how it’s built)
- **Firmware (C):** CyBot/Tiva MCU reads IR & PING sensors, filters data, computes object width, and packets readings as `!theta!r!size`.  
- **Comms (TCP):** Microcontroller opens a socket and streams packets to the PC.  
- **GUI (Python):** Tkinter + Matplotlib (polar) plots objects at true angles/distances and refreshes in real time.

---

## 🚀 Quick start
**GUI (Python)**
```bash
# from repo root
cd gui
python -m venv .venv && source .venv/bin/activate    # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python radar_gui.py
