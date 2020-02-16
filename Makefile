# all: bno055.o
# 	gcc -Wall bno055.o -o sensor_api

# main.o: main.c headers/HTSensors.h config.h
# 	gcc -c main.c

# bno055.o: drivers/bsh/bno055.c drivers/bsh/bno055.h drivers/bsh/bno055_support.c drivers/bsh/bno055_support.h 
# 	gcc -c drivers/bsh/bno055.c drivers/bsh/bno055_support.c 

# bno055_support.o: drivers/bsh/bno055_support.c drivers/bsh/bno055_support.h
# 	gcc -c bno055_support.c

mock_bsh.o: drivers/mock/bsh.c drivers/bsh/bno055_support.h headers/HTSensors.h
	gcc -c drivers/mock/bsh.c -o mock_bsh.o

mock_nxp.o: drivers/mock/nxp.c drivers/nxp/nxp_support.h headers/HTSensors.h
	gcc -c drivers/mock/nxp.c -o mock_nxp.o

mock_lsm.o: drivers/mock/lsm.c drivers/lsm/lsm9ds1_support.h headers/HTSensors.h
	gcc -c drivers/mock/lsm.c -o mock_lsm.o

driver_process.o: service/driver_process.c headers/HTSensors.h headers/HTReadScheduler.h
	gcc -c service/driver_process.c

service.o: service/service.c headers/HTSensors.h headers/HTReadScheduler.h
	gcc -c service/service.c

test_service: mock_bsh.o mock_nxp.o mock_lsm.o service.o driver_process.o
	gcc mock_lsm.o mock_bsh.o mock_nxp.o driver_process.o -o bin/driver_process -lrt -lwiringPi
	gcc service.o -o bin/service -lrt -lwiringPi
	gcc test/test_service.c -o bin/test_service -lrt

api.o: library/api.c headers/HTSensors.h
	gcc -c library/api.c

test_api: api.o test/test_api.c
	gcc api.o test/test_api.c -o bin/test_api -lrt

bsh.o: drivers/bsh/bno055.c  drivers/bsh/bno055.h
	gcc -c drivers/bsh/bno055.c -o bsh.o

bsh_support.o: drivers/bsh/bno055_support.h headers/HTSensors.h drivers/bsh/bno055_support.c
	gcc -c drivers/bsh/bno055_support.c -o bsh_support.o

test_bsh: bsh_support.o bsh.o test/test_bsh.c headers/HTSensors.h drivers/bsh/bno055_support.h
	gcc bsh.o bsh_support.o test/test_bsh.c -o bin/test_bsh

clean:
	rm bin/*
	rm *.o

install:
	cp HTSensors.service /etc/systemd/system/
	systemctl enable /etc/systemd/system/HTSensors.service
	systemctl start HTSensors.service