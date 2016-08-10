/**
 * @file offb_node.cpp
 * @brief offboard example node, written with mavros version 0.14.2, px4 flight
 * stack and tested in Gazebo SITL
 */

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>

#include "offboard_control.h"

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
	current_state = *msg;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "offb_node");
	ros::NodeHandle nh;

	ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, state_cb);
	ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10);
	ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
	ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");

	//the setpoint publishing rate MUST be faster than 2Hz
	ros::Rate rate(20.0);


	// Ensure that gazebo is
	ros::service::waitForService("/gazebo/set_model_state");


	// wait for FCU connection
	while(ros::ok() && current_state.connected){
		ros::spinOnce();
		rate.sleep();
	}

	ROS_INFO("Starting...");


	geometry_msgs::PoseStamped pose;
	pose.pose.position.x = 0;
	pose.pose.position.y = 0;
	pose.pose.position.z = 1;

	//send a few setpoints before starting
	for(int i = 100; ros::ok() && i > 0; --i){
		local_pos_pub.publish(pose);
		ros::spinOnce();
		rate.sleep();
	}

	ROS_INFO("Switching mode");

	mavros_msgs::SetMode offb_set_mode;
	offb_set_mode.request.custom_mode = "OFFBOARD";

	mavros_msgs::CommandBool arm_cmd;
	arm_cmd.request.value = true;

	ros::Time last_request = ros::Time::now();

	float angle = 0.0;

	testsetup::OffboardControl obc(nh);

	while(ros::ok()){
		if(current_state.mode != "OFFBOARD" && (ros::Time::now() - last_request > ros::Duration(1.0))){
			if(set_mode_client.call(offb_set_mode) && offb_set_mode.response.success){
				ROS_INFO("Offboard enabled");
			}
			last_request = ros::Time::now();
		} else {
			if(!current_state.armed && (ros::Time::now() - last_request > ros::Duration(1.0))){
				if(arming_client.call(arm_cmd) && arm_cmd.response.success){
					ROS_INFO("Vehicle armed");
				}
				last_request = ros::Time::now();
			}
		}


		if(current_state.armed){

			break;
			//pose.pose.position.x = 1*cosf(angle) - 1;
			//pose.pose.position.y = 1*sinf(angle);
			//angle += 0.01;
		}


		local_pos_pub.publish(pose);

		ros::spinOnce();
		rate.sleep();
	}



	ros::Rate loop_rate(20);
	obc.mode = testsetup::POSITION;


//	geometry_msgs::PoseStamped ps;
//	tf::pointEigenToMsg(pos_setpoint(1, 1, 1), ps.pose.position);
//	obc.wait_and_move(ps);


	obc.square_path_motion(loop_rate, testsetup::POSITION);



	return 0;
}
