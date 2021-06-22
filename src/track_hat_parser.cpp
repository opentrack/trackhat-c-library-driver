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

    size_t createMessageGetStatus(uint8_t* message)
    {
        size_t i = 0;
        message[i++] = MessageID::ID_GET_STATUS;
        message[i++] = transactionID++;
        appednCRC(message, i);
        return i;
    }

    void parseMessageStatus(std::vector<uint8_t>& input, MessageStatus& status)
    {
        WaitForSingleObject(status.m_mutex, INFINITE);

        status.m_transactionID = input[1];
        status.m_camStatus = static_cast<CameraStatus>(input[2]);
        status.m_camMode = static_cast<CameraMode>(input[3]);
        status.m_uptimeInSec = static_cast<uint32_t>(input[4]) << 24 |
                               static_cast<uint32_t>(input[5]) << 16 |
                               static_cast<uint32_t>(input[6]) << 8 |
                               static_cast<uint32_t>(input[7]);

        ReleaseMutex(status.m_mutex);
        SetEvent(status.m_newMessageEvent);
    }


    void parseInputData(std::vector<uint8_t>& input, trackHat_Messages_t& messages)
    {
        while (input.size() > 0)
        {
            switch (input[0])
            {
                case MessageID::ID_STATUS:
                {
                    if (input.size() >= MessageStatus::FrameSize)
                    {
                        if (checkCRC(input, MessageStatus::FrameSize))
                        {
                            LOG_INFO("New Status message.");
                            parseMessageStatus(input, messages.m_status);
                            input.erase(input.begin(), input.begin() + MessageStatus::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("New Status message - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
                    else
                    {
                        // Not enough data. Finish parsing
                        return;
                    }
                }

            }
        }
    }


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
