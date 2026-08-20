#ifndef DECLARE_FUNCTION_H
#define DECLARE_FUNCTION_H
#include <mbed.h>
#include <bsp_config.h>
#include <state_controller_api.h>

#pragma region DECLARE HAM

/**
 * @brief Xu ly hoat dong cua led bao trang thai
 * 
 */
void mtfc_led_status_processing(void);

/**
 * @brief Ham ngat voi chu ky 1 seconds
 * 
 */
void mtfc_one_seconds_ticker_event_handler();

/**
 * @brief Test cac che do hoat dong cua den
 * 
 */
void mtfc_test_state_active(void);


/**
 * @brief chuyen doi vi tri tin hieu xuat ra theo cai dat phan cung
 * 
 * @param signal  du lieu vao
 * @param idx     index port data  
 * @return uint8_t du lieu sau khi chuyen doi
 */
uint8_t mtfc_output_signal_position_hardware_convert(uint8_t signal, uint8_t idx);

/**
 * @brief Xuat tin hieu ra card cong suat
 * 
 * @param signal du lieu vao
 * @param num_side so pha xuat du lieu
 */
void mtfc_output_signal_data_to_card(uint8_t *signal, uint8_t num_side);

/**
 * @brief Xuat tin hieu ra card cong suat
 * 
 * @param mtfc vung du lieu lam viec
 * @param signal du lieu can xuat
 * @param num_side so pha xuat tin hieu
 */
void mtfc_output_signal_data_to_card(type_mtfc_cycle_working_package_t *mtfc, uint8_t *signal, uint8_t num_side);

/**
 * @brief Xuat tin hieu ngo ra ra man hinh
 * 
 * @param signal tin hieu den
 * @param countdown dem nguoc
 * @param num_side so pha du lieu
 * @param is_green chi so pha dang o trang thai xanh
 */
void mtfc_debug_output(uint8_t *signal, uint8_t *mapping, uint8_t *countdown, uint8_t num_side, uint8_t is_green);
/**
 * @brief Tinh xuat xuat tin hieu den va dem nguoc
 * 
 * @param mtfc bien luu cac thong so lam viec
 * @return int8_t 
 */
int8_t mtfc_seek_lamp_signal_convert(type_mtfc_cycle_working_package_t *mtfc);

/**
 * @brief load bieu mau chu ky chien luoc thoi gian cua den
 * 
 * @param src tong kho luu tru 
 * @param dat vung luu lam viec
 * @param idx chi so cua cycle form
 * @return int8_t #0 loi khong cap nhat duoc chu ky
 */
int8_t mtfc_cycle_load(type_mtfc_schedule_t *src, type_mtfc_cycle_working_package_t *dat, uint8_t idx);

/**
 * @brief Khoi tao lai cac thong so bo dem khi hoan tat 1 chu ky lam viec
 * 
 * @param dat vung du lieu lam viec
 */
void mtfc_seek_counter_reload(type_mtfc_cycle_working_package_t *dat);

/**
 * @brief ham xuat du lieu che do hoat dong tu dong
 * 
 * @param mtfc_schedule vung du lieu cai dat thong so xanh vang do
 * @param mtfc_current_cycle vung du lieu lam viec
 * @param mtfc_new_cycle vung du lieu luu giu cac cai dat moi
 * @param active_time  thoi gian 
 * @param idx chi so cycle form dang hoat dong
 * @return int8_t #0 load cycle form loi ->chuyen sang che do chop den vang
 */
int8_t mtfc_auto_lamp_countdown_processing(type_mtfc_schedule_t *mtfc_schedule, type_mtfc_cycle_working_package_t *mtfc_current_cycle, type_mtfc_cycle_working_package_t *mtfc_new_cycle, uint16_t *t_seek, bool active_time, uint8_t idx);
/**
 * @brief Tin trinh xuat tin hieu den che do chop den vang
 * 
 * @param obj Bieu mau luu thong so hoat dong
 * @return int8_t unused
 */
int8_t mtfc_yellow_lamp_flashing_processing(type_mtfc_cycle_working_package_t *obj);

