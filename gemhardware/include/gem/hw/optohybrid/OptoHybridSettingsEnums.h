#ifndef GEM_HW_OPTOHYBRID_OPTOHYBRIDSETTINGSENUMS_H
#define GEM_HW_OPTOHYBRID_OPTOHYBRIDSETTINGSENUMS_H
namespace gem {
namespace hw {
namespace optohybrid {
class OptoHybridLinkSettings {
public:
  struct LinkBitMasks {
    enum ELinkBitMasks {
      ERROR = 0x01,
      RECI2C = 0x02,
      SNTI2C = 0x04,
      RECREG = 0x08,
      SNTREG = 0x10,
    } LinkBitMasks;
  };
  struct LinkBitShifts {
    enum ELinkBitShifts {
      ERROR = 0,
      RECI2C = 1,
      SNTI2C = 2,
      RECREG = 3,
      SNTREG = 4,
    } LinkBitShifts;
  };
};
class OptoHybridSettings {
public:
  struct TTCMode {
    enum ETTCMode {
      GTX_TTC = 0x0,
      INTERNAL = 0x1,
      EXTERNAL = 0x2,
      LOOPBACK = 0x3,
      SENT = 0x4,
      GBT_TTC = 0x5,
      RESET = 0x6,
    } TTCMode;
  };
  struct TriggerSource {
    enum ETriggerSource {
      GTX_TTC = 0x0,
      INTERNAL = 0x1,
      EXTERNAL = 0x2,
      LOOPBACK = 0x3,
      ALL = 0x4,
      GBT_TTC = 0x5,
    } TriggerSource;
  };
};
}
}
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitMasks
OptoHybridLinkBitMasks;
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitShifts
OptoHybridLinkBitShifts;
typedef gem::hw::optohybrid::OptoHybridSettings::TTCMode OptoHybridTTCMode;
typedef gem::hw::optohybrid::OptoHybridSettings::TriggerSource
OptoHybridTrigSrc;
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitMasks::ELinkBitMasks
OptoHybridLinkBitMasksT;
typedef gem::hw::optohybrid::OptoHybridLinkSettings::LinkBitShifts::
    ELinkBitShifts OptoHybridLinkBitShiftsT;
typedef gem::hw::optohybrid::OptoHybridSettings::TTCMode::ETTCMode
OptoHybridTTCModeT;
typedef gem::hw::optohybrid::OptoHybridSettings::TriggerSource::ETriggerSource
OptoHybridTrigSrcT;
}
#endif
