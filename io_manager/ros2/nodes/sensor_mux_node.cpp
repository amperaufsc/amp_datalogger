#include "io_manager/nodes/sensor_mux_node.hpp"

SensorMuxNode::SensorMuxNode()
: Node("sensor_mux_node"), mux_state_(0)
{
    // ===== Parâmetros ROS2 =====
    this->declare_parameter<int>("pin_a", 17);
    this->declare_parameter<int>("pin_b", 27);
    this->declare_parameter<int>("pin_c", 22);

    this->declare_parameter<std::string>("i2c_device", "/dev/i2c-1");
    this->declare_parameter<int>("i2c_address", 0x48);

    int pin_a = this->get_parameter("pin_a").as_int();
    int pin_b = this->get_parameter("pin_b").as_int();
    int pin_c = this->get_parameter("pin_c").as_int();
    std::string i2c_device = this->get_parameter("i2c_device").as_string();
    int i2c_addr = this->get_parameter("i2c_address").as_int();

    // ===== Inicializa MUX =====
    mux_ = std::make_shared<Mux>(pin_a, pin_b, pin_c);
    if (!mux_->init()) {
        throw std::runtime_error("Falha ao inicializar MUX");
    }

    // ===== Inicializa ADS1115 =====
    ads_ = std::make_shared<Ads1115>(i2c_device, i2c_addr, 0);

    // ===== Mapas de interpretação =====
    map_ch3_ = { "brake_0", "brake_1", "brake_2", "brake_3",
                 "susp_fl", "susp_fr", "susp_rl", "susp_rr" };

    map_ch2_ = { "", "", "", "",
                 "steering", "extra_0", "extra_1", "extra_2" };

    // ===== Publisher ROS2 =====
    rclcpp::QoS qos(10);
    pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("sensor_mux/data", qos);

    // ===== Timer de loop (~1 kHz) =====
    timer_ = this->create_wall_timer(
        std::chrono::microseconds(1000),
        std::bind(&SensorMuxNode::loop, this)
    );
}

void SensorMuxNode::loop()
{
    // Alterna canal do MUX
    mux_->set(mux_state_);

    // Lê canais do ADS1115
    int ch3_value = ads_->read_channel(3);
    int ch2_value = ads_->read_channel(2);

    // Publica valores
    std_msgs::msg::Int32MultiArray msg;
    msg.data.push_back(mux_state_);
    msg.data.push_back(ch3_value);
    msg.data.push_back(ch2_value);

    pub_->publish(msg);

    // Próximo estado do MUX
    mux_state_ = (mux_state_ + 1) % 8;
}

// ===== Main =====
int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SensorMuxNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}