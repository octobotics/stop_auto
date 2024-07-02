#include <ros/ros.h>
#include <std_srvs/Trigger.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>
#include <cstdlib>

ros::Publisher cmd_vel_pub;



bool callTriggerService(const std::string& service_name) {
    ros::ServiceClient client = ros::NodeHandle().serviceClient<std_srvs::Trigger>(service_name);
    std_srvs::Trigger srv;
    if (client.call(srv)) {
        if (srv.response.success) {
            ROS_INFO("Service %s called successfully: %s", service_name.c_str(), srv.response.message.c_str());
            return true;
        } else {
            ROS_INFO("Service %s called successfully: %s", service_name.c_str(), srv.response.message.c_str());
            return true;
        }
    } else {
        ROS_ERROR("Failed to call service %s", service_name.c_str());
        return false;
    }
}


bool triggerCallback(std_srvs::Trigger::Request& req, std_srvs::Trigger::Response& res) {
   
    if (callTriggerService("/terminate_program")) {
        res.success = true;
        res.message = "Failed to call /terminate_program service.";
        return true;
    }


    if (callTriggerService("/stop")) {
        
        std::system("rosnode kill behavior_tree_node");

        ros::Duration(1.0).sleep();
        std::system("rosnode kill lateral_shift_controller");

        ros::Duration(1.0).sleep();

        std_msgs::Int32 stop_msg;
        stop_msg.data = 0;
        cmd_vel_pub.publish(stop_msg);
        ROS_INFO("Published zero velocities to /skid_steer/cmd_vel");
        res.success = true;
        res.message = "Failed to call /stop service.";
        return true;
    }


    
    //ros::NodeHandle nh;
    //ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/skid_steer/cmd_vel", 10);
    

    res.success = true;
    res.message = "All operations completed successfully.";
    return true;
}


int main(int argc, char** argv) {
    ros::init(argc, argv, "trigger_service_node");
    ros::NodeHandle nh;

    ros::ServiceServer service = nh.advertiseService("/stop_auto", triggerCallback);
    cmd_vel_pub = nh.advertise<std_msgs::Int32>("/navigation_control", 10);
    ROS_INFO("Trigger service ready.");
    while(ros::ok())
    {
    ros::spinOnce();}
    return 0;
}