/**
 * @brief Set side tin hieu thanh den xanh, cac pha con lai den do
 * 
 * @param obj vung luu du lieu lam viec
 * @param side side tin hieu can set
 * @return uint8_t chi so pha den xanh
 */
uint8_t mtfc_set_side_to_green(type_mtfc_cycle_working_package_t *obj, uint8_t side);

/**
 * @brief Set side tin hieu thanh den vang, cac pha con lai den do
 * 
 * @param obj vung luu du lieu lam viec
 * @param side side tin hieu can set
 * @return uint8_t chi so pha den xanh
 */
uint8_t mtfc_set_side_to_yellow(type_mtfc_cycle_working_package_t *obj, uint8_t side);

/**
 * @brief 
 * 
 * @param obj vung luu du lieu lam viec
 * @param side_change side tin hieu can set
 * @return int8_t chi so pha den xanh
 */
int8_t mftc_transfer_lamp_side(type_mtfc_cycle_working_package_t *obj, uint8_t side_change);

/**
 * @brief Tat toan bo ngo ra cua CPU
 * 
 * @param obj vung bien luu lam viec
 * @return int8_t 
 */
int8_t mtfc_off_lamp_processing(type_mtfc_cycle_working_package_t *obj);

/**
 * @brief Ham khoi dong mtfc
 * 
 * @return uint8_t trang thai khoi dong
 */
uint8_t mtfc_startup_cpu_processing(void);

/**
 * @brief Cac hoat dong chinh dieu khien den
 * 
 */
void mtfc_normal_cpu_processing(void);

/**
 * @brief Ham  ghi format string conlose len man hinh
 * 
 * @param format 
 * @param ... 
 */
void mtfc_glcd_console(const char *format, ...);

/**
 * @brief Ham ngat nhan du lieu tu card dieu khien cong suat
 * 
 * @param siz 
 */
void mtfc_card_rx_event_handler(uint8_t siz);

/**
 * @brief Ham kiem tra thong so cua card dieu khien
 * 
 * @param obj 
 * @return uint8_t 
 */
uint8_t mtfc_card_detect(type_mtfc_cycle_working_package_t *obj);

/**
 * @brief Ham ngat nhan du lieu tu gateway
 * 
 * @param siz 
 */
void mtfc_gateway_rx_event_handler(uint8_t siz);

/**
 * @brief Gui toan bo thong so schedule
 * 
 * @param p Cong xuat du lieu
 * @param mtfc_schedule vung du lieu can gui
 */
void mtfc_send_schedule(msp_serial *p, type_mtfc_schedule_t *mtfc_schedule);

/**
 * @brief Gui thong so cua schedule theo tien trinh 100ms
 * 
 * @param p Cong suat du lieu
 * @param mtfc_schedule vung du lieu gui
 * @param mtfc_wk vung du lieu lam viec
 * @return int8_t '0' <=> hoan tat qua trinh gui
 */
int8_t mtfc_send_schedule(msp_serial *p, type_mtfc_schedule_t *mtfc_schedule, type_mtfc_cycle_working_package_t *mtfc_wk);

/**
 * @brief Ham su kien phat sinh khi soft ngat ket noi voi bo bieu khien
 * 
 * @param mtfc_wk_base 
 * @param config 
 */
void mtfc_gateway_disconnect_soft_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller);

/**
 * @brief Ham su kien ket noi voi soft
 * 
 * @param mtfc_wk_base 
 * @param config 
 */
void mtfc_gateway_connect_soft_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller);

/**
 * @brief Ham su kien phat sinh khi ket noi voi cloud
 * 
 * @param mtfc_wk_base 
 * @param config 
 */
void mtfc_gateway_disconnect_cloud_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller);

/**
 * @brief Ham su kien phat sinh khi ngat ket noi voi cloud nghia la khong co ket noi voi phan mem
 * 
 * @param mtfc_wk_base 
 * @param config 
 */
void mtfc_gateway_connect_cloud_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller);

