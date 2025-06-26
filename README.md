# Precision Dispensing and Segregation System  
### Lam Research Challenge – Round 2 Submission (Team ID: TW-LAM-001)

## Overview

This project presents a **proof-of-concept precision liquid dispensing system** that utilizes a **peristaltic pump**, **load cell-based feedback**, and a **servo-driven pinch valve** mechanism. The system alternately dispenses red and blue liquids with high precision into segregated containers, minimizing cross-contamination and ensuring process repeatability.

This repository contains all necessary technical documentation, code, mechanical design files, diagrams, and media as per the challenge guidelines.

---

## 📦 Folder Structure


📁 001_LR2C1/
├── Code_LR2C1_326/ # Firmware (bare-metal code)

├── Photos/ # Contraption build and final setup photos

├── BoM_326_LR2C1.pdf # Bill of Materials

├── Circuit diagram.jpg # Electronics circuit wiring

├── Mechanical Drawings_LR2C1_326.pdf

├── Process Flow Chart_LR2C1_326.pdf

├── Wiring Diagram_LR2C1_326.pdf

└── Working Video (not included in repo but submitted via form)

yaml
Copy
Edit

---

## 🛠️ System Description

The system is designed to:

- Dispense **5g of red solution** and **10g of blue solution** into containers.
- Use a **peristaltic pump** and **pinch valves** to ensure **non-contact, contamination-free dispensing**.
- Employ a **1kg load cell** for weight-based feedback with real-time tare and display.
- Prevent mixing via **reverse fluid motion** post-dispense.
- Segregate the storage using a **linear actuator**, moving either the vessels or the spout.
- Alternate between red and blue solutions for **five full cycles**.

---

## 🔩 Components Used

| Component                      | Quantity | Description                                  |
|-------------------------------|----------|----------------------------------------------|
| Peristaltic Pump              | 1        | Precision metered liquid delivery            |
| Load Cell with HX711          | 1        | 1kg sensor with amplifier                    |
| MG90S Servo                   | 2        | Used for pinch valve actuation              |
| Stepper Motor + Lead Screw    | 1 + 1    | For linear movement (actuator)              |
| ESP32 Microcontroller         | 1        | Core control and feedback system            |
| I2C OLED Display              | 1        | Real-time weight display                    |
| 12V Power Supply              | 1        | Powers pump and control circuit             |
| Tubes, Valves, Reservoirs     | Multiple | Fluid handling and storage                  |

Detailed cost breakdown is available in `BoM_326_LR2C1.pdf`.

---

## 🧠 Functionality Flow

1. **Startup**: System initializes, OLED shows 0g after taring.
2. **Red Dispensing**: 5g of red liquid is dispensed based on live weight feedback.
3. **Return Residuals**: Any leftover fluid in the tube is pushed back to the red reservoir.
4. **Spout/Vessel Movement**: The linear actuator repositions the spout or container.
5. **Blue Dispensing**: 10g of blue solution is dispensed similarly.
6. **Cycle Repeat**: The system repeats the process for 5 cycles.

Detailed process flow is in `Process Flow Chart_LR2C1_326.pdf`.

---

## 📷 Media

Photos and videos of the build and final prototype:
- 📸 Multiple setup and work-in-progress pictures inside the `Photos/` folder.
- 🎥 Working demo video (submitted via Google Form, not in repo).

---

## 📄 Documents Included

- ✅ `BoM_326_LR2C1.pdf`: Bill of Materials
- ✅ `Mechanical Drawings_LR2C1_326.pdf`: Design sketches and views
- ✅ `Wiring Diagram_LR2C1_326.pdf`: Full connection diagram
- ✅ `Process Flow Chart_LR2C1_326.pdf`: Logical flow of operation
- ✅ `Circuit diagram.jpg`: Electronics overview
- ✅ `Code_LR2C1_326/`: Bare-metal embedded code and pseudocode

---

## 🎯 Outcomes Achieved

- Accurate gravimetric dispensing: ±0.5g tolerance
- Zero contamination via non-contact design
- Autonomous cycle with spout/vessel switching
- Display of real-time weight readings
- Completed under Rs 4700 budget

---

## 📅 Timeline & Resource Allocation

All planning, sourcing, integration, and testing was completed within the 13-day window. Detailed Gantt chart and task assignments are available in the project plan documents (available upon request).

---

## 🧾 Licensing

This project is part of the Lam Research Challenge 2024 and is shared for evaluation and educational use only.

---

## 🤝 Contributors

- Team: TW-LAM-001  
- Role Assignment: See resource plan document  
- Contact: Provided in submission form

---

> Built with precision and purpose, ensuring contamination-free dispensing under budget 🚀
