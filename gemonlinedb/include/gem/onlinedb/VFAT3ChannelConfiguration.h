#ifndef GEM_ONLINEDB_VFAT3CHANNELCONFIGURATION_H
#define GEM_ONLINEDB_VFAT3CHANNELCONFIGURATION_H

#include <array>
#include <cstdint>

#include "gem/onlinedb/ConfigurationTraits.h"
#include "gem/onlinedb/PartReference.h"
#include "gem/onlinedb/detail/RegisterData.h"

namespace gem {
    namespace onlinedb {
        /**
         * @brief Describes the configuration of channels in a VFAT 3
         */
        class VFAT3ChannelConfiguration
        {
        public:
            /**
             * @brief The number of channels in a VFAT 3
             */
            static const constexpr std::size_t CHANNEL_COUNT = 128;

            /**
             * @brief Represents the value of a channel register.
             */
            union ChannelRegisterValue
            {
                /**
                 * @brief The raw value.
                 */
                std::uint16_t registerValue;

                /**
                 * @brief Individual settings making up the value.
                 */
                struct {
                    /**
                     * @brief Arming comparator threshold trimming: amplitude.
                     */
                    std::uint16_t armDACCorrectionAmplitude : 6;

                    /**
                     * @brief Arming comparator threshold trimming: polarity.
                     */
                    bool armDACCorrectionPolarity : 1;

                    /**
                     * @brief Zero crossing comparator timing optimization:
                     *        amplitude.
                     */
                    std::uint16_t zccDACCorrectionAmplitude : 6;

                    /**
                     * @brief Zero crossing comparator timing optimization:
                     *        polarity.
                     */
                    bool zccDACCorrectionPolarity : 1;

                    /**
                     * @brief Mask the channel output for trigger and readout.
                     */
                    bool mask : 1;

                    /**
                     * @brief Enable the calibration pulse to the channel input.
                     */
                    bool cal : 1;
                } fields;

                /**
                 * @brief Compares two register unions for equality.
                 */
                inline bool operator== (const ChannelRegisterValue other) const
                {
                    return registerValue == other.registerValue;
                }
            };

            /* Check */
            static_assert(sizeof(ChannelRegisterValue) == sizeof(std::uint16_t),
                          "ChannelRegisterValue not packed in the expected way");

        private:
            std::array<ChannelRegisterValue, CHANNEL_COUNT> registers;

        public:
            /**
             * @brief Gets the configuration of the channel at the given index.
             */
            ChannelRegisterValue get(std::size_t channelIndex) const
            {
                return registers[channelIndex];
            }

            /**
             * @brief Sets the configuration of the channel at the given index.
             */
            void set(std::size_t channelIndex, ChannelRegisterValue configuration)
            {
                registers[channelIndex] = configuration;
            }

            /**
             * @brief Gets registers for serialization.
             */
            detail::RegisterData getRegisterData() const;

            /**
             * @brief Sets registers from serialized form.
             */
            void readRegisterData(const detail::RegisterData &data);

            /**
             * @brief Compares two VFAT3ChannelConfiguration for equality.
             */
            bool operator== (const VFAT3ChannelConfiguration &other) const;
        };

        template<>
        class ConfigurationTraits<VFAT3ChannelConfiguration>
        {
        public:
            static std::string extTableName() { return "GEM_VFAT3_CHANNEL_CONF"; }
            static std::string typeName() { return "GEM VFAT3 Channel Conf Lookup Table"; }
            static std::string kindOfPart() { return "GEM VFAT3"; }
            using PartType = PartReferenceBarcode;
        };

    } /* namespace onlinedb */
} /* namespace gem */

#endif // GEM_ONLINEDB_VFAT3CHANNELCONFIGURATION_H
