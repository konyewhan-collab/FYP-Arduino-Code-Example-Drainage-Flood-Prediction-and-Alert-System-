# FYP-Arduino-Code-Example-Drainage-Flood-Prediction-and-Alert-System
This is the sample of the Arduino IDE code that I use for the Final Year Project that i do in University.

# Hardware Component Use
Here below is the component that i use for the project.

**1. ESP32 DevKit V1** 

<p align="center">
  <img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/50746267-d60d-4f70-b156-2db3d7e29398" />
</p>
<p align="center">
<img width="800" height="400" alt="image" src="https://github.com/user-attachments/assets/c3d086a3-b05a-4cbd-8352-65331594d685" />
</p>

The ESP32 DevKit V1 is the main microcontroller in this project. It executes the Arduino code and controls all connected sensors in this project. 

**2. Ultrasonic Sensor** 

<img width="400" height="300" alt="image" src="https://github.com/user-attachments/assets/23aea58a-b011-45c3-8d9f-977a8d0507a6" />

Used to monitor the water level within the drainage system in real-time. By continuously measuring the distance to the water surface, this sensor detects abnormal rises in water levels to trigger early flood warnings.

**3. Waterflow Sensor YF-S201** 

<img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/6cca2734-9dcb-4f68-bf24-087201f8953a" />

Measures the flow rate and volume of water passing through the drain. Monitoring the water velocity helps the system identify potential blockages or heavy water surges before flooding occurs.

**4. GPS-Module Neo-6m** 

<img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/f6652566-d920-4764-bf3f-425d262492e8" />

Provides real-time geographical coordinates for the hardware node. This ensures that when a flood risk or blockage is detected, the system can pinpoint the exact location on the web dashboard for rapid response.

**Example of the Demonstration Environment Setup**

<img width="600" height="405" alt="image" src="https://github.com/user-attachments/assets/d016a51d-aea3-4b38-9702-9e8070e1be84" />

### Hardware Setup

Connect the components to the ESP32 DevKit V1 according to the pin definitions outlined at the top of the `.ino` file. General guidelines:
* **Ultrasonic Sensor:** Connect VCC (5V), GND, and map the Trig and Echo pins.
* **Waterflow Sensor YF-S201:** Connect VCC (5V), GND, and attach the yellow data wire to the designated interrupt pin on the ESP32.
* **GPS-Module Neo-6m:** Connect VCC (3.3V/5V), GND, and cross the TX/RX pins to the ESP32's defined serial pins.

