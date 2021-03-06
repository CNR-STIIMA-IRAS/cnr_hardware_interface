/*
 *  Software License Agreement (New BSD License)
 *
 *  Copyright 2020 National Council of Research of Italy (CNR)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef  CNR_HARDWARE_INTERFACE_CNR_ROBOT_HW_H
#define  CNR_HARDWARE_INTERFACE_CNR_ROBOT_HW_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <mutex>  // NOLINT
#include <functional>
#include <thread>  // NOLINT

#include <ros/ros.h>
#include <ros/callback_queue.h>
#include <realtime_utilities/diagnostics_interface.h>
#include <cnr_logger/cnr_logger.h>

#include <hardware_interface/robot_hw.h>
#include <configuration_msgs/SetConfig.h>
#include <configuration_msgs/GetConfig.h>
#include <cnr_hardware_interface/cnr_robot_hw_status.h>
#include <cnr_hardware_interface/internal/cnr_robot_hw_utils.h>


namespace cnr_hardware_interface
{

typedef std::function<void(const std::string&)> SetStatusParamFcn;

inline std::string extractRobotName(const std::string& hw_namespace)
{
  std::string hw_name = hw_namespace;
  hw_name      .erase(0, 1);
  std::replace(hw_name.begin(), hw_name.end(), '/', '_');
  return hw_name;
}

/**
 * @brief The RobotHW class
 */
class RobotHW: public hardware_interface::RobotHW, public realtime_utilities::DiagnosticsInterface
{
public:
  RobotHW();
  ~RobotHW();

  // ======================================================= final methods (cannot be overriden by the derived clases
  /**
   * @return true if ok
   * @brief If the cnr_hardware_nodelet_interface::RobotHwNodelet is used, this function is called by the onInit of
   * the nodelet that  stores the RobotHw.
   * In the onInit function of the cnr_hardware_nodelet_interface::RobotHwNodelet, the RobotHW::init() is called, 
   * and then a rt-thread is launched with an infinite loop standard  read - controller_manager::update - write.
   * Therefore, the RobotHW::init() is called before the creation of the thread. If some initialization needs to be 
   * timely close to the first read, you have to implement also the initRT() function that is called in the rt-thread
   * before the infinite loop.
   */
  bool init(ros::NodeHandle& root_nh, ros::NodeHandle &robot_hw_nh) final;
  void read(const ros::Time& time, const ros::Duration& period) final;
  void write(const ros::Time& time, const ros::Duration& period) final;
  bool prepareSwitch(const std::list< hardware_interface::ControllerInfo >& start_list,
                     const std::list< hardware_interface::ControllerInfo >& stop_list) final;
  void doSwitch(const std::list<hardware_interface::ControllerInfo>& start_list,
                const std::list<hardware_interface::ControllerInfo>& stop_list) final;
  bool checkForConflict(const std::list< hardware_interface::ControllerInfo >& info) const final;
  bool shutdown();
  // ======================================================= End - final methods

  // ======================================================= RT init
    /**
   * @return true if ok
   * @brief If the cnr_hardware_nodelet_interface::RobotHwNodelet is used, this function is called by the onInit of
   * the nodelet that  stores the RobotHw.
   * In the onInit function of the cnr_hardware_nodelet_interface::RobotHwNodelet, the RobotHW::init() is called, 
   * and then a rt-thread is launched with an infinite loop standard  read - controller_manager::update - write.
   * Therefore, the RobotHW::init() is called before the creation of the thread.
   * The initRT() is called at the begin of the RT_thread, just before the infinite loop, and timely very close to
   * the first read().
   */
  virtual bool initRT()
  {
    return true;
  }

  // ======================================================= Method to override inthe derived classes
  virtual bool doInit()
  {
    return true;
  }
  virtual bool doShutdown()
  {
    return true;
  }
  virtual bool doRead(const ros::Time& /*time*/, const ros::Duration& /*period*/)
  {
    return true;
  }
  virtual bool doWrite(const ros::Time& /*time*/, const ros::Duration& /*period*/)
  {
    return true;
  }
  virtual bool doPrepareSwitch(const std::list< hardware_interface::ControllerInfo >& /*start_list*/,
                               const std::list< hardware_interface::ControllerInfo >& /*stop_list*/)
  {
    return true;
  }
  virtual bool doDoSwitch(const std::list<hardware_interface::ControllerInfo>& /*start_list*/,
                          const std::list<hardware_interface::ControllerInfo>& /*stop_list*/)
  {
    return true;
  }

  virtual bool doCheckForConflict(const std::list< hardware_interface::ControllerInfo >& /*info*/) const
  {
    return false;
  }
  // ======================================================= END - Method to override inthe derived classes


  // =======================================================
  void setResourceNames(const std::vector<std::string>& resource_names)
  {
    m_resource_names = resource_names;
  }
  const std::vector<std::string>& resourceNames() const {return m_resource_names;}
  size_t resourceNumber() const {return m_resource_names.size();}
  // =======================================================

  // ======================================================= utils

  const cnr_hardware_interface::StatusHw&  getStatus() const
  {
    return m_status;
  }
  const std::string& getRobotHwNamespace() const
  {
    return m_robothw_nh.getNamespace();
  }
  // ======================================================= END - utils

protected:
  virtual bool setParamServer(configuration_msgs::SetConfigRequest& req, configuration_msgs::SetConfigResponse& res);
  virtual bool getParamServer(configuration_msgs::GetConfigRequest& req, configuration_msgs::GetConfigResponse& res);

  bool dump_state(const cnr_hardware_interface::StatusHw& status) const;
  bool dump_state() const;

private:
  virtual bool enterInit(ros::NodeHandle& root_nh, ros::NodeHandle &robot_hw_nh);
  virtual bool enterShutdown();
  virtual bool enterWrite();
  virtual bool enterPrepareSwitch(const std::list< hardware_interface::ControllerInfo >& start_list,
                                  const std::list< hardware_interface::ControllerInfo >& stop_list);
  virtual bool enterDoSwitch(const std::list<hardware_interface::ControllerInfo>& start_list,
                             const std::list<hardware_interface::ControllerInfo>& stop_list);
  virtual bool enterCheckForConflict(const std::list< hardware_interface::ControllerInfo >& info) const;

  virtual bool exitInit();
  virtual bool exitShutdown();
  virtual bool exitWrite();
  virtual bool exitPrepareSwitch();
  virtual bool exitDoSwitch();
  virtual bool exitCheckForConflict() const
  {
    return false;
  }

protected:

  double                                           m_sampling_period;
  std::string                                      m_robot_name;
  ros::NodeHandle                                  m_root_nh;
  ros::NodeHandle                                  m_robothw_nh;
  ros::CallbackQueue                               m_robot_hw_queue;
  mutable cnr_logger::TraceLogger                  m_logger;

  SetStatusParamFcn                                m_set_status_param;

  std::mutex                                       m_mutex;
  ros::ServiceServer                               m_get_param;
  ros::ServiceServer                               m_set_param;
  bool                                             m_stop_thread;

  bool                                             m_is_first_read;
  mutable cnr_hardware_interface::StatusHw         m_status;
  mutable std::vector<std::string>                 m_status_history;
  
  std::list< hardware_interface::ControllerInfo >  m_active_controllers;
  bool                                             m_shutted_down;



private:
    std::vector<std::string> m_resource_names;
};

typedef std::shared_ptr<RobotHW> RobotHWSharedPtr;

}  // namespace cnr_hardware_interface

#endif  // CNR_HARDWARE_INTERFACE_CNR_ROBOT_HW_H
