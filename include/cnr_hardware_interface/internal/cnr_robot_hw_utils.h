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
#ifndef CNR_HARDWARE_INTERFACE_INTERNAL_CNR_ROBOT_HW_UTILS_H
#define CNR_HARDWARE_INTERFACE_INTERNAL_CNR_ROBOT_HW_UTILS_H

#include <string>
#include <vector>
#include <ros/ros.h>
#include <cnr_hardware_interface/cnr_robot_hw_status.h>

namespace cnr_hardware_interface
{

// Iterator over enumr
typedef cnr_hardware_interface::EnumIterator< cnr_hardware_interface::tagStatusHw,
                                              cnr_hardware_interface::tagStatusHw::UNLOADED,
                                              cnr_hardware_interface::tagStatusHw::SRV_ERROR> StatusHwIterator;



template<class SharedPointer> 
struct Holder 
{
  SharedPointer p;

  Holder(const SharedPointer &p) : p(p) {}
  Holder(const Holder &other) : p(other.p) {}
  Holder(Holder &&other) : p(std::move(other.p)) {}

  void operator () (...) { p.reset(); }
};

template<class T> 
std::shared_ptr<T> to_std_ptr(const boost::shared_ptr<T> &p) 
{
  typedef Holder<std::shared_ptr<T>>   StandardHolder;
  typedef Holder<boost::shared_ptr<T>> BoostHolder;
  
  StandardHolder *h = boost::get_deleter<StandardHolder>(p);
  if( h ) 
  {
    return h->p;
  } 
  else 
  {
    return std::shared_ptr<T>(p.get(), BoostHolder(p));
  }
}

}  // namespace cnr_hardware_interface

#endif  // CNR_HARDWARE_INTERFACE_INTERNAL_CNR_ROBOT_HW_UTILS_H
