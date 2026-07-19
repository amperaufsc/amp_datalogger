#!/usr/bin/env bash

set -e

CAN_INTERFACE="can0"
BITRATE="500000"

echo "Configuring ${CAN_INTERFACE}..."
sudo ip link set "${CAN_INTERFACE}" down 2>/dev/null || true

sudo ip link set "${CAN_INTERFACE}" up \
    type can \
    bitrate "${BITRATE}" \
    restart-ms 100

echo
ip -details link show "${CAN_INTERFACE}"

echo
echo "SocketCAN interface ${CAN_INTERFACE} started at ${BITRATE} bit/s."