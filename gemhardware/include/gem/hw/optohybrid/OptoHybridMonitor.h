#ifndef GEM_HW_OPTOHYBRID_OPTOHYBRIDMONITOR_H
#define GEM_HW_OPTOHYBRID_OPTOHYBRIDMONITOR_H
#include "gem/base/GEMMonitor.h"
#include "gem/hw/optohybrid/exception/Exception.h"
#include "gem/hw/optohybrid/HwOptoHybrid.h"
namespace gem {
namespace hw {
namespace optohybrid {
class HwOptoHybrid;
class OptoHybridManager;
class OptoHybridMonitor : public gem::base::GEMMonitor {
public:
  OptoHybridMonitor(std::shared_ptr<HwOptoHybrid> optohybrid,
                    OptoHybridManager *optohybridManager, int const &index);
  virtual ~OptoHybridMonitor();
  virtual void updateMonitorables();
  virtual void reset();
  void setupHwMonitoring();
  void buildMonitorPage(xgi::Output *out);
  void buildWishboneCounterTable(xgi::Output *out);
  void buildVFATCRCCounterTable(xgi::Output *out);
  void buildT1CounterTable(xgi::Output *out);
  void buildOtherCounterTable(xgi::Output *out);
  void buildFirmwareScanTable(xgi::Output *out);
  std::string getDeviceID() { return p_optohybrid->getDeviceID(); }

private:
  std::shared_ptr<HwOptoHybrid> p_optohybrid;
};
}
}
}
#endif
