#include "ros/ros.h"
#include "openreroc_pwm/openreroc_pwm.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "sample_input_openreroc_pwm");

  ros::NodeHandle n;
  ros::Publisher pub_openreroc_pwm = n.advertise<openreroc_pwm::openreroc_pwm>("openreroc_pwm", 1000);

  ros::Rate loop_rate(1);
  openreroc_pwm::openreroc_pwm msg;

  int count = 0;
  while (ros::ok())
  {

    unsigned char dir_left;
    unsigned int para_left;
    unsigned char dir_right;
    unsigned int para_right;
    printf("type parameter [dir_left] [para_left] [dir_right] [para_right]\n");
    scanf("%u %u %u %u",&dir_left, &para_left ,&dir_right, &para_right);

    msg.dir_left = dir_left;
    msg.para_left = para_left;
    msg.dir_right = dir_right;
    msg.para_right = para_right;

    pub_openreroc_pwm.publish(msg);
    ros::spinOnce();
  }
  return 0;
}