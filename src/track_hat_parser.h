// File:   track_hat_parser.h
// Brief:  TrackHat parser functions
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_PARSER_H_
#define _TRACK_HAT_PARSER_H_

#include <track_hat_types_internal.h>

#include <stdint.h>
#include <vector>

namespace Parser
{

    /**
     * Create binary frame for GET_STATUS message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     *
     * \return                       Size of the output message.
     */
    size_t createMessageGetStatus(uint8_t* message);


    /**
     * Create binary frame for GET_DEVICE_INFO message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     *
     * \return                       Size of the output message.
     */
    size_t createMessageGetDeviceInfo(uint8_t* message);


    /**
     * Create binary frame for SET_MODE message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     * \param[in]      coordinates   0 - Idle Mode, 1 = Coordinates Mode
     *
     * \return                       Size of the output message.
     */
    size_t createMessageSetMode(uint8_t* message, bool coordinates, TH_FrameType frameType);

    /**
     * Create binary frame for SET_REGISTER message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     * \param[in]      setRegistr    Information about register to set
     *
     * \return                       Size of the output message.
     */
    size_t createMessageSetRegister(uint8_t* message, uint16_t bufferSize, trackHat_SetRegister_t* setRegister, uint8_t* messageTransactionID);
==== BASE ====

    /**
     * Create binary frame for SET_REGISTER_GROUP message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     * \param[in]      setRegistr    Information about register to set
     *
     * \return                       Size of the output message.
     */

    size_t createMessageSetRegisterGroup(uint8_t* message, uint16_t bufferSize, trackHat_SetRegisterGroup_t* setRegisterGroup, uint8_t* messageTransactionID);
 /**
     * Create binary frame for SET_LEDS message.
     *
     * \param[in/out]  message       Buffer to set the frame.
     * \param[in]      ledState      Information about leds to set
     *
     * \return                       Size of the output message.
     */
    size_t createMessageSetLeds(uint8_t* message, trackHat_SetLeds_t* setLeds, uint8_t *messageTransactionID);

   /**
     * Parse binary data na conver it to TrackHat messages.
     *
     * \param[in/out] input        Vector of the data to parse. The parsed bytes are deleted.
     * \param[in/out] messages     Structure of 'trackHat_Messages_t' where parsed data wil be stored.
     *
     */
    void parseInputData(std::vector<uint8_t>& input, trackHat_Messages_t& messages);

    /**
    * Add CRC at the end of frame.
    *
    * \param[in/out]  buffer        Buffer to calculate CRC and append result value.
    * \param[in/out]  intex         Size of current bytes in the message. It is increased after add
    *                               CRC at the end.
    */
    void appednCRC(uint8_t* buffer, size_t& index);


    /**
     * Check if CRC in the frame is correct.
     *
     * \param[in]  buffer        Buffer witch data to check.
     * \param[in]  size          Size of all data in the buffer.
     *
     * \return         true or false depending if CRC is correct or not.
     *
     */
    bool checkCRC(std::vector<uint8_t>& buffer, size_t size);

    void parseRawExtendedPointToHumanRedable(trackHat_ExtendedPointRaw_t& rawPoint, trackHat_ExtendedPoint_t& extendedPointsParsed);

    void printExtendedPoint(trackHat_ExtendedPoint_t extendedPoint, int i);

} // namespace Parser

#endif //_TRACK_HAT_PARSER_H_
