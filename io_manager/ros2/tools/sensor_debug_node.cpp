#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "io_manager/drivers/mux.hpp"
#include "io_manager/drivers/ads1115.hpp"

class SensorDebugNode : public rclcpp::Node
{
public:
    SensorDebugNode()
    : Node("sensor_debug_node")
    {
        declare_parameter("frequency", 100.0);

        declare_parameter("mux_channel", 5);
        declare_parameter("ads_channel", 3);

        declare_parameter("pin_a", 17);
        declare_parameter("pin_b", 27);
        declare_parameter("pin_c", 22);

        declare_parameter("i2c_device", "/dev/i2c-1");
        declare_parameter("i2c_address", 0x48);

        double frequency = get_parameter("frequency").as_double();
        mux_channel_ = get_parameter("mux_channel").as_int();
        ads_channel_ = get_parameter("ads_channel").as_int();

        int pin_a = get_parameter("pin_a").as_int();
        int pin_b = get_parameter("pin_b").as_int();
        int pin_c = get_parameter("pin_c").as_int();

        std::string device = get_parameter("i2c_device").as_string();
        int address = get_parameter("i2c_address").as_int();

        mux_ = std::make_shared<Mux>(pin_a, pin_b, pin_c);

        if (!mux_->init())
        {
            throw std::runtime_error("Failed to initialize MUX");
        }

        ads_ = std::make_shared<Ads1115>(device, address, 0);

        pub_ = create_publisher<std_msgs::msg::Float64>(
            "/sensor_steering", 10);

        auto period = std::chrono::microseconds(
            static_cast<int>(1e6 / frequency));

        timer_ = create_wall_timer(
            period,
            std::bind(&SensorDebugNode::loop, this));

        RCLCPP_INFO(get_logger(),
                    "Reading ADS channel %d, MUX channel %d",
                    ads_channel_,
                    mux_channel_);
    }

private:
    void loop()
    {
        mux_->set(mux_channel_);

        usleep(500);

        int value = ads_->read_channel(ads_channel_);

        std_msgs::msg::Float64 msg;
        msg.data = value;

        pub_->publish(msg);
    }

    int mux_channel_;
    int ads_channel_;

    std::shared_ptr<Mux> mux_;
    std::shared_ptr<Ads1115> ads_;

    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorDebugNode>());
    rclcpp::shutdown();
    return 0;
}
