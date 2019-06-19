#include <gem/utils/GEMRegisterUtils.h>
std::string gem::utils::uint32ToString(uint32_t const &val) {
  std::stringstream res;
  res << static_cast<char>((val >> 24) & 0xff);
  res << static_cast<char>((val >> 16) & 0xff);
  res << static_cast<char>((val >> 8) & 0xff);
  res << static_cast<char>((val >> 0) & 0xff);
  return res.str();
}
std::string gem::utils::uint32ToDottedQuad(uint32_t const &val) {
  std::stringstream res;
  res << static_cast<char>((val >> 24) & 0xff) << std::dec << ".";
  res << static_cast<char>((val >> 16) & 0xff) << std::dec << ".";
  res << static_cast<char>((val >> 8) & 0xff) << std::dec << ".";
  res << static_cast<char>((val >> 0) & 0xff) << std::dec;
  return res.str();
}
std::string gem::utils::uint32ToGroupedHex(uint32_t const &val1,
                                           uint32_t const &val2) {
  std::stringstream res;
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val1 >> 8) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val1 >> 0) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val2 >> 24) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val2 >> 16) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val2 >> 8) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint32_t>((val2 >> 0) & 0xff) << std::dec;
  return res.str();
}
std::string gem::utils::uint64ToGroupedHex(uint64_t const &val) {
  std::stringstream res;
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 40) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 32) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 24) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 16) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 8) & 0xff) << std::dec << ":";
  res << std::setfill('0') << std::setw(2) << std::hex
      << static_cast<uint64_t>((val >> 0) & 0xff) << std::dec;
  return res.str();
}
