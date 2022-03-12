# DryBoxESP

This project is the c code for a basic IoT device (in this case a DryBox) that works with an IoT server also available on my GitHub named Drybox. 

A DryBox is a container of 3D printing filament. It ensures that the filament is always dry. In our case, we add some sensors and a microcontroller to send some data about the internal humidity of the box.

For this project, I used an ESP8266. The script can do OTA, HTTP update (via IoT server), send information through the network, calibrate via the IoT server and use a display to print information such as the IoT state and the humidity.
