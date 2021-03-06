/** @file version.h */

#ifndef GEM_BASE_VERSION_H
#define GEM_BASE_VERSION_H

#ifndef DOXYGEN_IGNORE_THIS

#include "config/PackageInfo.h"

namespace gembase {

#define GEMBASE_VERSION_MAJOR 1
#define GEMBASE_VERSION_MINOR 0
#define GEMBASE_VERSION_PATCH 1
#define GEMBASE_PREVIOUS_VERSIONS "0.0.0,0.1.0,0.2.0,0.2.1,0.2.2,0.3.0,0.3.1,0.3.2,0.4.0,0.99.0,0.99.1,1.0.0"

#define GEMBASE_VERSION_CODE PACKAGE_VERSION_CODE(GEMBASE_VERSION_MAJOR, GEMBASE_VERSION_MINOR, GEMBASE_VERSION_PATCH)
#ifndef GEMBASE_PREVIOUS_VERSIONS
#define GEMBASE_FULL_VERSION_LIST PACKAGE_VERSION_STRING(GEMBASE_VERSION_MAJOR, GEMBASE_VERSION_MINOR, GEMBASE_VERSION_PATCH)
#else
#define GEMBASE_FULL_VERSION_LIST GEMBASE_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(GEMBASE_VERSION_MAJOR, GEMBASE_VERSION_MINOR, GEMBASE_VERSION_PATCH)
#endif

  const std::string package     = "gembase";
  const std::string versions    = GEMBASE_FULL_VERSION_LIST;
  const std::string summary     = "Base classes for GEM DAQ applications";
  const std::string description = "";
  const std::string authors     = "GEM Online Systems Group";
  const std::string link        = "https://cms-gem-daq-project.github.io/cmsgemos/";

  config::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (config::PackageInfo::VersionException);
  std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif // DOXYGEN_IGNORE_THIS

#endif // GEM_BASE_VERSION_H
