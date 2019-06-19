#ifndef GEM_HW_EXCEPTION_EXCEPTION_H
#define GEM_HW_EXCEPTION_EXCEPTION_H
#include <string>
#include "xcept/Exception.h"
#define GEM_HW_DEFINE_EXCEPTION(EXCEPTION_NAME)                                \
  namespace gem {                                                              \
  namespace hw {                                                               \
  namespace exception {                                                        \
  class EXCEPTION_NAME : virtual public xcept::Exception {                     \
  public:                                                                      \
    EXCEPTION_NAME() : xcept::Exception() {};                                  \
    EXCEPTION_NAME(std::string name, std::string message, std::string module,  \
                   int line, std::string function)                             \
        : xcept::Exception(name, message, module, line, function) {};          \
    EXCEPTION_NAME(std::string name, std::string message, std::string module,  \
                   int line, std::string function, xcept::Exception &err)      \
        : xcept::Exception(name, message, module, line, function, err) {};     \
  };                                                                           \
  }                                                                            \
  }                                                                            \
  }
GEM_HW_DEFINE_EXCEPTION(Exception)
GEM_HW_DEFINE_EXCEPTION(WriteValueMismatch)
GEM_HW_DEFINE_EXCEPTION(ConfigurationParseProblem)
GEM_HW_DEFINE_EXCEPTION(ConfigurationProblem)
GEM_HW_DEFINE_EXCEPTION(ConfigurationValidationProblem)
GEM_HW_DEFINE_EXCEPTION(HardwareProblem)
GEM_HW_DEFINE_EXCEPTION(UninitializedDevice)
GEM_HW_DEFINE_EXCEPTION(RCMSNotificationError)
GEM_HW_DEFINE_EXCEPTION(SOAPTransitionProblem)
GEM_HW_DEFINE_EXCEPTION(NULLReadoutPointer)
GEM_HW_DEFINE_EXCEPTION(SoftwareProblem)
GEM_HW_DEFINE_EXCEPTION(TransitionProblem)
GEM_HW_DEFINE_EXCEPTION(ValueError)
GEM_HW_DEFINE_EXCEPTION(RPCMethodError)
#define GEM_HW_DEFINE_ALARM(ALARM_NAME) GEM_HW_DEFINE_EXCEPTION(ALARM_NAME)
GEM_HW_DEFINE_ALARM(MonitoringFailureAlarm)
#define GEM_CATCH_RPC_ERROR(MSG_BASE, EX_TYPE)                                 \
  catch (xhal::utils::XHALRPCNotConnectedException const &e) {                 \
    std::stringstream errmsg;                                                  \
    errmsg << e.what();                                                        \
    CMSGEMOS_ERROR(MSG_BASE << "error: " << errmsg.str());                     \
    XCEPT_RAISE(EX_TYPE, errmsg.str());                                        \
  }                                                                            \
  catch (xhal::utils::XHALRPCException const &e) {                             \
    std::stringstream errmsg;                                                  \
    errmsg << e.what();                                                        \
    CMSGEMOS_ERROR(MSG_BASE << "error: " << errmsg.str());                     \
    XCEPT_RAISE(EX_TYPE, errmsg.str());                                        \
  }                                                                            \
  catch (gem::hw::exception::RPCMethodError const &e) {                        \
    std::stringstream errmsg;                                                  \
    errmsg << e.what();                                                        \
    CMSGEMOS_ERROR(MSG_BASE << "error: " << errmsg.str());                     \
    XCEPT_RAISE(EX_TYPE, errmsg.str());                                        \
  }
#define GEM_HW_TRANSITION_CATCH(MSG_BASE, EX_TYPE)                             \
  catch (uhalException const &e) {                                             \
    std::stringstream msg;                                                     \
    msg << MSG_BASE << " caught uHAL exception " << e.what();                  \
    CMSGEMOS_ERROR(msg.str());                                                 \
    XCEPT_RAISE(EX_TYPE, msg.str());                                           \
  }                                                                            \
  catch (EX_TYPE const &e) {                                                   \
    std::stringstream msg;                                                     \
    msg << MSG_BASE << " caught exception " << e.what();                       \
    CMSGEMOS_ERROR(msg.str());                                                 \
    XCEPT_RAISE(EX_TYPE, msg.str());                                           \
  }                                                                            \
  catch (toolbox::net::exception::MalformedURN const &e) {                     \
    std::stringstream msg;                                                     \
    msg << MSG_BASE << " caught exception " << e.what();                       \
    CMSGEMOS_ERROR(msg.str());                                                 \
    XCEPT_RAISE(EX_TYPE, msg.str());                                           \
  }                                                                            \
  catch (std::exception const &e) {                                            \
    std::stringstream msg;                                                     \
    msg << MSG_BASE << " caught exception " << e.what();                       \
    CMSGEMOS_ERROR(msg.str());                                                 \
    XCEPT_RAISE(EX_TYPE, msg.str());                                           \
  }
#endif
