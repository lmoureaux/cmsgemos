#ifndef GEM_HWMONITOR_GEMHWMONITORBASE_H
#define GEM_HWMONITOR_GEMHWMONITORBASE_H
#include <string>
#include <vector>
#include <cstdlib>
#include "xdaq/Application.h"
#include "xgi/framework/Method.h"
#include "gem/utils/gemXMLparser.h"
#include "gem/utils/gemComplexDeviceProperties.h"
#include "gem/utils/gemDeviceProperties.h"
namespace gem {
namespace hwMonitor {
template <class T> class gemHwMonitorBase {
public:
  gemHwMonitorBase()
  throw(xdaq::exception::Exception) {
    p_gemDevice = new T();
    m_isConfigured = false;
  }
  virtual ~gemHwMonitorBase() { delete p_gemDevice; }
  bool isConfigured() throw(xgi::exception::Exception) {
    return m_isConfigured;
  }
  void setIsConfigured(bool state) throw(xgi::exception::Exception) {
    m_isConfigured = state;
  }
  const std::string getDeviceId() throw(xgi::exception::Exception);
  unsigned int
  getSubDeviceStatus(unsigned int i) throw(xgi::exception::Exception) {
    return m_subDeviceStatus.at(i);
  }
  void
  setSubDeviceStatus(const unsigned int deviceStatus,
                     const unsigned int i) throw(xgi::exception::Exception) {
    m_subDeviceStatus.at(i) = deviceStatus;
  }
  void addSubDeviceStatus(unsigned int deviceStatus) throw(
      xgi::exception::Exception) {
    m_subDeviceStatus.push_back(deviceStatus);
  }
  unsigned int getDeviceStatus() throw(xgi::exception::Exception) {
    return m_deviceStatus;
  }
  void setDeviceStatus(const unsigned int deviceStatus) throw(
      xgi::exception::Exception) {
    m_deviceStatus = deviceStatus;
  }
  void setDeviceConfiguration(T &device) throw(xgi::exception::Exception);
  T *getDevice() throw(xgi::exception::Exception) { return p_gemDevice; }
  int getNumberOfSubDevices() throw(xgi::exception::Exception);
  const std::string getCurrentSubDeviceId(unsigned int subDeviceNumber) throw(
      xgi::exception::Exception);

protected:
private:
  bool m_isConfigured;
  unsigned int m_deviceStatus;
  std::vector<unsigned int> m_subDeviceStatus;
  std::string m_xmlConfigFileName;
  T *p_gemDevice;
};
typedef gemHwMonitorBase<gem::utils::gemSystemProperties> gemHwMonitorSystem;
typedef gemHwMonitorBase<gem::utils::gemCrateProperties> gemHwMonitorCrate;
typedef gemHwMonitorBase<gem::utils::gemGLIBProperties> gemHwMonitorGLIB;
typedef gemHwMonitorBase<gem::utils::gemOHProperties> gemHwMonitorOH;
typedef gemHwMonitorBase<gem::utils::gemVFATProperties> gemHwMonitorVFAT;
}
}
#endif
