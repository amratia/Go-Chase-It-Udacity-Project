#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

//--------------------------------------------------------------------------------------------------------------------------
// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}
//--------------------------------------------------------------------------------------------------------------------------
// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int ball_is_found = 0; // initially the ball is not found
    int image_size = img.height * img.step;

    for (int i = 0 ; i < image_size ; i=i+3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            ball_is_found = 1; //ball is found
            int position = i % img.step; // this is the position of the ball horizontally in the image frame 
            if (position <= img.step/3){
             drive_robot(0.5, 1.0);
             }
            else if (position > img.step/3 && position <= (img.step/3)*2){
             drive_robot(0.5, 0.0);
             }
            else if (position > (img.step/3)*2){
             drive_robot(0.5, -1.0);
             }
            break;
        }
    }
    if (ball_is_found == 0){ //ball is not found
     drive_robot(0, 0);
     }
}
//--------------------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
