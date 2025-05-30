# Motor Dyno
## 1. Project Overview
### 1.1 Project Name
> STEPPER MOTOR DYNO

### 1.2 Description
> a dyno designed to test the parameters of nema17 motors and check speed torque characteristics 

### 1.3 Goals & Objectives
GOALS:
- Evaluate and compare different NEMA17 stepper motors to determine their performance differences.
- Identify the best-performing motor based on key parameters such as torque, efficiency, heat generation, and noise levels.
- Help  engineers and hobbyists make an informed decision when selecting a NEMA17 stepper motor for 3d printers.

OBJECTIVES:
  -Test multiple NEMA17 stepper motors from different manufacturers.
  - Measure and analyze key performance metrics
  - Compare real-world performance vs. manufacturer specifications.
  - Provide practical insights and recommendations for choosing the best motor based on specific applications.
### 1.4 Key Features
Stepper motors tested:
- Hanpose stepper motors 17HS8401S and 17HS4401S and LDO -42STH8-2504AC are being tested.
- 
perfromance parameters:
- Torque output
- voltage and current under increasing breaking force.
- Rpm expected vs rpm live at every iteraion.

---

## 2. Requirements
### 2.1 Hardware Requirements
|Component|Datasheet|schematics|
|----------|-----------|----------|
|MKS Gen LV1.0|https://www.botnroll.com/img/cms/MKS-Gen-L.pdf|https://github.com/makerbase-mks/MKS-GEN_L/blob/master/hardware/MKS%20Gen_L%20V1.0_008/MKS%20Gen_L%20V1.0_008%20PIN.pdf|
|TMC2209 driver|https://components101.com/modules/tmc2209-stepper-motor-driver-module|https://www.analog.com/media/en/technical-documentation/data-sheets/tmc2209_datasheet_rev1.09.pdf|
|Stepper motor|https://dc3dprinters.co.za/product/hanpose-stepper-motor-17hs8401s/|-|
|Hx711|https://www.digikey.com/htmldatasheets/production/1836471/0/0/1/HX711.pdf|-|


- 

### 2.2 Software & Tools
- Arduino IDE
- Fusion
- 

### 2.3 Constraints & Challenges
- *Document limitations such as cost, size, power consumption, etc.*

---

## 3. Development Log
Tmc2209 coding mehthodlogy:
 

### 3.1 Research Links & Resources
| Link | Notes |
|----------|-----------|
|https://www.youtube.com/watch?v=MNTuMiNC2TU| Motor dyno sample |
|https://youtu.be/vSgcH2wjCwY?si=yuAE-5N2YzfLf8_i|Connection of Tmc2209 driver with MKS Gen L V1.0|
|https://www.youtube.com/watch?v=sxzoAGf1kOo| Hx711 connection with load cell|
|https://www.programming-electronics-diy.xyz/2023/12/tmc2209-stepper-driver-module-tutorial.html|Programming modes of Tmc2209|




### 3.2 Key Decisions & Changes
- Raspberry pi pico was used for rpm and force data and  mks board  was used to only run the motors, as the mks was not able to count pulses from the encoder after a range. 

### 3.3 Issues & Solutions
- **Issue:** *Describe the problem.*
  - **Solution:** *Describe how it was solved.*

---

## 4. Development Process
### 4.1 Initial Steps
- Refer the video for practical understanding.
- BOM to be orderd.
- Checking the possibility of  using 3d printed parts in assembly.
- Choosing suitable mcu to reduce circuit complexity.
- Run the motor with a single driver using mks board.

### 4.2 Iterative Development
-Mks firmare or others  like klipper was not used , the mks was programmed only using arduino ide
- Accel stepper library was used to run the stepper motors using mks board.
- Interrupt based pulse detection was used to measure rpm from encoder
- olav kallhovd library of hx711 was used in final code but for unit test and calibration of load cell the hx711 library by bogdan was used.
- python script was used to control the whole mks and pico setup and generate the graph, the rpm threshold detection is done by the python.
- 

### 4.3 Final Steps
- *Summarize the last steps taken before project completion.*

---

## 5. Testing & Validation
### 5.1 Test Cases
| Test | Expected Result | Actual Result | Status |
|------|----------------|---------------|--------|
| *Test Name* | *What should happen?* | *What actually happened?* | ✅ / ❌ |

### 5.2 Performance & Reliability
- The variatio

---

## 6. Final Documentation & Learnings
### 6.1 Finalized Steps
- Do unit testing of individual components  first before intergration.
- No matter the rms current rating of the test motor , keep vref close to 1v for consistent results.
- 

### 6.2 Lessons Learned
- *What worked well? What could be improved?*

### 6.3 Next Steps
- *Outline future improvements or extensions to this project.*

---

## 7. Additional Notes
*(Any other important information.)*

## Run the Code
Note: Close all instances of arduino ide before running
```bash
python dynotest.py
```
