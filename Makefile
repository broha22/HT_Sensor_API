


# all: bno055.o
# 	gcc -Wall bno055.o -o sensor_api

# main.o: main.c headers/HTSensors.h config.h
# 	gcc -c main.c

# bno055.o: drivers/bsh/bno055.c drivers/bsh/bno055.h drivers/bsh/bno055_support.c drivers/bsh/bno055_support.h 
# 	gcc -c drivers/bsh/bno055.c drivers/bsh/bno055_support.c 

# bno055_support.o: drivers/bsh/bno055_support.c drivers/bsh/bno055_support.h
# 	gcc -c bno055_support.c


install:
	cp HTSensors.service /etc/systemd/system/
	systemctl enable /etc/systemd/system/HTSensors.service
	systemctl start HTSensors.service