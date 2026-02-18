## ELM-4070 – Arduino Calculator with Keypad

Microcontroller Based System Design – Homework Project

# 📌 Overview

This project is an embedded calculator implemented on Arduino as part of the ELM-4070 Microcontroller Based System Design course.
The system integrates a 4x4 matrix keypad for input and a 16x2 I2C LCD for real-time display.
The software architecture is built around a deterministic state-based execution model to ensure robust and predictable behavior.
The focus of the implementation is not only arithmetic evaluation but also safe input handling, numeric formatting, and defensive embedded design.

# 🧠 System Architecture

The software is structured as a finite-state-driven input system:
* State-based input parsing
* Operator validation and correction
* Safe arithmetic execution
* Deterministic display update logic
* Guard-based error handling
The system avoids undefined behavior by validating every input transition.

# ⚙️ Features

* Basic arithmetic operations (+, −, ×, ÷)
* Consecutive operator correction
* Division-by-zero protection
* Real-time LCD feedback
* Trailing zero trimming
* Automatic result formatting
* Ans chaining (using previous result)
* Input state validation
* Embedded-safe execution logic

# 🔌 Hardware Components

* Arduino (Uno / compatible)
* 4x4 Matrix Keypad
* 16x2 LCD (I2C)
* Breadboard & standard wiring

# 🏗 Software Design Considerations

This project emphasizes:
* Finite State Machine (FSM) modeling
* Deterministic microcontroller execution
* Embedded defensive programming
* Edge-case handling in constrained environments
* Structured input parsing
The system is designed to prevent invalid state transitions and undefined numeric outputs.

# 🎓 Academic Context

Course: ELM-4070 Microcontroller Based System Design
Project Type: Group Homework Project
Topic: Arduino Calculator with Keypad

This repository contains the full implementation and documentation of the project.

# 🚀 Compilation & Upload

1. Open Arduino IDE
2. Install required libraries:
  * Keypad
  * LiquidCrystal_I2C
3. Upload to Arduino board
4. Connect hardware as specified

# 📎 Project Motivation

The objective was to design a reliable embedded system with structured input handling rather than implementing a trivial calculator logic.

The emphasis was placed on:
* Robust control flow
* Clean execution logic
* Defensive design principles
* Practical microcontroller programming
