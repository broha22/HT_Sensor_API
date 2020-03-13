# Installation
To install use ``` sudo make install ```.

# Example Code
Examples for using this library are located in ```/example```.

# Config File
Config files are written in CSV format as follows:

```Driver Library (BSH, NXP, LSM), Sensor Type (Gyr, Acc, Mag), I2C Address, X Offset, Y Offset, Z Offset```

Example: ```example/config.txt```

# Runnning the web app
* First install node & npm (node version 10.8.0 is recommended) on development machine
* Next navigate to ```cloud_app/front_end``` and run ```npm install```
* After the installation completes run ```npm run build```
* After the build copy over all code to the RPi for this you can use rsync in a similar fashion to the following
```rsync -arv -e ssh --delete --exclude={'HT_Sensor_API/cloud_app/front_end/node_modules', 'HT_Sensor_API/cloud_app/back_end/node_modules'} HT_Sensor_API/ pi@pizero.local:~/HT_Sensor_API```
* ssh into the pi and run ```sudo make install``` in the HT_Sensor_API directory
  * If a linking error occurs you may need to install additonal libraries
    * ```sudo apt-get install wiringPi```
    * ```sudo apt-get install i2ctools```
  * If an error occurs about the HTSensors.service not existing this may need to be commented out of the Makefile until after the first build (I'll likely be changing this from a systemd service as this is kind of silly to be running all the time)
* once the library is build and installed navigate to ```cloud_app/socket_server``` and run ```make```
* after the application is built run it with ```./ht_socket```
  * There are some bugs with the HTSensors service you may need to restart it every time you boot the pi or rerun the socket server. To restart run ```sudo systemctl restart HTSensors.service``` to start it run ```sudo systemctl start HTSensors.service``` (this will change soon).
* Now back on the development machine navigate to ```cloud_app/back_end``` and run ```node index.js```
* Now you can navigate to ```localhost:3000``` and explore the web app

#### Important Info about sensor config
The web app loads its sensor config from ```cloud_app/config.txt``` if sensors are in the config file but are not connected to the RPi the system will not work. Please ensure that the config file matches the physical connections. To test if the sensors are connected you can run ```i2ctools -y 1``` it will output sensors connected to the i2c line and their address. 