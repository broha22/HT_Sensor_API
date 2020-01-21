linux Kernel driver fxos8700 , and be compatible with Android
=====================

Supported chips:
  * Freescale Digital Combo sensor Devices FXOS8700CQ
    Addresses : I2C 0x1E
    Datasheet: http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=FXOS8700CQ
 

Author: Rick Zhang <Rick.Zhang@freescale.com>

Driver Features
---------------

1. support FXOs8700 data ready and motion detect interrupt, create two input devices to report sensor data and event if generate interrupt

   FreescaleAccelerometer  -- report Acclerometer raw data and motion detect event

   FreescaleMagnetometer   -- report Magnetometer raw data

2. create two misc device /dev/FreescaleAccelerometer and /dev/FreescaleMagnetometer
   which are suport ioctl interfaces to read sensor raw data and get/set sensor name, 
   power-on status.

3. create two sysfs dir /sys/class/misc/FreescaleAccelerometer and /sys/class/misc/FreescaleMagnetometer
   /sys/class/misc/FreescaleAccelerometer include:

   enable     	  --  write 1/0 to active/standby the Accelerometer sensor, as:
		      echo "1" > enable, to active the sensor 

   poll_delay 	  --  set sensor data sample rate time(ms) as:
		      echo "10" > enable, to set sensor odr as 100HZ

   motion_detect  --  set sensor motion detect parameter, as:
		      echo "21,1" > motion_detect, to set motion detect threshold as 21 and debounce time as 1


   /sys/class/misc/FreescaleMagnetometer include "enable" and "poll_delay" sysfs interfaces, with the same features as above
  

 

