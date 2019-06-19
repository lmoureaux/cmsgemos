#ifndef GEM_HW_GLIB_GLIBMANAGER_H
#define GEM_HW_GLIB_GLIBMANAGER_H
#include <array>
#include "gem/base/GEMFSMApplication.h"
#include "gem/hw/glib/exception/Exception.h"
#include "gem/utils/soap/GEMSOAPToolBox.h"
#include "gem/utils/exception/Exception.h"
namespace gem {
namespace hw {
namespace glib {
class HwGLIB;
class GLIBManagerWeb;
class GLIBMonitor;
typedef std::shared_ptr<HwGLIB> glib_shared_ptr;
typedef std::shared_ptr<gem::base::utils::GEMInfoSpaceToolBox> is_toolbox_ptr;
class GLIBManager : public gem::base::GEMFSMApplication {
  friend class GLIBManagerWeb;

public:
  XDAQ_INSTANTIATOR();
  GLIBManager(xdaq::ApplicationStub *s);
  virtual ~GLIBManager();

protected:
  void init();
  virtual void actionPerformed(xdata::Event &event) override;
  virtual void
  initializeAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void
  configureAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void
  startAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void
  pauseAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void
  resumeAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void stopAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void haltAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void
  resetAction() throw(gem::hw::glib::exception::Exception) override;
  virtual void failAction(toolbox::Event::Reference e) throw(
      toolbox::fsm::exception::Exception);
  virtual void resetAction(toolbox::Event::Reference e) throw(
      toolbox::fsm::exception::Exception);

protected:
  std::vector<uint32_t> dumpGLIBFIFO(int const &glib);
  void dumpGLIBFIFO(xgi::Input *in, xgi::Output *out);

private:
  void createGLIBInfoSpaceItems(is_toolbox_ptr is_glib, glib_shared_ptr glib);
  uint16_t m_amcEnableMask;
  toolbox::task::WorkLoop *p_amc_wl;
  toolbox::BSem m_amc_wl_semaphore[MAX_AMCS_PER_CRATE];
  class GLIBInfo {
  public:
    GLIBInfo();
    void registerFields(xdata::Bag<GLIBManager::GLIBInfo> *bag);
    xdata::Boolean present;
    xdata::Integer crateID;
    xdata::Integer slotID;
    xdata::String cardName;
    xdata::String birdName;
    xdata::String gtxLinkEnableList;
    xdata::UnsignedInteger32 gtxLinkEnableMask;
    xdata::Integer sbitSource;
    xdata::Boolean enableZS;
    inline std::string toString() {
      std::stringstream os;
      os << "present:" << present.toString() << std::endl
         << "crateID:" << crateID.toString() << std::endl
         << "slotID:" << slotID.toString() << std::endl
         << "cardName:" << cardName.toString() << std::endl
         << "birdName:" << birdName.toString() << std::endl
         << "gtxLinkEnableList:" << gtxLinkEnableList.toString() << std::endl
         << "gtxLinkEnableMask:" << std::hex << gtxLinkEnableMask.value_
         << std::dec << std::endl << "sbitSource:0x" << std::hex
         << sbitSource.value_ << std::dec << std::endl << "enableZS:0x"
         << std::hex << enableZS.value_ << std::dec << std::endl << std::endl;
      return os.str();
    };
  };
  mutable gem::utils::Lock m_deviceLock;
  std::array<glib_shared_ptr, MAX_AMCS_PER_CRATE> m_glibs;
  std::array<std::shared_ptr<GLIBMonitor>, MAX_AMCS_PER_CRATE> m_glibMonitors;
  std::array<is_toolbox_ptr, MAX_AMCS_PER_CRATE> is_glibs;
  xdata::Vector<xdata::Bag<GLIBInfo> > m_glibInfo;
  xdata::String m_amcSlots;
  xdata::String m_connectionFile;
  xdata::Boolean m_uhalPhaseShift;
  xdata::Boolean m_bc0LockPhaseShift;
  xdata::Boolean m_relockPhase;
  uint32_t m_lastLatency, m_lastVT1, m_lastVT2;
};
}
}
}
#endif
