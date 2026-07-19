/**
 * @file sensor_mux_node.hpp
 * @brief Nó ROS 2 para aquisição de sinais analógicos utilizando ADS1115 e multiplexador.
 *
 * Este nó realiza a leitura de até quatro canais do ADC ADS1115 enquanto
 * alterna sequencialmente os estados de um multiplexador controlado por
 * três GPIOs. A cada ciclo, os valores adquiridos são publicados através
 * da mensagem SensorMuxFrame.
 *
 * Funcionalidades:
 * - Configuração dos pinos de seleção do multiplexador via parâmetros ROS 2
 * - Configuração da interface I2C e endereço do ADS1115
 * - Leitura dos quatro canais do ADS1115
 * - Associação de nomes aos sensores conforme o estado atual do multiplexador
 * - Publicação dos dados no tópico sensor_mux/data
 * - Operação periódica em frequência configurável
 *
 * Parâmetros ROS 2:
 * - frequency (int): frequência de execução do nó em Hz
 * - pin_a (int): GPIO A do multiplexador
 * - pin_b (int): GPIO B do multiplexador
 * - pin_c (int): GPIO C do multiplexador
 * - i2c_device (string): dispositivo I2C (ex.: "/dev/i2c-1")
 * - i2c_address (int): endereço I2C do ADS1115
 *
 * Tópico publicado:
 * - sensor_mux/data (manager_msgs/msg/SensorMuxFrame)
 *
 * Estrutura da mensagem:
 * - header: timestamp e frame de referência
 * - valid: indica validade da aquisição
 * - mux_state: estado atual do multiplexador (0 a 7)
 * - channel_X_name: identificação lógica do sensor conectado ao canal
 * - channel_X_raw: valor bruto lido pelo ADS1115
 *
 * Mapeamento atual:
 * - Canal 2:
 *   - Estado 4 -> steering
 *   - Estado 5 -> extra_0
 *   - Estado 6 -> extra_1
 *   - Estado 7 -> extra_2
 *
 * - Canal 3:
 *   - Estados 0 a 3 -> brake_0 a brake_3
 *   - Estado 4 -> susp_fl
 *   - Estado 5 -> susp_fr
 *   - Estado 6 -> susp_rl
 *   - Estado 7 -> susp_rr
 */

#pragma once

#include <memory>
#include <array>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "io_manager/drivers/ads1115.hpp"
#include "io_manager/drivers/mux.hpp"

class SensorMuxNode : public rclcpp::Node
{
public:
    SensorMuxNode();

private:
    void loop();

    std::shared_ptr<Mux> mux_;
    std::shared_ptr<Ads1115> ads_;

    rclcpp::TimerBase::SharedPtr timer_;

    uint8_t mux_state_;

    bool enable_ch_[4];

    std::array<std::string, 8> map_ch0_;
    std::array<std::string, 8> map_ch1_;
    std::array<std::string, 8> map_ch2_;
    std::array<std::string, 8> map_ch3_;

    std::map<std::string, rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr> pubs_;
};