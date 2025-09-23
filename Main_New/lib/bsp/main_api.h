#ifndef MAIN_API_H
#define MAIN_API_H
#include <mbed.h>
#include <bsp_config.h>
#include <device_memory_api.h>


/**
 * @brief Khoi tao du lieu chien luoc den ve gia tri mac dinh
 * 
 * @param dat vung du lieu co so
 * @param num_phase so pha tin hieu can khoi tao
 */
void mtfc_contruct_schedule_default(type_mtfc_schedule_t *dat, uint8_t num_phase);

/**
 * @brief Khoi tao default chien luoc thoi gian 
 * 
 * @param dat vung du lieu co so
 * @param num_phase so pha tin hieu can khoi tao
 */
void mtfc_contruct_all_cycle_default(type_cycle_lamp_form_t *dat, uint8_t num_phase);

/**
 * @brief Khoi tao cycle form theo thong so cua nguoi dung
 * 
 * @param dat vung du lieu co so
 * @param num_phase so pha tin hieu can khoi tao
 */
void mtfc_contruct_all_cycle_user_config(type_cycle_lamp_form_t *dat, uint8_t num_phase);

/**
 * @brief Ham khoi tao thong so cho mtfc
 * 
 * @param obj vung du lieu luu lam viec
 */
void mtfc_contruct_mtfc_config_default(type_mtfc_config_t *obj);


/**
 * @brief Xuat du lieu cycle form ra man hinh
 * 
 * @param dat vung luu du lieu
 * @param idx chi so cycle form
 */
void mtfc_debug_print_cycle_form(type_cycle_lamp_form_t *dat, uint8_t idx);

/**
 * @brief Xuat du lieu cycle form ra man hinh
 * 
 * @param dat vung luu du lieu
 */
void mtfc_debug_print_cycle_form(type_cycle_lamp_form_t *dat);

/**
 * @brief Xuat man hinh active time
 * 
 * @param act_time 
 */
void mtfc_debug_print_active_time(type_active_lamp_t *act_time);

/**
 * @brief Xuat man hinh time form
 * 
 * @param time_form 
 */
void mtfc_debug_print_time_form(type_time_lamp_form_t *time_form);

/**
 * @brief Xuat man hinh days form
 * 
 * @param days_form 
 */
void mtfc_debug_print_days_form(type_days_lamp_form_t *days_form);

/**
 * @brief Ham xuat ra man hinh thong so cua card
 * 
 * @param obj 
 * @param slot 
 */
void mtfc_printOut_card_config(type_one_cardConfig_t *obj, uint8_t slot);
/**
 * @brief ngat nhan du lieu serial
 * 
 */
void mtfc_debug_console_rx_event_handler(void);

/**
 * @brief Xuat cau chan dieu khien tren card
 * 
 * @param phase_pin 
 */
void mtfc_printOut_pin_config(type_mtfc_phase_gate_config_t *phase_pin);

/**
         * @brief Kiểm tra active time
         * 
         * @param t_present Thời gian hiện hành
         * @param t_begin Thời giạn bắt đầu
         * @param t_end Thời gian kết thúc
         * @return true thời gian phù hợp với khoảng đang xét
         * @return false 
         */
bool mtfc_check_active_time(type_hm_time_t t_present, type_hm_time_t t_begin, type_hm_time_t t_end);
/**
         * @brief Kiem tra ngay hien tai trong tuan dang ap dung time form nao
         * 
         * @param day_form 
         * @param day_present 
         * @return uint8_t 255 khong co form nao duoc ap dung
         */
uint8_t mtfc_check_day_form(type_days_lamp_form_t *day_form, uint8_t day_present);
/**
 * @brief Kiem tra time line nao dang duoc ap dung
 * 
 * @param time_form 
 * @param index     chi so cua time form duoc ap dung
 * @param t_present Thoi gian hien tai
 * @param time_line 255 255 255 kkhong co time line nao ap dung
 * @return uint8_t  255 khong tim thay
 */
uint8_t mtfc_check_time_form(type_time_lamp_form_t *time_form, uint8_t index ,type_hm_time_t t_present,type_one_timeline_t *time_line);

/**
 * @brief Ham gui console control len gateway
 * 
 * @param format 
 * @param ... 
 */
void mtfc_gateway_control_message_log(const char *format, ...);

/**
 * @brief kiểm tra dữ liệu cycle từ app gửi xuống
 * 
 * @param cycle 
 * @param schedule 
 * @return int8_t -1 dữ liệu lỗi
 */
int8_t mtfc_check_cycle_config_data(type_mtfc_quick_cycle_config_t *cycle, type_mtfc_schedule_t *schedule);

/**
 * @brief 
 * 
 * @param temp 
 * @param cycle 
 * @return int8_t 
 */
int8_t mtfc_one_cycle_set_config(type_one_cycle_lamp_time_t *temp, type_mtfc_quick_cycle_config_t *cycle);
#endif