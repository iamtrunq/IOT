**Project Overview:**

Develop an automated system to control room lighting by adjusting blinds based on light sensor data, with light control accessible via a web interface.

**Key Components:**

Raspberry Pi 3+: The Raspberry Pi serves as the central control unit of the system, responsible for receiving data from one ESP8266, processing data, and sending control signals to another ESP8266 to control the blinds.

MariaDB: MariaDB is used to store sensor data and control information for the system.

ESP8266 #1: The first ESP8266 is connected to the light sensor to read data and send it to the Raspberry Pi.

ESP8266 #2: The second ESP8266 receives control signals from the Raspberry Pi to control the blinds.

**Operation Steps:**

1. ESP8266 #1 reads light sensor data and sends it to the Raspberry Pi.
2. The Raspberry Pi receives light level data from ESP8266 #1 and processes it.
3. Based on the sensor data, the Raspberry Pi decides whether to open or close the blinds.
4. The Raspberry Pi sends control signals to ESP8266 #2.
5. ESP8266 #2 receives the control signals and adjusts the mechanism for opening or closing the blinds.

