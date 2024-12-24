#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <gst/gst.h>
#include <string>

// 图像回调函数

class image_send {
public:
    image_send(std::string ip, uint16_t port, int width, int height) {
        this->ip= ip;
        this->port =port;
        image_width = width;
        image_height = height;
    }
    int init(int argc, char** argv) {
        gst_init(&argc, &argv);
        std::string pipecommand = "appsrc name=source ! videoconvert ! x264enc tune=zerolatency bitrate=3000 speed-preset=superfast intra-refresh=true key-int-max=5 ! rtph264pay ! udpsink host=" + ip + " port=" + std::to_string(port) + " name = udp";
        pipeline = gst_parse_launch(pipecommand.data(), NULL);
        source = gst_bin_get_by_name(GST_BIN(pipeline), "source");
        udpsink = gst_bin_get_by_name(GST_BIN(pipeline), "udp");
        std::cout << pipecommand << '\n';

        if (!source) {
            g_printerr ("source could be created.\n");
            return -1;
        }
        if (!pipeline) {
            g_printerr ("pipeline could be created.\n");
            return -1;
        }
        if (!udpsink) {
            g_printerr ("udpsink could be created.\n");
            return -1;
        }

        GstCaps* caps = gst_caps_new_simple("video/x-raw",
                                            "format", G_TYPE_STRING, "BGR",
                                            "width", G_TYPE_INT, image_width,
                                            "height", G_TYPE_INT, image_height,
                                            "framerate", GST_TYPE_FRACTION, 10, 1,
                                            NULL);               
        g_object_set(G_OBJECT(source), "caps", caps, NULL);  
        g_object_set(G_OBJECT(udpsink), "buffer-size", 10, NULL);    
        g_object_set (G_OBJECT(udpsink), "sync", FALSE, NULL);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);

        return 0;
    }
    int img_send(cv::Mat &image) {
        GstFlowReturn ret;
        const uint8_t* image_data = image.data;
        int image_size = image.cols * image.rows * 3;
        std::cout << image.cols << ' ' << image.rows << '\n';
        std::cout << "image_size: " << image_size << '\n';              
        GstBuffer* buffer = gst_buffer_new_allocate(NULL, image_size, NULL);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_WRITE);
        memcpy(map.data, image_data, image_size);
        gst_buffer_unmap(buffer, &map);
        if (source == NULL || buffer == NULL) {
            std::cout << "error\n";
            return -1;
        }
        g_signal_emit_by_name(source, "push-buffer", buffer, &ret);
        std::cout << "image send successfully\n";
        return 0;
    }
    ~image_send() {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
    }
private:
    std::string ip;
    uint16_t port;
    int image_width;
    int image_height;
    GstElement* pipeline;
    GstElement* source;
    GstElement* udpsink;
};


image_send IS("127.0.0.1", 12345, 752, 480);

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::RGB8);
        cv::Mat image = cv_ptr->image;
        IS.img_send(image);

    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_subscriber_rgb");
    ros::NodeHandle nh;
    ros::Subscriber sub_left = nh.subscribe("/iris_0/stereo_camera/left/image_raw", 10, imageCallback);


    IS.init(argc, argv);

    ros::spin();

    return 0;
}