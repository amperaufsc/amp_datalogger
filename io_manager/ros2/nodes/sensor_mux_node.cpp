#include "io_manager/nodes/sensor_mux_node.hpp"

SensorMuxNode::SensorMuxNode() : Node("sensor_mux_node"), mux_state_(0)
{
    this->declare_parameter<int>("frequency", 1000);

    this->declare_parameter<int>("pin_a", 17);
    this->declare_parameter<int>("pin_b", 27);
    this->declare_parameter<int>("pin_c", 22);

    this->declare_parameter<std::string>("i2c_device", "/dev/i2c-1");
    this->declare_parameter<int>("i2c_address", 0x48);

    int frequency = get_parameter("frequency").as_int();

    int pin_a = this->get_parameter("pin_a").as_int();
    int pin_b = this->get_parameter("pin_b").as_int();
    int pin_c = this->get_parameter("pin_c").as_int();

    std::string i2c_device = this->get_parameter("i2c_device").as_string();
    int i2c_addr = this->get_parameter("i2c_address").as_int();

    mux_ = std::make_shared<Mux>(pin_a, pin_b, pin_c);

    if (!mux_->init()) {
        throw std::runtime_error("Falha ao inicializar MUX");
    }

    if (frequency <= 0) {
        throw std::runtime_error("frequency must be greater than zero");
    }

    RCLCPP_INFO( this->get_logger(),"SensorMuxNode started at %d Hz", frequency);

    ads_ = std::make_shared<Ads1115>(i2c_device, i2c_addr, 0);

    map_ch0_.fill("");
    map_ch1_.fill("");

    map_ch2_ = {
        "",
        "",
        "",
        "",
        "steering",
        "extra_0",
        "extra_1",
        "extra_2"
    };

    map_ch3_ = {
        "brake_0",
        "brake_1",
        "brake_2",
        "brake_3",
        "susp_fl",
        "susp_fr",
        "susp_rl",
        "susp_rr"
    };

    rclcpp::QoS qos(10);

    pub_ = this->create_publisher<io_manager_msgs::msg::SensorMuxFrame>("sensor_mux/data", qos);

    timer_ = this->create_wall_timer( std::chrono::microseconds(static_cast<int>(1000000.0 / frequency)), std::bind(&SensorMuxNode::loop, this));
}

void SensorMuxNode::loop()
{
    mux_->set(mux_state_);

    int ch0_value = ads_->read_channel(0);
    int ch1_value = ads_->read_channel(1);
    int ch2_value = ads_->read_channel(2);
    int ch3_value = ads_->read_channel(3);

    io_manager_msgs::msg::SensorMuxFrame msg;

    msg.header.stamp = this->now();
    msg.header.frame_id = "sensor_mux";

    msg.valid = true;
    msg.mux_state = mux_state_;

    msg.channel_0_name = map_ch0_[mux_state_];
    msg.channel_0_raw = ch0_value;

    msg.channel_1_name = map_ch1_[mux_state_];
    msg.channel_1_raw = ch1_value;

    msg.channel_2_name = map_ch2_[mux_state_];
    msg.channel_2_raw = ch2_value;

    msg.channel_3_name = map_ch3_[mux_state_];
    msg.channel_3_raw = ch3_value;

    pub_->publish(msg);

    mux_state_ = (mux_state_ + 1) % 8;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SensorMuxNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}