#ifndef GEM_BASE_GEMFSMAPPLICATION_H
#define GEM_BASE_GEMFSMAPPLICATION_H
#include "toolbox/task/exception/Exception.h"
#include "gem/base/GEMApplication.h"
#include "gem/base/GEMFSM.h"
#include "gem/utils/Lock.h"
#include "gem/utils/LockGuard.h"
namespace toolbox {
namespace task {
class WorkLoop;
class ActionSignature;
}
}
namespace gem {
namespace base {
class GEMFSM;
class GEMApplication;
class GEMWebApplication;
class GEMFSMApplication : public GEMApplication {
public:
  friend class GEMFSM;
  friend class GEMWebApplication;
  GEMFSMApplication(xdaq::ApplicationStub *stub)
  throw(xdaq::exception::Exception);
  virtual ~GEMFSMApplication();

protected:
  void xgiInitialize(xgi::Input *in, xgi::Output *out);
  void xgiConfigure(xgi::Input *in, xgi::Output *out);
  void xgiStart(xgi::Input *in, xgi::Output *out);
  void xgiPause(xgi::Input *in, xgi::Output *out);
  void xgiResume(xgi::Input *in, xgi::Output *out);
  void xgiStop(xgi::Input *in, xgi::Output *out);
  void xgiHalt(xgi::Input *in, xgi::Output *out);
  void xgiReset(xgi::Input *in, xgi::Output *out);
  void jsonStateUpdate(xgi::Input *in, xgi::Output *out);
  void workloopDriver(std::string const &command);
  bool initialize(toolbox::task::WorkLoop *wl);
  bool configure(toolbox::task::WorkLoop *wl);
  bool start(toolbox::task::WorkLoop *wl);
  bool pause(toolbox::task::WorkLoop *wl);
  bool resume(toolbox::task::WorkLoop *wl);
  bool stop(toolbox::task::WorkLoop *wl);
  bool halt(toolbox::task::WorkLoop *wl);
  bool reset(toolbox::task::WorkLoop *wl);
  bool calibrationAction(toolbox::task::WorkLoop *wl) {
    return false;
  };
  bool calibrationSequencer(toolbox::task::WorkLoop *wl) {
    return false;
  };
  virtual void initializeAction() = 0;
  virtual void configureAction() = 0;
  virtual void startAction() = 0;
  virtual void pauseAction() = 0;
  virtual void resumeAction() = 0;
  virtual void stopAction() = 0;
  virtual void haltAction() = 0;
  virtual void resetAction() = 0;
  void transitionDriver(toolbox::Event::Reference e);
  virtual void resetAction(toolbox::Event::Reference e);
  virtual void stateChanged(toolbox::fsm::FiniteStateMachine &fsm);
  virtual void transitionFailed(toolbox::Event::Reference event);
  virtual void fireEvent(std::string event);
  virtual xoap::MessageReference changeState(xoap::MessageReference msg);
  std::string workLoopName;
  toolbox::task::ActionSignature *m_initSig;
  toolbox::task::ActionSignature *m_confSig;
  toolbox::task::ActionSignature *m_startSig;
  toolbox::task::ActionSignature *m_stopSig;
  toolbox::task::ActionSignature *m_pauseSig;
  toolbox::task::ActionSignature *m_resumeSig;
  toolbox::task::ActionSignature *m_haltSig;
  toolbox::task::ActionSignature *m_resetSig;

public:
  virtual std::string getCurrentState() {
    updateState();
    return m_stateName.toString();
  };
  virtual toolbox::fsm::State getCurrentFSMState() {
    return m_gemfsm.getCurrentFSMState();
  };
  std::shared_ptr<utils::GEMInfoSpaceToolBox> getAppStateISToolBox() {
    return p_appStateInfoSpaceToolBox;
  };

private:
  GEMFSM m_gemfsm;
  bool b_accept_web_commands;
  toolbox::BSem m_wl_semaphore;
  toolbox::BSem m_db_semaphore;
  toolbox::BSem m_cfg_semaphore;
  toolbox::BSem m_web_semaphore;
  toolbox::BSem m_infspc_semaphore;
  toolbox::task::WorkLoop *p_wl;

protected:
  virtual void updateState() {
    m_stateName = m_gemfsm.getCurrentState();
  };
  std::shared_ptr<utils::GEMInfoSpaceToolBox> p_appStateInfoSpaceToolBox;
  xdata::InfoSpace *p_appStateInfoSpace;
  xdata::Double m_progress;
  xdata::String m_stateName;
  xdata::String m_stateMessage;
};
}
}
#endif
