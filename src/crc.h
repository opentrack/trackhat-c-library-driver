// File:   crc.h
// Brief:  CRC calculation
//------------------------------------------------------

#ifndef _CRC_H_
#define _CRC_H_


#define CRC16_CCITT_POLYNOMIAL 0X1021 // X^16 + X^12 + X^

/* Calculate CRC for provided data */
template<typename T>
uint16_t calculateCCITTCRC16(const T message, const size_t numberOfBytes)
{
    uint16_t crcValue = 0xffff;
    uint16_t MSB_IN_UNIT16 = 0x8000;
    for (size_t byte = 0; byte < numberOfBytes; ++byte)
    {
        crcValue ^= (message[byte] << 8);
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (crcValue & MSB_IN_UNIT16)
            {
                crcValue = (crcValue << 1) ^ CRC16_CCITT_POLYNOMIAL;
            }
            else
            {
                crcValue = (crcValue << 1);
            }
        }
    }
    return crcValue;
}


#endif //_CRC_H_
