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

    size_t createMessageGetDeviceInfo(uint8_t* message)
    {
        size_t i = 0;
        message[i++] = MessageID::ID_GET_DEVICE_INFO;
        message[i++] = transactionID++;
        appednCRC(message, i);
        return i;
    }

    size_t createMessageSetMode(uint8_t* message, bool coordinates)
    {
        size_t i = 0;
        message[i++] = MessageID::ID_SET_MODE;
        message[i++] = transactionID++;
        message[i++] = static_cast<uint8_t>(coordinates);
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

    void parseMessageDeviceInfo(std::vector<uint8_t>& input, MessageDeviceInfo& deviceInfo)
    {
        WaitForSingleObject(deviceInfo.m_mutex, INFINITE);

        deviceInfo.m_transactionID = input[1];
        deviceInfo.m_hardwareVersion = input[2];
        deviceInfo.m_softwareVersionMajor = input[3];
        deviceInfo.m_softwareVersionMinor = input[4];
        deviceInfo.m_serialNumber = static_cast<uint32_t>(input[5]) << 24 |
                                    static_cast<uint32_t>(input[6]) << 16 |
                                    static_cast<uint32_t>(input[7]) << 8 |
                                    static_cast<uint32_t>(input[8]);

        ReleaseMutex(deviceInfo.m_mutex);
        SetEvent(deviceInfo.m_newMessageEvent);
    }

    void parseMessageCoordinates(std::vector<uint8_t>& input, MessageCoordinates& coordinates)
    {
        trackHat_Point_t* points = coordinates.m_points.m_point;
        uint16_t value = 0;
        size_t byte = 1;

        WaitForSingleObject(coordinates.m_mutex, INFINITE);

        for (size_t i = 0; i < TRACK_HAT_NUMBER_OF_POINTS; i++)
        {
            value = input[byte++] << 8;
            value = value | input[byte++];
            points[i].m_x = value;

            value = input[byte++] << 8;
            value = value | input[byte++];
            points[i].m_y = value;

            points[i].m_brightness = input[byte++];
        }

        ReleaseMutex(coordinates.m_mutex);
        SetEvent(coordinates.m_newMessageEvent);
    }

    void parseMessageACK(std::vector<uint8_t>& input, trackHat_Messages_t& messages)
    {
        messages.m_lastACKTransactionId = input[1];
    }

    void parseMessageNACK(std::vector<uint8_t>& input, MessageNACK& nack)
    {
        nack.m_transactionID = input[1];
        nack.m_reason = static_cast<NACKReason>(input[2]);
    }

    void parseInputData(std::vector<uint8_t>& input, trackHat_Messages_t& messages)
    {
        while (input.size() > 0)
        {
            // Validation of the frame :
            // 1) ID ok, CRC ok = > parse rest of the frame
            // 2) ID ok, CRC err = > leave the first byte; frames may be shifted
            //    and ID  might be correct by accident (there is no frame separator
            //    in the protocol)
            // 3) ID ok, insufficient data for CRC = > skip parsing; it is possible
            //    that not all data has been read or ID is wrong
            // 4) ID err = > leave the first byte; frames may be shifted

            switch (input[0])
            {
                case MessageID::ID_COORDINATE:
                {
                    if (input.size() >= MessageCoordinates::FrameSize)
                    {
                        if (checkCRC(input, MessageCoordinates::FrameSize))
                        {
                            //LOG_INFO("New Coordinates message.");
                            parseMessageCoordinates(input, messages.m_coordinates);
                            input.erase(input.begin(), input.begin() + MessageCoordinates::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("New Coordinates message - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
                    else
                    {
                        // Not enough data. Finish parsing
                        return;
                    }
                    break;
                }

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
                    break;
                }

                case MessageID::ID_DEVICE_INFO:
                {
                    if (input.size() >= MessageDeviceInfo::FrameSize)
                    {
                        if (checkCRC(input, MessageDeviceInfo::FrameSize))
                        {
                            LOG_INFO("New Device Info message.");
                            parseMessageDeviceInfo(input, messages.m_deviceInfo);
                            input.erase(input.begin(), input.begin() + MessageDeviceInfo::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("New Device Info message - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
                    else
                    {
                        // Not enough data. Finish parsing
                        return;
                    }
                    break;
                }

                case MessageID::ID_ACK:
                {
                    if (input.size() >= MessageACK::FrameSize)
                    {
                        if (checkCRC(input, MessageACK::FrameSize))
                        {
                            LOG_INFO("ACK");
                            parseMessageACK(input, messages);
                            input.erase(input.begin(), input.begin() + MessageACK::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("ACK - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
                    else
                    {
                        // Not enough data. Finish parsing
                        return;
                    }
                    break;
                }

                case MessageID::ID_NACK:
                {
                    if (input.size() >= MessageNACK::FrameSize)
                    {
                        if (checkCRC(input, MessageNACK::FrameSize))
                        {
                            LOG_ERROR("NACK");
                            parseMessageNACK(input, messages.m_nack);
                            input.erase(input.begin(), input.begin() + MessageNACK::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("NACK - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
                    else
                    {
                        // Not enough data. Finish parsing
                        return;
                    }
                    break;
                }

                default:
                {
                    LOG_ERROR("Unknown frame Id " << input[0] << ".");
                    input.erase(input.begin());
                    break;
                }
            }
        }
    }


    void appednCRC(uint8_t* message, size_t& index)
    {
        uint16_t crc = calculateCCITTCRC16(message, index);
        message[index++] = crc >> 8;
        message[index++] = crc & 0xff;
    }

    bool checkCRC(std::vector<uint8_t>& buffer, size_t size)
    {
        // Get CRC from the last two bytes
        uint16_t crc1 = static_cast<uint16_t>(buffer[size - 2]) << 8 |
                        static_cast<uint16_t>(buffer[size - 1]);
        // Calculate CRC without last two bytes
        uint16_t crc2 = calculateCCITTCRC16(buffer, size - 2);
        return crc1 == crc2;
    }

} // namespace Parser