#ifndef GEM_BASE_UTILS_GEMINFOSPACETOOLBOX_H
#define GEM_BASE_UTILS_GEMINFOSPACETOOLBOX_H
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include "xdata/InfoSpace.h"
#include "xdata/InfoSpaceFactory.h"
#include "xdata/SimpleType.h"
#include "xdata/Boolean.h"
#include "xdata/Integer.h"
#include "xdata/Integer32.h"
#include "xdata/Integer64.h"
#include "xdata/UnsignedLong.h"
#include "xdata/UnsignedInteger32.h"
#include "xdata/UnsignedInteger64.h"
#include "xdata/String.h"
#include "xdata/Float.h"
#include "xdata/Double.h"
#include "xdata/Serializable.h"
#include "gem/base/utils/exception/Exception.h"
#include "gem/utils/GEMLogging.h"
namespace gem {
namespace base {
class GEMApplication;
namespace utils {
class GEMInfoSpaceToolBox {
public:
  enum ItemType {
    UINT8,
    UINT16,
    UINT24,
    UINT32,
    UINT64,
    BOOL,
    INTEGER,
    INTEGER32,
    INTEGER64,
    DOUBLE,
    STRING,
    INFOSPACE
  };
  enum UpdateType {
    HW8,
    HW16,
    HW24,
    HW32,
    HW64,
    I2CSTAT,
    PROCESS,
    SOAP,
    TRACKER,
    NOUPDATE
  };
  class GEMInfoSpaceItem {
  public:
    GEMInfoSpaceItem(ItemType itype, UpdateType utype,
                     std::string const &itemname, std::string const &docstring,
                     std::string const &format)
        : m_itype(itype), m_utype(utype), m_name(itemname), m_format(format),
          m_docstring(docstring) {};
    ~GEMInfoSpaceItem() {};
    ItemType m_itype;
    UpdateType m_utype;
    std::string m_name;
    std::string m_format;
    std::string m_docstring;
  };
  GEMInfoSpaceToolBox(gem::base::GEMApplication *gemApp,
                      xdata::InfoSpace *infoSpace, bool autoPush);
  GEMInfoSpaceToolBox(gem::base::GEMApplication *gemApp,
                      std::string const &infoSpaceName, bool autoPush);
  ~GEMInfoSpaceToolBox();
  bool createString(std::string const &itemName, std::string const &value,
                    xdata::String *ptr = NULL, UpdateType type = NOUPDATE,
                    std::string const &docstring = "docstring",
                    std::string const &format = "");
  bool createBool(std::string const &itemName, bool const &value,
                  xdata::Boolean *ptr = NULL, UpdateType type = NOUPDATE,
                  std::string const &docstring = "docstring",
                  std::string const &format = "");
  bool createDouble(std::string const &itemName, double const &value,
                    xdata::Double *ptr = NULL, UpdateType type = NOUPDATE,
                    std::string const &docstring = "docstring",
                    std::string const &format = "");
  bool createInteger(std::string const &itemName, int const &value,
                     xdata::Integer *ptr = NULL, UpdateType type = PROCESS,
                     std::string const &docstring = "docstring",
                     std::string const &format = "dec");
  bool createInteger32(std::string const &itemName, int32_t const &value,
                       xdata::Integer32 *ptr = NULL, UpdateType type = PROCESS,
                       std::string const &docstring = "docstring",
                       std::string const &format = "dec");
  bool createInteger64(std::string const &itemName, int64_t const &value,
                       xdata::Integer64 *ptr = NULL, UpdateType type = PROCESS,
                       std::string const &docstring = "docstring",
                       std::string const &format = "dec");
  bool createUInt32(std::string const &itemName, uint32_t const &value,
                    xdata::UnsignedInteger32 *ptr = NULL,
                    UpdateType type = HW32,
                    std::string const &docstring = "docstring",
                    std::string const &format = "hex");
  bool createUInt64(std::string const &itemName, uint64_t const &value,
                    xdata::UnsignedInteger64 *ptr = NULL,
                    UpdateType type = HW64,
                    std::string const &docstring = "docstring",
                    std::string const &format = "hex");
  std::string getString(std::string const &itemName);
  bool getBool(std::string const &itemName);
  double getDouble(std::string const &itemName);
  int getInteger(std::string const &itemName);
  int32_t getInteger32(std::string const &itemName);
  int64_t getInteger64(std::string const &itemName);
  uint32_t getUInt32(std::string const &itemName);
  uint64_t getUInt64(std::string const &itemName);
  static std::string getString(xdata::InfoSpace *infoSpace,
                               std::string const &itemName);
  static bool getBool(xdata::InfoSpace *infoSpace, std::string const &itemName);
  static double getDouble(xdata::InfoSpace *infoSpace,
                          std::string const &itemName);
  static int getInteger(xdata::InfoSpace *infoSpace,
                        std::string const &itemName);
  static int32_t getInteger32(xdata::InfoSpace *infoSpace,
                              std::string const &itemName);
  static int64_t getInteger64(xdata::InfoSpace *infoSpace,
                              std::string const &itemName);
  static uint32_t getUInt32(xdata::InfoSpace *infoSpace,
                            std::string const &itemName);
  static uint64_t getUInt64(xdata::InfoSpace *infoSpace,
                            std::string const &itemName);
  bool setString(std::string const &itemName, std::string const &value);
  bool setBool(std::string const &itemName, bool const &value);
  bool setDouble(std::string const &itemName, double const &value);
  bool setInteger(std::string const &itemName, int const &value);
  bool setInteger32(std::string const &itemName, int32_t const &value);
  bool setInteger64(std::string const &itemName, int64_t const &value);
  bool setUInt32(std::string const &itemName, uint32_t const &value);
  bool setUInt64(std::string const &itemName, uint64_t const &value);
  xdata::InfoSpace *getInfoSpace() {
    return p_infoSpace;
  };
  std::string name() {
    return p_infoSpace->name();
  };
  bool find(std::string const &key) {
    return m_itemMap.find(key) != m_itemMap.end();
  };
  std::string getFormattedItem(std::string const &itemName,
                               std::string const &format);
  std::string getItemDocstring(std::string const &itemName);
  void reset();

private:
  log4cplus::Logger m_gemLogger;
  std::unordered_map<std::string, GEMInfoSpaceItem *> m_itemMap;
  template <typename T, typename P> struct InfoSpaceTypes {
    typedef std::unordered_map<std::string, std::pair<T, P> > ObjectMap;
  };
  InfoSpaceTypes<uint32_t, xdata::UnsignedInteger32 *>::ObjectMap m_uint32Items;
  InfoSpaceTypes<uint64_t, xdata::UnsignedInteger64 *>::ObjectMap m_uint64Items;
  InfoSpaceTypes<bool, xdata::Boolean *>::ObjectMap m_boolItems;
  InfoSpaceTypes<int, xdata::Integer *>::ObjectMap m_intItems;
  InfoSpaceTypes<int32_t, xdata::Integer32 *>::ObjectMap m_int32Items;
  InfoSpaceTypes<int64_t, xdata::Integer64 *>::ObjectMap m_int64Items;
  InfoSpaceTypes<double, xdata::Double *>::ObjectMap m_doubleItems;
  InfoSpaceTypes<std::string, xdata::String *>::ObjectMap m_stringItems;

protected:
  gem::base::GEMApplication *p_gemApp;
  xdata::InfoSpace *p_infoSpace;
};
}
}
typedef gem::base::utils::GEMInfoSpaceToolBox::UpdateType GEMUpdateType;
}
#endif
