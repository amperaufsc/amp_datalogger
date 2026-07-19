from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    parameters_file = os.path.join(
        get_package_share_directory('io_manager'),
        'config',
        'io_manager_sensor_mux.yaml'
    )

    sensor_mux_node = Node(
        package='io_manager',
        executable='sensor_mux_node',
        name='sensor_mux_node',
        namespace='io_manager',
        output='screen',
        parameters=[parameters_file]
    )

    return LaunchDescription([
        sensor_mux_node
    ])