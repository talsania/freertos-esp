# FreeRTOS-ESP

This repository contains a collection of ESP-IDF **template-based FreeRTOS applications** developed on the ESP32 platform. Each project in this repository was created using the official ESP-IDF **`template-app`** (via `idf.py create-project`) and then extended or modified to complete assignments from the **30-Day FreeRTOS Course for ESP32 Using ESP-IDF**.

The purpose of this repository is to document hands-on FreeRTOS learning, provide reusable ESP-IDF project templates, and serve as a reference for common FreeRTOS patterns on ESP32.

The original course material used as a reference can be found here:  
https://github.com/god233012yamil/30-Day-FreeRTOS-Course-for-ESP32-Using-ESP-IDF

Official ESP-IDF documentation reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/

---

## Building and Running a Project

1. Clone the repository:

   ```bash
   git clone https://github.com/talsania/freertos-esp.git
   ```

2. Navigate to a project directory:

   ```bash
   cd freertos-esp/project_1
   ```

3. Ensure the ESP-IDF environment is set up correctly.

4. Build, flash, and monitor:

   ```bash
   idf.py build
   idf.py flash monitor
   ```

Refer to the README inside each project directory for project-specific details.
