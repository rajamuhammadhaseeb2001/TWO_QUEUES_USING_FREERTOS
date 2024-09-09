#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu = 1;
#endif

static QueueHandle_t msg_queue1;
static QueueHandle_t msg_queue2;
char receivedData[100];
int i = 0;
int j = 0;
int delayTime = 0;
int blinking = 0;

void taskA(void *parameters) 
{
  char item2[100];
  while (1) {
    // Receive items from msg_queue2
    if (xQueueReceive(msg_queue2, item2, portMAX_DELAY) == pdTRUE) 
    {
      if (strncmp(item2, "delay", 5) == 0) 
      {
        char* numberStr = item2 + 6; // Skip past "delay" to get the number
        delayTime = atoi(numberStr); // Convert the number to integer
        Serial.print("delayTime: ");
        Serial.print(delayTime);
        Serial.println("ms");
        if (xQueueSend(msg_queue1, &delayTime, portMAX_DELAY) != pdTRUE) 
        {
          Serial.println("Queue 2 Full");
        }
      }
      if (strcmp(item2, "BLINKED") == 0)
      {
        Serial.println("LED BLINKED 100 TIMES");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Task delay
  }
}

void taskB(void *parameters) 
{
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char c = Serial.read();
      if (c == '\n' || c == '\r') 
      {
        if (i > 0) {
          receivedData[i] = '\0'; // Null-terminate the string
          Serial.print("Received Input: ");
          Serial.println(receivedData);
          i = 0; // Reset index for next input
          // Send the data to Queue 2
          if (xQueueSend(msg_queue2, (void *)&receivedData, portMAX_DELAY) != pdTRUE) {
            Serial.println("Queue 2 Full");
          }
          memset(receivedData, 0, sizeof(receivedData));
        }
      } 
      else 
      {
        if (i < sizeof(receivedData) - 1) 
        {
          receivedData[i++] = c;
        }
      }
    }
    if (xQueueReceive(msg_queue1, &blinking, 10) == pdTRUE)
    {
      for (j = 0; j < 100; j++)
      {
        digitalWrite(2, HIGH);
        vTaskDelay(blinking / portTICK_PERIOD_MS);
        digitalWrite(2, LOW);
        vTaskDelay(blinking / portTICK_PERIOD_MS);
      }

     // Send "BLINKED" to Queue 2 properly as a string
      const char* blinkedMessage = "BLINKED";
      if (xQueueSend(msg_queue2, blinkedMessage, portMAX_DELAY) != pdTRUE) 
     {
        Serial.println("Queue 2 Full");
     }
   }
    //Serial.println(blinking);
    vTaskDelay(500 / portTICK_PERIOD_MS); // Yield to FreeRTOS tasks
  }
}


void setup() {
  Serial.begin(115200); // Initialize serial communication
  delay(100); // Short delay to allow serial to initialize
  pinMode(2, OUTPUT);

  // Create queues
  msg_queue1 = xQueueCreate(5, sizeof(int)); // Queue to hold ints
  msg_queue2 = xQueueCreate(5, sizeof(char[100])); // Queue to hold strings

  if (msg_queue1 == NULL || msg_queue2 == NULL)
  {
    Serial.println("Failed to create queues");
    while (1); // Halt the program if queue creation failed
  }

  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(
    taskA,
    "TASK A",
    4096, // Increase stack size
    NULL,
    1,
    NULL,
    app_cpu
  );

  xTaskCreatePinnedToCore(
    taskB,
    "TASK B",
    4096, // Increase stack size
    NULL,
    1,
    NULL,
    app_cpu
  );
}

void loop() 
{
  // Empty loop as tasks are handled by FreeRTOS
}