# Vehicle Parking & Tracking System

**Project Code:** IU2541230781 – CPP71  
**Student:** Hitendra Singh Solanki  
**Semester:** 3  
**Language:** C++ (C++17)  
**Type:** Console-based Object-Oriented Programming project

## Project Overview

Vehicle Parking & Tracking is a menu-driven C++ console application for maintaining vehicle records and carrying out basic parking operations. It supports record management, parking entry/exit operations, reports, and saving/loading data through a local text file.

## Main Features

- Add, display, search, update, and delete vehicle records
- Park a registered vehicle and process its exit
- View active parking and system reports
- Save and load records using `parking_data.txt`
- Input validation and object-oriented class design

## OOP Concepts Demonstrated

- Classes and objects
- Encapsulation
- Constructors and destructors
- Inheritance and runtime polymorphism
- Dynamic memory allocation (`new[]` / `delete[]`)
- File handling and data persistence
- Input validation and exception handling where implemented

## Files

- `main.cpp` — application source code
- `parking_data.txt` — data file created/used by the application when it runs
- `UML_Diagrams/` — Use Case, Class, and two Sequence diagrams
- `IU2541230781_Vehicle_Parking_Tracking_Project_Report.pdf` — project report
- `IU2541230781_Vehicle_Parking_Tracking_Submission_Template_Filled.pdf` — submission template

## Requirements

- A C++ compiler that supports C++17 (for example, GCC/g++)
- Windows PowerShell or another terminal

## Compile and Run (Windows / PowerShell)

Open a terminal in the folder containing `main.cpp` and run:

```powershell
g++ -std=c++17 main.cpp -o VehicleParking.exe
.\VehicleParking.exe
```

If the executable is already supplied, run it directly with:

```powershell
.\VehicleParking.exe
```

The application uses `parking_data.txt` in its current working directory for saved records. Keep this file in the same working directory when existing saved data should be retained.

## UML Diagrams

The `UML_Diagrams` folder should contain:

1. `Use_Case_Diagram.png`
2. `Class_Diagram.png`
3. `Sequence_Diagram_1_Main_Parking_Transaction.png`
4. `Sequence_Diagram_2_Search_Record.png`

## Academic Note

This repository is part of the CPP71 C++ guided project submission. The code and screenshots should be checked against the final version run by the student before submission.
