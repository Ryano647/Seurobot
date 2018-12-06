#! /usr/bin/env python

from __future__ import print_function # Lets you print like Python 3
import rospy
import actionlib
# import simple_action_example.msg
# from motion_action_server import MoveRobot.msg
# import MoveRobot.msg
# import motion_action_server.msg
#import motion_msgs.msg  # import MoveRobotAction

import motion_msgs
from motion_msgs.msg import MoveRobotAction


def main_node():
    # SimpleActionClient construction, targeting the fibonacci topic of type Fibonacci
    client = actionlib.SimpleActionClient('motion',
                                          motion_msgs.msg.MoveRobotAction)

    # Waits until the action server has started up and started
    # listening for goals. (So the goals aren't ignored.)
    client.wait_for_server()

    # Creates a goal to send to the action server.
    goal = motion_msgs.msg.MoveRobotGoal()
    # print("\n".join(dir(goal.action_goal.goal)))
    goal.x=1
    goal.y=1
    goal.z=1
    goal.roll=0
    goal.pitch=0
    goal.yaw=0
    goal.frame="red"

    # Sends the goal to the action server.
    client.send_goal(goal)

    # Waits for the server to finish performing the action.
    client.wait_for_result()

    # Prints out the result of executing the action
    return client.get_result()  # A FibonacciResult


if __name__ == '__main__':
    print("Client Running")
    try:
        # Initializes a rospy node so that the SimpleActionClient can
        # publish and subscribe over ROS.
        rospy.init_node('main_node_py')
        result = main_node()

        if result is not None:
            #print("Result:", ', '.join([str(n) for n in result.sequence]))
            print("Server returned someting...")
        else:
            print("Server returned none")
    except rospy.ROSInterruptException:
        print("program interrupted before completion", file=sys.stderr)
