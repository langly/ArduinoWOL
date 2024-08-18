# ArduinoWOL

Simple program to wake up a computer on the LAN from Arduino. Tested on my
ESP32, and it works. Not sure about other devices. 

## Configuration

To configure it, set the mac address you want to wake up in the mac_addr field, 
and the WiFi passwrod in the wifi_name and wifi_pass Strings. 

The interrupt pin I've used for this particular example is 12. Feel free to map
this into whatever you want.

Enjoy!
