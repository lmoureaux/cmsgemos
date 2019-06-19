#ifndef GEM_HW_GEMHWDEVICE_H
#define GEM_HW_GEMHWDEVICE_H
#include <iomanip>
#include <memory>
#include "toolbox/string.h"
#include "uhal/uhal.hpp"
#include "uhal/utilities/bits.hpp"
#include "uhal/utilities/files.hpp"
#include "xhal/XHALInterface.h"
#include "gem/hw/utils/GEMCrateUtils.h"
#include "gem/hw/exception/Exception.h"
#include "gem/utils/GEMLogging.h"
#include "gem/utils/GEMRegisterUtils.h"
#include "gem/utils/Lock.h"
#include "gem/utils/LockGuard.h"
typedef uhal::exception::exception uhalException;
typedef std::pair<std::string, uint32_t> register_pair;
typedef std::vector<register_pair> register_pair_list;
typedef std::pair<uint32_t, uint32_t> addressed_register_pair;
typedef std::vector<addressed_register_pair> addressed_register_pair_list;
typedef std::pair<std::pair<uint32_t, uint32_t>, uint32_t> masked_register_pair;
typedef std::vector<masked_register_pair> masked_register_pair_list;
typedef std::pair<std::string, uhal::ValWord<uint32_t> > register_value;
typedef std::vector<register_value> register_val_list;
namespace gem {
namespace hw {
class GEMHwDevice : public xhal::XHALInterface, public uhal::HwInterface {
public:
  static const unsigned MAX_IPBUS_RETRIES = 5;
  typedef struct OpticalLinkStatus {
    uint32_t GTX_TRK_Errors;
    uint32_t GTX_TRG_Errors;
    uint32_t GTX_Data_Packets;
    uint32_t GBT_TRK_Errors;
    uint32_t GBT_Data_Packets;
    OpticalLinkStatus()
        : GTX_TRK_Errors(0), GTX_TRG_Errors(0), GTX_Data_Packets(0),
          GBT_TRK_Errors(0), GBT_Data_Packets(0) {};
    void reset() {
      GTX_TRK_Errors = 0;
      GTX_TRG_Errors = 0;
      GTX_Data_Packets = 0;
      GBT_TRK_Errors = 0;
      GBT_Data_Packets = 0;
      return;
    };
  } OpticalLinkStatus;
  typedef struct DeviceErrors {
    int BadHeader;
    int ReadError;
    int Timeout;
    int ControlHubErr;
    DeviceErrors()
        : BadHeader(0), ReadError(0), Timeout(0), ControlHubErr(0) {};
    void reset() {
      BadHeader = 0;
      ReadError = 0;
      Timeout = 0;
      ControlHubErr = 0;
      return;
    };
  } DeviceErrors;
  typedef std::pair<uint8_t, OpticalLinkStatus> linkStatus;
  GEMHwDevice(std::string const &deviceName, std::string const &connectionFile);
  GEMHwDevice(std::string const &deviceName, std::string const &connectionURI,
              std::string const &addressTable);
  GEMHwDevice(std::string const &deviceName,
              uhal::HwInterface const &uhalDevice);
  virtual ~GEMHwDevice();
  virtual bool isHwConnected() {
    return true;
  };
  uint32_t readReg(std::string const &regName);
  uint32_t readReg(uint32_t const &regAddr);
  uint32_t readReg(uint32_t const &regAddr, uint32_t const &regMask);
  uint32_t readReg(const std::string &regPrefix, const std::string &regName) {
    return readReg(regPrefix + "." + regName);
  };
  uint32_t readMaskedAddress(std::string const &regName);
  void readRegs(register_pair_list &regList, int const &freq = 8);
  void readRegs(addressed_register_pair_list &regList, int const &freq = 8);
  void readRegs(masked_register_pair_list &regList, int const &freq = 8);
  void writeReg(std::string const &regName, uint32_t const val);
  void writeReg(uint32_t const &regAddr, uint32_t const val);
  void writeReg(const std::string &regPrefix, const std::string &regName,
                uint32_t const val) {
    return writeReg(regPrefix + "." + regName, val);
  };
  void writeRegs(register_pair_list const &regList, int const &freq = 8);
  void writeValueToRegs(std::vector<std::string> const &regList,
                        uint32_t const &regValue, int const &freq = 8);
  void zeroReg(std::string const &regName) {
    writeReg(regName, 0);
  };
  void zeroRegs(std::vector<std::string> const &regNames, int const &freq = 8);
  std::vector<uint32_t> readBlock(std::string const &regName);
  std::vector<uint32_t> readBlock(std::string const &regName,
                                  size_t const &nWords);
  uint32_t readBlock(std::string const &regName, uint32_t *buffer,
                     size_t const &nWords);
  void writeBlock(std::string const &regName,
                  std::vector<uint32_t> const values);
  void zeroBlock(std::string const &regName);
  std::vector<uint32_t> readFIFO(std::string const &regName);
  std::vector<uint32_t> readFIFO(std::string const &regName,
                                 size_t const &nWords);
  void writeFIFO(std::string const &regName,
                 std::vector<uint32_t> const values);
  void zeroFIFO(std::string const &regName);
  const std::string getControlHubIPAddress() const {
    return m_controlHubIPAddress;
  };
  const std::string getDeviceIPAddress() const {
    return m_deviceIPAddress;
  };
  const std::string getIPBusProtocolVersion() const {
    return m_ipBusProtocol;
  };
  const std::string getAddressTableFileName() const {
    return m_addressTable;
  };
  const std::string getDeviceBaseNode() const {
    return m_deviceBaseNode;
  };
  const std::string getDeviceID() const {
    return m_deviceID;
  };
  const uint32_t getControlHubPort() const {
    return m_controlHubPort;
  };
  const uint32_t getIPBusPort() const {
    return m_ipBusPort;
  };
  void setControlHubIPAddress(std::string const &ipAddress) {
    m_controlHubIPAddress = ipAddress;
  };
  void setIPBusProtocolVersion(std::string const &version) {
    m_ipBusProtocol = version;
  };
  void setDeviceIPAddress(std::string const &deviceIPAddr) {
    m_deviceIPAddress = deviceIPAddr;
  };
  void setAddressTableFileName(std::string const &name) {
    m_addressTable = "file://${GEM_ADDRESS_TABLE_PATH}/" + name;
  };
  void setDeviceBaseNode(std::string const &deviceBase) {
    m_deviceBaseNode = deviceBase;
  };
  void setDeviceID(std::string const &deviceID) {
    m_deviceID = deviceID;
  };
  void setControlHubPort(uint32_t const &port) {
    m_controlHubPort = port;
  };
  void setIPBusPort(uint32_t const &port) {
    m_ipBusPort = port;
  };
  std::string getLoggerName() const {
    return m_gemLogger.getName();
  };
  void updateErrorCounters(std::string const &errCode);
  virtual std::string printErrorCounts() const;
  virtual void generalReset();
  virtual void counterReset();
  virtual void linkReset(uint8_t const &link);
  DeviceErrors m_ipBusErrs;
  bool b_is_connected;

protected:
  log4cplus::Logger m_gemLogger;
  mutable gem::utils::Lock m_hwLock;
  void setup(std::string const &deviceName);

private:
  GEMHwDevice();
  GEMHwDevice(const GEMHwDevice &other);
  GEMHwDevice &operator=(const GEMHwDevice &);
  virtual void connectRPC(bool reconnect = false) = 0;
  std::string m_controlHubIPAddress;
  std::string m_addressTable;
  std::string m_ipBusProtocol;
  std::string m_deviceIPAddress;
  std::string m_deviceBaseNode;
  std::string m_deviceID;
  uint32_t m_controlHubPort;
  uint32_t m_ipBusPort;
  bool knownErrorCode(std::string const &errCode) const;
};
}
}
#endif
