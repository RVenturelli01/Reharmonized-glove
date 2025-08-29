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
- Resistors (one per flex sensor) 
- Glove (fabric base)  
- Jumper wires & protoboard  
- Accelerometer MPU6050 (RP2040’s onboard IMU)  
- LiPo battery + step-up regulator (5V)
- 
### 2. Software setup  
To make the glove work you need both the Arduino code and the Python receiver running on your PC:  

1. **Arduino side**  
   - Upload the sketch `midi_sender.ino` to the Arduino Nano RP2040.  
   - This code reads the **flex sensors** and the **hand orientation** (via IMU/accelerometer).  
   - The processed data is sent as **MIDI messages over Wi-Fi** to your computer.  
   - ⚠️ In the Arduino code you must set:  
     - Your **Wi-Fi SSID** and **password** (same network for Arduino and PC).  
     - The **IP address of your PC**, so Arduino knows where to send the data.  

2. **PC side**  
   - Run the Python script `MIDI_receiver.py`.  
   - This script listens for the MIDI messages sent by Arduino and works as a **virtual synthesizer**.  
   - The received signals are converted into sounds, which are played through your computer (or a connected DAW / speakers).  


---

## 🛠️ How It Works  
1. Flex sensors measure **finger bending angles**.  
2. The microcontroller processes the signals and sends data via **USB or Wi-Fi**.  
3. Software maps the data into **musical notes, effects, or harmonies**.  
4. Users receive immediate **auditory feedback**, making therapy more engaging.  

## 📜 License  
This project is open-source under the **MIT License**.  
