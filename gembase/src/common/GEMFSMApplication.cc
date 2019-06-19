#include "gem/base/GEMFSMApplication.h"
#include "toolbox/BSem.h"
#include "toolbox/string.h"
#include "toolbox/fsm/AsynchronousFiniteStateMachine.h"
#include "toolbox/fsm/InvalidInputEvent.h"
#include "toolbox/fsm/FailedEvent.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "xcept/Exception.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/NamespaceURI.h"
#include "xgi/Input.h"
#include "xgi/Method.h"
#include "xgi/Output.h"
#include "xgi/framework/Method.h"
#include "xgi/framework/UIManager.h"
#include "xoap/Method.h"
#include "gem/base/GEMFSM.h"
#include "gem/base/GEMMonitor.h"
#include "gem/base/GEMWebApplication.h"
#include "gem/base/exception/Exception.h"
#include "gem/base/utils/GEMInfoSpaceToolBox.h"
#include "gem/base/utils/exception/Exception.h"
#include "gem/utils/soap/GEMSOAPToolBox.h"
#include "gem/utils/exception/Exception.h"
gem::base::GEMFSMApplication::GEMFSMApplication(
    xdaq::ApplicationStub *stub) throw(xdaq::exception::Exception)
    : GEMApplication(stub), m_gemfsm(this), m_progress(0.0),
      b_accept_web_commands(true), m_wl_semaphore(toolbox::BSem::FULL),
      m_db_semaphore(toolbox::BSem::FULL), m_cfg_semaphore(toolbox::BSem::FULL),
      m_web_semaphore(toolbox::BSem::FULL),
      m_infspc_semaphore(toolbox::BSem::FULL) {
  CMSGEMOS_DEBUG("GEMFSMApplication::ctor begin");
  CMSGEMOS_DEBUG("GEMFSMApplication::Creating xgi bindings...");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiInitialize,
                               "Initialize");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiConfigure,
                               "Configure");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiStart,
                               "Start");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiStop, "Stop");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiPause,
                               "Pause");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiResume,
                               "Resume");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiHalt, "Halt");
  xgi::framework::deferredbind(this, this, &GEMFSMApplication::xgiReset,
                               "Reset");
  xgi::bind(this, &GEMFSMApplication::jsonStateUpdate, "stateUpdate");
  CMSGEMOS_DEBUG("GEMFSMApplication::Created xgi bindings");
  xoap::bind(this, &GEMFSMApplication::changeState, "Initialize", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Configure", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Start", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Stop", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Pause", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Resume", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Halt", XDAQ_NS_URI);
  xoap::bind(this, &GEMFSMApplication::changeState, "Reset", XDAQ_NS_URI);
  CMSGEMOS_DEBUG("GEMFSMApplication::Created xoap bindings");
  m_initSig =
      toolbox::task::bind(this, &GEMFSMApplication::initialize, "initialize");
  m_confSig =
      toolbox::task::bind(this, &GEMFSMApplication::configure, "configure");
  m_startSig = toolbox::task::bind(this, &GEMFSMApplication::start, "start");
  m_stopSig = toolbox::task::bind(this, &GEMFSMApplication::stop, "stop");
  m_pauseSig = toolbox::task::bind(this, &GEMFSMApplication::pause, "pause");
  m_resumeSig = toolbox::task::bind(this, &GEMFSMApplication::resume, "resume");
  m_haltSig = toolbox::task::bind(this, &GEMFSMApplication::halt, "halt");
  m_resetSig = toolbox::task::bind(this, &GEMFSMApplication::reset, "reset");
  CMSGEMOS_DEBUG("GEMFSMApplication::Created task bindings");
  std::stringstream tmpLoopName;
  uint32_t localID = this->getApplicationDescriptor()->getLocalId();
  std::string className = this->getApplicationDescriptor()->getClassName();
  CMSGEMOS_DEBUG("GEMFSMApplication::Obtained local ID and class names "
                 << localID << ", " << className);
  tmpLoopName << "urn:toolbox-task-workloop:" << className << ":" << localID;
  workLoopName = tmpLoopName.str();
  CMSGEMOS_DEBUG("GEMFSMApplication::Created workloop name " << workLoopName);
  updateState();
  p_appInfoSpace->addListener(this, "urn:xdaq-event:setDefaultValues");
  toolbox::net::URN appStateISURN(m_urn +
                                  toolbox::toString(":appState-infospace"));
  if (xdata::getInfoSpaceFactory()->hasItem(appStateISURN.toString())) {
    CMSGEMOS_DEBUG("GEMFSMApplication::infospace "
                   << appStateISURN.toString() << " already exists, getting");
    p_appStateInfoSpace =
        xdata::getInfoSpaceFactory()->get(appStateISURN.toString());
  } else {
    CMSGEMOS_DEBUG("GEMFSMApplication::infospace "
                   << appStateISURN.toString() << " does not exist, creating");
    p_appStateInfoSpace =
        xdata::getInfoSpaceFactory()->create(appStateISURN.toString());
  }
  p_appStateInfoSpaceToolBox = std::shared_ptr<utils::GEMInfoSpaceToolBox>(
      new utils::GEMInfoSpaceToolBox(this, p_appStateInfoSpace, false));
  p_appInfoSpace->fireItemAvailable("application:state", p_appStateInfoSpace);
  p_appInfoSpaceToolBox->createString("StateName", m_stateName.toString(),
                                      &m_stateName,
                                      utils::GEMInfoSpaceToolBox::PROCESS);
  p_appInfoSpaceToolBox->createString("StateMessage", m_stateMessage.toString(),
                                      &m_stateMessage,
                                      utils::GEMInfoSpaceToolBox::PROCESS);
  p_appInfoSpaceToolBox->createDouble("StateProgress", m_progress.value_,
                                      &m_progress,
                                      utils::GEMInfoSpaceToolBox::PROCESS);
  p_appStateInfoSpaceToolBox->createString("StateName", m_stateName.toString(),
                                           &m_stateName,
                                           utils::GEMInfoSpaceToolBox::PROCESS);
  p_appStateInfoSpaceToolBox->createString(
      "StateMessage", m_stateMessage.toString(), &m_stateMessage,
      utils::GEMInfoSpaceToolBox::PROCESS);
  p_appStateInfoSpaceToolBox->createDouble("StateProgress", m_progress.value_,
                                           &m_progress,
                                           utils::GEMInfoSpaceToolBox::PROCESS);
  p_appInfoSpace->addItemRetrieveListener("StateName", this);
  p_appInfoSpace->addItemRetrieveListener("StateMessage", this);
  p_appInfoSpace->addItemRetrieveListener("StateProgress", this);
  p_appInfoSpace->addItemChangedListener("StateName", this);
  p_appInfoSpace->addItemChangedListener("StateMessage", this);
  p_appInfoSpace->addItemChangedListener("StateProgress", this);
  p_appInfoSpace->fireItemValueRetrieve("StateName");
  p_appInfoSpace->fireItemValueRetrieve("StateMessage");
  p_appInfoSpace->fireItemValueRetrieve("StateProgress");
  CMSGEMOS_DEBUG("GEMFSMApplication::ctor end");
}
gem::base::GEMFSMApplication::~GEMFSMApplication() {
  std::string msgBase = "[GEMFSMApplication::~GEMFSMApplication] ";
  CMSGEMOS_DEBUG(msgBase << "Begin");
  CMSGEMOS_DEBUG(msgBase << "End");
}
void gem::base::GEMFSMApplication::xgiInitialize(xgi::Input *in,
                                                 xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiInitialize] ";
  CMSGEMOS_DEBUG(msgBase << "Begin");
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG("GEMFSMApplication::xgiInitialize::Sending SOAP command "
                     "to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Initialize", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      std::stringstream msg, errmsg;
      m_stateMessage = msg.str();
      errmsg << "GEMFSMApplication::xgiInitialize Initialize failed: " << msg;
      CMSGEMOS_ERROR(errmsg.str());
      XCEPT_RETHROW(xgi::exception::Exception, errmsg.str(), e);
    }
  }
  CMSGEMOS_DEBUG("GEMFSMApplication::xgiInitialize end");
}
void gem::base::GEMFSMApplication::xgiConfigure(xgi::Input *in,
                                                xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiConfigure] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Configure", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Configure failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiStart(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiStart] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Start", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Start failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiStop(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiStop] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Stop", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Stop failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiPause(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiPause] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Pause", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Pause failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiResume(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiResume] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Resume", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Resume failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiHalt(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiHalt] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Halt", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Halt failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::xgiReset(xgi::Input *in, xgi::Output *out) {
  std::string msgBase = "[GEMFSMApplication::xgiReset] ";
  if (b_accept_web_commands) {
    try {
      CMSGEMOS_DEBUG(msgBase << "Sending SOAP command to application");
      gem::utils::soap::GEMSOAPToolBox::sendCommand(
          "Reset", p_appContext, p_appDescriptor, p_appDescriptor);
    }
    catch (toolbox::fsm::exception::Exception &e) {
      XCEPT_RETHROW(xgi::exception::Exception, "Reset failed", e);
    }
  }
}
void gem::base::GEMFSMApplication::jsonStateUpdate(xgi::Input *in,
                                                   xgi::Output *out) {
  p_gemWebInterface->jsonStateUpdate(in, out);
}
void gem::base::GEMFSMApplication::transitionDriver(
    toolbox::Event::Reference event) {
  std::string msgBase = "[GEMFSMApplication::transitionDriver] ";
  CMSGEMOS_DEBUG(msgBase << "(" << event->type() << ")");
  try {
    if (event->type() == "Initialize" || event->type() == "Configure" ||
        event->type() == "Start" || event->type() == "Stop" ||
        event->type() == "Pause" || event->type() == "Resume" ||
        event->type() == "Halt" || event->type() == "Reset") {
      CMSGEMOS_DEBUG(msgBase << "submitting workloopDriver(" << event->type()
                             << ")");
      workloopDriver(event->type());
      return;
    } else if (event->type() == "IsInitial" ||
               event->type() == "IsConfigured" ||
               event->type() == "IsRunning" || event->type() == "IsPaused" ||
               event->type() == "IsHalted") {
      CMSGEMOS_DEBUG(msgBase << "Recieved confirmation that state changed to "
                             << event->type());
    } else if (event->type() == "Fail" || event->type() == "fail") {
      CMSGEMOS_DEBUG(msgBase << "Recieved fail event type");
    } else {
      CMSGEMOS_DEBUG(msgBase << "Unknown transition command");
      XCEPT_RAISE(toolbox::fsm::exception::Exception,
                  "Unknown transition command");
    }
  }
  catch (gem::utils::exception::Exception &ex) {
    CMSGEMOS_ERROR(msgBase << "Caught gem::utils::exception::Exception");
    fireEvent("Fail");
    XCEPT_RETHROW(toolbox::fsm::exception::Exception, "State Transition Failed",
                  ex);
  }
  updateState();
}
void gem::base::GEMFSMApplication::workloopDriver(std::string const &command) {
  std::string msgBase = "[GEMFSMApplication::workloopDriver] ";
  CMSGEMOS_DEBUG(msgBase << "workloopDriver begin");
  try {
    toolbox::task::WorkLoopFactory *wlf =
        toolbox::task::WorkLoopFactory::getInstance();
    CMSGEMOS_DEBUG(msgBase << "Trying to access the workloop with name "
                           << workLoopName);
    toolbox::task::WorkLoop *loop = wlf->getWorkLoop(workLoopName, "waiting");
    if (!loop->isActive())
      loop->activate();
    CMSGEMOS_DEBUG(msgBase << "Workloop should now be active");
    if (command == "Initialize")
      loop->submit(m_initSig);
    else if (command == "Configure")
      loop->submit(m_confSig);
    else if (command == "Start")
      loop->submit(m_startSig);
    else if (command == "Stop")
      loop->submit(m_stopSig);
    else if (command == "Pause")
      loop->submit(m_pauseSig);
    else if (command == "Resume")
      loop->submit(m_resumeSig);
    else if (command == "Halt")
      loop->submit(m_haltSig);
    else if (command == "Reset")
      loop->submit(m_resetSig);
    CMSGEMOS_DEBUG(msgBase << "Workloop should now be submitted");
  }
  catch (gem::utils::exception::Exception &e) {
    std::stringstream msg;
    msg << "GEMFSMApplication::workloopDriver Workloop failure "
           "(gem::utils::exception)";
    CMSGEMOS_ERROR(msg.str());
    XCEPT_RETHROW(gem::utils::exception::Exception, msg.str(), e);
  }
  catch (toolbox::task::exception::Exception &e) {
    std::stringstream msg;
    msg << "GEMFSMApplication::workloopDriver Workloop failure "
           "(toolbox::task::exception)";
    CMSGEMOS_ERROR(msg.str());
    XCEPT_RETHROW(gem::utils::exception::Exception, msg.str(), e);
  }
  updateState();
  CMSGEMOS_DEBUG(msgBase << "workloopDriver end");
}
void
gem::base::GEMFSMApplication::resetAction(toolbox::Event::Reference event) {
  std::string msgBase = "[GEMFSMApplication::resetAction] ";
  CMSGEMOS_DEBUG(msgBase << "resetAction(" << event->type() << ")");
  p_gemMonitor->reset();
  CMSGEMOS_INFO(msgBase << "Firing 'IsInitial' into the FSM");
  fireEvent("IsInitial");
  updateState();
}
void gem::base::GEMFSMApplication::stateChanged(
    toolbox::fsm::FiniteStateMachine &fsm) {
  std::string msgBase = "[GEMFSMApplication::stateChanged] ";
  CMSGEMOS_INFO(msgBase << "stateChanged");
  updateState();
}
void gem::base::GEMFSMApplication::transitionFailed(
    toolbox::Event::Reference event) {
  std::string msgBase = "[GEMFSMApplication::transitionFailed] ";
  CMSGEMOS_WARN(msgBase << "transitionFailed(" << event->type() << ")");
  updateState();
}
void gem::base::GEMFSMApplication::fireEvent(std::string event) {
  std::string msgBase = "[GEMFSMApplication::fireEvent] ";
  CMSGEMOS_INFO(msgBase << "fireEvent(" << event << ")");
  try {
    toolbox::Event::Reference e(new toolbox::Event(event, this));
    m_gemfsm.fireEvent(e);
  }
  catch (toolbox::fsm::exception::Exception &e) {
    XCEPT_RETHROW(xoap::exception::Exception, "invalid command", e);
  }
  updateState();
}
xoap::MessageReference
gem::base::GEMFSMApplication::changeState(xoap::MessageReference msg) {
  std::string msgBase = "[GEMFSMApplication::changeState] ";
  CMSGEMOS_DEBUG(msgBase << "changeState");
  updateState();
  return m_gemfsm.changeState(msg);
}
bool gem::base::GEMFSMApplication::initialize(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::initialize] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(msgBase << "initialize called, current state: "
                        << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentFSMState()) != STATE_INITIALIZING) {
    CMSGEMOS_DEBUG(msgBase << "not in " << STATE_INITIALIZING << " sleeping ("
                           << m_gemfsm.getCurrentState() << ")");
    usleep(10);
  }
  CMSGEMOS_DEBUG(msgBase << "initialize called, current state: "
                         << m_gemfsm.getCurrentState());
  p_gemWebInterface->buildCfgWebpage();
  m_progress = 0.0;
  try {
    CMSGEMOS_DEBUG(msgBase << "Calling initializeAction");
    this->initializeAction();
    CMSGEMOS_DEBUG(msgBase << "Finished initializeAction");
    p_gemWebInterface->buildCfgWebpage();
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "Caught gem::utils::exception: " << ex.what();
    CMSGEMOS_ERROR(msgBase << msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "Caught toolbox::task::exception: " << ex.what();
    CMSGEMOS_ERROR(msgBase << msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "Malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msgBase << msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "Caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msgBase << msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "Caught other exception";
    CMSGEMOS_ERROR(msgBase << msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  if (p_gemMonitor && !m_disableMonitoring) {
    CMSGEMOS_DEBUG(
        msgBase
        << "initialize found p_gemMonitor pointer, starting monitoring");
    try {
      p_gemMonitor->startMonitoring();
    }
    catch (toolbox::task::exception::Exception const &ex) {
      CMSGEMOS_WARN("Unable to start monitoring " << ex.what());
    }
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::initialize Firing 'IsHalted' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  m_progress = 1.0;
  fireEvent("IsHalted");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::configure(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::configure] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "Called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) !=
         m_gemfsm.getStateName(STATE_CONFIGURING)) {
    CMSGEMOS_DEBUG(msgBase << "Not in " << STATE_CONFIGURING << " sleeping ("
                           << m_gemfsm.getCurrentState() << ")");
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "Called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    configureAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "Caught gem::utils::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "Caught toolbox::task::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "Firing 'IsConfigured' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsConfigured");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::start(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::start] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "start called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) !=
         m_gemfsm.getStateName(STATE_STARTING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "start called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    startAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::start caught gem::utils::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::start caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::start malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::start caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::start caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::start Firing 'IsRunning' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsRunning");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::pause(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::pause] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "pause called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) != m_gemfsm.getStateName(STATE_PAUSING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "pause called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    pauseAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::pause gem::utils::exception " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::pause caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::pause malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::pause caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::pause caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::pause Firing 'IsPaused' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsPaused");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::resume(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::resume] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(msgBase << "resume called, current state: "
                        << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) !=
         m_gemfsm.getStateName(STATE_RESUMING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(msgBase << "resume called, current state: "
                         << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    resumeAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::resume gem::utils::exception " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::resume caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::resume malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::resume caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::resume caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::resume Firing 'IsRunning' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsRunning");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::stop(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::stop] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "stop called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) !=
         m_gemfsm.getStateName(STATE_STOPPING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "stop called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    stopAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::stop gem::utils::exception " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::stop caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::stop malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::stop caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::stop caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::stop Firing 'IsConfigured' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsConfigured");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::halt(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::halt] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "halt called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) != m_gemfsm.getStateName(STATE_HALTING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "halt called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    haltAction();
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::halt gem::utils::exception " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::halt caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::halt malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::halt caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::halt caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::halt Firing 'IsHalted' into the FSM";
  CMSGEMOS_INFO(msg.str());
  m_stateMessage = msg.str();
  fireEvent("IsHalted");
  m_wl_semaphore.give();
  return false;
}
bool gem::base::GEMFSMApplication::reset(toolbox::task::WorkLoop *wl) {
  std::string msgBase = "[GEMFSMApplication::reset] ";
  m_wl_semaphore.take();
  CMSGEMOS_INFO(
      msgBase << "reset called, current state: " << m_gemfsm.getCurrentState());
  while ((m_gemfsm.getCurrentState()) !=
         m_gemfsm.getStateName(STATE_RESETTING)) {
    usleep(10);
  }
  CMSGEMOS_DEBUG(
      msgBase << "reset called, current state: " << m_gemfsm.getCurrentState());
  try {
    m_progress = 0.0;
    resetAction();
    m_progress = 0.90;
    m_progress = 0.95;
    p_gemWebInterface->buildCfgWebpage();
    m_progress = 1.0;
  }
  catch (gem::utils::exception::Exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::reset gem::utils::exception " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::task::exception::Exception &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::reset caught toolbox::task::exception: "
        << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (toolbox::net::exception::MalformedURN const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::reset malformed URN: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (std::exception const &ex) {
    std::stringstream msg;
    msg << "GEMFSMApplication::reset caught std::exception: " << ex.what();
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  catch (...) {
    std::stringstream msg;
    msg << "GEMFSMApplication::reset caught unknown exception";
    CMSGEMOS_ERROR(msg.str());
    m_stateMessage = msg.str();
    fireEvent("Fail");
    m_wl_semaphore.give();
    return false;
  }
  std::stringstream msg;
  msg << "GEMFSMApplication::reset Firing 'IsInitial' into the FSM";
  CMSGEMOS_INFO(msg.str());
  fireEvent("IsInitial");
  m_wl_semaphore.give();
  return false;
}
