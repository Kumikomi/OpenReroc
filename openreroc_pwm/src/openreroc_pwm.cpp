#include "ros/ros.h"
#include "openreroc_pwm/openreroc_pwm.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termio.h>
#include <signal.h>

void config_console();
void allwrite(int fd, unsigned char *buf, int len);
int fd_32;

unsigned int convert_state(unsigned char dir_left,
                          unsigned int para_left,
                          unsigned char dir_right,
                          unsigned int para_right);

void chatterCallback(const openreroc_pwm::openreroc_pwm msg)
{

    unsigned int state;
    int rc;
    unsigned char dir_left;
    unsigned int para_left;
    unsigned char dir_right;
    unsigned int para_right;

    dir_left = msg.dir_left;
    para_left = msg.para_left;
    dir_right = msg.dir_right;
    para_right = msg.para_right;

    state = convert_state(dir_left,para_left,dir_right,para_right);
    printf("input value = %x\n", state);
    printf("dir_left = %d para_left = %d\n", dir_left,para_left);
    printf("dir_right = %d para_right = %d\n", dir_right,para_right);
    rc = write(fd_32, &state, 4);
    // allwrite(fd_32, (unsigned char *)&state, 4);
}

int main(int argc, char **argv)
{

  config_console();
  fd_32 = open("/dev/xillybus_write_32", O_WRONLY);
  ros::init(argc, argv, "openreroc_pwm");
  ros::NodeHandle n;
  ros::Subscriber sub_openreroc_pwm = n.subscribe("openreroc_pwm", 1000, chatterCallback);
  ros::spin();

  close(fd_32);
  return 0;
}



void config_console() {
  struct termio console_attributes;

  if (ioctl(0, TCGETA, &console_attributes) != -1) {
    // If we got here, we're reading from console

    console_attributes.c_lflag &= ~ICANON; // Turn off canonical mode
    console_attributes.c_cc[VMIN] = 1; // One character at least
    console_attributes.c_cc[VTIME] = 0; // No timeouts

    if (ioctl(0, TCSETAF, &console_attributes) == -1)
      fprintf(stderr, "Warning: Failed to set console to char-by-char\n");    
  }
}

void allwrite(int fd, unsigned char *buf, int len) {
  int sent = 0;
  int rc;

  while (sent < len) {
    rc = write(fd, buf + sent, len - sent);
  
    if ((rc < 0) && (errno == EINTR))
      continue;

    if (rc < 0) {
      perror("allwrite() failed to write");
      exit(1);
    }
  
    if (rc == 0) {
      fprintf(stderr, "Reached write EOF (?!)\n");
      exit(1);
    }
 
    sent += rc;
  }
} 

unsigned int convert_state(unsigned char dir_left,
                          unsigned int para_left,
                          unsigned char dir_right,
                          unsigned int para_right){
  unsigned int state=0, temp;

  temp = para_left << 17;
  state = state + temp;
  temp = dir_left << 16;
  state = state + temp;
  temp = para_right << 1;
  state = state + temp;
  temp = dir_left;
  state = state + temp;

  return state;

}           