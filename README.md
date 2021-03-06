
# NodeMCU Weather Station Project

# todo: comments in code
This project contains the code I used to run a simple weatherstation Arduino project.
It uses an LCD to display temperature and humidity as well as having a motion sensor that activates a servo motor.

For our project it was also made to post this data to a webserver, but if you are running the system on your own this is obviously not possible.

If you want details on how I built this project and how you could build your own, please check out the write-up here:
## Full Setup instructions:

Disclaimer: This project was setup over a longer time and I might not remember all details at the time of this write-up, also, please test sensors and parts one by one to make sure they actually work. Trying to test all of them at once while everything is hooked up is *not* recommended.

### Parts list

The weather station requires a lot of parts, this section has a list of
them as well as their purpose.

-   ESP8266 NodeMCU: the main unit that powers our weather station, has
    WiFi connectivity built-in

-   1602A LCD: Used for displaying basic data

-   I2C adapter board: soldered onto 1602A LCD, has a controller that
    lets us easily interact with the display

-   DHT11 sensor: Measures temperature and humidity

-   Tower Pro SG90 Servo: Will be used to operate the door

-   HC-SR501 PIR Motion Sensor: Will be used to sense when a user nears
    the weather station to then activate the servo motor.

-   Large Breadboard

-   A bunch of Male to Female breadboard cables

-   A bunch Male to Male breadboard cables (just get a bundle of both of these)

## Hooking it up: Instruction details

For setting up the NodeMCU weatherstation properly we need to pay attention to a number of details. 
We need to make sure the data connections align with the definitions in code.
It is also important we use the right settings in the Arduino IDE.
Ensure that you have the ESP8266 library installed by adding it in the board manager. 
[these instructions helped me](https://www.instructables.com/id/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/)
\
Then select “NodeMCU 1.0 (ESP12-E)” under tools \> board.\
After setting up these basics we can start setting up all these parts.

-   1602A LCD (with I2C adapter!)

-   DHT11 sensor

-   SG90 servo

-   PIR motion sensor

### 1602A LCD setup


To properly set up the LCD we first need to check whether this LCD already has an I2C adapter. Look at the back of the LCD and check if the following board is attached to it:

![I2C adapter](i2c.png)

If this is the case we can continue with setting up the LCD.\
If not you will need to check online and purchase an I2C adapter,
 this will need to be soldered to the LCD.\
The soldering process is fairly simple, simply slide the adapter through the pinholes on the LCD, First solder the far left and right pins to ensure it stays in place and then solder the rest of the pins.
After doing this we can now start with the connection of the LCD and the NodeMCU.

Hook up the four LCD pins as follows (using male to female cables):

-   Ground to Ground

-   VCC to the VIN pin on NodeMCU (feeds 5V directly from USB, be
    careful with this)

-   SDA to pin D2

-   SCL to pin D1

After doing so we can test whether the I2C device is properly detected by our NodeMCU. 
We do this by using an I2C scanner sketch. 
[Arduino has one for us](https://playground.arduino.cc/Main/I2cScanner/)

If your device is detected, congrats, you set up the LCD in the right way! 
Be sure to check and note the memory address, you need to use this to interface with the device in the code.
As a final setup step, add the I2C library by “Frank de Brabander” through your Arduino IDE’s library manager, we use this to send data to the LCD.

### DHT11 setup

The DHT11 sensor is luckily much easier to set up than the LCD. 
Hook it up as follows into the breadboard with male to male pins:

-   PIN 1: VCC to 3.3V

-   PIN 2: Data to D4

-   PIN 3: unused

-   PIN 4: Ground to Ground

-   Install the DHT sensor library with the IDE library manager.

### SG90 servo setup

The servo is also relatively easy to set up. Using male to male cables we can hook it up as follows:

-   Brown cable to ground

-   Red cable to VIN 5V

-   Yellow to data pin D5 the Servo library in the library manager.

### PIR motion sensor setup

The final component is the PIR sensor. This sensor is very simple,
 it sends a “high” signal when something is detected and a “low” signal otherwise.\
Install the Adafruit unified sensor library to interface with it properly.

![PIR sensor diagram](PIR.png)

Hook up VCC to VIN, GND to Ground and digital OUT to data pin D6.
Then make sure the header is set to “H”.
You can use one potmeter to adjust the time the sensor keeps outputting a “HIGH” signal, ranging from a few seconds to about 7-10minutes.
The other potmeter adjusts how much movement it takes to trigger a “HIGH” state.

Set the time potmeter completely counterclockwise so it only stays active for a few seconds.
Leave the sensitivity meter as is unless you have trouble detecting motion, in that case you might want to adjust it.

### Code sketch: running it

After setting up all these sensors you can use my code to run the complete weather station. 
As is normal for development I recommend testing components separately before running the entire program with all sensors, the Servo and the LCD. 
