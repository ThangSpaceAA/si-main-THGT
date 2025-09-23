#ifndef CARD_CONTROLLER_H
#define CARD_CONTROLLER_H
#include <mbed.h>
#include <msp_serial_api.h>

#define CS_NULL     0x00
#define CS_SLOT_1   0x01
#define CS_SLOT_2   0x02
#define CS_SLOT_3   0x04
#define CS_SLOT_4   0x08
#define CS_SLOT_5   0x10
#define CS_SLOT_6   0x20
#define CS_SLOT_7   0x40
#define CS_SLOT_8   0x80

class card_control_api : public msp_serial
{
private:
    BusOut mtfc_bus_cs;
    uint8_t mtfc_current_slot_select;
public:
    card_control_api(PinName TX, PinName RX, PinName _cs1, PinName _cs2, PinName _cs3, PinName _cs4, PinName _cs5, PinName _cs6, PinName _cs7, PinName _cs8);
    ~card_control_api();
    void free(void);
    void cs_slot(uint8_t slot);
    uint8_t read_slot_slect_index(void);
};

#endif