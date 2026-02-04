from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    parameters_file = os.path.join(
        get_package_share_directory('io_manager'),
        'config',
        'io_manager_gpio_interrupts.yaml'
    )

    return LaunchDescription([
        Node(
            package='io_manager',
            executable='gpio_multi_interrupt_node',
            name='gpio_multi_interrupt_node',
            namespace='io_manager',
            output='screen',
            parameters=[parameters_file]
        )
    ])
