#ifndef     BSPCONFIG_H
#define     BSPCONFIG_H

//So pha tin hieu
#define     MAX_SIDE                8
//Thoi gian duoc xem la mat ket noi voi cpu chinh(s)
#define     TIME_OUT_CONNECTIONS    4

#define     AC_TYPE_CARD            HIGH
#define     DC_TYPE_CARD            LOW

#define     AC_TYPE           1
#define     DC_TYPE           0

#define     MAIN_DEBUG               0    
#define     OUPUT_DEBUG              1
#define     ERROR_DEBUG              0       

/**
 * @brief phan cung communication
 * 
 */

#define     CARD_TX_PIN             PA2
#define     CARD_RX_PIN             PA3


#define     DEBUG_TX_PIN            PA9
#define     DEBUG_RX_PIN            PA10


/**
 * @brief phan cung chan dieu khien ngo ra
 * 
 */

/**
 * GREEN - PB6 change   GREEN   - PB0
 * OP1   - PB0          OP1     - PB6
 * version phan cung bi dao thu tu chan (18042024)    
 */
#define OUT_GREEN_GATE              PB0
#define OUT_YELLOW_GATE             PB1
#define OUT_RED_GATE                PB3
#define OUT_WALKING_GREEN_GATE      PB4
#define OUT_WALKING_RED_GATE        PB5 
#define OUT_OP1_GATE                PB6   

#define OUT_OP2_GATE                PB8   
#define OUT_OP3_GATE                PB9   


/**
 * @brief phan cung cac chan doc phan hoi
 * 
 */
#define FB_GREEN_GATE              PA11
#define FB_YELLOW_GATE             PA8
#define FB_RED_GATE                PA7
#define FB_WALKING_GREEN_GATE      PA6
#define FB_WALKING_RED_GATE        PA5 
#define FB_OP1_GATE                PA4

#define FB_OP2_GATE                PA9  
#define FB_OP3_GATE                PA10


#define GREEN_BIT                   0
#define YELLOW_BIT                  1
#define RED_BIT                     2
#define WALKING_GREEN_BIT           3
#define WALKING_RED_BIT             4 
#define OP1_BIT                     5  
#define OP2_BIT                     6  
#define OP3_BIT                     7

// PIN doc tin hieu cmd
#define CS_CARD_GATE               PA0

/**
 * @brief Chan led trang thai hoat dong
 * [soild]: cmd mode
 * [fast flash 200ms]: no config 
 * [slow flash 1s]: active mode
 */
#define LED_STATUS_GATE            PB7

/**
 * @brief pin select card
 * [0] active
 */
#define TYPE_CARD_GATE              PB11 

/** 
 *  @brief Chan hoat dong chip flash
 * */
#define FLASH_CHIP_SELECT           PB12

#define FLASH_FAST_TIME             250
#define FLASH_LOW_TIME              1000
#define NO_CONFIG_PHASE             255

/**
 * Dia chi luu data tren flash
 */
#define ADDR_STORAGE_INFO_CARD      0

//Dinh nghia cac cau truc bien

enum cardWorkingState_t
{
    ACTIVE_STATE = 0,
    CMD_STATE,
    NO_CONFIG_STATE,
};

typedef struct __attribute__((packed))
{
    char sn[32];
    uint32_t imei;
    uint8_t phase;
    uint8_t is_railway_enabled;
    uint8_t is_walking_enabled;
    uint8_t is_dependent_phase;
    uint8_t time_delay_dependent_phase;
} type_one_cardConfig_t;


typedef struct __attribute__((packed)) {
    uint32_t    imei;
	uint8_t     phaseData[8];
}type_cardDataInOut_t;

typedef struct __attribute__((packed)) {
    volatile bool        is_out_data_to_output;
	volatile bool        isWriteConfig;
    volatile bool        isDeleteConfig;
    volatile bool        isRequestConfig;
    volatile bool        is_enable_process_readSensor;
    volatile bool        is_error_output;
    volatile bool        is_request_get_sensor;
    volatile bool        time_setting;
    volatile bool        is_turn_right; 
}type_sysFlag_t;

typedef struct __attribute__((packed))
{
    uint8_t ouput[MAX_SIDE];
    uint8_t mapping[MAX_SIDE];
    uint8_t pair_signal;
} type_mtfc_card_data_t;

typedef struct __attribute__((packed))
{
    uint8_t output_data;
    uint8_t sensor_readback;
    uint32_t mill_time_get_sample;
    char  sms[48];
    uint8_t error_cash[10];
    uint8_t mark_output;
    uint8_t card_type;
} type_mtfc_workbase_t;

typedef struct __attribute__((packed))
{
    uint8_t phase;
    uint8_t mask_output;
    uint32_t imei;
    char    sms[48];
} type_sensor_pkg_t;

typedef struct __attribute__((packed))
{
    uint8_t phase;
    uint32_t imei;
} type_check_pkg_t;

typedef struct __attribute__((packed))
{
    uint8_t phase;
    uint8_t time_delay_dependent_phase;
    uint8_t time_delay_on_railway;
    uint8_t time_delay_off_railway;
    uint8_t time_walking;
} time_setting_t;

/**
 * @brief MACRO COMMUNICATION
 * 
 */

//Xuat du lieu ra card
#define MSP_CARD_WRITE_DATA     0
//Du lieu ngo ra card cong suat
#define MSP_CARD_FB_DATA        1
//Lenh doc cai dat tu card
#define MSP_CARD_READ_CONFIG    2
//Lenh ghi cai dat cho card
#define MSP_CARD_WRITE_CONFIG   3
//Lenh xoa cai dat cua card
#define MSP_CARD_DELETE         4
//Phan hoi cai dat cua card
#define MSP_CARD_FB_CONFIG      5        
//Thong so cam bien cua card
#define MSP_CARD_DATA_SENSOR        6
//Ma lenh yeu cau lay du lieu cua card 
#define MSP_CARD_CHECK_SENSOR       7   
#define CMD_MASTER_TO_SLAVE_TIME_SETTING       51  
#endif