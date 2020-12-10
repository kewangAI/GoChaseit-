#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float linear_x, float angular_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = linear_x;
    srv.request.angular_z = angular_z;

    // Call the command_robot service and pass the requested motor commands
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    float lin_array[] = {0.1, 0.2, 0.1};
    float ang_array[] = {0.1, 0.0, -0.1};
    int white_pixel = 255;
 
    bool found_ball = false;
    
    int row = 0;
    int step = 0;
    int i = 0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (row = 0; row < img.height && found_ball == false; row++)
    {
        for (step = 0; step < img.step && found_ball == false; ++step)
        {   
            i = (row*img.step)+step;
            //ROS_INFO("row: %d, step: %d, i: %d", row, step, i);
            if (img.data[i] == white_pixel)
            {   
                found_ball = true;
                
            }
		}
    }
    
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    if (found_ball)
    {
        // Then, identify if this pixel falls in the left, mid, or right side of the image
        int turn_index = std::min(int(step/img.width), 2);
        // Depending on the white ball position, call the drive_bot function and pass velocities to it
        drive_robot(lin_array[turn_index],ang_array[turn_index]);
        
    }
    // Request a stop when there's no white ball seen by the camera
  	else 
    {
        drive_robot(0.0, 0.0);
        //ROS_INFO("Not found, stop!");
    }

}

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