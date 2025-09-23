#include <card_control_api.h>

card_control_api::card_control_api(PinName TX, PinName RX, PinName _cs1, PinName _cs2, PinName _cs3, PinName _cs4, PinName _cs5, PinName _cs6, PinName _cs7, PinName _cs8)
    : msp_serial(TX, RX), mtfc_bus_cs(_cs1, _cs2, _cs3, _cs4, _cs5, _cs6, _cs7, _cs8)
{
    this->free();
    mtfc_current_slot_select = 0;
}

card_control_api::~card_control_api()
{
    this->free();
    mtfc_current_slot_select = 0;
}

void card_control_api::free(void)
{
    mtfc_current_slot_select = 0;
    mtfc_bus_cs.write(CS_NULL);
}
void card_control_api::cs_slot(uint8_t slot)
{
    switch (slot)
    {
    case 1:
        mtfc_bus_cs.write(CS_SLOT_1);
        mtfc_current_slot_select = 1;
        break;
    case 2:
        mtfc_bus_cs.write(CS_SLOT_2);
        mtfc_current_slot_select = 2;
        break;
    case 3:
        mtfc_bus_cs.write(CS_SLOT_3);
        mtfc_current_slot_select = 3;
        break;
    case 4:
        mtfc_bus_cs.write(CS_SLOT_4);
        mtfc_current_slot_select = 4;
        break;
    case 5:
        mtfc_bus_cs.write(CS_SLOT_5);
        mtfc_current_slot_select = 5;
        break;
    case 6:
        mtfc_bus_cs.write(CS_SLOT_6);
        mtfc_current_slot_select = 6;
        break;
    case 7:
        mtfc_bus_cs.write(CS_SLOT_7);
        mtfc_current_slot_select = 7;
        break;
    case 8:
        mtfc_bus_cs.write(CS_SLOT_8);
        mtfc_current_slot_select = 8;
        break;
    default:
        break;
    }
}

uint8_t card_control_api::read_slot_slect_index(void)
{
    return mtfc_current_slot_select;
}