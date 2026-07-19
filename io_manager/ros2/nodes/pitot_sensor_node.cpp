/**
 * @file pitot_sensor_node.cpp
 *
 * @brief ROS2 node for MS4525DO differential pressure sensor.
 *
 * This node performs:
 *  - Periodic acquisition of differential pressure data
 *  - Basic validation of sensor status
 *  - Publication of differential pressure values
 *
 * The node does not calculate derived physical quantities such as airspeed.
 *
 * Published topics:
 *  /pitot/differential_pressure
 *
 * Message type:
 *  std_msgs/msg/Float64
 */


#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "io_manager/drivers/ms4525do.hpp"


using namespace std::chrono_literals;


class PitotSensorNode : public rclcpp::Node
{

public:

    PitotSensorNode()
    : Node("pitot_sensor_node")
    {

        this->declare_parameter<int>("i2c_bus", 1);
        this->declare_parameter<int>("i2c_address", 0x28);

        this->declare_parameter<double>(
            "pressure_min",
            -1000.0
        );

        this->declare_parameter<double>(
            "pressure_max",
            1000.0
        );

        this->declare_parameter<double>(
            "frequency",
            100.0
        );


        int bus =
            this->get_parameter("i2c_bus").as_int();

        int address =
            this->get_parameter("i2c_address").as_int();


        double pressure_min =
            this->get_parameter("pressure_min").as_double();

        double pressure_max =
            this->get_parameter("pressure_max").as_double();


        double frequency =
            this->get_parameter("frequency").as_double();



        sensor_ = std::make_unique<MS4525DO>(
            bus,
            address,
            pressure_min,
            pressure_max
        );


        if (!sensor_->initialize())
        {
            RCLCPP_FATAL(
                this->get_logger(),
                "Failed to initialize MS4525DO"
            );

            throw std::runtime_error(
                "MS4525DO initialization failed"
            );
        }


        publisher_ =
            this->create_publisher<std_msgs::msg::Float64>(
                "/pitot/differential_pressure",
                10
            );


        auto period =
            std::chrono::duration<double>(
                1.0 / frequency
            );


        timer_ =
            this->create_wall_timer(
                period,
                std::bind(
                    &PitotSensorNode::read_sensor,
                    this
                )
            );


        RCLCPP_INFO(
            this->get_logger(),
            "Pitot sensor node started"
        );
    }


private:


    void read_sensor()
    {

        auto measurement =
            sensor_->read();


        if (!measurement.valid)
        {

            switch(measurement.status)
            {

            case MS4525DO::Status::I2C_ERROR:

                RCLCPP_ERROR_THROTTLE(
                    this->get_logger(),
                    *this->get_clock(),
                    2000,
                    "I2C communication error"
                );

                break;


            case MS4525DO::Status::STALE_DATA:

                RCLCPP_WARN_THROTTLE(
                    this->get_logger(),
                    *this->get_clock(),
                    2000,
                    "Sensor returned stale data"
                );

                break;


            case MS4525DO::Status::DIAGNOSTIC_FAULT:

                RCLCPP_ERROR_THROTTLE(
                    this->get_logger(),
                    *this->get_clock(),
                    2000,
                    "Sensor diagnostic fault"
                );

                break;


            default:
                break;
            }


            return;
        }



        std_msgs::msg::Float64 msg;

        msg.data =
            measurement.differential_pressure_pa;


        publisher_->publish(msg);
    }



    std::unique_ptr<MS4525DO> sensor_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;

    rclcpp::TimerBase::SharedPtr timer_;

};


int main(int argc, char **argv)
{

    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<PitotSensorNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}