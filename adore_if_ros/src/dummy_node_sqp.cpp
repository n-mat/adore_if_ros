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
 *   Matthias Nichting
 ********************************************************************************/

#include <adore_if_ros_scheduling/baseapp.h>
#include <adore_if_ros/factorycollection.h>
#include <adore/apps/dummy_sqp_app.h>
#include <iostream>

namespace adore
{
  namespace if_ROS
  {  
    class DummySQPNode : public FactoryCollection, public adore_if_ros_scheduling::Baseapp
    {
      public:
      adore::apps::DummySQP* app_;
      DummySQPNode(){}
      
      void init(int argc, char **argv, double rate, std::string nodename)
      {
        Baseapp::init(argc, argv, rate, nodename);
        Baseapp::initSim();
        FactoryCollection::init(getRosNodeHandle());
        app_ = new adore::apps::DummySQP();
        
        // timer callbacks
        std::function<void()> run_fcn(std::bind(&adore::apps::DummySQP::run,app_));
        Baseapp::addTimerCallback(run_fcn);
      }
    };
  }
}

int main(int argc,char **argv)
{
    adore::if_ROS::DummySQPNode node;
    node.init(argc, argv, 1, "dummy_sqp_node");
    node.run();
    return 0;
}