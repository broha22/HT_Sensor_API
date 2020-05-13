/*
 * @Author: you@you.you
 * @Date:   Sunday February 16th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Sunday February 16th 2020
 * @Copyright:  (c) Oregon State University 2020
 */

#include <HTSensors.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/time.h>
#define PORT 1337
#define MAX 1024
void read_client(int sockfd) { 
  char buff[MAX]; 
  // infinite loop for chat 
  int n;
  while (1) { 
    bzero(buff, MAX); 

    // read the message from client and copy it in buffer 
    read(sockfd, buff, MAX); 
    if (strlen(buff) != 0) {
      
      // check if client sent read command
      if (strncmp("read", buff, 4) == 0) {
        int sensor_count;
        SensorConfig* sensors = read_sensors(&sensor_count); //read sensors
        bzero(buff, MAX);
        /*  message format looks like this:
            
            START,N
            LIB,TYP,X,Y,Z,S,NS,I
            END

            for N sensors
            LIB is library
            TYP is type
            X is x axis reading
            Y is y axis reading
            Z is z axis reading
            S is seconds
            NS is nanoseconds
        */
        sprintf(buff, "START,%d\n", sensor_count);
        write(sockfd, buff, MAX); //write each line
        for (int i = 0; i < sensor_count; i++) { //loop through each sensor
          bzero(buff, MAX);
          // printf("%u\n", sensors[i].index);
          sprintf(buff, "%d,%d,%f,%f,%f,%d,%lu,%u\n",sensors[i].driver_library, 
                                                  sensors[i].sensor_type,
                                                  sensors[i].last_read.x,
                                                  sensors[i].last_read.y,
                                                  sensors[i].last_read.z,
                                                  (int)sensors[i].last_read.time.tv_sec,
                                                  sensors[i].last_read.time.tv_nsec,
                                                  sensors[i].index);
          write(sockfd, buff, MAX);
        }
        
        free(sensors); //free allocated array
        bzero(buff, MAX);
        
        sprintf(buff, "END\n");
        write(sockfd, buff, MAX);

      //exit condition
      } else if (strncmp("exit", buff, 4) == 0) { 
          printf("Server Exit...\n"); 
          break; 
      } 
    }
  }
}

int main(int argc, char** argv) {
  /* Start Sensor API, clear any sensors on shm and load new config */
  init_ht_api();
  clear_sensors();
  load_sensors("/home/pi/HT_Sensor_API/cloud_app/config.txt");
  //Begin setting up TCP Socket server
  int server_fd, new_socket, valread; 
  struct sockaddr_in address; 
  int opt = 1; 
  int addrlen = sizeof(address); 
      
  // Creating socket file descriptor 
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
      perror("socket failed"); 
      exit(EXIT_FAILURE); 
  } 
      
  // Forcefully attaching socket to the port 8080 
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
      perror("setsockopt"); 
      exit(EXIT_FAILURE); 
  } 
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons(PORT); 
      
  // Forcefully attaching socket to the port 8080 
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
      exit(EXIT_FAILURE); 
  }
  //Start listening for connections
  if (listen(server_fd, 3) < 0) {   
    perror("listen");   
    exit(EXIT_FAILURE);   
  }
  /* TODO: make this work more than once */
  //Accept next connection
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
      exit(EXIT_FAILURE); 
  }
  //begin client read loop
  read_client(new_socket);
  close(new_socket);
  exit(0);
}
