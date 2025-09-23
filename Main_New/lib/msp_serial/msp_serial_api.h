#ifndef MSP_SERIAL_API_H
#define MSP_SERIAL_API_H
#include <mbed.h>


/**
 * @brief Trang thai cua cong com
 * 
 */
typedef enum _serial_state_
{
    _IDLE,
    _HEADER_START,
    _HEADER_M,
    _HEADER_ARROW,
    _HEADER_SIZE,
    _HEADER_CMD,
} _serial_state_;

class msp_serial:public Serial
{
private:
    void rxInterrupt (void);
	_serial_state_  c_state;
	Callback<void(uint8_t)>    _function;
    void serialize8(uint8_t a);
	void serialize16(int16_t a);
	void serialize32(uint32_t a);
	void headSerialResponse(uint8_t err, uint8_t s);
	void headSerialReply(uint8_t s);
	void headSerialError(uint8_t s);
	void tailSerialReply(void);
	void serializeNames(const char *s);        
	uint8_t checksum;
	uint8_t	indRX;
	uint8_t	inBuf[255];				
	uint8_t offset;
	uint8_t dataSize;
	uint8_t cmdMSP;
public:
    /**
     * Ham khoi tao
     * @param TX
     * @param RX
    */    
    msp_serial(PinName TX, PinName RX);

    /**
     * Ham huy
    */    
    ~msp_serial();

    /**
     * Ham khoi tao phan cung che do router
     * @param _baud: ham khoi tao toc do truyen
    */    
    int8_t begin(uint32_t _baud);       

    /**
     * Ham cho phep ngat nhan du lieu hoat dong
    */    
    void isr_enable (void);

    /**
     * Ham khong cho phep ngat nhan hoat dong
    */    
    void isr_disable(void);

    /**
     * Cac ham truyen cau truc
    */     
	void  	 send_struct(uint8_t cmd,uint8_t *cb, uint8_t siz);
	void 	 readstruct(uint8_t *pt, uint8_t size);
	void 	 send_byte(uint8_t cmd,uint8_t data);
	uint8_t  read8(void);
	uint16_t read16(void);
	int16_t  readint16(void);
	uint32_t read32(void);
	uint8_t  get_cmd(void);    
    void     set_callbackRx(Callback<void(uint8_t)> func); 

//Cac ham lien quan den dieu khien module------------------------------------------------------------------------------------------ 
};
#endif