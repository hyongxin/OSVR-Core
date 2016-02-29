/** @date 2015
    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_example_Tracker_json.h"

// Library/third-party includes

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

// Anonymous namespace to avoid symbol collision
namespace {

class TrackerDevice {
  public:
    TrackerDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // configure our tracker
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the device token with the options
        m_dev.initAsync(ctx, "Tracker", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_example_Tracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);

        /// Seed pseudo random generator.
        std::srand(std::time(0));
    }

    OSVR_ReturnCode update() {

        std::this_thread::sleep_for(std::chrono::milliseconds(
            5)); // Simulate waiting a quarter second for data.

        /// Report the identity pose for sensor 0
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        osvrDeviceTrackerSendPose(m_dev, m_tracker, &pose, 0);

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;

        /// we always detect device in sample plugin
        m_found = true;

        std::cout << "PLUGIN: We have detected Tracker device! "
                  << std::endl;
        /// Create our device object
        osvr::pluginkit::registerObjectForDeletion(ctx, new TrackerDevice(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_Tracker) {

    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
