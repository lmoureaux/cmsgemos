/** @file version.h */

#ifndef GEM_SUPERVISOR_VERSION_H
#define GEM_SUPERVISOR_VERSION_H

#ifndef DOXYGEN_IGNORE_THIS

#include "config/PackageInfo.h"

namespace gemsupervisor {

#define GEMSUPERVISOR_VERSION_MAJOR 1
#define GEMSUPERVISOR_VERSION_MINOR 0
#define GEMSUPERVISOR_VERSION_PATCH 1
#define GEMSUPERVISOR_PREVIOUS_VERSIONS "0.0.0,0.1.0,0.2.0,0.2.1,0.3.0,0.3.1,0.3.2,0.4.0,0.99.0,0.99.1,1.0.0"

#define GEMSUPERVISOR_VERSION_CODE PACKAGE_VERSION_CODE(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)

#ifndef GEMSUPERVISOR_PREVIOUS_VERSIONS
#define GEMSUPERVISOR_FULL_VERSION_LIST PACKAGE_VERSION_STRING(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)
#else
#define GEMSUPERVISOR_FULL_VERSION_LIST GEMSUPERVISOR_PREVIOUS_VERSIONS "," PACKAGE_VERSION_STRING(GEMSUPERVISOR_VERSION_MAJOR, GEMSUPERVISOR_VERSION_MINOR, GEMSUPERVISOR_VERSION_PATCH)
#endif

  const std::string package     = "gemsupervisor";
  const std::string versions    = GEMSUPERVISOR_FULL_VERSION_LIST;
  const std::string summary     = "GEM Supervisor";
  const std::string description = "";
  const std::string authors     = "GEM Online Systems Group";
  const std::string link        = "https://cms-gem-daq-project.github.io/cmsgemos/";

  config::PackageInfo getPackageInfo();
  void checkPackageDependencies() throw (config::PackageInfo::VersionException);
  std::set<std::string, std::less<std::string> > getPackageDependencies();
}

#endif // DOXYGEN_IGNORE_THIS

#endif // GEM_SUPERVISOR_VERSION_H
