// Copyright 2022 INRAE, French National Research Institute for Agriculture, Food and Environment
// Add license


// std
#include <map>
#include <string>

// local
#include "romea_teleop/skid_steering_teleop.hpp"

namespace romea
{

//-----------------------------------------------------------------------------
SkidSteeringTeleop::SkidSteeringTeleop(const rclcpp::NodeOptions & options)
: TeleopBase(options),
  maximal_linear_speeds_(),
  maximal_angular_speeds_(),
  sent_disable_msg_(false)
{
  try {
    declare_parameters_();
    init_joystick_();
    init_command_publisher_();
  } catch (const std::exception & e) {
    RCLCPP_ERROR_STREAM(node_->get_logger(), e.what());
  }
}

//-----------------------------------------------------------------------------
void SkidSteeringTeleop::declare_command_ranges_()
{
  declare_maximal_linear_speeds(node_);
  declare_maximal_angular_speeds(node_);
}

//-----------------------------------------------------------------------------
void SkidSteeringTeleop::get_command_ranges_()
{
  maximal_linear_speeds_ = get_maximal_linear_speeds(node_);
  maximal_angular_speeds_ = get_maximal_angular_speeds(node_);
}

//-----------------------------------------------------------------------------
void SkidSteeringTeleop::declare_joystick_axes_mapping_()
{
  declare_linear_speed_axe_mapping(node_);
  declare_angular_speed_axe_mapping(node_);
}

//-----------------------------------------------------------------------------
void SkidSteeringTeleop::declare_joystick_buttons_mapping_()
{
  declare_slow_mode_button_mapping(node_);
  declare_turbo_mode_button_mapping(node_);
}

//-----------------------------------------------------------------------------
std::map<std::string, int> SkidSteeringTeleop::get_joystick_axes_mapping_()
{
  return {
    {"linear_speed", get_linear_speed_axe_mapping(node_)},
    {"angular_speed", get_angular_speed_axe_mapping(node_)},
  };
}

//-----------------------------------------------------------------------------
std::map<std::string, int> SkidSteeringTeleop::get_joystick_buttons_mapping_()
{
  return {
    {"slow_mode", get_slow_mode_button_mapping(node_)},
    {"turbo_mode", get_turbo_mode_button_mapping(node_)}
  };
}

//-----------------------------------------------------------------------------
void SkidSteeringTeleop::joystick_callback_(const Joystick & joy)
{
  SkidSteeringCommand cmd_msg;
  if (joy.getButtonValue("turbo_mode")) {
    cmd_msg.longitudinalSpeed = joy.getAxeValue("linear_speed") * maximal_linear_speeds_.turbo_mode;
    cmd_msg.angularSpeed = joy.getAxeValue("angular_speed") * maximal_angular_speeds_.turbo_mode;
    cmd_pub_->publish(cmd_msg);
    sent_disable_msg_ = false;
  } else if (joy.getButtonValue("slow_mode")) {
    cmd_msg.longitudinalSpeed = joy.getAxeValue("linear_speed") * maximal_linear_speeds_.slow_mode;
    cmd_msg.angularSpeed = joy.getAxeValue("angular_speed") * maximal_angular_speeds_.slow_mode;
    cmd_pub_->publish(cmd_msg);
    sent_disable_msg_ = false;
  } else {
    if (!sent_disable_msg_) {
      cmd_pub_->publish(cmd_msg);
      sent_disable_msg_ = true;
    }
  }
}

}  // namespace romea

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(romea::SkidSteeringTeleop)
