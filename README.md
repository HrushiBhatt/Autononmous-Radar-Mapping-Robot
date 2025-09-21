# 🤖 Autonomous Radar-Mapping Robot

> A microcontroller-driven robot that scans its surroundings with **IR + ultrasonic sensors**, streams measurements over **TCP**, and renders a **live 180° radar map** in a Python GUI.

---

## 📸 Demo
![View radar scan](https://drive.google.com/file/d/16xv_A377L0o5gDUbLVbmbky7p9fTFUoK/view?usp=drive_link)
![Demo Video of Robot](https://drive.google.com/file/d/1TPPskg4r-oUXWgeg2_9LvXb_GC-wiGCU/view?usp=drive_link)

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
