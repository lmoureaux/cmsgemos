#ifndef GEM_HW_CTP7_HWCTP7_H
#define GEM_HW_CTP7_HWCTP7_H
#include "gem/hw/GEMHwDevice.h"
#include "gem/hw/ctp7/exception/Exception.h"
#include "gem/hw/ctp7/CTP7SettingsEnums.h"
namespace gem {
namespace hw {
namespace ctp7 {
class HwCTP7 : public gem::hw::HwGenericAMC {
public:
  static const unsigned N_GTX = 2;
  typedef struct CTP7IPBusCounters {
    uint32_t OptoHybridStrobe;
    uint32_t OptoHybridAck;
    uint32_t TrackingStrobe;
    uint32_t TrackingAck;
    uint32_t CounterStrobe;
    uint32_t CounterAck;
    CTP7IPBusCounters()
        : OptoHybridStrobe(0), OptoHybridAck(0), TrackingStrobe(0),
          TrackingAck(0), CounterStrobe(0), CounterAck(0) {};
    void reset() {
      OptoHybridStrobe = 0;
      OptoHybridAck = 0;
      TrackingStrobe = 0;
      TrackingAck = 0;
      CounterStrobe = 0;
      CounterAck = 0;
      return;
    };
  } CTP7IPBusCounters;
  HwCTP7();
  HwCTP7(std::string const &ctp7Device, std::string const &connectionFile);
  HwCTP7(std::string const &ctp7Device, std::string const &connectionURI,
         std::string const &addressTable);
  HwCTP7(std::string const &ctp7Device, uhal::HwInterface &uhalDevice);
  HwCTP7(int const &crate, int const &slot);
  virtual ~HwCTP7();
  bool isHwConnected();
  std::string getBoardID();
  uint32_t getBoardIDRaw();
  std::string getSystemID();
  uint32_t getSystemIDRaw();
  std::string getIPAddress();
  uint32_t getIPAddressRaw();
  std::string getMACAddress();
  uint64_t getMACAddressRaw();
  std::string getFirmwareVer(bool const &system = true);
  uint32_t getFirmwareVerRaw(bool const &system = true);
  std::string getFirmwareDate(bool const &system = true);
  uint32_t getFirmwareDateRaw(bool const &system = true);
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
  uint32_t getUserFirmware();
  std::string getUserFirmwareDate();

private:
  bool linkCheck(uint8_t const &gtx, std::string const &opMsg);

public:
  GEMHwDevice::OpticalLinkStatus LinkStatus(uint8_t const &gtx);
  void LinkReset(uint8_t const &gtx, uint8_t const &resets);
  void ResetLinks(uint8_t const &resets) {
    for (auto gtx = v_activeLinks.begin(); gtx != v_activeLinks.end(); ++gtx)
      LinkReset(gtx->first, resets);
  };
  void setTrigSource(uint8_t const &mode, uint8_t const &gtx = 0x0) {
    return;
  };
  uint8_t getTrigSource(uint8_t const &gtx = 0x0) {
    return 0;
  };
  void setSBitSource(uint8_t const &mode, uint8_t const &gtx = 0x0) {
    return;
  };
  uint8_t getSBitSource(uint8_t const &gtx = 0x0) {
    return 0;
  };
  CTP7IPBusCounters getIPBusCounters(uint8_t const &gtx, uint8_t const &mode);
  uint32_t getL1ACount() {
    return readReg(getDeviceBaseNode(), "COUNTERS.T1.L1A");
  };
  uint32_t getCalPulseCount() {
    return readReg(getDeviceBaseNode(), "COUNTERS.T1.CalPulse");
  };
  uint32_t getResyncCount() {
    return readReg(getDeviceBaseNode(), "COUNTERS.T1.Resync");
  };
  uint32_t getBC0Count() {
    return readReg(getDeviceBaseNode(), "COUNTERS.T1.BC0");
  };
  void resetIPBusCounters(uint8_t const &gtx, uint8_t const &mode);
  void resetL1ACount() {
    return writeReg(getDeviceBaseNode(), "COUNTERS.T1.L1A.Reset", 0x1);
  };
  void resetCalPulseCount() {
    return writeReg(getDeviceBaseNode(), "COUNTERS.T1.CalPulse.Reset", 0x1);
  };
  void resetResyncCount() {
    return writeReg(getDeviceBaseNode(), "COUNTERS.T1.Resync.Reset", 0x1);
  };
  void resetBC0Count() {
    return writeReg(getDeviceBaseNode(), "COUNTERS.T1.BC0.Reset", 0x1);
  };
  uint32_t readTriggerFIFO(uint8_t const &gtx);
  void flushTriggerFIFO(uint8_t const &gtx);
  uint32_t getFIFOOccupancy(uint8_t const &gtx);
  uint32_t getFIFOVFATBlockOccupancy(uint8_t const &gtx);
  bool hasTrackingData(uint8_t const &gtx);
  std::vector<uint32_t> getTrackingData(uint8_t const &gtx,
                                        size_t const &nBlocks = 1);
  uint32_t getTrackingData(uint8_t const &gtx, uint32_t *data,
                           size_t const &nBlocks = 1);
  void flushFIFO(uint8_t const &gtx);
  void enableDAQLink(uint32_t const &enableMask = 0x1);
  void disableDAQLink();
  void resetDAQLink(uint32_t const &davTO = 0x3d090);
  uint32_t getDAQLinkControl();
  uint32_t getDAQLinkStatus();
  bool daqLinkReady();
  bool daqClockLocked();
  bool daqTTCReady();
  bool daqAlmostFull();
  uint8_t daqTTSState();
  uint32_t getDAQLinkEventsSent();
  uint32_t getDAQLinkL1AID();
  uint32_t getDAQLinkDisperErrors();
  uint32_t getDAQLinkNonidentifiableErrors();
  uint32_t getDAQLinkInputMask();
  uint32_t getDAQLinkDAVTimeout();
  uint32_t getDAQLinkDAVTimer(bool const &max);
  uint32_t getLinkDAQStatus(uint8_t const &gtx);
  uint32_t getLinkDAQCounters(uint8_t const &gtx, uint8_t const &mode);
  uint32_t getLinkLastDAQBlock(uint8_t const &gtx);
  uint32_t getDAQLinkInputTimeout();
  uint32_t getDAQLinkRunType();
  uint32_t getDAQLinkRunParameters();
  uint32_t getDAQLinkRunParameter(uint8_t const &parameter);
  void setDAQLinkInputTimeout(uint32_t const &value);
  void setDAQLinkRunType(uint32_t const &value);
  void setDAQLinkRunParameters(uint32_t const &value);
  void setDAQLinkRunParameter(uint8_t const &parameter, uint8_t const &value);
  uint32_t getTTCControl();
  CTP7TTCEncoding getTTCEncoding();
  void setTTCEncoding(CTP7TTCEncoding ttc_enc);
  bool getL1AInhibit();
  void setL1AInhibit(bool inhibit);
  void resetTTC();
  uint32_t getTTCSpyBuffer();
  virtual void generalReset();
  virtual void counterReset();
  void resetT1Counters();
  void resetLinkCounters();
  virtual void linkReset(uint8_t const &link);
  std::vector<CTP7IPBusCounters> m_ipBusCounters;

protected:
  bool b_links[N_GTX];
  std::vector<linkStatus> v_activeLinks;

private:
  int m_crate, m_slot;
};
}
}
}
#endif
