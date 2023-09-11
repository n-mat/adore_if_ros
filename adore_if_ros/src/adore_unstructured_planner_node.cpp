/********************************************************************************
 * Copyright (C) 2017-2023 German Aerospace Center (DLR). 
 * Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
 *
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 
 *
 * Contributors: 
 *   Reza Dariani
 *   Matthias Nichting
 ********************************************************************************/

#include <adore_if_ros_scheduling/baseapp.h>
#include <adore_if_ros/factorycollection.h>

#include <adore_if_ros_msg/PointArray.h>
#include <adore/apps/unstructured_planner.h>


namespace adore
{
  namespace if_ROS
  {  
    class UnstructuredPlannerNode  : public FactoryCollection, public adore_if_ros_scheduling::Baseapp
    {
      public:
      typedef adore::apps::UnstructuredPlanner TUnstructuredPlanner;
      TUnstructuredPlanner* app_;

      //adore::apps::TrajectoryPlannerLM* planner_;

      ros::Publisher occupancies_publisher_;
      UnstructuredPlannerNode(){}
      void init(int argc, char **argv, double rate, std::string nodename)
      {
        Baseapp::init(argc, argv, rate, nodename);
        Baseapp::initSim();
        FactoryCollection::init(getRosNodeHandle());

        app_ = new TUnstructuredPlanner;

        //planner_ = new adore::apps::TrajectoryPlannerLM(directionLeft,name,id);

        bool use_scheduler = false;
        getRosNodeHandle()->getParam("/use_scheduler",use_scheduler);

        //error: no matching function for call to ‘std::function<void()>::function(std::_Bind_helper<false, void (adore::apps::TrajectoryPlannerBase::*)(), adore::fun::BasicUnstructuredPlanner*&>::type)’
        if(use_scheduler)
        {
          //std::function<void()> run_fcn(std::bind(&adore::apps::TrajectoryPlannerBase::planning_request_handler,planner_));
          std::function<void()> run_fcn(std::bind(&adore::apps::TrajectoryPlannerBase::planning_request_handler,app_));
          Baseapp::addTimerCallback(run_fcn);
        }
        else
        {
          app_->prime();//node is executed event-based on PlanningRequest, prime sets trigger
        }
        // timer callbacks
        std::function<void()> run_fcn(std::bind(&UnstructuredPlannerNode::publish_occupancy_grid,this));
        Baseapp::addTimerCallback(run_fcn);
        occupancies_publisher_ = getRosNodeHandle()->advertise<adore_if_ros_msg::PointArray>("occupancies",1);

      }
      void publish_occupancy_grid()
      {
        return;
        std::cout << "publish_occ_grid_start"<<std::endl;
        adore_if_ros_msg::PointArray msg;
        msg.x.push_back(app_->getWidth());
        msg.y.push_back(app_->getLength());
        app_->getOccupancies_x(msg.x);
        app_->getOccupancies_y(msg.y);
        occupancies_publisher_.publish(msg);
        std::cout << "publish_occ_grid_end"<<std::endl;

      }

    };
  }
}

int main(int argc,char **argv)
{
    adore::if_ROS::UnstructuredPlannerNode node;
    node.init(argc, argv, 10.0, "adore_unstructured_planner_node");
    node.run();
    return 0;
}