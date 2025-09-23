#include <msp_serial_api.h>
#include <bsp_config.h>
#include "debug.h"




msp_serial::msp_serial(PinName TX, PinName RX)
:Serial(TX,RX)
{
	this->baud(115200);
	this->attach(this, &msp_serial::rxInterrupt, Serial::RxIrq);
}

int8_t msp_serial::begin(uint32_t _baud)
{
	this->baud(_baud);
}

msp_serial::~msp_serial()
{

}

void msp_serial::isr_disable(void)
{
	this->attach(0);
}

void msp_serial::isr_enable (void)
{
	this->attach(this,&msp_serial::rxInterrupt,SerialBase::RxIrq);
}

void msp_serial::rxInterrupt (void)
{
	uint8_t c = 0;
	if(this->readable()){    
		c = this->getc();
		//dbCom.putc(c);
		if (c_state == _IDLE) {
			c_state = (c == '$') ? _HEADER_START : _IDLE;
			if (c_state == _IDLE){           //todo 
			}
		} else if (c_state == _HEADER_START) {
			c_state = (c == 'M') ? _HEADER_M : _IDLE;
		} else if (c_state == _HEADER_M) {
			c_state = (c == '>') ? _HEADER_ARROW : _IDLE;
		} else if (c_state == _HEADER_ARROW) {
			if (c > 254) {       		    // now we are expecting the payload size
				c_state = _IDLE;
			}else{
				dataSize = c;
				offset = 0;
				checksum = 0;
				indRX = 0;
				checksum ^= c;
				c_state = _HEADER_SIZE;      // the command is to follow
			}
		} else if (c_state == _HEADER_SIZE) {
			cmdMSP = c;
			checksum ^= c;
			c_state = _HEADER_CMD;
		} else if (c_state == _HEADER_CMD && offset < dataSize){
			checksum ^= c;
			inBuf[offset++] = c;
		} else if (c_state == _HEADER_CMD && offset >= dataSize){
			if (checksum == c) {            //so sanh su lieu checksum					

					if(_function)
					{
						_function(offset-1);
					}
				}														
			
			c_state = _IDLE;
		}
	}
}

void msp_serial::serialize8(uint8_t a)//Truyen 1 byte
{
    this->putc(a);
     checksum ^= a;
}

void msp_serial::serialize16(int16_t a)//Truyen 2 byte
{
    static uint8_t t;
    t = a ;
    this->putc(t);
    checksum ^= t;
    t = (a >> 8) & 0xff;
    this->putc(t);
    checksum ^= t;
}

void msp_serial:: serialize32(uint32_t a)//Truyen 4 byte
{
    static uint8_t t;
    t = a;
    this->putc(t);
    checksum ^= t;
    t = a >> 8;
    this->putc(t);
    checksum ^= t;
    t = a >> 16;
    this->putc(t);
    checksum ^= t;
    t = a >> 24;
    this->putc(t);
    checksum ^= t;
}

void msp_serial:: headSerialResponse(uint8_t err, uint8_t s)//Ham dinh dang truyen chuan Multiwii
{
    serialize8('$');
    serialize8('M');
    serialize8(err ? '!' : '>');
    checksum = 0;               // start calculating a new checksum
    serialize8(s);
    serialize8(cmdMSP);
}

void msp_serial:: headSerialReply(uint8_t s)//Ham bao truyen bao nhieu byte
{
    headSerialResponse(0, s);
}

void msp_serial:: headSerialError(uint8_t s)//Ham bao loi truyen 
{
    headSerialResponse(1, s);
}

void msp_serial:: tailSerialReply(void)//Truyen ma checksum
{
    serialize8(checksum);
}

void msp_serial:: serializeNames(const char *s)
{
    const char *c;
    for (c = s;*c; c++)
        serialize8(*c);
}

uint8_t msp_serial:: read8(void)//Read 1 byte
{
	   return inBuf[indRX++] & 0xff;
}

uint16_t msp_serial::read16(void)//Read 2 byte
{
    uint16_t t = read8();
    t += (uint16_t) read8() << 8;
    return t;
}
 
int16_t msp_serial::readint16(void)//
{
	int16_t temp = (inBuf[indRX++]);
	temp = temp+((inBuf[indRX++])<<8);
    return temp;
}

uint32_t msp_serial:: read32(void)//Read 4 byte
{
    uint32_t t = read16();
    t += (uint32_t) read16() << 16;
    return t;
}

void  msp_serial:: send_struct(uint8_t cmd,uint8_t *cb, uint8_t siz){
	cmdMSP = cmd;
    headSerialReply(siz);
    while (siz--){
        serialize8(*cb++);
    }
	tailSerialReply();
}

void msp_serial::send_byte(uint8_t cmd,uint8_t data){
	cmdMSP = cmd;
	headSerialReply(1);
	serialize8(data);
	tailSerialReply();
}

void msp_serial:: readstruct(uint8_t *pt, uint8_t size){
    uint16_t i = 0;	
    for (i = 0; i < size; i++){				         	
        *pt =	inBuf[indRX++];
        pt++;
    }	
}

uint8_t msp_serial::get_cmd(void)
{
	return cmdMSP;
}


void msp_serial::set_callbackRx(Callback<void(uint8_t)> func)
{
	core_util_critical_section_enter();
	if(func)
		_function = func;
	c_state = _IDLE;
	core_util_critical_section_exit();
}
