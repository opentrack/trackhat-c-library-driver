// File:   track_hat_parser.cpp
// Brief:  TrackHat parser functions
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#include "track_hat_parser.h"

#include "crc.h"
#include "logger.h"
#include "track_hat_types_internal.h"

#include <atomic>
#include <iostream>

/* Transaction ID counter is one for all library */
static std::atomic<uint8_t> transactionID(255);

namespace Parser
{

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

    size_t createMessageSetMode(uint8_t* message, bool coordinates, TH_FrameType frameType)
    {
        size_t i = 0;
        message[i++] = MessageID::ID_SET_MODE;
        message[i++] = transactionID++;
        message[i++] = static_cast<uint8_t>(coordinates);
        message[i++] = static_cast<uint8_t>(frameType);
        appednCRC(message, i);
        return i;
    }

    size_t createMessageSetRegister(uint8_t* message, uint16_t bufferSize, trackHat_SetRegister_t* setRegister, uint8_t* messageTransactionID)
    {
        if (bufferSize < 7)
            return 0;
        *messageTransactionID = transactionID;
        message[0] = MessageID::ID_SET_REGISTER_VALUE;
        message[1] = transactionID++;
        message[2] = static_cast<uint8_t>(setRegister->m_registerBank);
        message[3] = static_cast<uint8_t>(setRegister->m_registerAddress);
        message[4] = static_cast<uint8_t>(setRegister->m_registerValue);
        size_t messageLength = 5;
        appednCRC(message, messageLength);
        return messageLength;
    }
    size_t createMessageSetRegisterGroup(uint8_t* message, uint16_t bufferSize, trackHat_SetRegisterGroup_t* setRegisterGroup,
                                         uint8_t* messageTransactionID)
    {
        if (bufferSize < 8)
            return 0;
        *messageTransactionID = transactionID;
        message[0] = MessageID::ID_SET_REGISTER_GROUP;
        message[1] = transactionID++;
        message[2] = static_cast<uint8_t>(setRegisterGroup->numberOfRegisters);
        for(uint8_t i = 0; i < setRegisterGroup->numberOfRegisters; i++)
        {
            size_t indexOfCurrentRegister = 3 + (3*i);
            message[indexOfCurrentRegister] = static_cast<uint8_t>(setRegisterGroup->setRegisterGroupValue[0].m_registerBank);
            message[indexOfCurrentRegister + 1] = static_cast<uint8_t>(setRegisterGroup->setRegisterGroupValue[0].m_registerAddress);
            message[indexOfCurrentRegister + 2] = static_cast<uint8_t>(setRegisterGroup->setRegisterGroupValue[0].m_registerValue);
        }
        size_t messageLength = 1 + 1 + 1 + 3*setRegisterGroup->numberOfRegisters;
        appednCRC(message, messageLength);
        return messageLength;
    }

