#include "gem/onlinedb/OHv3Configuration.h"

#include <sstream>

#include "gem/onlinedb/VFAT3ChipConfiguration.h"

namespace gem {
    namespace onlinedb {

        detail::RegisterData OHv3Configuration::getRegisterData() const
        {
            auto data = OHv3ConfigurationGen::getRegisterData();

            for (std::size_t wire = 0; wire < getHDMISBitModes().size(); ++wire) {
                std::stringstream ss;
                ss << "HDMI_SBIT_MODE" << wire;
                data[ss.str()] = getHDMISBitModes()[wire];
            }

            for (std::size_t wire = 0; wire < getHDMISBitSel().size(); ++wire) {
                std::stringstream ss;
                ss << "HDMI_SBIT_SEL" << wire;
                data[ss.str()] = getHDMISBitSel()[wire];
            }

            for (std::size_t vfat = 0; vfat < getSotTapDelays().size(); ++vfat) {
                std::stringstream ss;
                ss << "SOT_TAP_DELAY_VFAT" << vfat;
                data[ss.str()] = getSotTapDelays()[vfat];
            }

            for (std::size_t vfat = 0; vfat < getTrigTapDelays().size(); ++vfat) {
                for (std::size_t i = 0; i < getTrigTapDelays(vfat).size(); ++i) {
                    std::stringstream ss;
                    ss << "TRIG_TAP_DELAY_VFAT" << vfat << "_BIT" << i;
                    data[ss.str()] = getTrigTapDelay(vfat, i);
                }
            }

            return data;
        }

        void OHv3Configuration::readRegisterData(const detail::RegisterData &data)
        {
            OHv3ConfigurationGen::readRegisterData(data);

            for (std::size_t wire = 0; wire < getHDMISBitModes().size(); ++wire) {
                std::stringstream ss;
                ss << "HDMI_SBIT_MODE" << wire;
                setHDMISBitMode(wire, data.at(ss.str()));
            }

            for (std::size_t wire = 0; wire < getHDMISBitSel().size(); ++wire) {
                std::stringstream ss;
                ss << "HDMI_SBIT_SEL" << wire;
                setHDMISBitSel(wire, data.at(ss.str()));
            }

            for (std::size_t vfat = 0; vfat < getSotTapDelays().size(); ++vfat) {
                std::stringstream ss;
                ss << "SOT_TAP_DELAY_VFAT" << vfat;
                setSotTapDelay(vfat, data.at(ss.str()));
            }

            for (std::size_t vfat = 0; vfat < getTrigTapDelays().size(); ++vfat) {
                for (std::size_t i = 0; i < getTrigTapDelays(vfat).size(); ++i) {
                    std::stringstream ss;
                    ss << "TRIG_TAP_DELAY_VFAT" << vfat << "_BIT" << i;
                    setTrigTapDelay(vfat, i, data.at(ss.str()));
                }
            }
        }

        bool OHv3Configuration::operator== (const OHv3Configuration &other) const
        {
            return OHv3ConfigurationGen::operator== (other)
                && getHDMISBitModes() == other.getHDMISBitModes()
                && getHDMISBitSel() == other.getHDMISBitSel()
                && getSotTapDelays() == other.getSotTapDelays()
                && getTrigTapDelays() == other.getTrigTapDelays()
                && getVFATConfigs() == other.getVFATConfigs();
        }

        void OHv3Configuration::createAllVFATConfigs()
        {
            for (auto &c : getVFATConfigs()) {
                c = std::make_shared<VFAT3ChipConfiguration>();
            }
        }

        void OHv3Configuration::createAllVFATConfigs(
                const VFAT3ChipConfiguration &config)
        {
            for (auto &c : getVFATConfigs()) {
                c = std::make_shared<VFAT3ChipConfiguration>(config);
            }
        }

    } /* namespace onlinedb */
} /* namespace gem */
