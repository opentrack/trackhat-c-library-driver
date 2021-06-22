// File:   track_hat_parser.h
// Brief:  TrackHat parser functions
// Author: Piotr Nowicki <piotr.nowicki@wizzdev.pl>
//------------------------------------------------------

#ifndef _TRACK_HAT_PARSER_H_
#define _TRACK_HAT_PARSER_H_

#include <track_hat_types_internal.h>

#include <stdint.h>
#include <vector>

namespace Parser {

    /**
    * Add CRC at the end of frame.
    *
    * \param[in/out]  buffer        Buffer to calculate CRC and append result value.
    * \param[in/out]  intex         Size of current bytes in the message. It is increased after add
    *                               CRC at the end.
    */
    void appednCRC(uint8_t* buffer, size_t& intex);


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

} // namespace Parser

#endif //_TRACK_HAT_PARSER_H_
