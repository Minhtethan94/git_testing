/**
 * Copyright 2019 by Institute for Infocomm Research, Singapore (I2R). All rights reserved.
 * @author Ng Kam Pheng (ngkp@i2r.a-star.edu.sg)
 * 
 * This is a sample source for sending custom status data to the command centre.
 * The custom status data will be part of the status data that is sent regularly
 * to the command centre. Using this custom status data, you can send back your
 * own custom status data for processing at the command centre using your own
 * plugin to decode your custom data.
 *
 * The corresponding GUI plugin that interacts with this node is example_mrccc_custom_status_data
 */

// I modified this for 2nd version
// To check the git diff

// For fifth version

#include <ros/ros.h>
#include <jsoncpp/json/value.h>
#include "mrccc_custom_status_data_server/RegisterData.h"
#include "mrccc_custom_status_data_server/UpdateData.h"

// name of service
#define K_SERVICE_NAME_REGISTER_DATA  "mrccc_custom_status_data_server/register_data"

/// name of this ROS node
const std::string kROSNodeName("example_mrccc_custom_status_data_node");

class MyCustomStatusData
{
public:
    /// Class constructor
    MyCustomStatusData()
        : m_test_count(0)
    {
        m_nh_global = ros::NodeHandle();
        m_nh_private = ros::NodeHandle("~");

        // wait for service
        ROS_INFO_STREAM(ros::this_node::getName() << " - waiting for service " << K_SERVICE_NAME_REGISTER_DATA);
        ros::service::waitForService(K_SERVICE_NAME_REGISTER_DATA);
    }

    /// Initialises the system
    void init()
    {
        // register for service
        InitROSServices();

        // publisher to custom status data
        m_ros_pub_update_data = m_nh_global.advertise<mrccc_custom_status_data_server::UpdateData>("mrccc_custom_status_data_server/update_data", 10);

        // timer for publishing our custom status data
        m_timer = m_nh_private.createTimer(ros::Duration(1.0f),
                                           &MyCustomStatusData::OnTimerCallback,
                                           this,
                                           false);
    }

private:
    /// Registers our data to the service server
    void InitROSServices()
    {
        ros::ServiceClient srv_register_data = m_nh_global.serviceClient<mrccc_custom_status_data_server::RegisterData>(K_SERVICE_NAME_REGISTER_DATA);
        mrccc_custom_status_data_server::RegisterData register_data;
        register_data.request.name = kROSNodeName; // unique name to identify us
        if (srv_register_data.call(register_data))
        {
            ROS_INFO_STREAM(ros::this_node::getName() << " - successfully registered to service");
        }
        else
        {
            ROS_ERROR_STREAM(ros::this_node::getName() << " - failed to register to the service " << K_SERVICE_NAME_REGISTER_DATA);
        }
    }

    /**
     * Periodic callback to send our custom status data
     * @param e Not used
     */
    void OnTimerCallback(const ros::TimerEvent &e)
    {
        Json::Value jval_all_status;

        // sample field data, structure and data is up to you to define.
        // on command centre side, you just have to decode the data/structure yourself
        jval_all_status["sample_field_str"] = "teststr";
        jval_all_status["sample_field_int"] = ++m_test_count;

        // send status data to the mrccc_custom_status_data server
        mrccc_custom_status_data_server::UpdateData update_msg;
        update_msg.name = kROSNodeName;
        update_msg.jsonstr = jval_all_status.toStyledString();
        m_ros_pub_update_data.publish(update_msg);
    }

    /// test count for demo
    int m_test_count;

    /// global node handle
    ros::NodeHandle m_nh_global;

    /// private node handle
    ros::NodeHandle m_nh_private;

    /// ros publisher for publishing to custom status data
    ros::Publisher m_ros_pub_update_data;

    /// timer for publishing our custom status data
    ros::Timer m_timer;
};

/**
 * Main entry of application
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @return always return 0
 */
int main(int argc, char *argv[])
{
    // initialise ROS and start it
    ros::init(argc, argv, kROSNodeName);

    // create the custom status data handler
    MyCustomStatusData csd;
    csd.init();

    ros::spin();
    return 0;
}
