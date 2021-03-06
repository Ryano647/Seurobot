/* AVRS-MQP: motion_msgs -g
 *  Maintainer: avrs.mqp@gmail.com
 *  Authors: Nikolas Gamarra, Ryan O'Brien
 */
//ROS 
#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>

#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_datatypes.h>
#include <geometry_msgs/Pose.h>
//MoveIt
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_msgs/GetCartesianPath.h>
//#include <moveit/planning_interface/move_group.h>
#include <moveit/move_group_interface/move_group_interface.h>

#include <motion_msgs/MoveRobotFastAction.h>

//general
#include <math.h>


#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/DisplayRobotState.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>


class MoveRobotFastAction
{
  protected:

    ros::NodeHandle nh_;
    actionlib::SimpleActionServer<motion_msgs::MoveRobotFastAction> as_; // NodeHandle instance must be created before this line. Otherwise strange error occurs.
    std::string action_name_;
    // create messages that are used to published feedback/result
    motion_msgs::MoveRobotFastFeedback feedback_;
    motion_msgs::MoveRobotFastResult result_;

  public:

    MoveRobotFastAction(std::string name) :
      as_(nh_, name, boost::bind(&MoveRobotFastAction::executeCB, this, _1), false),
      action_name_(name)
  {
    as_.start();
  }

    ~MoveRobotFastAction(void)
    {
    }

    void executeCB(const motion_msgs::MoveRobotFastGoalConstPtr &goal)
    {
      // helper variables
      ros::Rate r(1);
      bool success = true;


      //FEEDBACK NEEDS UPDATE
      /*    
	    feedback_.sequence.clear();
	    feedback_.sequence.push_back(0);
	    feedback_.sequence.push_back(1);
       */

      ROS_INFO("%s: ExcutingCB: X:%f Y:%f Z:%f ",action_name_.c_str(),goal->x,goal->y,goal->z);
      std::string group="manipulator";
      //create quaternion
      tf::Quaternion q_rot;
      tf::TransformListener listener;

      float roll, pitch, yaw, x, y, z;
      //pull all the values from goal
      roll=goal->roll;
      pitch=goal->pitch;
      yaw=goal->yaw;
      x=goal->x;
      y=goal->y;
      z=goal->z;
      group=goal->frame;
      //convert deg to rad
      roll=roll*(M_PI/180);
      pitch=pitch*(M_PI/180);
      yaw=yaw*(M_PI/180);

      //create and fill pose	
      q_rot = tf::createQuaternionFromRPY(roll, pitch, yaw);//roll(x), pitch(y), yaw(z),
      geometry_msgs::Pose poseEOAT;
      quaternionTFToMsg(q_rot,poseEOAT.orientation);
      poseEOAT.position.x= x;
      poseEOAT.position.y= y;
      poseEOAT.position.z= z;
      //setup move_group and run 
      std::string base_frame = "/base_link";

      geometry_msgs::Pose move_target = poseEOAT;
      moveit::planning_interface::MoveGroupInterface move_group(group);
      // Plan for robot to move to part

      moveit::planning_interface::MoveGroupInterface::Plan my_plan;



      //ros::Publisher display_publisher = nh_.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
      //moveit_msgs::DisplayTrajectory display_trajectory;


//TEST ZONE

      geometry_msgs::Pose poseA;
      geometry_msgs::Pose poseB;
      geometry_msgs::Pose poseC;
      geometry_msgs::Pose poseD;


      quaternionTFToMsg(q_rot,poseA.orientation);
      quaternionTFToMsg(q_rot,poseB.orientation);
      quaternionTFToMsg(q_rot,poseC.orientation);
      quaternionTFToMsg(q_rot,poseD.orientation);


      poseA.position.x= 0.0;
      poseA.position.y= -1.0;
      poseA.position.z= .3;
  
      poseB.position.x= 0.1;
      poseB.position.y= -1.0;
      poseB.position.z= .3;
    
      poseC.position.x= 0.1;
      poseC.position.y= -.9;
      poseC.position.z= .3;
      
      poseD.position.x= 0.0;
      poseD.position.y= -1.0;
      poseD.position.z= .3;



      int mode=2;

      int visualize=0;


double status;
      moveit_msgs::RobotTrajectory trajectory;

      move_group.setPoseReferenceFrame(base_frame);

      // Mode 1 plans motion for a single point, used for testing
      if(mode==1){
	move_group.setPoseTarget(move_target);

	int status;
	move_group.move();
	success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
      }

      //Mode 2 executes as many points as it recieves with linear motion planning
      //moves to 'touch' point between each point
      else if(mode==2){

	std::vector< geometry_msgs::Pose > poses;


	//moveit_msgs::GetCartesianPath srv;
	//srv.request.wait_for_execution = true;

	//ros::ServiceClient executeKnownTrajectoryServiceClient = nh_.serviceClient<moveit_msgs::GetCartesianPathExecuteKnownTrajectory>("/execute_kinematic_path");


	//poses.push_back(poseEOAT);
	poses.push_back(poseA);
	poses.push_back(poseB);
	poses.push_back(poseC);
	poses.push_back(poseD);


	status=	move_group.computeCartesianPath(poses, 0.005, 0.0, trajectory, true);


	my_plan.trajectory_=trajectory;

	move_group.execute(my_plan);
	//	move_group.move();
	success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);

      }

      feedback_.current=status;

      if(visualize=1){
	//display_publisher.publish(trajectory);

      }


      /*
	 if (status>0.0){
	 success=true;
	 }else {

	 success=false;
	 }
       */


      //____EX looped execution with preemption
      // check that preempt has not been requested by the client
      if (as_.isPreemptRequested() || !ros::ok())
      {
	ROS_INFO("%s: Preempted", action_name_.c_str());
	// set the action state to preempted
	as_.setPreempted();
	success = false;
	move_group.stop();
      }
      // publish the feedback
      as_.publishFeedback(feedback_);

      //check success 
      if(success)
      {
	result_.statusCode =1;
	ROS_INFO("%s: Succeeded", action_name_.c_str());
	// set the action state to succeeded
	as_.setSucceeded(result_);
      }else{//failure
	result_.statusCode=0;
	ROS_INFO("%s: Failure",action_name_.c_str());
	as_.setAborted(result_);
      }

    }

};


int main(int argc, char** argv)
{
  bool start=true;
  if(start)
    ROS_INFO("Server Running...");
  start=false;
  //ros::AsyncSpinner async_spinner(1);
  //async_spinner.start();
  ros::init(argc, argv, "motion");

  MoveRobotFastAction motion("motion");
  ros::spin();
  //ros::waitForShutdown();

  return 0;
}