    size_t createMessageSetLeds(uint8_t* message, trackHat_SetLeds_t* setLeds, uint8_t* messageTransactionID)
    {
        *messageTransactionID = transactionID;
        message[0] = MessageID::ID_SET_LEDS;
        message[1] = transactionID++;
        message[2] = static_cast<uint8_t>(setLeds->ledRedState);
        message[3] = static_cast<uint8_t>(setLeds->ledGreenState);
        message[4] = static_cast<uint8_t>(setLeds->ledBlueState);
        size_t messageLength = 5;
        appednCRC(message, messageLength);
        return messageLength;
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

    void parseMessageCoordinates(const std::vector<uint8_t>& input, MessageCoordinates& coordinates)
    {
        trackHat_Point_t* points = coordinates.m_points.m_point;
        uint16_t value = 0;
        size_t byte = 1;

        ::WaitForSingleObject(coordinates.m_mutex, INFINITE);

        for (size_t i = 0; i < TRACK_HAT_NUMBER_OF_POINTS; i++)
        {
            value = static_cast<uint16_t>(input[byte++] << 8);
            value = value | input[byte++];
            points[i].m_x = value;

            value = static_cast<uint16_t>(input[byte++] << 8);
            value = value | input[byte++];
            points[i].m_y = value;

            points[i].m_brightness = input[byte++];
        }

        ::ReleaseMutex(coordinates.m_mutex);
        ::SetEvent(coordinates.m_newMessageEvent);
        ::SetEvent(coordinates.m_newCallbackEvent);
    }

    void parseMessageExtendedCoordinates(const std::vector<uint8_t>& input, MessageExtendedCoordinates& extendedCoordinates)
    {
        trackHat_ExtendedPointRaw_t rawPoints[TRACK_HAT_NUMBER_OF_POINTS];

        bool result = checkCRC(input, MessageExtendedCoordinates::FrameSize);
        if (result)
        {
            ::memcpy(&rawPoints, input.data()+1, MessageExtendedCoordinates::FrameSize-3);
            for (size_t i=0; i<TRACK_HAT_NUMBER_OF_POINTS; i++)
            {
                parseRawExtendedPointToHumanRedable(rawPoints[i], extendedCoordinates.m_points.m_point[i]);
            }
            ::ReleaseMutex(extendedCoordinates.m_mutex);
            ::SetEvent(extendedCoordinates.m_newMessageEvent);
            ::SetEvent(extendedCoordinates.m_newCallbackEvent);
        }
    }

    void parseMessageACK(const std::vector<uint8_t>& input, trackHat_Messages_t& messages)
    {
        messages.m_lastACKTransactionId = input[1];
    }

    void parseMessageNACK(const std::vector<uint8_t>& input, MessageNACK& nack)
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

				case MessageID::ID_EXTENDED_COORDINATES:
				{

				    if (input.size() >= MessageExtendedCoordinates::FrameSize)
					{
                        if (checkCRC(input, MessageNACK::FrameSize))
                        {
                            //LOG_INFO("New Extended Coordinates message.");
                            parseMessageExtendedCoordinates(input, messages.m_extendedCoordinates);
                            input.erase(input.begin(), input.begin() + MessageExtendedCoordinates::FrameSize);
                        }
                        else
                        {
                            LOG_ERROR("New Extended Coordinates message - wrong CRC.");
                            input.erase(input.begin());
                        }
                    }
				    else
					{
						return;
					}
					break;
				}

				default:
				{
					char bytes[8];
					sprintf_s(bytes, sizeof(bytes), "0x%02x", input[0]);
					LOG_ERROR("Unknown frame Id " << bytes << ".");
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

    bool checkCRC(const std::vector<uint8_t>& buffer, size_t size)
    {
        // Get CRC from the last two bytes
        auto crc1 = static_cast<uint16_t>(
			static_cast<unsigned>(buffer[size - 2]) << 8 | static_cast<unsigned>(buffer[size - 1]));
        // Calculate CRC without last two bytes
        uint16_t crc2 = calculateCCITTCRC16(buffer, size - 2);
        return crc1 == crc2;
    }

    void parseRawExtendedPointToHumanRedable(const trackHat_ExtendedPointRaw_t& rawPoint, trackHat_ExtendedPoint_t& extendedPointsParsed)
    {
        extendedPointsParsed.m_area = static_cast<uint16_t>(static_cast<unsigned>(rawPoint.m_areaHigh << 8) | static_cast<unsigned>(rawPoint.m_areaLow));
        extendedPointsParsed.m_coordinateX = static_cast<uint16_t>(static_cast<unsigned>(rawPoint.m_coordinateXHigh << 8) | static_cast<unsigned>(rawPoint.m_coordinateXLow)) & 0x3fff;
        extendedPointsParsed.m_coordinateY = static_cast<uint16_t>(static_cast<unsigned>(rawPoint.m_coordinateYHigh << 8) | static_cast<unsigned>(rawPoint.m_coordinateYLow)) & 0x3fff;
        extendedPointsParsed.m_averageBrightness = rawPoint.m_averageBrightness;
        extendedPointsParsed.m_maximumBrightness = rawPoint.m_maximumBrightness;
        extendedPointsParsed.m_range = rawPoint.m_range;
        extendedPointsParsed.m_radius = rawPoint.m_radius;
        extendedPointsParsed.m_boundryLeft = rawPoint.m_boundryLeft;
        extendedPointsParsed.m_boundryRigth = rawPoint.m_boundryRigth;
        extendedPointsParsed.m_boundryUp = rawPoint.m_boundryUp;
        extendedPointsParsed.m_boundryDown = rawPoint.m_boundryDown;
        extendedPointsParsed.m_aspectRatio = rawPoint.m_aspectRatio;
        extendedPointsParsed.m_vx = rawPoint.m_vx;
        extendedPointsParsed.m_vy = rawPoint.m_vy;
    }

    void printExtendedPoint(trackHat_ExtendedPoint_t extendedPoint, int i)
    {
        std::cout << "Index = " << i << " X = " << extendedPoint.m_coordinateX << " Y = " << extendedPoint.m_coordinateY << std::endl;
    }


} // namespace Parser
