# TWO_QUEUES_USING_FREERTOS
ESP32-based FreeRTOS project demonstrating inter-task communication using queues. Task A processes commands like 'delay' to control an LED blink rate and signals completion. Task B handles serial input, sends data to Task A, and blinks the LED based on received commands. Includes multi-core support, making it suitable for embedded IoT applications.
