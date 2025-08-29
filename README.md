# 🎶 Reharmonized Glove  
*A sensor-equipped glove that transforms hand movements into music for engaging fine motor skills therapy.*  


## 🎥 Watch the demo on YouTube  

[![Watch the demo on YouTube](https://img.youtube.com/vi/HFlzflYO1M4/0.jpg)](https://www.youtube.com/watch?v=HFlzflYO1M4)

---

## ✨ Overview  
Reharmonized Glove is an interactive wearable device designed to **translate hand and finger movements into musical notes and controls**.  
By combining **flex sensors** and **orientation tracking**, the glove captures the motion of the hand and uses it to generate a wide variety of sounds.  

The goal is to create a tool that makes **motor coordination therapy** not only effective but also **fun and motivating**, turning rehabilitation exercises into a playful and creative experience.  

---

## 🔧 Features  
- **Finger flex detection** → flex sensors capture fine finger movements.  
- **Hand orientation tracking** → allows mapping tilt/rotation to sound modulation.  
- **MIDI/Audio output** → compatible with common DAWs, synthesizers, or custom sound engines.  
- **Customizable mappings** → assign gestures to different instruments, notes, or effects.  
- **Therapeutic use case** → designed to support **rehabilitation of fine motor skills**.  

---

## 🧩 Applications  
- 🎹 **Musical instrument** → perform and compose music using only hand gestures.  
- 🧠 **Rehabilitation tool** → enhance patient engagement during physical therapy.  
- 🎮 **Interactive installations** → use as a controller for sound, visuals, or games.  

---

## 🚀 Getting Started  

### 1. Hardware requirements  
- Arduino Nano RP2040 Connect (or compatible board)  
- Flex sensors (one per finger)  
- Glove (fabric base)  
- Jumper wires & protoboard  
- (Optional) Accelerometer MPU6050 if not using RP2040’s onboard IMU  
- LiPo battery + step-up regulator (5V)  

### 2. Software setup  
- Arduino IDE (with necessary libraries)  
- Python scripts (for MIDI/OSC communication)  
- Any DAW or synthesizer to receive sound  

---

## 🛠️ How It Works  
1. Flex sensors measure **finger bending angles**.  
2. The microcontroller processes the signals and sends data via **USB or Wi-Fi**.  
3. Software maps the data into **musical notes, effects, or harmonies**.  
4. Users receive immediate **auditory feedback**, making therapy more engaging.  

## 📜 License  
This project is open-source under the **MIT License**.  
