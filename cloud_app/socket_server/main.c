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
#define PORT 1337
#define MAX 2048
void read_client(int sockfd) { 
  char buff[MAX]; 
  // infinite loop for chat 
  for (;;) { 
    bzero(buff, MAX); 

    // read the message from client and copy it in buffer 
    read(sockfd, buff, sizeof(buff)); 
    // print buffer which contains the client contents 
    printf("From client: %s\t To client : ", buff); 
    bzero(buff, MAX); 
    if (strncmp("read", buff, 4) == 0) {
      bzero(buff, MAX);
      int sensor_count;
      SensorConfig* sensors = read_sensors(&sensor_count);
      sprintf(buff, "START,%d\n", sensor_count);
      write(sockfd, buff, sizeof(buff));
      for (int i = 0; i < sensor_count; i++) {
        bzero(buff, MAX);
        sprintf(buff, "%d,%d,%f,%f,%f,%d,%lu\n",sensors[i].driver_library, 
                                                sensors[i].sensor_type,
                                                sensors[i].last_read.x,
                                                sensors[i].last_read.y,
                                                sensors[i].last_read.z,
                                                (int)sensors[i].last_read.time.tv_spec,
                                                sensors[i].last_read.time.tv_nsec);
        write(sockfd, buff, sizeof(buff));
      }
      free(sensors);
      bzero(buff, MAX);
      sprintf(buff, "END\n");
      write(sockfd, buff, sizeof(buff));
    }
    
    if (strncmp("exit", buff, 4) == 0) { 
        printf("Server Exit...\n"); 
        break; 
    } 
  }
}

int main(int argc, char** argv) {
  init_ht_api();
  clear_sensors();
  load_sensors("/home/pi/HT_Sensor_API/cloud_app/config.txt");
  
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
  if (listen(server_fd, 3) < 0) { 
      exit(EXIT_FAILURE); 
  } 
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
      exit(EXIT_FAILURE); 
  } 
  read_client(new_socket);
  close(new_socket);
}
