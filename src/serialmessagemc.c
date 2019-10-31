/**
 * @file
 */

#include "../inc/serialmessagemc.h"


/**
 * Расчитывает контрольную сумму пакета.\n
 * @warning Использовать только эту функцию, иначе не сойдется!
 *
 *  - Name  : CRC-16 CCITT\n
 *  - Poly  : 0x1021    x^16 + x^12 + x^5 + 1\n
 *  - Init  : 0xFFFF\n
 *  - Revert: false\n
 *  - XorOut: 0x0000\n
 *  - Check : 0x29B1 ("123456789")\n
 *  - MaxLen: 4095 байт (32767 бит)\n
 *
 * Обнаружение одинарных, двойных, тройных и всех нечетных ошибок\n
 */
unsigned short Crc16(uint8_t *pcBlock, unsigned short len) {
    unsigned short crc = 0xFFFF;
    unsigned char i;

    while (len--) {
        crc ^= *pcBlock++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

Header* getHeader(uint8_t *packet) {
    return (Header*) (packet);
}

unsigned char* getData(uint8_t *packet) {
    return (unsigned char*) (packet + 4);
}

bool checkCrc(uint8_t *packet) {
    Header* header = getHeader(packet);
    unsigned char*   data   = getData(packet);

    unsigned short int compCrc = Crc16(data, header->length - 4);

    return compCrc == header->crc;
}
