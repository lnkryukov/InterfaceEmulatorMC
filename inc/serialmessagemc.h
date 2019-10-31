#ifndef SERIALMESSAGEMC_H
#define SERIALMESSAGEMC_H

#include "stdint.h"
#include "stdbool.h"

/**
 * @file
 * @namespace SerialMessageMC
 * @brief Неймспейс с функциями и структурами для работы с пакетами на стороне микроконтроллера
 */

    /**
     * @brief Структура, описывающая заголовок пакета.
     *
     * Состоит из 4 байтов, расположенных в памяти последовательно.
     */
    typedef struct __attribute__((__packed__)) {

        /**
         * @brief Интерфейс, куда будут пересылаться данные
         */
        uint8_t type;
        /**
         * @brief Длинна всего пакета
         */
        uint8_t length;
        /**
         * @brief Контрольная сумма
         */
        uint16_t crc;
    } Header;

    /**
     * @brief Список доступных интерфейсов
     */
    enum Command {
        RS485 = 0,
        CAN = 1,
        UPDATE = 2
    };

    /**
    * @brief Функция расчета CRC16
    * @param pcBlock Указатель на массив байтов
    * @param len Длинна массива
    * @return Контрольная сумма
    */
    unsigned short int Crc16(uint8_t *pcBlock, unsigned short len); //Для расчета и проверки checksum использовать эту функцию


    /**
     * @brief Получает заголовок из пакета
     * @param packet Указатель на массив байтов, который является пакетом
     * @return Указатель на структуру, содержащую в себе заголовок
     */
    Header*         getHeader(uint8_t* packet);

    /**
     * @brief Получает указатель на массив данных из пакета
     * @param packet Указатель на массив байтов, который является пакетом
     * @return Указатель на массив байтов, которые являются данными из пакета
     */
    unsigned char*  getData  (uint8_t* packet);

    /**
     * @brief Проверяет соответствие контрольной суммы из пакета расчитанной контрольной сумме
     * @param packet Указатель на массив байтов, который является пакетом
     * @return **True** если контрольная сумма совпала, **False** если нет
     */
    bool            checkCrc (uint8_t* packet);


#endif // SERIALMESSAGEMC_H
