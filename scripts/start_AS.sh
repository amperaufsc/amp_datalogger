#!/bin/bash

echo "Starting AS system"

set -e

BAG_NAME="$1"

echo "Carregando workspace..."
cd ~/ws
source install/setup.bash

cleanup() {
    echo ""
    echo "Encerrando processos..."

    kill $FOXGLOVE_PID 2>/dev/null || true
    kill $VN_PID 2>/dev/null || true
    kill $CAN_PID 2>/dev/null || true
    kill $SENSOR_PID 2>/dev/null || true

    wait $FOXGLOVE_PID 2>/dev/null || true
    wait $VN_PID 2>/dev/null || true
    wait $CAN_PID 2>/dev/null || true
    wait $SENSOR_PID 2>/dev/null || true

    echo "Finalizado."
}

trap cleanup EXIT INT TERM

echo "Iniciando Foxglove Bridge..."
ros2 launch foxglove_bridge foxglove_bridge_launch.xml &
FOXGLOVE_PID=$!

sleep 5

echo "Iniciando VectorNav..."
ros2 launch vectornav vectornav.launch.py &
VN_PID=$!

sleep 5

echo "Iniciando CAN Receiver..."
ros2 run can_driver can_receiver_node &
CAN_PID=$!

sleep 2

echo "Iniciando Sensor Debug Node..."
ros2 run io_manager sensor_debug_node &
SENSOR_PID=$!