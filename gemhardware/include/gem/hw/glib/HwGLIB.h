#ifndef GEM_HW_GLIB_HWGLIB_H
#define GEM_HW_GLIB_HWGLIB_H
#include "gem/hw/HwGenericAMC.h"
#include "gem/hw/glib/exception/Exception.h"
#include "gem/hw/glib/GLIBSettingsEnums.h"
namespace gem {
namespace hw {
namespace glib {
class HwGLIB : public gem::hw::HwGenericAMC {
public:
  HwGLIB(std::string const &glibDevice, std::string const &connectionFile);
  HwGLIB(std::string const &glibDevice, std::string const &connectionURI,
         std::string const &addressTable);
  HwGLIB(std::string const &glibDevice, uhal::HwInterface &uhalDevice);
  virtual ~HwGLIB();
  std::string getBoardID();
  uint32_t getBoardIDRaw();
  std::string getSystemID();
  uint32_t getSystemIDRaw();
  std::string getIPAddress();
  uint32_t getIPAddressRaw();
  std::string getMACAddress();
  uint64_t getMACAddressRaw();
  void PCIeClkFSel(uint8_t const &factor) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "PCIE_CLK_FSEL",
             (uint32_t)factor);
  };
  uint8_t PCIeClkFSel() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(),
                            regName.str() + "PCIE_CLK_FSEL");
  };
  void PCIeClkMaster(bool reset) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "PCIE_CLK_MR",
             (uint32_t)reset);
  };
  uint8_t PCIeClkMaster() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(), regName.str() + "PCIE_CLK_MR");
  };
  void PCIeClkOutput(bool enable) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "PCIE_CLK_OE",
             (uint32_t)enable);
  };
  uint8_t PCIeClkOutput() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(), regName.str() + "PCIE_CLK_OE");
  };
  void CDCEPower(bool powerup) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "CDCE_POWERUP",
             (uint32_t)powerup);
  };
  uint8_t CDCEPower() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(),
                            regName.str() + "CDCE_POWERUP");
  };
  void CDCEReference(bool refsrc) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "CDCE_REFSEL",
             (uint32_t)refsrc);
  };
  uint8_t CDCEReference() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(), regName.str() + "CDCE_REFSEL");
  };
  void CDCESync(bool sync) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    if (sync) {
      writeReg(getDeviceBaseNode(), regName.str() + "CDCE_SYNC", 0x0);
      writeReg(getDeviceBaseNode(), regName.str() + "CDCE_SYNC", 0x1);
    }
  };
  uint8_t CDCESync() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(), regName.str() + "CDCE_SYNC");
  };
  void CDCEControl(bool source) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "CDCE_CTRLSEL",
             (uint32_t)source);
  };
  uint8_t CDCEControl() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(),
                            regName.str() + "CDCE_CTRLSEL");
  };
  void TClkBOutput(bool enable) {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    writeReg(getDeviceBaseNode(), regName.str() + "TCLKB_DR_EN",
             (uint32_t)enable);
  };
  uint8_t TClkBOutput() {
    std::stringstream regName;
    regName << "SYSTEM.CLK_CTRL.";
    return (uint8_t)readReg(getDeviceBaseNode(), regName.str() + "TCLKB_DR_EN");
  };
  void XPointControl(bool xpoint2, uint8_t const &input, uint8_t const &output);
  uint8_t XPointControl(bool xpoint2, uint8_t const &output);
  uint8_t SFPStatus(uint8_t const &sfpcage);
  bool FMCPresence(bool fmc2);
  bool GbEInterrupt();
  bool FPGAResetStatus();
  uint8_t V6CPLDStatus();
  bool CDCELockStatus();
  uint32_t getFIFOOccupancy(uint8_t const &gtx);
  uint32_t getFIFOVFATBlockOccupancy(uint8_t const &gtx);
  bool hasTrackingData(uint8_t const &gtx);
  std::vector<uint32_t> getTrackingData(uint8_t const &gtx,
                                        size_t const &nBlocks = 1);
  uint32_t getTrackingData(uint8_t const &gtx, uint32_t *data,
                           size_t const &nBlocks = 1);
  void flushFIFO(uint8_t const &gtx);

protected:
private:
};
}
}
}
#endif
