// File:   track_hat_parser.cpp
// Brief:  TrackHat parser functions
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_parser.h"

#include "crc.h"
#include "logger.h"
#include "track_hat_types_internal.h"

#include <atomic>

/* Transaction ID counter is one for all library */
static std::atomic<uint8_t> transactionID = 255;

namespace Parser {

    void appednCRC(uint8_t* message, size_t& intex)
    {
        uint16_t crc = calculateCCITTCRC16(message, intex);
        message[intex++] = crc >> 8;
        message[intex++] = crc & 0xff;
    }

    bool checkCRC(std::vector<uint8_t>& buffer, size_t size)
    {
        uint16_t crc1 = static_cast<uint16_t>(buffer[size - 2]) << 8 |
                        static_cast<uint16_t>(buffer[size - 1]);
        uint16_t crc2 = calculateCCITTCRC16(buffer, size - 2);
        return crc1 == crc2;
    }

} // namespace Parser
