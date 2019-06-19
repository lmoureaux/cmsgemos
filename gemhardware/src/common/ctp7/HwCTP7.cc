#include <iomanip>
#include "gem/hw/ctp7/HwCTP7.h"
gem::hw::ctp7::HwCTP7::HwCTP7()
    : gem::hw::GEMHwDevice::GEMHwDevice("HwCTP7"), m_crate(-1), m_slot(-1) {
  CMSGEMOS_INFO("HwCTP7 ctor");
  setDeviceID("CTP7Hw");
  setAddressTableFileName("ctp7_address_table.xml");
  setDeviceBaseNode("CTP7");
  for (unsigned li = 0; li < N_GTX; ++li) {
    b_links[li] = false;
    CTP7IPBusCounters tmpGTXCounter;
    m_ipBusCounters.push_back(tmpGTXCounter);
  }
  CMSGEMOS_INFO("HwCTP7 ctor done, deviceBaseNode is " << getDeviceBaseNode());
}
gem::hw::ctp7::HwCTP7::HwCTP7(std::string const &ctp7Device,
                              std::string const &connectionFile)
    : gem::hw::GEMHwDevice::GEMHwDevice(ctp7Device, connectionFile),
      m_crate(-1), m_slot(-1) {
  setDeviceBaseNode("CTP7");
  for (unsigned li = 0; li < N_GTX; ++li) {
    b_links[li] = false;
    CTP7IPBusCounters tmpGTXCounter;
    m_ipBusCounters.push_back(tmpGTXCounter);
  }
  CMSGEMOS_INFO("HwCTP7 ctor done, deviceBaseNode is " << getDeviceBaseNode());
}
gem::hw::ctp7::HwCTP7::HwCTP7(std::string const &ctp7Device,
                              std::string const &connectionURI,
                              std::string const &addressTable)
    : gem::hw::GEMHwDevice::GEMHwDevice(ctp7Device, connectionURI,
                                        addressTable),
      m_crate(-1), m_slot(-1) {
  CMSGEMOS_INFO("trying to create HwCTP7(" << ctp7Device << "," << connectionURI
                                           << "," << addressTable);
  setDeviceBaseNode("CTP7");
  for (unsigned li = 0; li < N_GTX; ++li) {
    b_links[li] = false;
    CTP7IPBusCounters tmpGTXCounter;
    m_ipBusCounters.push_back(tmpGTXCounter);
  }
  CMSGEMOS_INFO("HwCTP7 ctor done, deviceBaseNode is " << getDeviceBaseNode());
}
gem::hw::ctp7::HwCTP7::HwCTP7(std::string const &ctp7Device,
                              uhal::HwInterface &uhalDevice)
    : gem::hw::GEMHwDevice::GEMHwDevice(ctp7Device, uhalDevice), m_crate(-1),
      m_slot(-1) {
  setDeviceBaseNode("CTP7");
  for (unsigned li = 0; li < N_GTX; ++li) {
    b_links[li] = false;
    CTP7IPBusCounters tmpGTXCounter;
    m_ipBusCounters.push_back(tmpGTXCounter);
  }
  CMSGEMOS_INFO("HwCTP7 ctor done, deviceBaseNode is " << getDeviceBaseNode());
}
gem::hw::ctp7::HwCTP7::HwCTP7(const int &crate, const int &slot)
    : gem::hw::GEMHwDevice::GEMHwDevice(
          toolbox::toString("gem.shelf%02d.ctp7%02d", crate, slot)),
      m_crate(crate), m_slot(slot) {
  CMSGEMOS_INFO("HwCTP7 ctor");
  setDeviceID(toolbox::toString("gem.shelf%02d.ctp7%02d", crate, slot));
  CMSGEMOS_INFO("getting the ConnectionManager pointer");
  p_gemConnectionManager.reset(new uhal::ConnectionManager(
      "file://${GEM_ADDRESS_TABLE_PATH}/connections_ch.xml"));
  CMSGEMOS_INFO("getting HwInterface " << getDeviceID()
                                       << " pointer from ConnectionManager");
  p_gemHW.reset(new uhal::HwInterface(
      p_gemConnectionManager->getDevice(this->getDeviceID())));
  CMSGEMOS_INFO("setting the device base node");
  setDeviceBaseNode("CTP7");
  for (unsigned li = 0; li < N_GTX; ++li) {
    b_links[li] = false;
    CTP7IPBusCounters tmpGTXCounter;
    m_ipBusCounters.push_back(tmpGTXCounter);
  }
  CMSGEMOS_INFO("HwCTP7 ctor done, deviceBaseNode is " << getDeviceBaseNode());
}
gem::hw::ctp7::HwCTP7::~HwCTP7() {}
bool gem::hw::ctp7::HwCTP7::isHwConnected() {
  if (b_is_connected) {
    CMSGEMOS_INFO("basic check: HwCTP7 connection good");
    return true;
  } else if (gem::hw::GEMHwDevice::isHwConnected()) {
    std::vector<linkStatus> tmp_activeLinks;
    tmp_activeLinks.reserve(N_GTX);
    for (unsigned int gtx = 0; gtx < N_GTX; ++gtx) {
      if ((this->getBoardID()).rfind("CTP7") != std::string::npos) {
        b_links[gtx] = true;
        CMSGEMOS_DEBUG("gtx" << gtx << " present(" << this->getFirmwareVer()
                             << ")");
        tmp_activeLinks.push_back(std::make_pair(gtx, this->LinkStatus(gtx)));
      } else {
        b_links[gtx] = false;
        CMSGEMOS_INFO("gtx" << gtx << " not reachable (unable to find 2 or 5 "
                                      "or 201 in the firmware string, "
                            << "or 'CTP7' in the board ID)"
                            << " board ID " << this->getBoardID()
                            << " user firmware version "
                            << this->getFirmwareVer());
      }
    }
    v_activeLinks = tmp_activeLinks;
    if (!v_activeLinks.empty()) {
      b_is_connected = true;
      CMSGEMOS_INFO("checked gtxs: HwCTP7 connection good");
      return true;
    } else {
      b_is_connected = false;
      return false;
    }
  } else {
    return false;
  }
}
std::string gem::hw::ctp7::HwCTP7::getBoardID() {
  std::string res = "???";
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.BOARD_ID");
  res = gem::utils::uint32ToString(val);
  return res;
}
uint32_t gem::hw::ctp7::HwCTP7::getBoardIDRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.BOARD_ID");
  return val;
}
std::string gem::hw::ctp7::HwCTP7::getSystemID() {
  std::string res = "???";
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.SYSTEM_ID");
  res = gem::utils::uint32ToString(val);
  return res;
}
uint32_t gem::hw::ctp7::HwCTP7::getSystemIDRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.SYSTEM_ID");
  return val;
}
std::string gem::hw::ctp7::HwCTP7::getIPAddress() {
  std::string res = "N/A";
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.IP_INFO");
  res = gem::utils::uint32ToDottedQuad(val);
  return res;
}
uint32_t gem::hw::ctp7::HwCTP7::getIPAddressRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "SYSTEM.IP_INFO");
  return val;
}
std::string gem::hw::ctp7::HwCTP7::getMACAddress() {
  std::string res = "N/A";
  uint32_t val1 = readReg(getDeviceBaseNode(), "SYSTEM.MAC.UPPER");
  uint32_t val2 = readReg(getDeviceBaseNode(), "SYSTEM.MAC.LOWER");
  res = gem::utils::uint32ToGroupedHex(val1, val2);
  return res;
}
uint64_t gem::hw::ctp7::HwCTP7::getMACAddressRaw() {
  uint32_t val1 = readReg(getDeviceBaseNode(), "SYSTEM.MAC.UPPER");
  uint32_t val2 = readReg(getDeviceBaseNode(), "SYSTEM.MAC.LOWER");
  return ((uint64_t)val1 << 32) + val2;
}
std::string gem::hw::ctp7::HwCTP7::getFirmwareDate(bool const &system) {
  std::stringstream res;
  std::stringstream regName;
  uint32_t fwid = readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.DATE");
  res << std::setfill('0') << std::setw(2) << (fwid & 0x1f) << "-"
      << std::setfill('0') << std::setw(2) << ((fwid >> 5) & 0x0f) << "-"
      << std::setw(4) << 2000 + ((fwid >> 9) & 0x7f);
  return res.str();
}
uint32_t gem::hw::ctp7::HwCTP7::getFirmwareDateRaw(bool const &system) {
  if (system)
    return readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.DATE");
  else
    return readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.DATE");
}
std::string gem::hw::ctp7::HwCTP7::getFirmwareVer(bool const &system) {
  std::stringstream res;
  std::stringstream regName;
  uint32_t fwid;
  if (system)
    fwid = readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.ID");
  else
    fwid = readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.ID");
  res << ((fwid >> 12) & 0x0f) << "." << ((fwid >> 8) & 0x0f) << "."
      << ((fwid) & 0xff);
  return res.str();
}
uint32_t gem::hw::ctp7::HwCTP7::getFirmwareVerRaw(bool const &system) {
  if (system)
    return readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.ID");
  else
    return readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE.ID");
}
void gem::hw::ctp7::HwCTP7::XPointControl(bool xpoint2, uint8_t const &input,
                                          uint8_t const &output) {
  if (xpoint2 && (input > 2 || output > 0)) {
    std::string msg = toolbox::toString(
        "Invalid clock routing for XPoint2 %d -> %d", input, output);
    CMSGEMOS_ERROR(msg);
    return;
  }
  if ((input > 3 || output > 3)) {
    std::string msg = toolbox::toString(
        "Invalid clock routing for XPoint%d %d -> %d", xpoint2, input, output);
    CMSGEMOS_ERROR(msg);
    return;
  }
  std::stringstream regName;
  if (xpoint2)
    regName << "SYSTEM.CLK_CTRL.XPOINT2";
  else
    regName << "SYSTEM.CLK_CTRL.XPOINT1";
  switch (output) {
  case(0) :
    regName << ".S1";
  case(1) :
    regName << ".S2";
  case(2) :
    regName << ".S3";
  case(3) :
    regName << ".S4";
  }
  writeReg(getDeviceBaseNode(), regName.str() + "1", input & 0x01);
  writeReg(getDeviceBaseNode(), regName.str() + "0", (input & 0x10) >> 1);
}
uint8_t gem::hw::ctp7::HwCTP7::XPointControl(bool xpoint2,
                                             uint8_t const &output) {
  if (output > 3) {
    std::string msg = toolbox::toString("Invalid clock output for XPoint%d %d",
                                        xpoint2, output);
    CMSGEMOS_ERROR(msg);
    return output;
  }
  std::stringstream regName;
  if (xpoint2)
    regName << "SYSTEM.CLK_CTRL.XPOINT2";
  else
    regName << "SYSTEM.CLK_CTRL.XPOINT1";
  switch (output) {
  case(0) :
    regName << ".S1";
  case(1) :
    regName << ".S2";
  case(2) :
    regName << ".S3";
  case(3) :
    regName << ".S4";
  }
  uint8_t input = 0x0;
  input |= (readReg(getDeviceBaseNode(), regName.str() + "0") & 0x1) << 1;
  input |= (readReg(getDeviceBaseNode(), regName.str() + "1") & 0x1);
  return input;
}
uint8_t gem::hw::ctp7::HwCTP7::SFPStatus(uint8_t const &sfpcage) {
  if (sfpcage < 1 || sfpcage > 4) {
    std::string msg = toolbox::toString(
        "Status requested for SFP (%d): outside expectation (1,4)", sfpcage);
    CMSGEMOS_ERROR(msg);
    return 0;
  }
  std::stringstream regName;
  regName << "SYSTEM.STATUS.SFP" << (int)sfpcage << ".STATUS";
  return (uint8_t)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::ctp7::HwCTP7::FMCPresence(bool fmc2) {
  std::stringstream regName;
  regName << "SYSTEM.STATUS.FMC" << (int)fmc2 + 1 << "_PRESENT";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::ctp7::HwCTP7::GbEInterrupt() {
  std::stringstream regName;
  regName << "SYSTEM.STATUS.GBE_INT";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::ctp7::HwCTP7::FPGAResetStatus() {
  std::stringstream regName;
  regName << "SYSTEM.STATUS.FPGA_RESET";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
uint8_t gem::hw::ctp7::HwCTP7::V6CPLDStatus() {
  std::stringstream regName;
  regName << "SYSTEM.STATUS.V6_CPLD";
  return (uint8_t)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::ctp7::HwCTP7::CDCELockStatus() {
  std::stringstream regName;
  regName << "SYSTEM.STATUS.CDCE_LOCK";
  return static_cast<bool>(readReg(getDeviceBaseNode(), regName.str()));
}
uint32_t gem::hw::ctp7::HwCTP7::getUserFirmware() {
  return readReg(getDeviceBaseNode(), "SYSTEM.FIRMWARE");
}
std::string gem::hw::ctp7::HwCTP7::getUserFirmwareDate() {
  std::stringstream res;
  res << "0x" << std::hex << getUserFirmware() << std::dec;
  return res.str();
}
bool gem::hw::ctp7::HwCTP7::linkCheck(uint8_t const &gtx,
                                      std::string const &opMsg) {
  if (gtx > N_GTX) {
    std::string msg = toolbox::toString(
        "%s requested for gtx (%d): outside expectation (0-%d)", opMsg.c_str(),
        gtx, N_GTX);
    CMSGEMOS_ERROR(msg);
    return false;
  } else if (!b_links[gtx]) {
    std::string msg =
        toolbox::toString("%s requested inactive gtx (%d)", opMsg.c_str(), gtx);
    CMSGEMOS_ERROR(msg);
    return false;
  }
  return true;
}
gem::hw::GEMHwDevice::OpticalLinkStatus
gem::hw::ctp7::HwCTP7::LinkStatus(uint8_t const &gtx) {
  gem::hw::GEMHwDevice::OpticalLinkStatus linkStatus;
  if (linkCheck(gtx, "Link status")) {
    linkStatus.TRK_Errors = readReg(
        getDeviceBaseNode(), toolbox::toString("COUNTERS.GTX%d.TRK_ERR", gtx));
    linkStatus.TRG_Errors = readReg(
        getDeviceBaseNode(), toolbox::toString("COUNTERS.GTX%d.TRG_ERR", gtx));
    linkStatus.Data_Packets =
        readReg(getDeviceBaseNode(),
                toolbox::toString("COUNTERS.GTX%d.DATA_Packets", gtx));
  }
  return linkStatus;
}
void gem::hw::ctp7::HwCTP7::LinkReset(uint8_t const &gtx,
                                      uint8_t const &resets) {
  if (linkCheck(gtx, "Link reset")) {
    if (resets & 0x1)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.GTX%d.TRK_ERR.Reset", gtx), 0x1);
    if (resets & 0x2)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.GTX%d.TRG_ERR.Reset", gtx), 0x1);
    if (resets & 0x4)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.GTX%d.DATA_Packets.Reset", gtx),
               0x1);
  }
}
gem::hw::ctp7::HwCTP7::CTP7IPBusCounters
gem::hw::ctp7::HwCTP7::getIPBusCounters(uint8_t const &gtx,
                                        uint8_t const &mode) {
  if (linkCheck(gtx, "IPBus counter")) {
    if (mode & 0x01)
      m_ipBusCounters.at(gtx).OptoHybridStrobe = readReg(
          getDeviceBaseNode(),
          toolbox::toString("COUNTERS.IPBus.Strobe.OptoHybrid_%d", gtx));
    if (mode & 0x02)
      m_ipBusCounters.at(gtx).OptoHybridAck =
          readReg(getDeviceBaseNode(),
                  toolbox::toString("COUNTERS.IPBus.Ack.OptoHybrid_%d", gtx));
    if (mode & 0x04)
      m_ipBusCounters.at(gtx).TrackingStrobe = readReg(
          getDeviceBaseNode(),
          toolbox::toString("COUNTERS.IPBus.Strobe.OptoHybrid_%d", gtx));
    if (mode & 0x08)
      m_ipBusCounters.at(gtx).TrackingAck =
          readReg(getDeviceBaseNode(),
                  toolbox::toString("COUNTERS.IPBus.Ack.OptoHybrid_%d", gtx));
    if (mode & 0x10)
      m_ipBusCounters.at(gtx).CounterStrobe =
          readReg(getDeviceBaseNode(),
                  toolbox::toString("COUNTERS.IPBus.Strobe.Counters"));
    if (mode & 0x20)
      m_ipBusCounters.at(gtx).CounterAck =
          readReg(getDeviceBaseNode(),
                  toolbox::toString("COUNTERS.IPBus.Ack.Counters"));
  }
  return m_ipBusCounters.at(gtx);
}
void gem::hw::ctp7::HwCTP7::resetIPBusCounters(uint8_t const &gtx,
                                               uint8_t const &resets) {
  if (linkCheck(gtx, "Reset IPBus counters")) {
    if (resets & 0x01)
      writeReg(
          getDeviceBaseNode(),
          toolbox::toString("COUNTERS.IPBus.Strobe.OptoHybrid_%d.Reset", gtx),
          0x1);
    if (resets & 0x02)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.IPBus.Ack.OptoHybrid_%d.Reset", gtx),
               0x1);
    if (resets & 0x04)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.IPBus.Strobe.TRK_%d.Reset", gtx),
               0x1);
    if (resets & 0x08)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.IPBus.Ack.TRK_%d.Reset", gtx), 0x1);
    if (resets & 0x10)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.IPBus.Strobe.Counters.Reset"), 0x1);
    if (resets & 0x20)
      writeReg(getDeviceBaseNode(),
               toolbox::toString("COUNTERS.IPBus.Ack.Counters.Reset"), 0x1);
  }
}
uint32_t gem::hw::ctp7::HwCTP7::readTriggerFIFO(uint8_t const &gtx) {
  return 0;
}
void gem::hw::ctp7::HwCTP7::flushTriggerFIFO(uint8_t const &gtx) { return; }
uint32_t gem::hw::ctp7::HwCTP7::getFIFOOccupancy(uint8_t const &gtx) {
  uint32_t fifocc = 0;
  if (linkCheck(gtx, "FIFO occupancy")) {
    std::stringstream regName;
    regName << "TRK_DATA.OptoHybrid_" << (int)gtx;
    fifocc = readReg(getDeviceBaseNode(), regName.str() + ".DEPTH");
    CMSGEMOS_DEBUG(toolbox::toString("getFIFOOccupancy(%d) %s.%s%s:: %d", gtx,
                                     getDeviceBaseNode().c_str(),
                                     regName.str().c_str(), ".DEPTH", fifocc));
  }
  return fifocc;
}
uint32_t gem::hw::ctp7::HwCTP7::getFIFOVFATBlockOccupancy(uint8_t const &gtx) {
  return getFIFOOccupancy(gtx) / 7;
}
bool gem::hw::ctp7::HwCTP7::hasTrackingData(uint8_t const &gtx) {
  bool hasData = false;
  if (linkCheck(gtx, "Tracking data")) {
    std::stringstream regName;
    regName << "TRK_DATA.OptoHybrid_" << (int)gtx << ".ISEMPTY";
    hasData = !readReg(getDeviceBaseNode(), regName.str());
  }
  return hasData;
}
std::vector<uint32_t>
gem::hw::ctp7::HwCTP7::getTrackingData(uint8_t const &gtx,
                                       size_t const &nBlocks) {
  if (!linkCheck(gtx, "Tracking data")) {
    std::vector<uint32_t> data(7 * nBlocks, 0x0);
    return data;
  }
  std::stringstream regName;
  regName << getDeviceBaseNode() << ".TRK_DATA.OptoHybrid_" << (int)gtx
          << ".FIFO";
  return readBlock(regName.str(), 7 * nBlocks);
}
uint32_t gem::hw::ctp7::HwCTP7::getTrackingData(uint8_t const &gtx,
                                                uint32_t *data,
                                                size_t const &nBlocks) {
  if (data == NULL) {
    std::string msg =
        toolbox::toString("Block read requested for null pointer");
    CMSGEMOS_ERROR(msg);
    XCEPT_RAISE(gem::hw::ctp7::exception::NULLReadoutPointer, msg);
  } else if (!linkCheck(gtx, "Tracking data")) {
    return 0;
  }
  std::stringstream regName;
  regName << getDeviceBaseNode() << ".TRK_DATA.OptoHybrid_" << (int)gtx
          << ".FIFO";
  return nBlocks;
}
void gem::hw::ctp7::HwCTP7::flushFIFO(uint8_t const &gtx) {
  if (linkCheck(gtx, "Flush FIFO")) {
    std::stringstream regName;
    regName << "TRK_DATA.OptoHybrid_" << (int)gtx;
    CMSGEMOS_INFO("Tracking FIFO" << (int)gtx << ":"
                                  << " ISFULL  0x" << std::hex
                                  << readReg(getDeviceBaseNode(),
                                             regName.str() + ".ISFULL")
                                  << std::dec << " ISEMPTY 0x" << std::hex
                                  << readReg(getDeviceBaseNode(),
                                             regName.str() + ".ISEMPTY")
                                  << std::dec << " Depth   0x" << std::hex
                                  << getFIFOOccupancy(gtx) << std::dec);
    writeReg(getDeviceBaseNode(), regName.str() + ".FLUSH", 0x1);
    CMSGEMOS_INFO("Tracking FIFO" << (int)gtx << ":"
                                  << " ISFULL  0x" << std::hex
                                  << readReg(getDeviceBaseNode(),
                                             regName.str() + ".ISFULL")
                                  << std::dec << " ISEMPTY 0x" << std::hex
                                  << readReg(getDeviceBaseNode(),
                                             regName.str() + ".ISEMPTY")
                                  << std::dec << " Depth   0x" << std::hex
                                  << getFIFOOccupancy(gtx) << std::dec);
  }
}
void gem::hw::ctp7::HwCTP7::enableDAQLink(uint32_t const &enableMask) {
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.INPUT_ENABLE_MASK", enableMask);
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.DAQ_ENABLE", 0x1);
}
void gem::hw::ctp7::HwCTP7::disableDAQLink() {
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.INPUT_ENABLE_MASK", 0x0);
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.DAQ_ENABLE", 0x0);
}
void gem::hw::ctp7::HwCTP7::resetDAQLink(uint32_t const &davTO) {
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.RESET", 0x1);
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.RESET", 0x0);
  disableDAQLink();
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.DAV_TIMEOUT", davTO);
  for (unsigned li = 0; li < N_GTX; ++li) {
    writeReg(getDeviceBaseNode(),
             toolbox::toString("DAQ.GTX%d.CONTROL.DAV_TIMEOUT", li), 0x30D4);
  }
  writeReg(getDeviceBaseNode(), "DAQ.CONTROL.TTS_OVERRIDE", 0x8);
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkControl() {
  return readReg(getDeviceBaseNode(), "DAQ.CONTROL");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkStatus() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS");
}
bool gem::hw::ctp7::HwCTP7::daqLinkReady() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS.DAQ_LINK_RDY");
}
bool gem::hw::ctp7::HwCTP7::daqClockLocked() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS.DAQ_CLK_LOCKED");
}
bool gem::hw::ctp7::HwCTP7::daqTTCReady() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS.TTC_RDY");
}
bool gem::hw::ctp7::HwCTP7::daqAlmostFull() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS.DAQ_AFULL");
}
uint8_t gem::hw::ctp7::HwCTP7::daqTTSState() {
  return readReg(getDeviceBaseNode(), "DAQ.STATUS.TTS_STATE");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkEventsSent() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.EVT_SENT");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkL1AID() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.L1AID");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkDisperErrors() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.DISPER_ERR");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkNonidentifiableErrors() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.NOTINTABLE_ERR");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkInputMask() {
  return readReg(getDeviceBaseNode(), "DAQ.CONTROL.INPUT_ENABLE_MASK");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkDAVTimeout() {
  return readReg(getDeviceBaseNode(), "DAQ.CONTROL.DAV_TIMEOUT");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkDAVTimer(bool const &max) {
  if (max)
    return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.MAX_DAV_TIMER");
  else
    return readReg(getDeviceBaseNode(), "DAQ.EXT_STATUS.LAST_DAV_TIMER");
}
uint32_t gem::hw::ctp7::HwCTP7::getLinkDAQStatus(uint8_t const &gtx) {
  std::stringstream regBase;
  regBase << "DAQ.GTX" << (int)gtx;
  return readReg(getDeviceBaseNode(), regBase.str() + ".STATUS");
}
uint32_t gem::hw::ctp7::HwCTP7::getLinkDAQCounters(uint8_t const &gtx,
                                                   uint8_t const &mode) {
  std::stringstream regBase;
  regBase << "DAQ.GTX" << (int)gtx << ".COUNTERS";
  if (mode == 0)
    return readReg(getDeviceBaseNode(),
                   regBase.str() + ".CORRUPT_VFAT_BLK_CNT");
  else
    return readReg(getDeviceBaseNode(), regBase.str() + ".EVN");
}
uint32_t gem::hw::ctp7::HwCTP7::getLinkLastDAQBlock(uint8_t const &gtx) {
  std::stringstream regBase;
  regBase << "DAQ.GTX" << (int)gtx;
  return readReg(getDeviceBaseNode(), regBase.str() + ".LASTBLOCK");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkInputTimeout() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_CONTROL.INPUT_TIMEOUT");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkRunType() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_CONTROL.RUN_TYPE");
}
uint32_t gem::hw::ctp7::HwCTP7::getDAQLinkRunParameters() {
  return readReg(getDeviceBaseNode(), "DAQ.EXT_CONTROL.RUN_PARAMS");
}
uint32_t
gem::hw::ctp7::HwCTP7::getDAQLinkRunParameter(uint8_t const &parameter) {
  std::stringstream regBase;
  regBase << "DAQ.EXT_CONTROL.RUN_PARAM" << (int)parameter;
  return readReg(getDeviceBaseNode(), regBase.str());
}
void gem::hw::ctp7::HwCTP7::setDAQLinkInputTimeout(uint32_t const &value) {
  for (unsigned li = 0; li < N_GTX; ++li) {
    writeReg(getDeviceBaseNode(),
             toolbox::toString("DAQ.GTX%d.CONTROL.DAV_TIMEOUT", li), 0x30D4);
  }
}
void gem::hw::ctp7::HwCTP7::setDAQLinkRunType(uint32_t const &value) {
  return writeReg(getDeviceBaseNode(), "DAQ.EXT_CONTROL.RUN_TYPE", value);
}
void gem::hw::ctp7::HwCTP7::setDAQLinkRunParameters(uint32_t const &value) {
  return writeReg(getDeviceBaseNode(), "DAQ.EXT_CONTROL.RUN_PARAMS", value);
}
void gem::hw::ctp7::HwCTP7::setDAQLinkRunParameter(uint8_t const &parameter,
                                                   uint8_t const &value) {
  if (parameter < 1 || parameter > 3) {
    std::string msg =
        toolbox::toString("Attempting to set DAQ link run parameter %d: "
                          "outside expectation (1-%d)",
                          (int)parameter, 3);
    CMSGEMOS_ERROR(msg);
    return;
  }
  std::stringstream regBase;
  regBase << "DAQ.EXT_CONTROL.RUN_PARAM" << (int)parameter;
  writeReg(getDeviceBaseNode(), regBase.str(), value);
}
uint32_t gem::hw::ctp7::HwCTP7::getTTCControl() {
  return readReg(getDeviceBaseNode(), "TTC.CONTROL");
}
gem::hw::ctp7::TTCEncoding gem::hw::ctp7::HwCTP7::getTTCEncoding() {
  return (TTCEncoding)readReg(getDeviceBaseNode(), "TTC.CONTROL.GEMFORMAT");
}
bool gem::hw::ctp7::HwCTP7::getL1AInhibit() {
  return readReg(getDeviceBaseNode(), "TTC.CONTROL.INHIBIT_L1A");
}
uint32_t gem::hw::ctp7::HwCTP7::getTTCSpyBuffer() {
  return readReg(getDeviceBaseNode(), "TTC.SPY");
}
void gem::hw::ctp7::HwCTP7::setTTCEncoding(CTP7TTCEncoding ttc_enc) {
  return writeReg(getDeviceBaseNode(), "TTC.CONTROL.GEMFORMAT",
                  (uint32_t)ttc_enc);
}
void gem::hw::ctp7::HwCTP7::setL1AInhibit(bool inhibit) {
  return writeReg(getDeviceBaseNode(), "TTC.CONTROL.INHIBIT_L1A",
                  (uint32_t)inhibit);
}
void gem::hw::ctp7::HwCTP7::resetTTC() {
  return writeReg(getDeviceBaseNode(), "TTC.CONTROL.RESET", 0x1);
}
void gem::hw::ctp7::HwCTP7::generalReset() {
  counterReset();
  for (unsigned gtx = 0; gtx < N_GTX; ++gtx)
    linkReset(gtx);
  return;
}
void gem::hw::ctp7::HwCTP7::counterReset() {
  resetT1Counters();
  for (unsigned gtx = 0; gtx < N_GTX; ++gtx)
    resetIPBusCounters(gtx, 0xff);
  linkCounterReset();
  return;
}
void gem::hw::ctp7::HwCTP7::resetT1Counters() {
  WARN("HwCTP7::resetT1Counters is obsolete and will be removed in a future "
       "release");
  writeReg(getDeviceBaseNode(), "T1.L1A.RESET", 0x1);
  writeReg(getDeviceBaseNode(), "T1.CalPulse.RESET", 0x1);
  writeReg(getDeviceBaseNode(), "T1.Resync.RESET", 0x1);
  writeReg(getDeviceBaseNode(), "T1.BC0.RESET", 0x1);
  return;
}
void gem::hw::ctp7::HwCTP7::linkCounterReset() { return; }
void gem::hw::ctp7::HwCTP7::linkReset(uint8_t const &gtx) { return; }
