#include "io_manager/nodes/sensor_mux_node.hpp"

using std::placeholders::_1;

SensorMuxNode::SensorMuxNode() : Node("sensor_mux_node"), mux_state_(0)
{
    this->declare_parameter<int>("frequency", 1000);

    this->declare_parameter<int>("pin_a", 17);
    this->declare_parameter<int>("pin_b", 27);
    this->declare_parameter<int>("pin_c", 22);

    this->declare_parameter<std::string>("i2c_device", "/dev/i2c-1");
    this->declare_parameter<int>("i2c_address", 0x48);

    this->declare_parameter<bool>("enable_channel_0", true);
    this->declare_parameter<bool>("enable_channel_1", true);
    this->declare_parameter<bool>("enable_channel_2", true);
    this->declare_parameter<bool>("enable_channel_3", true);

    this->declare_parameter<std::vector<std::string>>(
        "map_ch0",
        std::vector<std::string>(8, "")
    );

    this->declare_parameter<std::vector<std::string>>(
        "map_ch1",
        std::vector<std::string>(8, "")
    );

    this->declare_parameter<std::vector<std::string>>(
        "map_ch2",
        std::vector<std::string>(8, "")
    );

    this->declare_parameter<std::vector<std::string>>(
        "map_ch3",
        std::vector<std::string>(8, "")
    );

    int frequency = this->get_parameter("frequency").as_int();

    int pin_a = this->get_parameter("pin_a").as_int();
    int pin_b = this->get_parameter("pin_b").as_int();
    int pin_c = this->get_parameter("pin_c").as_int();

    std::string i2c_device = this->get_parameter("i2c_device").as_string();
    int i2c_addr = this->get_parameter("i2c_address").as_int();

    enable_ch_[0] = this->get_parameter("enable_channel_0").as_bool();
    enable_ch_[1] = this->get_parameter("enable_channel_1").as_bool();
    enable_ch_[2] = this->get_parameter("enable_channel_2").as_bool();
    enable_ch_[3] = this->get_parameter("enable_channel_3").as_bool();

    std::vector<std::string> dbg;

    this->get_parameter("map_ch3", dbg);

    RCLCPP_INFO(this->get_logger(),
                "map_ch3 size = %ld",
                dbg.size());

    for (auto &s : dbg)
    {
        RCLCPP_INFO(this->get_logger(), "item = '%s'", s.c_str());
    }

    mux_ = std::make_shared<Mux>(pin_a, pin_b, pin_c);

    if (!mux_->init()) {
        throw std::runtime_error("Falha ao inicializar MUX");
    }

    ads_ = std::make_shared<Ads1115>(i2c_device, i2c_addr, 0);

    auto load_map = [&](const std::string &key, std::array<std::string, 8> &map_out)
    {
        std::vector<std::string> tmp;
        this->get_parameter(key, tmp);

        for (size_t i = 0; i < 8; i++)
            map_out[i] = (i < tmp.size()) ? tmp[i] : "";
    };

    load_map("map_ch0", map_ch0_);
    load_map("map_ch1", map_ch1_);
    load_map("map_ch2", map_ch2_);
    load_map("map_ch3", map_ch3_);

    RCLCPP_INFO(this->get_logger(), "map_ch2[4]=%s", map_ch2_[4].c_str());
    RCLCPP_INFO(this->get_logger(), "map_ch3[0]=%s", map_ch3_[0].c_str());

    RCLCPP_INFO(this->get_logger(),
    "enable_channel_0=%d enable_channel_1=%d enable_channel_2=%d enable_channel_3=%d",
    enable_ch_[0],
    enable_ch_[1],
    enable_ch_[2],
    enable_ch_[3]);

    auto register_sensor = [&](const std::string &name)
    {
        if (name.empty()) return;

        std::string topic = "/sensor_mux/" + name;

        if (pubs_.find(name) == pubs_.end())
        {
            pubs_[name] =
                this->create_publisher<std_msgs::msg::Float64>(topic, 10);
        }
    };

    for (auto &n : map_ch0_) register_sensor(n);
    for (auto &n : map_ch1_) register_sensor(n);
    for (auto &n : map_ch2_) register_sensor(n);
    for (auto &n : map_ch3_) register_sensor(n);

    if (frequency <= 0) {
        throw std::runtime_error("frequency must be > 0");
    }

    auto period = std::chrono::microseconds(
        static_cast<int>(1000000.0 / frequency)
    );

    timer_ = this->create_wall_timer(
        period,
        std::bind(&SensorMuxNode::loop, this)
    );

    RCLCPP_INFO(this->get_logger(),
        "SensorMuxNode running at %d Hz (dynamic topics mode)",
        frequency);
}

void SensorMuxNode::loop()
{
    using Clock = std::chrono::steady_clock;
    using us = std::chrono::microseconds;

    auto t0 = Clock::now();

    auto t_mux0 = Clock::now();
    mux_->set(mux_state_);
    auto t_mux1 = Clock::now();

    auto t_sleep0 = Clock::now();
    usleep(500);
    auto t_sleep1 = Clock::now();

    auto t_stamp0 = Clock::now();
    auto stamp = this->get_clock()->now();
    auto t_stamp1 = Clock::now();

    auto t_ads0 = Clock::now();

    //int ch0 = ads_->read_channel(0);
    //int ch1 = ads_->read_channel(1);
    //int ch2 = ads_->read_channel(2);
    int ch3 = ads_->read_channel(3);

    auto t_ads1 = Clock::now();

    auto t_pub0 = Clock::now();

    auto publish = [&](const std::string &name, double value)
    {
        if (name.empty())
            return;

        auto it = pubs_.find(name);
        if (it == pubs_.end())
            return;

        std_msgs::msg::Float64 msg;
        msg.data = value;

        it->second->publish(msg);
    };

    //if (enable_ch_[2])
    //    publish(map_ch2_[mux_state_], ch2);

    if (enable_ch_[3])
        publish(map_ch3_[mux_state_], ch3);

    auto t_pub1 = Clock::now();

    auto t_state0 = Clock::now();

    mux_state_ = (mux_state_ + 1) % 8;

    auto t_state1 = Clock::now();

    auto t_end = Clock::now();

    std::cout
        << "MUX: "
        << std::chrono::duration_cast<us>(t_mux1 - t_mux0).count()
        << " us | Sleep: "
        << std::chrono::duration_cast<us>(t_sleep1 - t_sleep0).count()
        << " us | Stamp: "
        << std::chrono::duration_cast<us>(t_stamp1 - t_stamp0).count()
        << " us | ADS1115: "
        << std::chrono::duration_cast<us>(t_ads1 - t_ads0).count()
        << " us | Publish: "
        << std::chrono::duration_cast<us>(t_pub1 - t_pub0).count()
        << " us | State: "
        << std::chrono::duration_cast<us>(t_state1 - t_state0).count()
        << " us | TOTAL: "
        << std::chrono::duration_cast<us>(t_end - t0).count()
        << " us"
        << std::endl;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorMuxNode>());
    rclcpp::shutdown();
    return 0;
}