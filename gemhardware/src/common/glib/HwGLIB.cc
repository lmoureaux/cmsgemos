#include <iomanip>
#include "gem/hw/glib/HwGLIB.h"
gem::hw::glib::HwGLIB::HwGLIB(std::string const &glibDevice,
                              std::string const &connectionFile)
    : gem::hw::HwGenericAMC::HwGenericAMC(glibDevice, connectionFile) {
  CMSGEMOS_INFO("HwGLIB::HwGLIB trying to create HwGLIB(" << glibDevice << ","
                                                          << connectionFile);
  this->setExpectedBoardID("GLIB");
  CMSGEMOS_INFO("HwGLIB::HwGLIB ctor done " << isHwConnected());
}
gem::hw::glib::HwGLIB::HwGLIB(std::string const &glibDevice,
                              std::string const &connectionURI,
                              std::string const &addressTable)
    : gem::hw::HwGenericAMC::HwGenericAMC(glibDevice, connectionURI,
                                          addressTable) {
  CMSGEMOS_INFO("HwGLIB::HwGLIB trying to create HwGLIB("
                << glibDevice << "," << connectionURI << "," << addressTable);
  this->setExpectedBoardID("GLIB");
  CMSGEMOS_INFO("HwGLIB::HwGLIB ctor done " << isHwConnected());
}
gem::hw::glib::HwGLIB::HwGLIB(std::string const &glibDevice,
                              uhal::HwInterface &uhalDevice)
    : gem::hw::HwGenericAMC::HwGenericAMC(glibDevice, uhalDevice) {
  CMSGEMOS_INFO("HwGLIB::HwGLIB ctor");
  this->setExpectedBoardID("GLIB");
  CMSGEMOS_INFO("HwGLIB::HwGLIB ctor done " << isHwConnected());
}
gem::hw::glib::HwGLIB::~HwGLIB() {}
std::string gem::hw::glib::HwGLIB::getBoardID() {
  std::string res = "N/A";
  res = gem::utils::uint32ToString(getBoardIDRaw());
  return res;
}
uint32_t gem::hw::glib::HwGLIB::getBoardIDRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "GLIB_SYSTEM.SYSTEM.BOARD_ID");
  return val;
}
std::string gem::hw::glib::HwGLIB::getSystemID() {
  std::string res = "N/A";
  res = gem::utils::uint32ToString(getSystemIDRaw());
  return res;
}
uint32_t gem::hw::glib::HwGLIB::getSystemIDRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "GLIB_SYSTEM.SYSTEM.SYSTEM_ID");
  return val;
}
std::string gem::hw::glib::HwGLIB::getIPAddress() {
  std::string res = "N/A";
  res = gem::utils::uint32ToDottedQuad(getIPAddressRaw());
  return res;
}
uint32_t gem::hw::glib::HwGLIB::getIPAddressRaw() {
  uint32_t val = readReg(getDeviceBaseNode(), "GLIB_SYSTEM.SYSTEM.IP_INFO");
  return val;
}
std::string gem::hw::glib::HwGLIB::getMACAddress() {
  std::string res = "N/A";
  uint64_t val = getMACAddressRaw();
  res = gem::utils::uint32ToGroupedHex((val >> 32) & 0xffffffff,
                                       val & 0xffffffff);
  return res;
}
uint64_t gem::hw::glib::HwGLIB::getMACAddressRaw() {
  uint32_t val1 = readReg(getDeviceBaseNode(), "GLIB_SYSTEM.SYSTEM.MAC.UPPER");
  uint32_t val2 = readReg(getDeviceBaseNode(), "GLIB_SYSTEM.SYSTEM.MAC.LOWER");
  return ((uint64_t)val1 << 32) + val2;
}
void gem::hw::glib::HwGLIB::XPointControl(bool xpoint2, uint8_t const &input,
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
    regName << "GLIB_SYSTEM.SYSTEM.CLK_CTRL.XPOINT2";
  else
    regName << "GLIB_SYSTEM.SYSTEM.CLK_CTRL.XPOINT1";
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
uint8_t gem::hw::glib::HwGLIB::XPointControl(bool xpoint2,
                                             uint8_t const &output) {
  if (output > 3) {
    std::string msg = toolbox::toString("Invalid clock output for XPoint%d %d",
                                        xpoint2, output);
    CMSGEMOS_ERROR(msg);
    return output;
  }
  std::stringstream regName;
  if (xpoint2)
    regName << "GLIB_SYSTEM.SYSTEM.CLK_CTRL.XPOINT2";
  else
    regName << "GLIB_SYSTEM.SYSTEM.CLK_CTRL.XPOINT1";
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
uint8_t gem::hw::glib::HwGLIB::SFPStatus(uint8_t const &sfpcage) {
  if (sfpcage < 1 || sfpcage > 4) {
    std::string msg = toolbox::toString(
        "Status requested for SFP (%d): outside expectation (1,4)", sfpcage);
    CMSGEMOS_ERROR(msg);
    return 0;
  }
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.SFP" << (int)sfpcage << ".STATUS";
  return (uint8_t)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::glib::HwGLIB::FMCPresence(bool fmc2) {
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.FMC" << (int)fmc2 + 1 << "_PRESENT";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::glib::HwGLIB::GbEInterrupt() {
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.GBE_INT";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::glib::HwGLIB::FPGAResetStatus() {
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.FPGA_RESET";
  return (bool)readReg(getDeviceBaseNode(), regName.str());
}
uint8_t gem::hw::glib::HwGLIB::V6CPLDStatus() {
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.V6_CPLD";
  return (uint8_t)readReg(getDeviceBaseNode(), regName.str());
}
bool gem::hw::glib::HwGLIB::CDCELockStatus() {
  std::stringstream regName;
  regName << "GLIB_SYSTEM.SYSTEM.STATUS.CDCE_LOCK";
  return static_cast<bool>(readReg(getDeviceBaseNode(), regName.str()));
}
uint32_t gem::hw::glib::HwGLIB::getFIFOOccupancy(uint8_t const &gtx) {
  return 0x0;
}
uint32_t gem::hw::glib::HwGLIB::getFIFOVFATBlockOccupancy(uint8_t const &gtx) {
  return 0x0;
}
bool gem::hw::glib::HwGLIB::hasTrackingData(uint8_t const &gtx) {
  return false;
}
std::vector<uint32_t>
gem::hw::glib::HwGLIB::getTrackingData(uint8_t const &gtx,
                                       size_t const &nBlocks) {
  std::vector<uint32_t> res;
  return res;
}
uint32_t gem::hw::glib::HwGLIB::getTrackingData(uint8_t const &gtx,
                                                uint32_t *data,
                                                size_t const &nBlocks) {
  return 0x0;
}
void gem::hw::glib::HwGLIB::flushFIFO(uint8_t const &gtx) { return; }
