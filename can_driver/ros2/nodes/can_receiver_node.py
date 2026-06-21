#!/usr/bin/env python3

from pathlib import Path
import threading

import rclpy
from rclpy.node import Node
from ament_index_python.packages import get_package_share_directory

from dbc_parser import DbcParser
from socketcan_driver import SocketCanDriver
from can_topic_manager import CanTopicManager

class CanReceiverNode(Node):

    def __init__(self):
        super().__init__("can_receiver")

        default_dbc = str(
            Path(get_package_share_directory("can_driver"))
            / "dbc"
            / "amp226.dbc"
        )

        self.declare_parameter("dbc_path", default_dbc)
        self.declare_parameter("can_interface", "vcan0")
        self.declare_parameter("receive_timeout", 0.1)

        dbc_path = self.get_parameter("dbc_path").value
        can_interface = self.get_parameter("can_interface").value
        receive_timeout = self.get_parameter("receive_timeout").value

        self._dbc = DbcParser(Path(dbc_path))

        self._driver = SocketCanDriver(
            channel=can_interface,
            receive_timeout=receive_timeout,
        )

        self._driver.open()

        #
        # Cria todos os publishers a partir do DBC
        #
        self._topic_manager = CanTopicManager(
            node=self,
            dbc=self._dbc,
        )

        self._running = True

        self._thread = threading.Thread(
            target=self._receive_loop,
            daemon=True,
        )
        self._thread.start()

        self.get_logger().info("CAN Receiver started.")

    def destroy_node(self):

        self._running = False

        if self._thread.is_alive():
            self._thread.join(timeout=1.0)

        self._driver.close()

        super().destroy_node()

    def _receive_loop(self):

        while rclpy.ok() and self._running:

            frame = self._driver.receive()

            if frame is None:
                continue

            try:
                self._topic_manager.publish(frame)

            except KeyError:
                #
                # Frame não existe no DBC.
                #
                continue

            except Exception as e:
                self.get_logger().warning(
                    f"Error processing frame 0x{frame.arbitration_id:X}: {e}"
                )


def main(args=None):

    rclpy.init(args=args)

    node = CanReceiverNode()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()