/**
 * @brief Tien trinh xu ly giao tiep gateway
 * 
 * @param mtfc_wk_base 
 */
void mtfc_gateway_processing(type_mtfc_cycle_working_package_t *mtfc_wk_base);

/**
 * @brief Gui schedule
 * 
 * @param p 
 * @param mtfc_schedule 
 */
void mtfc_send_schedule(msp_serial *p, type_mtfc_schedule_t *mtfc_schedule);

/**
 * @brief Gui toa do
 * 
 * @param p 
 * @param coor 
 */
void mtfc_send_coordinates(msp_serial *p, type_mtfc_coordinates_message_t *coor);

/**
 * @brief Gui goi tin details
 * 
 * @param p 
 * @param details 
 */
void mtfc_send_mtfc_details(msp_serial *p, type_mtfc_details_message_t *details);

/**
 * @brief Gui goi tin 1s len soft
 * 
 * @param p 
 * @param mes 
 */
void mtfc_send_mtfc_working_one_seconds_message(msp_serial *p, type_mtfc_working_message_t *mes);

/**
 * @brief Gui loai type cpu
 * 
 * @param p 
 * @param mtfc_config 
 */
void mtfc_send_mtfc_type(msp_serial *p, type_mtfc_config_t *mtfc_config);

/**
 * @brief Gui cau hinh phan cung cai dat cua thiet bi
 * 
 * @param p 
 * @param mtfc_config 
 */
void mtfc_send_mtfc_config(msp_serial *p, type_mtfc_config_t *mtfc_config);

/**
 * @brief Ham chuyen doi du lieu cong debug
 * 
 * @param str 
 * @param size 
 */
void mtfc_debug_console_part_data(uint8_t *str, uint8_t size);

/**
 * @brief Console working log to cloud
 * 
 * @param format 
 * @param ... 
 */
void mtfc_gateway_message_log(const char *format, ...);

/**
 * @brief Cai dat thoi gian he thong
 * 
 */
void mtfc_rtc_update(type_date_time_t *t);

/**
 * @brief Xu ly cac tien trinh 1s
 * 
 * @param mtfc_wk_base 
 */
void mtfc_main_ticker_one_seconds (type_mtfc_cycle_working_package_t *mtfc_wk_base);

/**
 * @brief ham ngat nhan du lieu glcd
 * 
 * @param siz 
 */
void mtfc_glcd_rx_event_handler(uint8_t siz);
/**
 * @brief Ham xu ly tien trinh GLCD
 * 
 * @param mtfc_wk_base 
 * @param mtfc_config 
 * @param state 
 */
void mtfc_glcd_processing(type_mtfc_schedule_t *schedule,type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *mtfc_config, type_date_time_t *rtc);

/**
 * @brief Ham cap nhat cac thay doi vao bo nho flash
 * 
 * @param mtfc_wk_base 
 * @param config 
 */
void mtfc_update_mtfc_parameters(type_mtfc_schedule_t *schedule,type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config);

/**
 * @brief Ham kiem tra card duoc ket noi voi pha nao
 * 
 * @param mtfc_wk 
 * @param mtfc_config 
 * @param mtfc_schedule 
 */
void mtfc_check_phase_connect_railway(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config,type_mtfc_schedule_t *mtfc_schedule);

/**
 * @brief Xu ly du lieu giao tiep voi card giao tiep mo rong
 * 
 * @param mtfc_wk 
 * @param mtfc_config 
 * @param mtfc_schedule 
 */
void mtfc_ex_card_communication(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config,type_mtfc_schedule_t *mtfc_schedule);

/**
 * @brief ex card com rx handler
 * 
 * @param siz 
 */
void mtfc_ex_card_com_rx_event_handler(uint8_t siz);

/**
 * @brief Ham doc va gui detect loi card dieu khien
 * 
 * @param mtfc_wk 
 * @param mtfc_config 
 */
void mtfc_sensor_detect_processing(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config);


void mtfc_check_phase_connect_option(type_mtfc_cycle_working_package_t *mtfc_op, type_mtfc_config_t *mtfc_config);
#pragma endregion

#endif