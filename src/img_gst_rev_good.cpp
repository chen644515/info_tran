#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <gst/gst.h>

class image_rev {
public:
    image_rev(uint16_t port) {
        this->port = port;
    }
    int init() {
        pipecommand = "udpsrc port=" + std::to_string(port) +" ! application/x-rtp,media=video,encoding-name=H264 ! rtph264depay ! queue ! h264parse ! avdec_h264 ! videoconvert ! appsink name=appsink";
        std::cout << pipecommand << '\n';
        cap_raw = cv::VideoCapture(pipecommand, cv::CAP_GSTREAMER);
        if(!cap_raw.isOpened()) {
            std::cout << "open failed" << '\n';
            return -1;
        }
        return 0;
    }
    cv::Mat img_rev() {
        cv::Mat frame_raw;
        cap_raw >> frame_raw;
        return frame_raw;
    }
    ~image_rev() {
        cap_raw.release();//释放资源
    }
private:
    uint16_t port;
    cv::VideoCapture cap_raw;
    std::string pipecommand;
    cv::Mat image;
};


int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_publisher_rgb");
    ros::NodeHandle nh;
    ros::Publisher image_pub_right = nh.advertise<sensor_msgs::Image>("/camera/color/image", 10);
    image_rev IR(12345);
    IR.init();
    cv::Mat image;
    int count = 0;
    while (ros::ok()) {
        image = IR.img_rev();

        cv_bridge::CvImage cv_image_rgb;
        cv_image_rgb.image = image;
        cv_image_rgb.encoding = "rgb8";
        sensor_msgs::Image ros_image_rgb;
        cv_image_rgb.toImageMsg(ros_image_rgb);
        ros_image_rgb.header.frame_id = "camera_color_optical_frame_right";
        ros_image_rgb.header.seq = count;
        ros_image_rgb.header.stamp = ros::Time::now(); 
        image_pub_right.publish(ros_image_rgb);

        count++;
    }
    return 0;
}