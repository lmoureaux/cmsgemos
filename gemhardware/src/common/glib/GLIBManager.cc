#include "gem/hw/glib/GLIBManager.h"
#include <iterator>
#include "gem/hw/glib/HwGLIB.h"
#include "gem/hw/glib/GLIBMonitor.h"
#include "gem/hw/glib/GLIBManagerWeb.h"
#include "gem/hw/glib/exception/Exception.h"
#include "gem/hw/utils/GEMCrateUtils.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPConstants.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "xoap/AttachmentPart.h"
#include "xoap/domutils.h"
XDAQ_INSTANTIATOR_IMPL(gem::hw::glib::GLIBManager);
gem::hw::glib::GLIBManager::GLIBInfo::GLIBInfo() {
  present = false;
  crateID = -1;
  slotID = -1;
  cardName = "";
  birdName = "";
  sbitSource = 0;
  enableZS = true;
}
void gem::hw::glib::GLIBManager::GLIBInfo::registerFields(
    xdata::Bag<gem::hw::glib::GLIBManager::GLIBInfo> *bag) {
  bag->addField("crateID", &crateID);
  bag->addField("slot", &slotID);
  bag->addField("present", &present);
  bag->addField("CardName", &cardName);
  bag->addField("BirdName", &birdName);
  bag->addField("sbitSource", &sbitSource);
  bag->addField("enableZS", &enableZS);
}
gem::hw::glib::GLIBManager::GLIBManager(xdaq::ApplicationStub *stub)
    : gem::base::GEMFSMApplication(stub), m_amcEnableMask(0),
      m_uhalPhaseShift(false), m_bc0LockPhaseShift(false), m_relockPhase(true) {
  m_glibInfo.setSize(MAX_AMCS_PER_CRATE);
  p_appInfoSpace->fireItemAvailable("AllGLIBsInfo", &m_glibInfo);
  p_appInfoSpace->fireItemAvailable("AMCSlots", &m_amcSlots);
  p_appInfoSpace->fireItemAvailable("ConnectionFile", &m_connectionFile);
  p_appInfoSpace->fireItemAvailable("UHALPhaseShift", &m_uhalPhaseShift);
  p_appInfoSpace->fireItemAvailable("BC0LockPhaseShift", &m_bc0LockPhaseShift);
  p_appInfoSpace->fireItemAvailable("RelockPhase", &m_relockPhase);
  p_appInfoSpace->addItemRetrieveListener("AllGLIBsInfo", this);
  p_appInfoSpace->addItemRetrieveListener("AMCSlots", this);
  p_appInfoSpace->addItemRetrieveListener("ConnectionFile", this);
  p_appInfoSpace->addItemRetrieveListener("UHALPhaseShift", this);
  p_appInfoSpace->addItemRetrieveListener("BC0LockPhaseShift", this);
  p_appInfoSpace->addItemRetrieveListener("RelockPhase", this);
  p_appInfoSpace->addItemChangedListener("AllGLIBsInfo", this);
  p_appInfoSpace->addItemChangedListener("AMCSlots", this);
  p_appInfoSpace->addItemChangedListener("ConnectionFile", this);
  p_appInfoSpace->addItemChangedListener("UHALPhaseShift", this);
  p_appInfoSpace->addItemChangedListener("BC0LockPhaseShift", this);
  p_appInfoSpace->addItemChangedListener("RelockPhase", this);
  xgi::bind(this, &GLIBManager::dumpGLIBFIFO, "dumpGLIBFIFO");
  CMSGEMOS_DEBUG("GLIBManager::Connecting to the GLIBManagerWeb interface");
  p_gemWebInterface = new gem::hw::glib::GLIBManagerWeb(this);
  CMSGEMOS_DEBUG("GLIBManager::done");
  init();
}
gem::hw::glib::GLIBManager::~GLIBManager() {}
std::vector<uint32_t>
gem::hw::glib::GLIBManager::dumpGLIBFIFO(int const &glib) {
  std::vector<uint32_t> dump;
  if (glib < 0 || glib > 11) {
    CMSGEMOS_WARN("GLIBManager::dumpGLIBFIFO Specified invalid GLIB card "
                  << glib + 1);
    return dump;
  } else if (!m_glibs.at(glib)) {
    CMSGEMOS_WARN("GLIBManager::dumpGLIBFIFO Specified GLIB card "
                  << glib + 1 << " is not connected");
    return dump;
  }
  try {
    CMSGEMOS_INFO(
        "GLIBManager::dumpGLIBFIFO Dumping FIFO for specified GLIB card "
        << glib + 1);
    return m_glibs.at(glib)->getTrackingData(0, 24);
  }
  catch (gem::hw::glib::exception::Exception const &e) {
    std::stringstream msg;
    msg << "GLIBManager::dumpGLIBFIFO Unable to read tracking data from AMC"
        << glib + 1 << " FIFO, caught exception " << e.what();
    CMSGEMOS_ERROR(msg.str());
    return dump;
  }
  catch (std::exception const &e) {
    std::stringstream msg;
    msg << "GLIBManager::dumpGLIBFIFO Unable to read tracking data from AMC"
        << glib + 1 << " FIFO, caught exception " << e.what();
    CMSGEMOS_ERROR(msg.str());
    return dump;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GLIBManager::dumpGLIBFIFO Unable to read tracking data from AMC"
        << glib + 1 << " FIFO, caught unknown exception ";
    CMSGEMOS_ERROR(msg.str());
    return dump;
  }
}
void gem::hw::glib::GLIBManager::actionPerformed(xdata::Event &event) {
  if (event.type() == "setDefaultValues" ||
      event.type() == "urn:xdaq-event:setDefaultValues") {
    CMSGEMOS_DEBUG(
        "GLIBManager::actionPerformed() setDefaultValues"
        << "Default configuration values have been loaded from xml profile");
    m_amcEnableMask =
        gem::hw::utils::parseAMCEnableList(m_amcSlots.toString(), m_gemLogger);
    CMSGEMOS_INFO(
        "GLIBManager::actionPerformed() Parsed AMCEnableList m_amcSlots = "
        << m_amcSlots.toString() << " to slotMask 0x" << std::hex
        << m_amcEnableMask << std::dec);
    for (auto slot = m_glibInfo.begin(); slot != m_glibInfo.end(); ++slot) {
      if (slot->bag.crateID.value_ > -1) {
        slot->bag.present = true;
        CMSGEMOS_DEBUG("GLIBManager::Found attribute:" << slot->bag.toString());
      }
    }
  }
  gem::base::GEMApplication::actionPerformed(event);
}
void gem::hw::glib::GLIBManager::init() {}
void gem::hw::glib::GLIBManager::initializeAction() throw(
    gem::hw::glib::exception::Exception) {
  CMSGEMOS_DEBUG("GLIBManager::initializeAction begin");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding expected cards");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if ((m_amcEnableMask >> (slot)) & 0x1) {
      CMSGEMOS_DEBUG("GLIBManager::info:" << info.toString());
      CMSGEMOS_DEBUG("GLIBManager::expect a card in slot " << (slot + 1));
      CMSGEMOS_DEBUG("GLIBManager::bag"
                     << "crate " << info.crateID.value_ << " slot "
                     << info.slotID.value_);
      info.slotID = slot + 1;
      CMSGEMOS_DEBUG("GLIBManager::bag"
                     << "crate " << info.crateID.value_ << " slot "
                     << info.slotID.value_);
      info.present = true;
    }
  }
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    CMSGEMOS_DEBUG("GLIBManager::creating pointer to card in slot "
                   << (slot + 1));
    std::string deviceName = info.cardName.toString();
    if (deviceName.empty())
      deviceName = toolbox::toString("gem-shelf%02d-amc%02d",
                                     info.crateID.value_, info.slotID.value_);
    toolbox::net::URN hwCfgURN("urn:gem:hw:" + deviceName);
    if (xdata::getInfoSpaceFactory()->hasItem(hwCfgURN.toString())) {
      CMSGEMOS_DEBUG("GLIBManager::initializeAction::infospace "
                     << hwCfgURN.toString() << " already exists, getting");
      is_glibs.at(slot) =
          is_toolbox_ptr(new gem::base::utils::GEMInfoSpaceToolBox(
              this, xdata::getInfoSpaceFactory()->get(hwCfgURN.toString()),
              true));
    } else {
      CMSGEMOS_DEBUG("GLIBManager::initializeAction::infospace "
                     << hwCfgURN.toString() << " does not exist, creating");
      is_glibs.at(slot) =
          is_toolbox_ptr(new gem::base::utils::GEMInfoSpaceToolBox(
              this, hwCfgURN.toString(), true));
    }
    try {
      CMSGEMOS_DEBUG("GLIBManager::obtaining pointer to HwGLIB");
      m_glibs.at(slot) = glib_shared_ptr(
          new gem::hw::glib::HwGLIB(deviceName, m_connectionFile.toString()));
      glib_shared_ptr amc = m_glibs.at(slot);
      if (amc->isHwConnected()) {
        CMSGEMOS_DEBUG("GLIBManager::Creating InfoSpace items for GLIB device "
                       << deviceName);
        amc->disableDAQLink();
        createGLIBInfoSpaceItems(is_glibs.at(slot), amc);
        if (!m_disableMonitoring) {
          m_glibMonitors.at(slot) = std::shared_ptr<GLIBMonitor>(
              new GLIBMonitor(amc, this, slot + 1));
          m_glibMonitors.at(slot)
              ->addInfoSpace("HWMonitoring", is_glibs.at(slot));
          m_glibMonitors.at(slot)->setupHwMonitoring();
          m_glibMonitors.at(slot)->startMonitoring();
        }
      } else {
        std::stringstream msg;
        msg << "GLIBManager::initializeAction unable to communicate with GLIB "
               "in slot " << slot;
        CMSGEMOS_ERROR(msg.str());
        XCEPT_RAISE(gem::hw::glib::exception::Exception,
                    "initializeAction failed");
      }
    }
    GEM_HW_TRANSITION_CATCH("GLIBManager::initializeAction",
                            gem::hw::glib::exception::Exception);
    CMSGEMOS_DEBUG("GLIBManager::connected");
  }
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      CMSGEMOS_DEBUG("GLIBManager::connected a card in slot " << (slot + 1));
    } else {
      std::stringstream msg;
      msg << "GLIBManager::initializeAction GLIB in slot " << (slot + 1)
          << " is not connected";
      CMSGEMOS_ERROR(msg.str());
      XCEPT_RAISE(gem::hw::glib::exception::Exception, msg.str());
    }
  }
  CMSGEMOS_INFO("GLIBManager::initializeAction end");
}
void gem::hw::glib::GLIBManager::configureAction() throw(
    gem::hw::glib::exception::Exception) {
  CMSGEMOS_DEBUG("GLIBManager::configureAction");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      bool enableZS = info.enableZS.value_;
      bool doPhaseShift = m_uhalPhaseShift.value_;
      uint32_t runType = 0x0;
      try {
        amc->configureDAQModule(enableZS, doPhaseShift, runType, 0xfaac,
                                m_relockPhase.value_,
                                m_bc0LockPhaseShift.value_);
      }
      GEM_CATCH_RPC_ERROR("GLIBManager::configureAction",
                          gem::hw::glib::exception::ConfigurationProblem);
      if (m_scanType.value_ == 2) {
        CMSGEMOS_INFO("GLIBManager::configureAction: FIRST  "
                      << m_scanMin.value_);
        amc->setDAQLinkRunType(0x2);
        amc->setDAQLinkRunParameter(0x1, m_scanMin.value_);
      } else if (m_scanType.value_ == 3) {
        uint32_t initialVT1 = m_scanMin.value_;
        uint32_t initialVT2 = 0;
        CMSGEMOS_INFO("GLIBManager::configureAction FIRST VT1 "
                      << initialVT1 << " VT2 " << initialVT2);
        amc->setDAQLinkRunType(0x3);
        amc->setDAQLinkRunParameter(0x2, initialVT1);
        amc->setDAQLinkRunParameter(0x3, initialVT2);
      } else {
        amc->setDAQLinkRunType(0x1);
        amc->setDAQLinkRunParameters(0xfaac);
      }
      CMSGEMOS_INFO(
          "GLIBManager::configureAction running confAllChambers for P5 setup");
      std::stringstream confcmd;
      confcmd << "confAllChambers.py -s" << (slot + 1)
              << " --shelf=" << info.crateID.toString() << " --ztrim=" << 4.0
              << " --vt1bump=" << 10 << " --config --run";
      CMSGEMOS_INFO("GLIBManager::configureAction executing " << confcmd.str());
      int retval = std::system(confcmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::configureAction unable to configure chambers: "
            << retval;
        CMSGEMOS_WARN(msg.str());
      }
      std::stringstream statuscmd;
      statuscmd << "amc_info_uhal.py -s" << (slot + 1)
                << " --shelf=" << info.crateID.toString();
      CMSGEMOS_INFO("GLIBManager::configureAction running "
                    << statuscmd.str() << " after configuring");
      retval = std::system(statuscmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::configureAction unable to check AMC status: "
            << retval;
        CMSGEMOS_WARN(msg.str());
      }
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->resumeMonitoring();
    } else {
      std::stringstream msg;
      msg << "GLIBManager::configureAction GLIB in slot " << (slot + 1)
          << " is not connected";
      CMSGEMOS_ERROR(msg.str());
      XCEPT_RAISE(gem::hw::glib::exception::Exception, msg.str());
    }
  }
  CMSGEMOS_INFO("GLIBManager::configureAction end");
}
void gem::hw::glib::GLIBManager::startAction() throw(
    gem::hw::glib::exception::Exception) {
  if (m_scanType.value_ == 2) {
    CMSGEMOS_INFO("GLIBManager::startAction() " << std::endl
                                                << m_scanInfo.bag.toString());
    m_lastLatency = m_scanMin.value_;
    m_lastVT1 = 0;
  } else if (m_scanType.value_ == 3) {
    CMSGEMOS_INFO("GLIBManager::startAction() " << std::endl
                                                << m_scanInfo.bag.toString());
    m_lastLatency = 0;
    m_lastVT1 = m_scanMin.value_;
  }
  CMSGEMOS_INFO("GLIBManager::startAction begin");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding infospace items");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      CMSGEMOS_DEBUG("connected a card in slot " << (slot + 1));
      amc->ttcModuleReset();
      amc->enableDAQLink(0x4);
      amc->resetDAQLink();
      amc->setZS(info.enableZS.value_);
      amc->setL1AEnable(true);
      usleep(10);
      std::stringstream statuscmd;
      statuscmd << "amc_info_uhal.py -s" << (slot + 1)
                << " --shelf=" << info.crateID.toString();
      CMSGEMOS_INFO("GLIBManager::startAction running " << statuscmd.str()
                                                        << " after starting");
      int retval = std::system(statuscmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::startAction unable to check AMC status: "
            << retval;
        CMSGEMOS_WARN(msg.str());
      }
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->resumeMonitoring();
    } else {
      std::stringstream msg;
      msg << "GLIBManager::startAction GLIB in slot " << (slot + 1)
          << " is not connected";
      CMSGEMOS_ERROR(msg.str());
      XCEPT_RAISE(gem::hw::glib::exception::Exception, msg.str());
    }
  }
  CMSGEMOS_INFO("GLIBManager::startAction end");
}
void gem::hw::glib::GLIBManager::pauseAction() throw(
    gem::hw::glib::exception::Exception) {
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding infospace items");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      CMSGEMOS_DEBUG("connected a card in slot " << (slot + 1));
      if (m_scanType.value_ == 2) {
        uint8_t updatedLatency = m_lastLatency + m_stepSize.value_;
        CMSGEMOS_INFO("GLIBManager::pauseAction LatencyScan AMC"
                      << (slot + 1) << " Latency " << (int)updatedLatency);
        while (!amc->l1aFIFOIsEmpty()) {
          CMSGEMOS_DEBUG("GLIBManager::pauseAction waiting for AMC"
                         << (slot + 1) << " to finish building events");
          usleep(10);
        }
        CMSGEMOS_DEBUG("GLIBManager::pauseAction AMC"
                       << (slot + 1)
                       << " finished building events, updating run parameter "
                       << (int)updatedLatency);
        amc->setDAQLinkRunParameter(0x1, updatedLatency);
      } else if (m_scanType.value_ == 3) {
        uint8_t updatedVT1 = m_lastVT1 + m_stepSize.value_;
        uint8_t updatedVT2 = 0;
        CMSGEMOS_INFO("GLIBManager::pauseAction ThresholdScan AMC"
                      << (slot + 1) << ""
                      << " VT1 " << (int)updatedVT1 << " VT2 "
                      << (int)updatedVT2);
        while (!amc->l1aFIFOIsEmpty()) {
          CMSGEMOS_DEBUG("GLIBManager::pauseAction waiting for AMC"
                         << (slot + 1) << " to finish building events");
          usleep(10);
        }
        CMSGEMOS_DEBUG("GLIBManager::pauseAction finished AMC"
                       << (slot + 1) << " building events, updating VT1 "
                       << (int)updatedVT1 << " and VT2 " << (int)updatedVT2);
        amc->setDAQLinkRunParameter(0x2, updatedVT1);
        amc->setDAQLinkRunParameter(0x3, updatedVT2);
      }
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->resumeMonitoring();
    } else {
      std::stringstream msg;
      msg << "GLIBManager::pauseAction GLIB in slot " << (slot + 1)
          << " is not connected";
      CMSGEMOS_ERROR(msg.str());
      XCEPT_RAISE(gem::hw::glib::exception::Exception, msg.str());
    }
  }
  if (m_scanType.value_ == 2) {
    CMSGEMOS_INFO("GLIBManager::pauseAction LatencyScan old Latency "
                  << (int)m_lastLatency);
    m_lastLatency += m_stepSize.value_;
    CMSGEMOS_INFO("GLIBManager::pauseAction LatencyScan new Latency "
                  << (int)m_lastLatency);
  } else if (m_scanType.value_ == 3) {
    CMSGEMOS_INFO("GLIBManager::pauseAction ThresholdScan old VT1 "
                  << (int)m_lastVT1);
    m_lastVT1 += m_stepSize.value_;
    CMSGEMOS_INFO("GLIBManager::pauseAction ThresholdScan new VT1 "
                  << (int)m_lastVT1);
  }
  CMSGEMOS_INFO("GLIBManager::pauseAction end");
}
void gem::hw::glib::GLIBManager::resumeAction() throw(
    gem::hw::glib::exception::Exception) {
  usleep(10);
  CMSGEMOS_INFO("GLIBManager::resumeAction end");
}
void gem::hw::glib::GLIBManager::stopAction() throw(
    gem::hw::glib::exception::Exception) {
  CMSGEMOS_INFO("GLIBManager::stopAction begin");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding infospace items");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      amc->setL1AEnable(false);
      amc->disableDAQLink();
      amc->resetDAQLink();
      std::stringstream statuscmd;
      statuscmd << "amc_info_uhal.py -s" << (slot + 1)
                << " --shelf=" << info.crateID.toString();
      CMSGEMOS_INFO("GLIBManager::stopAction running "
                    << statuscmd.str() << " after configuration");
      int retval = std::system(statuscmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::stopAction unable to check AMC status: " << retval;
        CMSGEMOS_WARN(msg.str());
      }
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->resumeMonitoring();
    }
  }
  usleep(10);
  CMSGEMOS_INFO("GLIBManager::stopAction end");
}
void gem::hw::glib::GLIBManager::haltAction() throw(
    gem::hw::glib::exception::Exception) {
  CMSGEMOS_DEBUG("GLIBManager::resetAction begin");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding infospace items");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      amc->setL1AEnable(false);
      amc->writeReg("GEM_AMC.DAQ.CONTROL.INPUT_ENABLE_MASK", 0x0);
      std::stringstream statuscmd;
      statuscmd << "amc_info_uhal.py -s" << (slot + 1)
                << " --shelf=" << info.crateID.toString();
      CMSGEMOS_INFO("GLIBManager::haltAction running " << statuscmd.str()
                                                       << " after halting");
      int retval = std::system(statuscmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::haltAction unable to check AMC status: " << retval;
        CMSGEMOS_WARN(msg.str());
      }
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->resumeMonitoring();
    }
  }
  CMSGEMOS_INFO("GLIBManager::haltAction end");
}
void gem::hw::glib::GLIBManager::resetAction() throw(
    gem::hw::glib::exception::Exception) {
  CMSGEMOS_DEBUG("GLIBManager::resetAction begin");
  for (unsigned slot = 0; slot < MAX_AMCS_PER_CRATE; ++slot) {
    CMSGEMOS_DEBUG("GLIBManager::looping over slots("
                   << (slot + 1) << ") and finding infospace items");
    GLIBInfo &info = m_glibInfo[slot].bag;
    if (!info.present)
      continue;
    glib_shared_ptr amc = m_glibs.at(slot);
    if (amc->isHwConnected()) {
      if (m_glibMonitors.at(slot))
        m_glibMonitors.at(slot)->pauseMonitoring();
      amc->setL1AEnable(false);
      amc->writeReg("GEM_AMC.DAQ.CONTROL.INPUT_ENABLE_MASK", 0x0);
      std::stringstream statuscmd;
      statuscmd << "amc_info_uhal.py -s" << (slot + 1)
                << " --shelf=" << info.crateID.toString();
      CMSGEMOS_INFO("GLIBManager::resetAction running " << statuscmd.str()
                                                        << " after resetting");
      int retval = std::system(statuscmd.str().c_str());
      if (retval) {
        std::stringstream msg;
        msg << "GLIBManager::resetAction unable to check AMC status: "
            << retval;
        CMSGEMOS_WARN(msg.str());
      }
    }
    if (m_glibMonitors.at(slot))
      m_glibMonitors.at(slot)->reset();
    CMSGEMOS_DEBUG(
        "GLIBManager::looking for hwCfgInfoSpace items for GLIB in slot "
        << (slot + 1));
    toolbox::net::URN hwCfgURN("urn:gem:hw:" +
                               toolbox::toString("gem-shelf%02d-amc%02d",
                                                 info.crateID.value_,
                                                 info.slotID.value_));
    if (xdata::getInfoSpaceFactory()->hasItem(hwCfgURN.toString())) {
      CMSGEMOS_DEBUG("GLIBManager::revoking config parameters infospace");
      is_glibs.at(slot)->reset();
    } else {
      CMSGEMOS_DEBUG("GLIBManager::resetAction::infospace "
                     << hwCfgURN.toString()
                     << " does not exist, no further action");
      continue;
    }
  }
  CMSGEMOS_INFO("GLIBManager::resetAction end");
}
void gem::hw::glib::GLIBManager::failAction(toolbox::Event::Reference e) throw(
    toolbox::fsm::exception::Exception) {}
