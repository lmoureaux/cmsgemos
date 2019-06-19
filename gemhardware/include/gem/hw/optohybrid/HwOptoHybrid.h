#ifndef GEM_HW_OPTOHYBRID_HWOPTOHYBRID_H
#define GEM_HW_OPTOHYBRID_HWOPTOHYBRID_H
#include "gem/hw/GEMHwDevice.h"
#include "gem/hw/glib/HwGLIB.h"
#include "gem/hw/vfat/HwVFAT2.h"
#include "gem/hw/optohybrid/exception/Exception.h"
#include "gem/hw/optohybrid/OptoHybridSettingsEnums.h"
namespace gem {
namespace hw {
namespace optohybrid {
class HwOptoHybrid : public gem::hw::GEMHwDevice {
public:
  typedef struct OptoHybridWBMasterCounters {
    std::pair<uint32_t, uint32_t> GTX;
    std::pair<uint32_t, uint32_t> GBT;
    std::pair<uint32_t, uint32_t> ExtI2C;
    std::pair<uint32_t, uint32_t> Scan;
    std::pair<uint32_t, uint32_t> DAC;
    OptoHybridWBMasterCounters()
        : GTX(std::make_pair(0, 0)), GBT(std::make_pair(0, 0)),
          ExtI2C(std::make_pair(0, 0)), Scan(std::make_pair(0, 0)),
          DAC(std::make_pair(0, 0)) {};
    void reset() {
      GTX = std::make_pair(0, 0);
      GBT = std::make_pair(0, 0);
      ExtI2C = std::make_pair(0, 0);
      Scan = std::make_pair(0, 0);
      DAC = std::make_pair(0, 0);
      return;
    };
  } OptoHybridWBMasterCounters;
  typedef struct OptoHybridWBSlaveCounters {
    std::vector<std::pair<uint32_t, uint32_t> > I2C;
    std::pair<uint32_t, uint32_t> ExtI2C;
    std::pair<uint32_t, uint32_t> Scan;
    std::pair<uint32_t, uint32_t> T1;
    std::pair<uint32_t, uint32_t> DAC;
    std::pair<uint32_t, uint32_t> ADC;
    std::pair<uint32_t, uint32_t> Clocking;
    std::pair<uint32_t, uint32_t> Counters;
    std::pair<uint32_t, uint32_t> System;
    OptoHybridWBSlaveCounters()
        : I2C(6, std::make_pair(0, 0)), ExtI2C(std::make_pair(0, 0)),
          Scan(std::make_pair(0, 0)), T1(std::make_pair(0, 0)),
          DAC(std::make_pair(0, 0)), ADC(std::make_pair(0, 0)),
          Clocking(std::make_pair(0, 0)), Counters(std::make_pair(0, 0)),
          System(std::make_pair(0, 0)) {};
    void reset() {
      std::fill(I2C.begin(), I2C.end(), std::make_pair(0, 0));
      ExtI2C = std::make_pair(0, 0);
      Scan = std::make_pair(0, 0);
      T1 = std::make_pair(0, 0);
      DAC = std::make_pair(0, 0);
      ADC = std::make_pair(0, 0);
      Clocking = std::make_pair(0, 0);
      Counters = std::make_pair(0, 0);
      System = std::make_pair(0, 0);
      return;
    };
  } OptoHybridWBSlaveCounters;
  typedef struct OptoHybridT1Counters {
    std::vector<uint32_t> GTX_TTC;
    std::vector<uint32_t> GBT_TTC;
    std::vector<uint32_t> Firmware;
    std::vector<uint32_t> External;
    std::vector<uint32_t> Loopback;
    std::vector<uint32_t> Sent;
    OptoHybridT1Counters()
        : GTX_TTC(4, 0), GBT_TTC(4, 0), Firmware(4, 0), External(4, 0),
          Loopback(4, 0), Sent(4, 0) {};
    void reset() {
      std::fill(GTX_TTC.begin(), GTX_TTC.end(), 0);
      std::fill(GBT_TTC.begin(), GBT_TTC.end(), 0);
      std::fill(Firmware.begin(), Firmware.end(), 0);
      std::fill(External.begin(), External.end(), 0);
      std::fill(Loopback.begin(), Loopback.end(), 0);
      std::fill(Sent.begin(), Sent.end(), 0);
      return;
    };
  } OptoHybridT1Counters;
  typedef struct OptoHybridVFATCRCCounters {
    std::vector<std::pair<uint32_t, uint32_t> > CRCCounters;
    OptoHybridVFATCRCCounters() : CRCCounters(24, std::make_pair(0, 0)) {};
    void reset() {
      std::fill(CRCCounters.begin(), CRCCounters.end(), std::make_pair(0, 0));
      return;
    };
  } OptoHybridVFATCRCCounters;
  HwOptoHybrid(std::string const &optohybridDevice,
               std::string const &connectionFile);
  HwOptoHybrid(std::string const &optohybridDevice,
               std::string const &connectionURI,
               std::string const &addressTable);
  HwOptoHybrid(std::string const &optohybridDevice,
               uhal::HwInterface &uhalDevice);
  HwOptoHybrid(gem::hw::glib::HwGLIB const &glib, uint8_t const &slot);
  virtual ~HwOptoHybrid();
  void connectRPC(bool reconnect = false) override;
  virtual bool isHwConnected() override;
  uint32_t getFirmwareVersion() {
    uint32_t fwver = readReg(getDeviceBaseNode(), "STATUS.FW.VERSION");
    CMSGEMOS_TRACE("OH has firmware version 0x" << std::hex << fwver << std::dec
                                                << std::endl);
    return fwver;
  };
  std::string getFirmwareVersionString() {
    std::stringstream retval;
    uint32_t fwver = getFirmwareVersion();
    retval << std::hex << ((fwver >> 24) & 0xff) << "."
           << ((fwver >> 16) & 0xff) << "." << ((fwver >> 8) & 0xff) << "."
           << ((fwver) & 0xff) << std::dec;
    return retval.str();
  };
  uint32_t getFirmwareDate() {
    uint32_t fwver = readReg(getDeviceBaseNode(), "STATUS.FW.DATE");
    CMSGEMOS_TRACE("OH has firmware date 0x" << std::hex << fwver << std::dec
                                             << std::endl);
    return fwver;
  };
  std::string getFirmwareDateString() {
    std::stringstream retval;
    uint32_t rawDate = getFirmwareDate();
    retval << std::hex << std::setw(2) << std::setfill('0')
           << ((rawDate) & 0xff) << std::dec << "/" << std::hex << std::setw(2)
           << std::setfill('0') << ((rawDate >> 8) & 0xff) << std::dec << "/"
           << std::hex << std::setw(4) << std::setfill('0')
           << ((rawDate >> 16) & 0xffff) << std::dec;
    return retval.str();
  };

public:
  virtual void generalReset() override;
  virtual void counterReset() override;
  virtual void linkReset(uint8_t const &link) override;
  GEMHwDevice::OpticalLinkStatus LinkStatus();
  void LinkReset(uint8_t const &resets);
  void ResetLinks(uint8_t const &resets) {
    return;
  };
  uint32_t getVFATMask() {
    return readReg(getDeviceBaseNode(),
                   toolbox::toString("CONTROL.VFAT.TRK_MASK"));
  };
  void setVFATMask(uint32_t const mask) {
    CMSGEMOS_DEBUG("HwOptoHybrid::setVFATMask setting tracking mask to "
                   << std::hex << std::setw(8) << std::setfill('0') << mask
                   << std::dec);
    return writeReg(getDeviceBaseNode(),
                    toolbox::toString("CONTROL.VFAT.TRK_MASK"),
                    mask & 0x00ffffff);
  };
  void resetVFATs() {
    return writeReg(getDeviceBaseNode(),
                    toolbox::toString("CONTROL.VFAT.RESET"), 0x1);
  };
  void setSBitMask(uint32_t const mask) {
    writeReg(getDeviceBaseNode(), "CONTROL.VFAT.SBIT_MASK", mask & 0x00ffffff);
  };
  uint32_t getSBitMask() {
    return readReg(getDeviceBaseNode(), "CONTROL.VFAT.SBIT_MASK");
  };
  uint32_t getDropBadCRCStatus() {
    return readReg(getDeviceBaseNode(),
                   toolbox::toString("CONTROL.VFAT.DROP_BAD_CRC"));
  };
  void setDropBadCRCStatus(bool const drop) {
    CMSGEMOS_DEBUG("HwOptoHybrid::setVFATMask setting DROP_BAD_CRC to "
                   << std::hex << drop << std::dec);
    return writeReg(getDeviceBaseNode(),
                    toolbox::toString("CONTROL.VFAT.DROP_BAD_CRC"), drop);
  };
  void setTrigSource(uint8_t const &mode) {
    switch (mode) {
    case(OptoHybridTrigSrc::GTX_TTC) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    case(OptoHybridTrigSrc::INTERNAL) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    case(OptoHybridTrigSrc::EXTERNAL) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    case(OptoHybridTrigSrc::LOOPBACK) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    case(OptoHybridTrigSrc::GBT_TTC) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    case(OptoHybridTrigSrc::ALL) :
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE", mode);
      return;
    default:
      writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE",
               OptoHybridTrigSrc::GBT_TTC);
      return;
    }
  };
  uint8_t getTrigSource() {
    return readReg(getDeviceBaseNode(), "CONTROL.TRIGGER.SOURCE");
  };
  void setLoopbackSBitSource(uint8_t const &sbit) {
    writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.LOOPBACK", sbit);
  };
  uint8_t getLoopbackSBitSource() {
    return readReg(getDeviceBaseNode(), "CONTROL.TRIGGER.LOOPBACK");
  };
  void setTriggerThrottle(uint32_t const &throttle) {
    writeReg(getDeviceBaseNode(), "CONTROL.TRIGGER.THROTTLE", throttle);
  };
  uint8_t getTriggerThrottle() {
    return readReg(getDeviceBaseNode(), "CONTROL.TRIGGER.THROTTLE");
  };
  void setHDMISBitSource(uint32_t const mask) {
    writeReg(getDeviceBaseNode(), "CONTROL.HDMI_OUTPUT.SBITS", mask);
  };
  void setHDMISBitSource(std::array<uint8_t, 6> const sources) {
    uint32_t mask = 0x0;
    for (int i = 0; i < 6; ++i)
      mask |= (sources[i] << (5 * i));
    setHDMISBitSource(mask);
  };
  uint32_t getHDMISBitSource() {
    return readReg(getDeviceBaseNode(), "CONTROL.HDMI_OUTPUT.SBITS");
  };
  void setHDMISBitMode(uint8_t const mode) {
    writeReg(getDeviceBaseNode(), "CONTROL.HDMI_OUTPUT.SBIT_MODE", mode);
  };
  uint32_t getHDMISBitMode() {
    return readReg(getDeviceBaseNode(), "CONTROL.HDMI_OUTPUT.SBIT_MODE");
  };
  void setZS(bool const &on) {
    writeReg(getDeviceBaseNode(), "CONTROL.ZS", uint32_t(on));
  };
  uint32_t getZSStatus() {
    return readReg(getDeviceBaseNode(), "CONTROL.ZS");
  };
  void setReferenceClock(uint8_t const &source) {
    writeReg(getDeviceBaseNode(), "CONTROL.CLOCK.REF_CLK", (uint32_t)source);
  };
  uint32_t getReferenceClock() {
    return readReg(getDeviceBaseNode(), "CONTROL.CLOCK.REF_CLK");
  };
  bool isQPLLLocked() {
    return readReg(getDeviceBaseNode(), "STATUS.QPLL_LOCK");
  };
  bool isQPLLFPGAPLLLocked() {
    return readReg(getDeviceBaseNode(), "STATUS.QPLL_FPGA_PLL_LOCK");
  };
  bool hasSEUError() {
    return readReg(getDeviceBaseNode(), "STATUS.SEU");
  };
  typedef struct T1Sequence {
    uint64_t l1a_seq;
    uint64_t cal_seq;
    uint64_t rsy_seq;
    uint64_t bc0_seq;
  } T1Sequence;
  void configureT1Generator(uint8_t const &mode, uint8_t const &type,
                            T1Sequence sequence, bool reset);
  void startT1Generator(uint32_t const &ntrigs, uint32_t const &rate,
                        uint32_t const &delay);
  void stopT1Generator(bool reset);
  void resetT1Generator() {
    writeReg(getDeviceBaseNode(), "T1Controller.RESET", 0x1);
  };
  uint8_t getT1GeneratorStatus() {
    return readReg(getDeviceBaseNode(), "T1Controller.MONITOR");
  };
  void sendL1A(uint32_t const &ntrigs, uint32_t const &rate = 1);
  void sendCalPulse(uint32_t const &npulse, uint32_t const &rate = 1);
  void sendL1ACalPulse(uint32_t const &npulse, uint32_t const &delay,
                       uint32_t const &rate = 1);
  void sendResync(uint32_t const &nresync = 1, uint32_t const &rate = 1);
  void sendBC0(uint32_t const &nbc0 = 1, uint32_t const &rate = 1);
  uint32_t getFPGATemp() {
    return readReg(getDeviceBaseNode(), "ADC.TEMP");
  };
  uint32_t getFPGAMaxTemp() {
    return readReg(getDeviceBaseNode(), "ADC.TEMP.MAX");
  };
  uint32_t getFPGAMinTemp() {
    return readReg(getDeviceBaseNode(), "ADC.TEMP.MIN");
  };
  uint32_t getFPGAVccInt() {
    return readReg(getDeviceBaseNode(), "ADC.VCCINT");
  };
  uint32_t getFPGAMaxVccInt() {
    return readReg(getDeviceBaseNode(), "ADC.VCCINT.MAX");
  };
  uint32_t getFPGAMinVccInt() {
    return readReg(getDeviceBaseNode(), "ADC.VCCINT.MIN");
  };
  uint32_t getFPGAVccAux() {
    return readReg(getDeviceBaseNode(), "ADC.VCCAUX");
  };
  uint32_t getFPGAMaxVccAux() {
    return readReg(getDeviceBaseNode(), "ADC.VCCAUX.MAX");
  };
  uint32_t getFPGAMinVccAux() {
    return readReg(getDeviceBaseNode(), "ADC.VCCAUX.MIN");
  };
  uint32_t getADCVPVN() {
    return readReg(getDeviceBaseNode(), "ADC.VPVN");
  };
  uint32_t getADCVAUX(uint8_t const &vaux);
  uint32_t getVFATDACOutV(uint8_t const &column);
  uint32_t getVFATDACOutI(uint8_t const &column);
  void configureScanModule(uint8_t const &mode, uint8_t const &chip,
                           uint8_t const &channel, uint8_t const &min,
                           uint8_t const &max, uint8_t const &step,
                           uint32_t const &nevts, bool useUltra = true,
                           bool reset = false);
  void startScanModule(uint32_t const &nevts, bool useUltra = true);
  void stopScanModule(bool useUltra = true, bool reset = false);
  uint8_t getScanStatus(bool useUltra = true);
  std::vector<uint32_t> getScanResults(uint32_t const &npoints);
  std::vector<std::vector<uint32_t> >
  getUltraScanResults(uint32_t const &npoints);
  OptoHybridWBMasterCounters getWBMasterCounters() {
    return m_wbMasterCounters;
  };
  void updateWBMasterCounters();
  void resetWBMasterCounters();
  OptoHybridWBSlaveCounters getWBSlaveCounters() {
    return m_wbSlaveCounters;
  };
  void updateWBSlaveCounters();
  void resetWBSlaveCounters();
  OptoHybridT1Counters getT1Counters() {
    return m_t1Counters;
  };
  void updateT1Counters();
  void resetT1Counters();
  OptoHybridVFATCRCCounters getVFATCRCCounters() {
    return m_vfatCRCCounters;
  };
  void updateVFATCRCCounters();
  void resetVFATCRCCounters();
  std::pair<uint32_t, uint32_t> getVFATCRCCount(uint8_t const &chip);
  void resetVFATCRCCount(uint8_t const &chip);
  void resetVFATCRCCount();
  uint32_t getT1Count(uint8_t const &signal, uint8_t const &mode);
  uint32_t getL1ACount(uint8_t const &mode) {
    return getT1Count(0x0, mode);
  };
  uint32_t getCalPulseCount(uint8_t const &mode) {
    return getT1Count(0x1, mode);
  };
  uint32_t getResyncCount(uint8_t const &mode = 0x0) {
    return getT1Count(0x2, mode);
  };
  uint32_t getBC0Count(uint8_t const &mode = 0x0) {
    return getT1Count(0x3, mode);
  };
  uint32_t getBXCountCount() {
    return 0x0;
  };
  void resetT1Count(uint8_t const &signal, uint8_t const &mode = 0x5);
  void resetL1ACount(uint8_t const &mode = 0x5) {
    resetT1Count(0x0, mode);
  };
  void resetCalPulseCount(uint8_t const &mode = 0x5) {
    return resetT1Count(0x1, mode);
  };
  void resetResyncCount(uint8_t const &mode = 0x5) {
    return resetT1Count(0x2, mode);
  };
  void resetBC0Count(uint8_t const &mode = 0x5) {
    return resetT1Count(0x3, mode);
  };
  void resetBXCount() {
    return;
  };
  uint32_t getQPLLUnlockCount() {
    return readReg(getDeviceBaseNode(), "COUNTERS.QPLL_LOCK");
  };
  void resetQPLLUnlockCount() {
    writeReg(getDeviceBaseNode(), "COUNTERS.QPLL_LOCK.Reset", 0x1);
  };
  uint32_t getQPLLFPGAPLLUnlockCount() {
    return readReg(getDeviceBaseNode(), "COUNTERS.QPLL_FPGA_PLL_LOCK");
  };
  void resetQPLLFPGAPLLUnlockCount() {
    writeReg(getDeviceBaseNode(), "COUNTERS.QPLL_FPGA_PLL_LOCK.Reset", 0x1);
  };
  void setVFATClock(bool source, bool fallback) {};
  std::pair<bool, bool> StatusVFATClock() {
    return std::make_pair(0, 0);
  };
  void setCDCEClock(bool source, bool fallback) {};
  std::pair<bool, bool> StatusCDCEClock() {
    return std::make_pair(0, 0);
  };
  std::vector<uint32_t> broadcastRead(std::string const &name,
                                      uint32_t const &mask =
                                          gem::hw::utils::ALL_VFATS_BCAST_MASK,
                                      bool reset = false);
  void broadcastWrite(std::string const &name, uint32_t const &value,
                      uint32_t const &mask =
                          gem::hw::utils::ALL_VFATS_BCAST_MASK,
                      bool reset = false);
  std::vector<std::pair<uint8_t, uint32_t> > getConnectedVFATs(bool update =
                                                                   false);
  uint32_t getConnectedVFATMask(bool update = false);
  void setVFATsToDefaults(uint8_t const &vt1, uint8_t const &vt2,
                          uint8_t const &latency,
                          uint32_t const &broadcastMask);
  void setVFATsToDefaults(std::map<std::string, uint8_t> const &regvals,
                          uint32_t const &broadcastMask);
  const uhal::HwInterface &getOptoHybridHwInterface() const {
    return dynamic_cast<const uhal::HwInterface &>(*this);
  };
  std::vector<linkStatus> getActiveLinks() { return v_activeLinks; }
  bool isLinkActive(int i) { return b_links[i]; }
  OptoHybridWBMasterCounters m_wbMasterCounters;
  OptoHybridWBSlaveCounters m_wbSlaveCounters;
  OptoHybridT1Counters m_t1Counters;
  OptoHybridVFATCRCCounters m_vfatCRCCounters;

protected:
  bool b_links[3];
  std::vector<linkStatus> v_activeLinks;

private:
  HwOptoHybrid();
  HwOptoHybrid(const HwOptoHybrid &other);
  HwOptoHybrid &operator=(const HwOptoHybrid &);
  bool b_is_initial;
  std::vector<std::pair<uint8_t, uint32_t> > m_chipIDs;
  uint32_t m_disabledMask;
  uint32_t m_connectedMask;
  uint8_t m_controlLink;
  int m_slot;
};
}
}
}
#endif
