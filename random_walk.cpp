#include "ros/ros.h"
#include "std_msgs/String.h"

#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"

#include <time.h>

static ros::Publisher chatter_pub;

static const int sensor_left_start = 150;
static const int sensor_left_end = 541/2;
static const int sensor_right_start = 541/2+1;
static const int sensor_right_end = 541-150;

static const float run_speed = 0.2;
static const float turn_speed = 1.;

static const float min_distance = 0.5;

static int max_count = 25*3;

static bool dir = 0;

static int turn_time = 15;
static int counter = 0;

void chatterCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
// 	for(int i = 0;i< msg->points.size();i++){
// 		ROS_INFO("msg->points x:%d, y:%d, z:%d", msg->points[i].x,msg->points[i].y,msg->points[i].z);
// 	}
	
	bool obstacle_left = 0;
	bool obstacle_right = 0;
	
	for(int i = sensor_left_start; i < sensor_left_end; i++){
		if(msg->ranges[i] < min_distance){
			obstacle_left = 1;
			ROS_INFO("LEFT");
			counter = 0;
			break;
		}
	}
	for(int i = sensor_right_start; i < sensor_right_end; i++){
		if(msg->ranges[i] < min_distance){
			obstacle_right = 1;
			ROS_INFO("RIGHT");
			counter = 0;
			break;
		}
	}
	if (obstacle_left == 0 && obstacle_right == 0) {
	    ROS_INFO("FORWARD");
	    counter++;
	}
	
	
	//ROS_INFO("obstacle_left:%d obstacle_right%d",obstacle_left,obstacle_right);
	
	geometry_msgs::Twist msg_return = geometry_msgs::Twist();//= {linear:  {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}
	
	if(obstacle_left){
		if(obstacle_right){
			msg_return.angular.z = -turn_speed; 
		}else{
			msg_return.angular.z = turn_speed;
		}
	}else if(obstacle_right){
		msg_return.angular.z = -turn_speed;
	}else{
	  if(counter < max_count ){
		  msg_return.linear.x = run_speed;
		}else{
		  if(counter < max_count+turn_time){
		    msg_return.angular.z = dir ? turn_speed : -turn_speed;
		  }else{
		    msg_return.linear.x = run_speed;
		    dir = rand() % 2;
		    max_count = rand() % (3*25) + 1*25;
		    turn_time = rand() % (1*25) + 5;
		    counter = 0;
		  }
		}
	}
	
	chatter_pub.publish(msg_return);
	
}

int main(int argc, char **argv)
{
  srand(time(NULL));

  ros::init(argc, argv, "random_walker");

  ros::NodeHandle n;
	ROS_INFO("Starting");
  chatter_pub = n.advertise<geometry_msgs::Twist>("/RosAria/cmd_vel", 1000);
  ros::Subscriber sub = n.subscribe("/scan", 1000, chatterCallback);
	ROS_INFO("Running");
  ros::spin();
 


  return 0;
}
