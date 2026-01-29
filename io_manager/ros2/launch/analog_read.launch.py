from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():

    pkg_share = get_package_share_directory('io_manager')

    params_file = os.path.join(
        pkg_share,
        'config',
        'io_manager_analog.yaml'
    )

    analog_read_node = Node(
        package='io_manager',
        executable='analog_read_node',
        name='analog_read_node',
        namespace='io_manager',
        output='screen',
        parameters=[params_file]
    )

    return LaunchDescription([
        analog_read_node
    ])
