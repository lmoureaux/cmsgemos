#ifndef GEM_BASE_GEMAPPLICATION_H
#define GEM_BASE_GEMAPPLICATION_H
#include <cstdlib>
#include <limits>
#include <string>
#include <memory>
#include <deque>
#include <map>
#include "log4cplus/logger.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/Application.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationDescriptorImpl.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/WebApplication.h"
#include "xdaq/XceptSerializer.h"
#include "xdata/Bag.h"
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedShort.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Vector.h"
#include "toolbox/string.h"
#include "toolbox/TimeVal.h"
#include "xgi/Input.h"
#include "xgi/Method.h"
#include "xgi/Output.h"
#include "xgi/framework/UIManager.h"
#include "xoap/Method.h"
#include "xcept/Exception.h"
#include "xcept/tools.h"
#include "gem/utils/GEMLogging.h"
#include "gem/utils/db/GEMDatabaseUtils.h"
#include "gem/base/exception/Exception.h"
#include "gem/base/utils/exception/Exception.h"
#include "gem/base/utils/GEMInfoSpaceToolBox.h"
namespace xdaq {
class ApplicationStub;
}
namespace xgi {
class Input;
class Output;
}
namespace gem {
namespace base {
class GEMMonitor;
class GEMFSMApplication;
class GEMWebApplication;
namespace utils {
class GEMInfoSpaceToolBox;
}
class GEMApplication : public xdaq::WebApplication,
                       public xdata::ActionListener {
  friend class GEMMonitor;
  friend class GEMWebApplication;

public:
  static const unsigned MAX_AMCS_PER_CRATE = 12;
  static const unsigned MAX_OPTOHYBRIDS_PER_AMC = 12;
  static const unsigned MAX_VFATS_PER_GEB = 24;
  GEMApplication(xdaq::ApplicationStub *stub)
  throw(xdaq::exception::Exception);
  virtual ~GEMApplication();
  std::string getFullURL();
  virtual void actionPerformed(xdata::Event &event);
  void xgiDefault(xgi::Input *in, xgi::Output *out);
  void xgiMonitor(xgi::Input *in, xgi::Output *out);
  void xgiExpert(xgi::Input *in, xgi::Output *out);
  void jsonUpdate(xgi::Input *in, xgi::Output *out);
  std::shared_ptr<utils::GEMInfoSpaceToolBox> getAppISToolBox() {
    return p_appInfoSpaceToolBox;
  };
  std::shared_ptr<utils::GEMInfoSpaceToolBox> getMonISToolBox() {
    return p_monitorInfoSpaceToolBox;
  };
  std::shared_ptr<utils::GEMInfoSpaceToolBox> getCfgISToolBox() {
    return p_configInfoSpaceToolBox;
  };

protected:
  virtual GEMWebApplication *getWebApp() const {
    return p_gemWebInterface;
  };
  virtual GEMMonitor *getMonitor() const {
    return p_gemMonitor;
  };
  log4cplus::Logger m_gemLogger;
  std::shared_ptr<utils::GEMInfoSpaceToolBox> p_appInfoSpaceToolBox;
  std::shared_ptr<utils::GEMInfoSpaceToolBox> p_monitorInfoSpaceToolBox;
  std::shared_ptr<utils::GEMInfoSpaceToolBox> p_configInfoSpaceToolBox;
  xdata::InfoSpace *p_appInfoSpace;
  xdata::InfoSpace *p_monitorInfoSpace;
  xdata::InfoSpace *p_configInfoSpace;
  virtual void importConfigurationParameters();
  virtual void fillConfigurationInfoSpace();
  virtual void updateConfigurationInfoSpace();
  virtual void importMonitoringParameters();
  virtual void fillMonitoringInfoSpace();
  virtual void updateMonitoringInfoSpace();
  GEMWebApplication *p_gemWebInterface;
  GEMMonitor *p_gemMonitor;

public:
  class ScanInfo {
  public:
    ScanInfo();
    void registerFields(xdata::Bag<GEMApplication::ScanInfo> *bag);
    xdata::UnsignedInteger32 scanType;
    xdata::UnsignedInteger32 scanMin;
    xdata::UnsignedInteger32 scanMax;
    xdata::UnsignedInteger32 stepSize;
    xdata::UnsignedInteger64 nTriggers;
    inline std::string toString() {
      std::stringstream os;
      os << "scanType:" << scanType.toString() << std::endl
         << "scanMin:" << scanMin.toString() << std::endl
         << "scanMax:" << scanMax.toString() << std::endl
         << "stepSize:" << stepSize.toString() << std::endl
         << "nTrigger:" << nTriggers.value_ << std::endl << std::endl;
      return os.str();
    };
  };
  xdaq::ApplicationDescriptor *p_appDescriptor;
  xdaq::ApplicationContext *p_appContext;
  xdaq::ApplicationGroup *p_appGroup;
  xdaq::Zone *p_appZone;
  std::string m_xmlClass;
  std::string m_urn;
  uint32_t m_instance;

protected:
  xdata::Integer64 m_runNumber;
  xdata::String m_runType;
  xdata::String m_cfgType;
  xdata::Boolean m_disableMonitoring;
  xdata::Bag<ScanInfo> m_scanInfo;
  xdata::UnsignedInteger32 m_scanType;
  xdata::UnsignedInteger32 m_scanMin;
  xdata::UnsignedInteger32 m_scanMax;
  xdata::UnsignedInteger32 m_stepSize;
  xdata::UnsignedInteger64 m_nScanTriggers;
  std::shared_ptr<gem::utils::db::GEMDatabaseUtils> p_gemDBHelper;

private:
};
}
}
#endif
