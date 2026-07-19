#!/bin/bash

echo "Starting datalogger"

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
    kill $MUX_PID 2>/dev/null || true
    kill $PRESSURE_PID 2>/dev/null || true

    wait $FOXGLOVE_PID 2>/dev/null || true
    wait $VN_PID 2>/dev/null || true
    wait $CAN_PID 2>/dev/null || true
    wait $MUX_PID 2>/dev/null || true
    wait $PRESSURE_PID 2>/dev/null || true

    echo "Finalizado."
}

trap cleanup EXIT INT TERM

echo "Iniciando VectorNav..."
ros2 launch vectornav vectornav.launch.py &
VN_PID=$!

sleep 5

echo "Iniciando CAN Receiver..."
ros2 run can_driver can_receiver_node &
CAN_PID=$!

sleep 2

echo "Iniciando Sensor Mux..."
ros2 launch io_manager sensor_mux_launch.py &
MUX_PID=$!

sleep 2

echo "Iniciando Pressure Measurement..."
ros2 run datalogger pressure_measurement_node &
PRESSURE_PID=$!

sleep 2

BAG_ROOT=~/bags

DAY_FOLDER=$(date +"%Y_%m_%d")

mkdir -p "$BAG_ROOT/$DAY_FOLDER"

if [ -z "$BAG_NAME" ]; then
    BAG_NAME=$(date +"%H-%M-%S")
fi

BAG_PATH="$BAG_ROOT/$DAY_FOLDER/$BAG_NAME"

echo "Gravando bag em: $BAG_PATH"

ros2 bag record -a \
    --storage mcap \
    -o "$BAG_PATH" \
    --max-bag-duration 30