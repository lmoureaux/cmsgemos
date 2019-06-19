#include "gem/hw/optohybrid/HwOptoHybrid.h"
#include <algorithm>
#include <array>
#include <iterator>
#include "gem/hw/optohybrid/OptoHybridMonitor.h"
#include "gem/hw/optohybrid/OptoHybridManager.h"
#include "gem/base/GEMApplication.h"
#include "gem/base/GEMFSMApplication.h"
typedef gem::base::utils::GEMInfoSpaceToolBox::UpdateType GEMUpdateType;
gem::hw::optohybrid::OptoHybridMonitor::OptoHybridMonitor(
    std::shared_ptr<HwOptoHybrid> optohybrid,
    OptoHybridManager *optohybridManager, int const &index)
    : GEMMonitor(optohybridManager->getApplicationLogger(),
                 static_cast<xdaq::Application *>(optohybridManager), index),
      p_optohybrid(optohybrid) {
  updateMonitorables();
}
void gem::hw::optohybrid::OptoHybridMonitor::setupHwMonitoring() {
  addMonitorableSet("Status and Control", "HWMonitoring");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("VFAT_Mask", "CONTROL.VFAT.TRK_MASK"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("SBit_Mask", "CONTROL.VFAT.SBIT_MASK"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("Drop Bad CRC", "CONTROL.VFAT.DROP_BAD_CRC"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("TrgSource", "CONTROL.TRIGGER.SOURCE"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("SBitLoopback", "CONTROL.TRIGGER.LOOPBACK"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("TrgThrottle", "CONTROL.TRIGGER.THROTTLE"),
                 GEMUpdateType::HW32, "dec");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("HDMI SBitsOut", "CONTROL.HDMI_OUTPUT.SBITS"),
                 GEMUpdateType::HW32, "hex");
  addMonitorable(
      "Status and Control", "HWMonitoring",
      std::make_pair("HDMI SBitMode", "CONTROL.HDMI_OUTPUT.SBIT_MODE"),
      GEMUpdateType::HW32, "hex");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("ZS", "CONTROL.ZS"), GEMUpdateType::HW32,
                 "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("REF_CLK", "CONTROL.CLOCK.REF_CLK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("FIRMWARE_DATE", "STATUS.FW.DATE"),
                 GEMUpdateType::HW32, "dateoh");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("FIRMWARE_VERSION", "STATUS.FW.VERSION"),
                 GEMUpdateType::HW32, "fwveroh");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("FPGA_PLL_IS_LOCKED", "STATUS.FPGA_PLL_LOCK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("EXT_PLL_IS_LOCKED", "STATUS.EXT_PLL_LOCK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("CDCE_IS_LOCKED", "STATUS.CDCE_LOCK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("GTX_IS_LOCKED", "STATUS.GTX_LOCK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable("Status and Control", "HWMonitoring",
                 std::make_pair("QPLL_IS_LOCKED", "STATUS.QPLL_LOCK"),
                 GEMUpdateType::HW32, "bit");
  addMonitorable(
      "Status and Control", "HWMonitoring",
      std::make_pair("QPLL_FPGA_PLL_IS_LOCKED", "STATUS.QPLL_FPGA_PLL_LOCK"),
      GEMUpdateType::HW32, "bit");
  addMonitorableSet("Wishbone Counters", "HWMonitoring");
  std::array<std::string, 5> wbMasters = { { "GTX",  "GBT", "ExtI2C",
                                             "Scan", "DAC" } };
  for (auto master = wbMasters.begin(); master != wbMasters.end(); ++master) {
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Master:" + (*master) + "Strobe",
                                  "COUNTERS.WB.MASTER.Strobe." + (*master)),
                   GEMUpdateType::HW32, "dec");
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Master:" + (*master) + "Ack",
                                  "COUNTERS.WB.MASTER.Ack." + (*master)),
                   GEMUpdateType::HW32, "dec");
  }
  for (int i2c = 0; i2c < 6; ++i2c) {
    std::stringstream ss;
    ss << "I2C" << i2c;
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Slave:" + ss.str() + "Strobe",
                                  "COUNTERS.WB.SLAVE.Strobe." + ss.str()),
                   GEMUpdateType::HW32, "dec");
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Slave:" + ss.str() + "Ack",
                                  "COUNTERS.WB.SLAVE.Ack." + ss.str()),
                   GEMUpdateType::HW32, "dec");
  }
  std::array<std::string, 8> wbSlaves = { { "ExtI2C",   "Scan",  "T1",
                                            "DAC",      "ADC",   "Clocking",
                                            "Counters", "System" } };
  for (auto slave = wbSlaves.begin(); slave != wbSlaves.end(); ++slave) {
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Slave:" + (*slave) + "Strobe",
                                  "COUNTERS.WB.SLAVE.Strobe." + (*slave)),
                   GEMUpdateType::HW32, "dec");
    addMonitorable("Wishbone Counters", "HWMonitoring",
                   std::make_pair("Slave:" + (*slave) + "Ack",
                                  "COUNTERS.WB.SLAVE.Ack." + (*slave)),
                   GEMUpdateType::HW32, "dec");
  }
  addMonitorableSet("VFAT CRCs", "HWMonitoring");
  for (int vfat = 0; vfat < 24; ++vfat) {
    std::stringstream ss;
    ss << "VFAT" << vfat;
    addMonitorable(
        "VFAT CRCs", "HWMonitoring",
        std::make_pair(ss.str() + "_Valid", "COUNTERS.CRC.VALID." + ss.str()),
        GEMUpdateType::HW32, "dec");
    addMonitorable("VFAT CRCs", "HWMonitoring",
                   std::make_pair(ss.str() + "_Incorrect",
                                  "COUNTERS.CRC.INCORRECT." + ss.str()),
                   GEMUpdateType::HW32, "dec");
  }
  addMonitorableSet("T1 Counters", "HWMonitoring");
  std::array<std::string, 6> t1sources = {
    { "SENT", "GBT_TTC", "GTX_TTC", "INTERNAL", "EXTERNAL", "LOOPBACK" }
  };
  for (auto t1src = t1sources.begin(); t1src != t1sources.end(); ++t1src) {
    addMonitorable(
        "T1 Counters", "HWMonitoring",
        std::make_pair((*t1src) + "L1A", "COUNTERS.T1." + (*t1src) + ".L1A"),
        GEMUpdateType::HW32, "dec");
    addMonitorable("T1 Counters", "HWMonitoring",
                   std::make_pair((*t1src) + "CalPulse",
                                  "COUNTERS.T1." + (*t1src) + ".CalPulse"),
                   GEMUpdateType::HW32, "dec");
    addMonitorable("T1 Counters", "HWMonitoring",
                   std::make_pair((*t1src) + "Resync",
                                  "COUNTERS.T1." + (*t1src) + ".Resync"),
                   GEMUpdateType::HW32, "dec");
    addMonitorable(
        "T1 Counters", "HWMonitoring",
        std::make_pair((*t1src) + "BC0", "COUNTERS.T1." + (*t1src) + ".BC0"),
        GEMUpdateType::HW32, "dec");
  }
  addMonitorableSet("Other Counters", "HWMonitoring");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("GTX_TrackingLinkErrors", "COUNTERS.GTX_LINK.TRK_ERR"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("GTX_TriggerLinkErrors", "COUNTERS.GTX_LINK.TRG_ERR"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("GTX_DataPackets", "COUNTERS.GTX_LINK.DATA_Packets"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("GBT_TrackingLinkErrors", "COUNTERS.GBT_LINK.TRK_ERR"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("GBT_DataPackets", "COUNTERS.GBT_LINK.DATA_Packets"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable("Other Counters", "HWMonitoring",
                 std::make_pair("QPLL_LOCK", "COUNTERS.QPLL_LOCK"),
                 GEMUpdateType::PROCESS, "raw/rate");
  addMonitorable(
      "Other Counters", "HWMonitoring",
      std::make_pair("QPLL_FPGA_PLL_LOCK", "COUNTERS.QPLL_FPGA_PLL_LOCK"),
      GEMUpdateType::PROCESS, "raw/rate");
  addMonitorableSet("ADC", "HWMonitoring");
  addMonitorableSet("Firmware Scan Controller", "HWMonitoring");
  std::array<std::pair<std::string, std::string>, 3> scans = {
    { std::make_pair("Single VFAT Threshold/Latency/SCurve", "THLAT"),
      std::make_pair("Ultra VFATs Threshold/Latency/SCurve", "ULTRA"),
      std::make_pair("DAC", "DAC") }
  };
  std::array<std::string, 8> scanregs = {
    { "MODE", "CHIP", "CHAN", "MIN", "MAX", "STEP", "NTRIGS", "MONITOR" }
  };
  for (auto scan = scans.begin(); scan != scans.end(); ++scan) {
    for (auto scanreg = scanregs.begin(); scanreg != scanregs.end();
         ++scanreg) {
      if (scan->first == "DAC" && (*scanreg) == "CHAN")
        continue;
      if (scan->second == "ULTRA" && (*scanreg) == "CHIP")
        addMonitorable(
            "Firmware Scan Controller", "HWMonitoring",
            std::make_pair(scan->first + "MASK",
                           "ScanController." + scan->second + ".CONF.MASK"),
            GEMUpdateType::HW32, "hex");
      else if (*scanreg == "MONITOR")
        addMonitorable(
            "Firmware Scan Controller", "HWMonitoring",
            std::make_pair(scan->first + (*scanreg),
                           "ScanController." + scan->second + "." + (*scanreg)),
            GEMUpdateType::HW32, "hex");
      else
        addMonitorable("Firmware Scan Controller", "HWMonitoring",
                       std::make_pair(scan->first + (*scanreg),
                                      "ScanController." + scan->second +
                                          ".CONF." + (*scanreg)),
                       GEMUpdateType::HW32, "hex");
    }
  }
  updateMonitorables();
}
gem::hw::optohybrid::OptoHybridMonitor::~OptoHybridMonitor() {}
void gem::hw::optohybrid::OptoHybridMonitor::updateMonitorables() {
  CMSGEMOS_DEBUG("OptoHybridMonitor: Updating monitorables");
  for (auto monlist = m_monitorableSetsMap.begin();
       monlist != m_monitorableSetsMap.end(); ++monlist) {
    CMSGEMOS_DEBUG("OptoHybridMonitor: Updating monitorables in set "
                   << monlist->first);
    for (auto monitem = monlist->second.begin();
         monitem != monlist->second.end(); ++monitem) {
      CMSGEMOS_DEBUG("OptoHybridMonitor: Updating monitorable "
                     << monitem->first);
      std::stringstream regName;
      regName << p_optohybrid->getDeviceBaseNode() << "."
              << monitem->second.regname;
      uint32_t address = p_optohybrid->getNode(regName.str()).getAddress();
      uint32_t mask = p_optohybrid->getNode(regName.str()).getMask();
      if (monitem->second.updatetype == GEMUpdateType::HW8) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW16) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW24) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW32) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::HW64) {
        address = p_optohybrid->getNode(regName.str() + ".LOWER").getAddress();
        mask = p_optohybrid->getNode(regName.str() + ".LOWER").getMask();
        uint32_t lower = p_optohybrid->readReg(address, mask);
        address = p_optohybrid->getNode(regName.str() + ".UPPER").getAddress();
        mask = p_optohybrid->getNode(regName.str() + ".UPPER").getMask();
        uint32_t upper = p_optohybrid->readReg(address, mask);
        (monitem->second.infoSpace)
            ->setUInt64(monitem->first, (((uint64_t)upper) << 32) + lower);
      } else if (monitem->second.updatetype == GEMUpdateType::I2CSTAT) {
        std::stringstream strobeReg;
        strobeReg << regName.str() << ".Strobe." << monitem->first;
        address = p_optohybrid->getNode(strobeReg.str()).getAddress();
        mask = p_optohybrid->getNode(strobeReg.str()).getMask();
        uint32_t strobe = p_optohybrid->readReg(address, mask);
        std::stringstream ackReg;
        ackReg << regName.str() << ".Ack." << monitem->first;
        address = p_optohybrid->getNode(ackReg.str()).getAddress();
        mask = p_optohybrid->getNode(ackReg.str()).getMask();
        uint32_t ack = p_optohybrid->readReg(address, mask);
        (monitem->second.infoSpace)
            ->setUInt64(monitem->first, (((uint64_t)ack) << 32) + strobe);
      } else if (monitem->second.updatetype == GEMUpdateType::PROCESS) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::TRACKER) {
        (monitem->second.infoSpace)
            ->setUInt32(monitem->first, p_optohybrid->readReg(address, mask));
      } else if (monitem->second.updatetype == GEMUpdateType::NOUPDATE) {
        continue;
      } else {
        CMSGEMOS_ERROR("OptoHybridMonitor: Unknown update type encountered");
        continue;
      }
    }
  }
}
void
gem::hw::optohybrid::OptoHybridMonitor::buildMonitorPage(xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildMonitorPage");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  for (auto monset = monsets.begin(); monset != monsets.end(); ++monset) {
    *out << "<div class=\"xdaq-tab\" title=\"" << *monset << "\" >"
         << std::endl;
    if ((*monset).rfind("Firmware Scan Controller") == std::string::npos) {
      *out << "<table class=\"xdaq-table\" id=\"" << *monset << "_table\">"
           << std::endl << cgicc::thead() << std::endl << cgicc::tr()
           << std::endl << cgicc::th() << "Register name" << cgicc::th()
           << std::endl;
      if ((*monset).rfind("Wishbone Counters") != std::string::npos) {
        *out << cgicc::th() << "Strobes" << cgicc::th() << std::endl
             << cgicc::th() << "Acks" << cgicc::th() << std::endl;
      } else if ((*monset).rfind("VFAT CRCs") != std::string::npos) {
        *out << cgicc::th() << "Valid" << cgicc::th() << std::endl
             << cgicc::th() << "Incorrect" << cgicc::th() << std::endl;
      } else if ((*monset).rfind("T1 Counters") != std::string::npos) {
        *out << cgicc::th() << "Sent" << cgicc::th() << std::endl << cgicc::th()
             << "GBT_TTC" << cgicc::th() << std::endl << cgicc::th()
             << "GTX_TTC" << cgicc::th() << std::endl << cgicc::th()
             << "Internal" << cgicc::th() << std::endl << cgicc::th()
             << "External" << cgicc::th() << std::endl << cgicc::th()
             << "Loopback" << cgicc::th() << std::endl;
      } else if ((*monset).rfind("Other Counters") != std::string::npos) {
        *out << cgicc::th() << "Count" << cgicc::th() << std::endl
             << cgicc::th() << "Rate" << cgicc::th() << std::endl;
      } else {
        *out << cgicc::th() << "Value" << cgicc::th() << std::endl;
      }
      *out << cgicc::th() << "Register address" << cgicc::th() << std::endl
           << cgicc::th() << "Description" << cgicc::th() << std::endl
           << cgicc::tr() << std::endl << cgicc::thead() << std::endl
           << "<tbody>" << std::endl;
    }
    if ((*monset).rfind("Wishbone Counters") != std::string::npos) {
      buildWishboneCounterTable(out);
    } else if ((*monset).rfind("VFAT CRCs") != std::string::npos) {
      buildVFATCRCCounterTable(out);
    } else if ((*monset).rfind("T1 Counters") != std::string::npos) {
      buildT1CounterTable(out);
    } else if ((*monset).rfind("Other Counters") != std::string::npos) {
      buildOtherCounterTable(out);
    } else if ((*monset).rfind("Firmware Scan Controller") !=
               std::string::npos) {
      buildFirmwareScanTable(out);
    } else {
      for (auto monitem = m_monitorableSetsMap.find(*monset)->second.begin();
           monitem != m_monitorableSetsMap.find(*monset)->second.end();
           ++monitem) {
        *out << "<tr>" << std::endl;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        *out << "<td>" << std::endl << monitem->first << "</td>" << std::endl;
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
        *out << "<td>" << std::endl << monitem->second.regname << "</td>"
             << std::endl;
        *out << "<td>" << std::endl << "description"
             << "</td>" << std::endl;
        *out << "</tr>" << std::endl;
      }
    }
    if ((*monset).rfind("Firmware Scan Controller") == std::string::npos) {
      *out << "</tbody>" << std::endl << "</table>" << std::endl;
    }
    *out << "</div>" << std::endl;
  }
  *out << "</div>" << std::endl;
}
void gem::hw::optohybrid::OptoHybridMonitor::buildWishboneCounterTable(
    xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildWishboneCounterTable");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  if (std::find(monsets.begin(), monsets.end(), "Wishbone Counters") ==
      monsets.end()) {
    CMSGEMOS_WARN("Unable to find item set 'Wishbone Counters' in monitor");
    return;
  }
  auto monset = m_monitorableSetsMap.find("Wishbone Counters")->second;
  std::array<std::string, 2> strbacks = { { "Strobe", "Ack" } };
  std::array<std::string, 4> wbMasters = { { "GTX", "ExtI2C", "Scan", "DAC" } };
  for (auto wbMaster = wbMasters.begin(); wbMaster != wbMasters.end();
       ++wbMaster) {
    *out << "<tr>" << std::endl;
    *out << "<td>" << std::endl << "Master:" << (*wbMaster) << "</td>"
         << std::endl;
    for (auto strback = strbacks.begin(); strback != strbacks.end();
         ++strback) {
      for (auto monpair = monset.begin(); monpair != monset.end(); ++monpair) {
        if ((monpair->first).rfind("Master:" + (*wbMaster) + (*strback)) ==
            std::string::npos)
          continue;
        auto monitem = monpair;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
      }
    }
    *out << "<td>" << std::endl << "COUNTERS.WB.MASTER.<strb/ack>." +
                                       (*wbMaster) << "</td>" << std::endl;
    *out << "<td>" << std::endl << "description"
         << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
  }
  std::array<std::string, 14> wbSlaves{
    { "I2C0", "I2C1", "I2C2", "I2C3", "I2C4",     "I2C5",     "ExtI2C",
      "Scan", "T1",   "DAC",  "ADC",  "Clocking", "Counters", "System" }
  };
  for (auto wbSlave = wbSlaves.begin(); wbSlave != wbSlaves.end(); ++wbSlave) {
    *out << "<tr>" << std::endl;
    *out << "<td>" << std::endl << "Slave:" << (*wbSlave) << "</td>"
         << std::endl;
    for (auto strback = strbacks.begin(); strback != strbacks.end();
         ++strback) {
      for (auto monpair = monset.begin(); monpair != monset.end(); ++monpair) {
        if ((monpair->first).rfind("Slave:" + (*wbSlave) + (*strback)) ==
            std::string::npos)
          continue;
        auto monitem = monpair;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
      }
    }
    *out << "<td>" << std::endl << "COUNTERS.WB.SLAVE.&lt;strb/ack&gt;." +
                                       (*wbSlave) << "</td>" << std::endl;
    *out << "<td>" << std::endl << "description"
         << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
  }
}
void gem::hw::optohybrid::OptoHybridMonitor::buildVFATCRCCounterTable(
    xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildVFATCRCCounterTable");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  if (std::find(monsets.begin(), monsets.end(), "VFAT CRCs") == monsets.end()) {
    CMSGEMOS_WARN("Unable to find item set 'VFAT CRCs' in list of HWMonitoring "
                  "monitor sets");
    return;
  }
  auto monset = m_monitorableSetsMap.find("VFAT CRCs")->second;
  std::array<std::string, 2> crcs = { { "Valid", "Incorrect" } };
  for (int vfat = 0; vfat < 24; ++vfat) {
    std::stringstream ss;
    ss << "VFAT" << vfat;
    *out << "<tr>" << std::endl;
    *out << "<td>" << std::endl << ss.str() << "</td>" << std::endl;
    for (auto crc = crcs.begin(); crc != crcs.end(); ++crc) {
      for (auto monpair = monset.begin(); monpair != monset.end(); ++monpair) {
        if ((monpair->first).rfind(ss.str() + "_" + (*crc)) ==
            std::string::npos)
          continue;
        auto monitem = monpair;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
      }
    }
    *out << "<td>" << std::endl << "COUNTERS.CRC.&lt;flag&gt;." + ss.str()
         << "</td>" << std::endl;
    *out << "<td onMouseOver=\"expandedDescription('Number of data packets "
            "received from GEB slot " << vfat
         << " with Valid/Invalid CRC')\" id=\"description\">" << std::endl
         << "Slot " << vfat << " Valid/Invalid CRC" << std::endl << "</td>"
         << std::endl;
    *out << "</tr>" << std::endl;
  }
}
void
gem::hw::optohybrid::OptoHybridMonitor::buildT1CounterTable(xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildT1CounterTable");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  if (std::find(monsets.begin(), monsets.end(), "T1 Counters") ==
      monsets.end()) {
    CMSGEMOS_WARN("Unable to find item set 'T1 Counters' in list of "
                  "HWMonitoring monitor sets");
    return;
  }
  auto monset = m_monitorableSetsMap.find("T1 Counters")->second;
  std::array<std::string, 6> t1sources = {
    { "SENT", "GBT_TTC", "GTX_TTC", "INTERNAL", "EXTERNAL", "LOOPBACK" }
  };
  std::array<std::string, 4> t1signals = {
    { "L1A", "CalPulse", "Resync", "BC0" }
  };
  for (auto t1signal = t1signals.begin(); t1signal != t1signals.end();
       ++t1signal) {
    *out << "<tr>" << std::endl;
    *out << "<td>" << std::endl << *t1signal << "</td>" << std::endl;
    for (auto t1source = t1sources.begin(); t1source != t1sources.end();
         ++t1source) {
      std::string keyname = (*t1source) + (*t1signal);
      for (auto monpair = monset.begin(); monpair != monset.end(); ++monpair) {
        if ((monpair->first).rfind(keyname) == std::string::npos) {
          continue;
        }
        auto monitem = monpair;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
      }
    }
    *out << "<td>" << std::endl << "COUNTERS.T1.&lt;source&gt;." + (*t1signal)
         << "</td>" << std::endl;
    *out << "<td>" << std::endl << "Number of " << *t1signal
         << " signals received"
         << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
  }
}
void gem::hw::optohybrid::OptoHybridMonitor::buildOtherCounterTable(
    xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildOtherCounterTable");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  if (std::find(monsets.begin(), monsets.end(), "Other Counters") ==
      monsets.end()) {
    CMSGEMOS_WARN("Unable to find item set 'Other Counters' in list of "
                  "HWMonitoring monitor sets");
    return;
  }
  auto monset = m_monitorableSetsMap.find("Other Counters")->second;
  for (auto monitem = monset.begin(); monitem != monset.end(); ++monitem) {
    *out << "<tr>" << std::endl;
    std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
        monitem->first, monitem->second.format);
    *out << "<td>" << std::endl << monitem->first << "</td>" << std::endl;
    CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first << " formatted to "
                                         << formatted);
    *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
         << monitem->first << "\">" << std::endl << formatted << "</td>"
         << std::endl;
    *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
         << monitem->first << "\">" << std::endl << formatted << "</td>"
         << std::endl;
    *out << "<td>" << std::endl << monitem->second.regname << "</td>"
         << std::endl;
    *out << "<td>" << std::endl << "description"
         << "</td>" << std::endl;
    *out << "</tr>" << std::endl;
  }
}
void gem::hw::optohybrid::OptoHybridMonitor::buildFirmwareScanTable(
    xgi::Output *out) {
  CMSGEMOS_DEBUG("OptoHybridMonitor::buildFirmwareScanTable");
  if (m_infoSpaceMonitorableSetMap.find("HWMonitoring") ==
      m_infoSpaceMonitorableSetMap.end()) {
    CMSGEMOS_WARN("Unable to find item set HWMonitoring in monitor");
    return;
  }
  auto monsets = m_infoSpaceMonitorableSetMap.find("HWMonitoring")->second;
  if (std::find(monsets.begin(), monsets.end(), "Firmware Scan Controller") ==
      monsets.end()) {
    CMSGEMOS_WARN("Unable to find item set 'Firmware Scan Controller' in list "
                  "of HWMonitoring monitor sets");
    return;
  }
  auto monset = m_monitorableSetsMap.find("Firmware Scan Controller")->second;
  std::array<std::pair<std::string, std::string>, 3> scans = {
    { std::make_pair("Single VFAT Threshold/Latency/SCurve", "THLAT"),
      std::make_pair("Ultra VFATs Threshold/Latency/SCurve", "ULTRA"),
      std::make_pair("DAC", "DAC") }
  };
  *out << "<div class=\"xdaq-tab-wrapper\">" << std::endl;
  for (auto scan = scans.begin(); scan != scans.end(); ++scan) {
    *out << "<div class=\"xdaq-tab\" title=\"" << scan->first << "\" >"
         << std::endl;
    *out << "<table class=\"xdaq-table\" id=\"" << scan->first << "_table\">"
         << std::endl << cgicc::thead() << std::endl << cgicc::tr() << std::endl
         << cgicc::th() << "Register name" << cgicc::th() << std::endl
         << cgicc::th() << "Value" << cgicc::th() << std::endl << cgicc::th()
         << "Register address" << cgicc::th() << std::endl << cgicc::th()
         << "Description" << cgicc::th() << std::endl << cgicc::tr()
         << std::endl << cgicc::thead() << std::endl << "<tbody>" << std::endl;
    for (auto monitem = monset.begin(); monitem != monset.end(); ++monitem) {
      if (scan->first == "DAC" &&
          (monitem->first).rfind("CHAN") != std::string::npos)
        continue;
      std::string regname = monitem->second.regname;
      if ((monitem->first).rfind(scan->first) != std::string::npos) {
        *out << "<tr>" << std::endl;
        std::string formatted = (monitem->second.infoSpace)->getFormattedItem(
            monitem->first, monitem->second.format);
        std::string monname = monitem->first;
        *out << "<td>" << std::endl << monname.erase(0, scan->first.length())
             << "</td>" << std::endl;
        CMSGEMOS_DEBUG("OptoHybridMonitor::" << monitem->first
                                             << " formatted to " << formatted);
        *out << "<td id=\"" << monitem->second.infoSpace->name() << "-"
             << monitem->first << "\">" << std::endl << formatted << "</td>"
             << std::endl;
        *out << "<td>" << std::endl << regname << "</td>" << std::endl;
        *out << "<td>" << std::endl << "description"
             << "</td>" << std::endl;
        *out << "</tr>" << std::endl;
      }
    }
    *out << "</tbody>" << std::endl << "</table>" << std::endl;
    *out << "</div>" << std::endl;
  }
  *out << "</div>" << std::endl;
}
void gem::hw::optohybrid::OptoHybridMonitor::reset() {
  CMSGEMOS_DEBUG("GEMMonitor::reset");
  for (auto infoSpace = m_infoSpaceMap.begin();
       infoSpace != m_infoSpaceMap.end(); ++infoSpace) {
    CMSGEMOS_DEBUG("OptoHybridMonitor::reset removing " << infoSpace->first
                                                        << " from p_timer");
    try {
      p_timer->remove(infoSpace->first);
    }
    catch (toolbox::task::exception::Exception &te) {
      CMSGEMOS_ERROR(
          "OptoHybridMonitor::Caught exception while removing timer task "
          << infoSpace->first << " " << te.what());
    }
  }
  stopMonitoring();
  CMSGEMOS_DEBUG("GEMMonitor::reset removing timer " << m_timerName
                                                     << " from timerFactory");
  try {
    toolbox::task::getTimerFactory()->removeTimer(m_timerName);
  }
  catch (toolbox::task::exception::Exception &te) {
    CMSGEMOS_ERROR("OptoHybridMonitor::Caught exception while removing timer "
                   << m_timerName << " " << te.what());
  }
  CMSGEMOS_DEBUG("OptoHybridMonitor::reset - clearing all maps");
  m_infoSpaceMap.clear();
  m_infoSpaceMonitorableSetMap.clear();
  m_monitorableSetInfoSpaceMap.clear();
  m_monitorableSetsMap.clear();
}
