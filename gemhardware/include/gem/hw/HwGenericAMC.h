#ifndef GEM_HW_HWGENERICAMC_H
#define GEM_HW_HWGENERICAMC_H
#include "gem/hw/GEMHwDevice.h"
#include "gem/hw/exception/Exception.h"
#include "gem/hw/GenericAMCSettingsEnums.h"
namespace gem {
namespace hw {
class HwGenericAMC : public gem::hw::GEMHwDevice {
public:
  typedef struct AMCIPBusCounters {
    uint32_t OptoHybridStrobe;
    uint32_t OptoHybridAck;
    uint32_t TrackingStrobe;
    uint32_t TrackingAck;
    uint32_t CounterStrobe;
    uint32_t CounterAck;
    AMCIPBusCounters()
        : OptoHybridStrobe(0), OptoHybridAck(0), TrackingStrobe(0),
          TrackingAck(0), CounterStrobe(0), CounterAck(0) {}
    void reset() {
      OptoHybridStrobe = 0;
      OptoHybridAck = 0;
      TrackingStrobe = 0;
      TrackingAck = 0;
      CounterStrobe = 0;
      CounterAck = 0;
      return;
    }
  } AMCIPBusCounters;
  HwGenericAMC(std::string const &amcDevice, std::string const &connectionFile);
  HwGenericAMC(std::string const &amcDevice, std::string const &connectionURI,
               std::string const &addressTable);
  HwGenericAMC(std::string const &amcDevice, uhal::HwInterface &uhalDevice);
  virtual ~HwGenericAMC();
  virtual bool isHwConnected() override;
  virtual std::string getBoardID();
  virtual uint32_t getBoardIDRaw();
  uint32_t getSupportedOptoHybrids() {
    return readReg(getDeviceBaseNode(), "GEM_SYSTEM.CONFIG.NUM_OF_OH");
  }
  uint32_t supportsGBTLink() {
    return readReg(getDeviceBaseNode(), "GEM_SYSTEM.CONFIG.USE_GBT");
  }
  uint32_t supportsTriggerLink() {
    return readReg(getDeviceBaseNode(), "GEM_SYSTEM.CONFIG.USE_TRIG_LINKS");
  }
  virtual std::string getFirmwareVer(bool const &system = true);
  virtual uint32_t getFirmwareVerRaw(bool const &system = true);
  virtual std::string getFirmwareDate(bool const &system = true);
  virtual uint32_t getFirmwareDateRaw(bool const &system = true);
  virtual uint32_t getUserFirmware();
  virtual std::string getUserFirmwareDate();

private:
  void connectRPC(bool reconnect = false) override;
  virtual bool linkCheck(uint8_t const &gtx, std::string const &opMsg);

public:
  virtual GEMHwDevice::OpticalLinkStatus LinkStatus(uint8_t const &gtx);
  virtual void LinkReset(uint8_t const &gtx, uint8_t const &resets);
  virtual void ResetLinks(uint8_t const &resets) {
    for (auto gtx = v_activeLinks.begin(); gtx != v_activeLinks.end(); ++gtx)
      LinkReset(gtx->first, resets);
  }
  virtual void setTrigSource(uint8_t const &mode, uint8_t const &gtx = 0x0) {
    return;
  }
  virtual uint8_t getTrigSource(uint8_t const &gtx = 0x0) { return 0; }
  virtual void setSBitSource(uint8_t const &mode, uint8_t const &gtx = 0x0) {
    return;
  }
  virtual uint8_t getSBitSource(uint8_t const &gtx = 0x0) { return 0; }
  virtual AMCIPBusCounters getIPBusCounters(uint8_t const &gtx,
                                            uint8_t const &mode);
  uint32_t getL1ACount() {
    return readReg(getDeviceBaseNode(), "TTC.CMD_COUNTERS.L1A");
  }
  uint32_t getCalPulseCount() {
    return readReg(getDeviceBaseNode(), "TTC.CMD_COUNTERS.CALPULSE");
  }
  uint32_t getResyncCount() {
    return readReg(getDeviceBaseNode(), "TTC.CMD_COUNTERS.RESYNC");
  }
  uint32_t getBC0Count() {
    return readReg(getDeviceBaseNode(), "TTC.CMD_COUNTERS.BC0");
  }
  virtual void resetIPBusCounters(uint8_t const &gtx, uint8_t const &mode);
  void resetL1ACount() {
    return writeReg(getDeviceBaseNode(), "TTC.CTRL.CNT_RESET", 0x1);
  }
  void resetCalPulseCount() {
    return writeReg(getDeviceBaseNode(), "TTC.CTRL.CNT_RESET", 0x1);
  }
  void resetResyncCount() {
    return writeReg(getDeviceBaseNode(), "TTC.CTRL.CNT_RESET", 0x1);
  }
  void resetBC0Count() {
    return writeReg(getDeviceBaseNode(), "TTC.CTRL.CNT_RESET", 0x1);
  }
  uint32_t readTriggerFIFO(uint8_t const &gtx);
  void flushTriggerFIFO(uint8_t const &gtx);
  void configureDAQModule(bool enableZS, bool doPhaseShift,
                          uint32_t const &runType = 0x0,
                          uint32_t const &marker = 0xfaac, bool relock = false,
                          bool bc0LockPSMode = false);
  void enableDAQLink(uint32_t const &enableMask = 0x1);
  void disableDAQLink();
  void setZS(bool enable = true);
  void disableZS() {
    setZS(false);
  };
  void resetDAQLink(uint32_t const &davTO = 0x500,
                    uint32_t const &ttsOverride = 0x0);
  uint32_t getDAQLinkControl();
  uint32_t getDAQLinkStatus();
  bool daqLinkReady();
  bool daqClockLocked();
  bool daqTTCReady();
  uint8_t daqTTSState();
  bool daqAlmostFull();
  bool l1aFIFOIsEmpty();
  bool l1aFIFOIsAlmostFull();
  bool l1aFIFOIsFull();
  bool l1aFIFOIsUnderflow();
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
  void setDAQLinkInputTimeout(uint32_t const &value = 0x100);
  void setDAQLinkRunType(uint32_t const &value);
  void setDAQLinkRunParameters(uint32_t const &value);
  void setDAQLinkRunParameter(uint8_t const &parameter, uint8_t const &value);
  void ttcModuleReset();
  void ttcMMCMReset();
  void ttcMMCMPhaseShift(bool relock = false, bool modeBC0 = false,
                         bool scan = false);
  int checkPLLLock(uint32_t readAttempts);
  double getMMCMPhaseMean(uint32_t readAttempts);
  double getMMCMPhaseMedian(uint32_t readAttempts);
  double getGTHPhaseMean(uint32_t readAttempts);
  double getGTHPhaseMedian(uint32_t readAttempts);
  virtual void ttcCounterReset();
  virtual bool getL1AEnable();
  virtual void setL1AEnable(bool enable = true);
  virtual uint32_t getTTCConfig(AMCTTCCommandT const &cmd);
  virtual void setTTCConfig(AMCTTCCommandT const &cmd, uint8_t const &value);
  virtual uint32_t getTTCStatus();
  virtual uint32_t getTTCErrorCount(bool const &single = true);
  virtual uint32_t getTTCCounter(AMCTTCCommandT const &cmd);
  virtual uint32_t getL1AID();
  virtual uint32_t getL1ARate();
  virtual uint32_t getTTCSpyBuffer();
  virtual void scaHardResetEnable(bool const &en = false);
  virtual void triggerReset();
  virtual void triggerCounterReset();
  virtual uint32_t getOptoHybridKillMask();
  virtual void setOptoHybridKillMask(uint32_t const &mask);
  virtual uint32_t getORTriggerRate();
  virtual uint32_t getORTriggerCount();
  virtual uint32_t getOptoHybridTriggerRate(uint8_t const &oh);
  virtual uint32_t getOptoHybridTriggerCount(uint8_t const &oh);
  virtual uint32_t getOptoHybridClusterRate(uint8_t const &oh,
                                            uint8_t const &cs);
  virtual uint32_t getOptoHybridClusterCount(uint8_t const &oh,
                                             uint8_t const &cs);
  virtual uint32_t getOptoHybridDebugLastCluster(uint8_t const &oh,
                                                 uint8_t const &cs);
  virtual uint32_t getOptoHybridTriggerLinkCount(uint8_t const &oh,
                                                 uint8_t const &link,
                                                 AMCOHLinkCountT const &count);
  virtual void generalReset();
  virtual void counterReset();
  virtual void resetT1Counters();
  virtual void linkCounterReset();
  virtual void linkReset(uint8_t const &link);
  std::vector<AMCIPBusCounters> m_ipBusCounters;

protected:
  bool b_links[gem::hw::utils::N_GTX];
  uint32_t m_links;
  uint32_t m_maxLinks;
  std::vector<linkStatus> v_activeLinks;
  void setExpectedBoardID(std::string const &boardID) { m_boardID = boardID; }
  std::string m_boardID;
  int m_crate;
  int m_slot;

private:
  HwGenericAMC();
  HwGenericAMC(const HwGenericAMC &other);
  HwGenericAMC &operator=(const HwGenericAMC &);
};
}
}
#endif
