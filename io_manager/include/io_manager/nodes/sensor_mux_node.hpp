/**
 * @file sensor_mux_node.hpp / sensor_mux_node.cpp
 * @brief Nó ROS 2 para leitura de sinais analógicos via ADS1115 com MUX.
 *
 * Este nó permite adquirir sinais analógicos de múltiplos sensores conectados
 * a um conversor ADC ADS1115, possivelmente multiplexados por um MUX controlado
 * por três pinos GPIO. Os valores lidos são publicados em um tópico ROS 2 como
 * Int32MultiArray para cada ciclo do MUX.
 *
 * Características principais:
 * - Configuração de pinos do MUX via parâmetros ROS 2 (pin_a, pin_b, pin_c)
 * - Configuração do ADS1115 via parâmetros ROS 2 (i2c_device, i2c_address)
 * - Leitura periódica dos canais 2 e 3 do ADS1115
 * - Publicação do estado do MUX e dos valores lidos em /sensor_mux/data
 * - Loop em alta frequência (~1 kHz) utilizando rclcpp::TimerBase
 *
 * Parâmetros ROS 2:
 * - pin_a/pin_b/pin_c (int): pinos GPIO usados para controle do MUX
 * - i2c_device (string): caminho do dispositivo I2C (ex: "/dev/i2c-1")
 * - i2c_address (int): endereço I2C do ADS1115
 *
 * Tópicos publicados:
 * - /sensor_mux/data (std_msgs/msg/Int32MultiArray)
 *   Conteúdo:
 *     [0] -> estado atual do MUX (0-7)
 *     [1] -> valor do canal 3 do ADS1115
 *     [2] -> valor do canal 2 do ADS1115
 *
 * Exemplo de uso:
 * - Configurar parâmetros via YAML ou launch file
 * - O nó alterna os estados do MUX e lê os canais do ADS1115 periodicamente
 * - Os valores são publicados continuamente no tópico /sensor_mux/data
 *
 * Observações:
 * - Implementa leitura de canais críticos para suspensão, freio e direção
 * - Mapas de interpretação para canais podem ser modificados em map_ch3_ e map_ch2_
 */

#pragma once

#include <memory>
#include <array>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32_multi_array.hpp"

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

    rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    uint8_t mux_state_;

    std::array<std::string, 8> map_ch3_;
    std::array<std::string, 8> map_ch2_;
};