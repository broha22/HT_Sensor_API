system=1
all: api service
	$(eval system=1)

install: all
	systemctl stop HTSensors
	cp bin/driver_process /usr/bin/ht_driver_process
	cp bin/service /usr/bin/ht_service
	cp bin/libHTApi.so /usr/lib/libHTApi.so
	cp headers/HTSensors.h /usr/include/HTSensors.h
	cp service/HTSensors.service /etc/systemd/system/HTSensors.service
	systemctl daemon-reload
	systemctl start HTSensors.service

api: api.o
	ar rcs bin/libHTApi.so api.o

mock_bsh.o: drivers/mock/bsh.c drivers/bsh/bno055_support.h headers/HTSensors.h
	gcc -c drivers/mock/bsh.c -o mock_bsh.o

mock_nxp.o: drivers/mock/nxp.c drivers/nxp/nxp_support.h headers/HTSensors.h
	gcc -c drivers/mock/nxp.c -o mock_nxp.o

mock_lsm.o: drivers/mock/lsm.c drivers/lsm/lsm9ds1_support.h headers/HTSensors.h
	gcc -c drivers/mock/lsm.c -o mock_lsm.o

driver_process.o: service/driver_process.c headers/HTSensors.h headers/HTReadScheduler.h
	gcc -c service/driver_process.c

service.o: service/service.c headers/HTSensors.h headers/HTReadScheduler.h
ifeq ($(system),1)
	echo "I made the right one"
	gcc -DHT_DRIVER_PROCESS=\"/usr/bin/ht_driver_process\" -c service/service.c
else
	gcc  -DHT_DRIVER_PROCESS=\"./driver_process\" -c service/service.c
endif

test_service: mock_bsh.o mock_nxp.o mock_lsm.o service.o driver_process.o
	gcc mock_lsm.o mock_bsh.o mock_nxp.o driver_process.o -o bin/driver_process -lrt -lwiringPi
	gcc service.o -o bin/service -lrt -lwiringPi
	gcc test/test_service.c -o bin/test_service -lrt

service: bsh.o lsm.o bsh_support.o nxp_support.o lsm_support.o service.o driver_process.o
	gcc lsm.o bsh.o nxp_support.o bsh_support.o lsm_support.o driver_process.o -o bin/driver_process -lrt -lwiringPi
	gcc service.o -o bin/service -lrt -lwiringPi

api.o: library/api.c headers/HTSensors.h
	gcc -c library/api.c

test_api: api.o test/test_api.c
	gcc api.o test/test_api.c -o bin/test_api -lrt

bsh.o: drivers/bsh/bno055.c  drivers/bsh/bno055.h
	gcc -c drivers/bsh/bno055.c -o bsh.o

nxp_support.o: drivers/nxp/nxp_support.c  drivers/nxp/nxp_support.h
	gcc -c drivers/nxp/nxp_support.c -o nxp_support.o

lsm.o: drivers/lsm/lsm9ds1_reg.c  drivers/lsm/lsm9ds1_reg.h
	gcc -c drivers/lsm/lsm9ds1_reg.c -o lsm.o

lsm_support.o: drivers/lsm/lsm9ds1_support.c  drivers/lsm/lsm9ds1_support.h
	gcc -c drivers/lsm/lsm9ds1_support.c -o lsm_support.o

bsh_support.o: drivers/bsh/bno055_support.h headers/HTSensors.h drivers/bsh/bno055_support.c
	gcc -c drivers/bsh/bno055_support.c -o bsh_support.o

test_bsh: bsh_support.o bsh.o test/test_bsh.c headers/HTSensors.h drivers/bsh/bno055_support.h
	gcc bsh.o bsh_support.o test/test_bsh.c -o bin/test_bsh

clean:
	rm bin/*
	rm *.o