void gem::hw::glib::GLIBManager::resetAction(toolbox::Event::Reference e) throw(
    toolbox::fsm::exception::Exception) {}
void
gem::hw::glib::GLIBManager::createGLIBInfoSpaceItems(is_toolbox_ptr is_glib,
                                                     glib_shared_ptr glib) {
  is_glib->createUInt32("BOARD_ID", glib->getBoardIDRaw(), NULL,
                        GEMUpdateType::NOUPDATE, "docstring", "id");
  is_glib->createUInt32("SYSTEM_ID", glib->getSystemIDRaw(), NULL,
                        GEMUpdateType::NOUPDATE, "docstring", "id");
  is_glib->createUInt32("FIRMWARE_VERSION", glib->getFirmwareVerRaw(), NULL,
                        GEMUpdateType::PROCESS, "docstring", "fwver");
  is_glib->createUInt32("FIRMWARE_DATE", glib->getFirmwareDateRaw(), NULL,
                        GEMUpdateType::PROCESS, "docstring", "date");
  is_glib->createUInt32("AMC_FIRMWARE_VERSION", glib->getFirmwareVerRaw(), NULL,
                        GEMUpdateType::PROCESS, "docstring", "fwverglib");
  is_glib->createUInt32("AMC_FIRMWARE_DATE", glib->getFirmwareDateRaw(), NULL,
                        GEMUpdateType::PROCESS, "docstring", "dateoh");
  is_glib->createUInt32("IP_ADDRESS", glib->getIPAddressRaw(), NULL,
                        GEMUpdateType::NOUPDATE, "docstring", "ip");
  is_glib->createUInt64("MAC_ADDRESS", glib->getMACAddressRaw(), NULL,
                        GEMUpdateType::NOUPDATE, "docstring", "mac");
  is_glib->createUInt32("SFP1_STATUS", glib->SFPStatus(1), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("SFP2_STATUS", glib->SFPStatus(2), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("SFP3_STATUS", glib->SFPStatus(3), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("SFP4_STATUS", glib->SFPStatus(4), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("FMC1_STATUS", glib->FMCPresence(0), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("FMC2_STATUS", glib->FMCPresence(1), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("FPGA_RESET", glib->FPGAResetStatus(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GBE_INT", glib->GbEInterrupt(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("V6_CPLD", glib->V6CPLDStatus(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("CPLD_LOCK", glib->CDCELockStatus(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("L1A", glib->getTTCCounter(AMCTTCCommand::TTC_L1A),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("BC0", glib->getTTCCounter(AMCTTCCommand::TTC_BC0),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("EC0", glib->getTTCCounter(AMCTTCCommand::TTC_EC0),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("RESYNC",
                        glib->getTTCCounter(AMCTTCCommand::TTC_RESYNC), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("OC0", glib->getTTCCounter(AMCTTCCommand::TTC_OC0),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("HARD_RESET",
                        glib->getTTCCounter(AMCTTCCommand::TTC_HARD_RESET),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("CALPULSE",
                        glib->getTTCCounter(AMCTTCCommand::TTC_CALPULSE), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("START", glib->getTTCCounter(AMCTTCCommand::TTC_START),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("STOP", glib->getTTCCounter(AMCTTCCommand::TTC_STOP),
                        NULL, GEMUpdateType::HW32);
  is_glib->createUInt32("TEST_SYNC",
                        glib->getTTCCounter(AMCTTCCommand::TTC_TEST_SYNC), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("CONTROL", glib->getDAQLinkControl(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("STATUS", glib->getDAQLinkStatus(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("INPUT_ENABLE_MASK", glib->getDAQLinkInputMask(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DAV_TIMEOUT", glib->getDAQLinkDAVTimeout(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("MAX_DAV_TIMER", glib->getDAQLinkDAVTimer(0), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("LAST_DAV_TIMER", glib->getDAQLinkDAVTimer(1), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("NOTINTABLE_ERR",
                        glib->getDAQLinkNonidentifiableErrors(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DISPER_ERR", glib->getDAQLinkDisperErrors(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("EVT_SENT", glib->getDAQLinkEventsSent(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("L1AID", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("RUN_TYPE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("RUN_PARAMS", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("L1A_FIFO_DATA_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DAQ_FIFO_DATA_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("L1A_FIFO_NEAR_FULL_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DAQ_FIFO_NEAR_FULL_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DAQ_ALMOST_FULL_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTS_WARN_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("DAQ_WORD_RATE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_SPY", glib->getTTCSpyBuffer(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("MMCM_LOCKED", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("BC0_LOCKED", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PHASE_LOCKED", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("SYNC_DONE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("L1A_RATE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("MMCM_UNLOCK_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("BC0_UNLOCK_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PHASE_UNLOCK_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PHASE_UNLOCK_TIME", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("SYNC_DONE_TIME", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("BC0_OVERFLOW_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("BC0_UNDERFLOW_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PA_PLL_LOCK_WINDOW", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PA_PHASE_SHIFT_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PA_PLL_LOCK_CLOCKS", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("PA_FSM_STATE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_MEAN", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_MIN", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_MAX", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_JUMP_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_JUMP_SIZE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("TTC_PM_PHASE_JUMP_TIME", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_MEAN", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_MIN", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_MAX", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_JUMP_CNT", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_JUMP_SIZE", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("GTH_PM_PHASE_JUMP_TIME", glib->getDAQLinkL1AID(), NULL,
                        GEMUpdateType::HW32);
  is_glib->createUInt32("BC0_CMD", glib->getTTCConfig(AMCTTCCommand::TTC_BC0),
                        NULL, GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("EC0_CMD", glib->getTTCConfig(AMCTTCCommand::TTC_EC0),
                        NULL, GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("RESYNC_CMD",
                        glib->getTTCConfig(AMCTTCCommand::TTC_RESYNC), NULL,
                        GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("OC0_CMD", glib->getTTCConfig(AMCTTCCommand::TTC_OC0),
                        NULL, GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("HARD_RESET_CMD",
                        glib->getTTCConfig(AMCTTCCommand::TTC_HARD_RESET), NULL,
                        GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("CALPULSE_CMD",
                        glib->getTTCConfig(AMCTTCCommand::TTC_CALPULSE), NULL,
                        GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("START_CMD",
                        glib->getTTCConfig(AMCTTCCommand::TTC_START), NULL,
                        GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("STOP_CMD", glib->getTTCConfig(AMCTTCCommand::TTC_STOP),
                        NULL, GEMUpdateType::NOUPDATE);
  is_glib->createUInt32("TEST_SYNC_CMD",
                        glib->getTTCConfig(AMCTTCCommand::TTC_TEST_SYNC), NULL,
                        GEMUpdateType::NOUPDATE);
  for (uint8_t oh = 0; oh < glib->getSupportedOptoHybrids(); ++oh) {
    std::stringstream ohname;
    ohname << "OH" << (int)oh;
    is_glib->createUInt32(ohname.str() + "_STATUS", glib->getLinkDAQStatus(oh),
                          NULL, GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_CORRUPT_VFAT_BLK_CNT",
                          glib->getLinkDAQCounters(oh, 0), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_EVN",
                          glib->getLinkDAQCounters(oh, 1), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_EOE_TIMEOUT",
                          glib->getDAQLinkDAVTimer(oh), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_MAX_EOE_TIMER",
                          glib->getDAQLinkDAVTimer(0), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_LAST_EOE_TIMER",
                          glib->getDAQLinkDAVTimer(1), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_TRIGGER_CNT",
                          glib->getDAQLinkDAVTimer(0), NULL,
                          GEMUpdateType::HW32);
    is_glib->createUInt32(ohname.str() + "_TRIGGER_RATE",
                          glib->getDAQLinkDAVTimer(0), NULL,
                          GEMUpdateType::HW32);
    for (int cluster = 0; cluster < 8; ++cluster) {
      std::stringstream cluname;
      cluname << "CLUSTER_SIZE_" << cluster;
      is_glib->createUInt32(ohname.str() + "_" + cluname.str() + "_RATE",
                            glib->getLinkDAQCounters(oh, 1), NULL,
                            GEMUpdateType::HW32);
      is_glib->createUInt32(ohname.str() + "_" + cluname.str() + "_CNT",
                            glib->getLinkDAQCounters(oh, 1), NULL,
                            GEMUpdateType::HW32);
      cluname.str("");
      cluname.clear();
      cluname << "DEBUG_LAST_CLUSTER_" << cluster;
      is_glib->createUInt32(ohname.str() + "_" + cluname.str(),
                            glib->getLinkDAQCounters(oh, 1), NULL,
                            GEMUpdateType::HW32);
    }
  }
}
void gem::hw::glib::GLIBManager::dumpGLIBFIFO(xgi::Input *in,
                                              xgi::Output *out) {
  dynamic_cast<GLIBManagerWeb *>(p_gemWebInterface)->dumpGLIBFIFO(in, out);
}
