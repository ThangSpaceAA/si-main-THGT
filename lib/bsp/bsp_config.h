#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H
#include <mbed.h>
#include <rtc_api_cus.h>

// Su dung watchdog timer
#define USING_WATCHDOG_TIMER 1

// So luowng card cua 1 tu
#define MAX_SIDE 8
// So luong bieu may chu ky tin hieu den
#define MAX_CYCLE_FORM 5
// So luong bieu mau timer
#define MAX_TIME_FORM 5
// So luong timer trong 1 ngay
#define MAX_TIME_LINE 5
// So ngay trong tuan 1
#define MAX_DAYS_WORKING 7
// So tu toi da trong nut den tin hieu
#define MAX_BOX_IN_CROSSROAD 4
// Thoi gian ban chu ky chop den vang (ms)
#define DUTY_TIME_FLASHING_YELLOW_LAMP 1000
#define STARTUP_TIME_FLASH 10000
#define KEEPALIVE_TIME 5000
#define TIME_PING_CYCLE_FORM 500

#define COUNTDOWN_TO_ZERO 1

#define GATEWAY_CONSOLE_MESSAGE_LENGTH 32

#define GATEWAY_LOG_CONTROL_MESSAGE_LENGTH 64
#define TIME_LOOP_SEND_CONFIG_CARD 1000
#define TIME_FLASHING_GREEN_WALKING 6
#define TIME_OFFSET_OFF_WALKING_GREEN 2
#define MIN_GREEN_TIME 5
#define MAX_GREEN_TIME 240
#define MAX_YELLOW_TIME 10
#define MAX_CLEARANCE_TIME 10
#define TIME_NOISE_RAILWAY_SIGNAL 15

#define DEFAULT_YELLOW_TIME 3
#define DEFAULT_CLEARANCE_TIME 0
#define DEFAULT_ACTIVE_BEGIN_H 5
#define DEFAULT_ACTIVE_BEGIN_M 0
#define DEFAULT_ACTIVE_END_H 22
#define DEFAULT_ACTIVE_END_M 0
#define DEFAULT_CYCLE_APPLY 0
#define DEFAULT_TIME_FORM_APPLY 1
#define DEFAULT_NUM_PHASES 2

#define UART1_TX_PIN PA_9
#define UART1_RX_PIN PA_10
#define UART2_TX_PIN PA_0
#define UART2_RX_PIN PA_1
#define UART3_TX_PIN PB_10
#define UART3_RX_PIN PB_11
#define UART4_TX_PIN PA_0
#define UART4_RX_PIN PA_1
#define UART5_TX_PIN PC_12
#define UART5_RX_PIN PD_2
#define UART6_TX_PIN PC_6
#define UART6_RX_PIN PC_7

#define LED_ACT_PIN PE_2
#define LED_CONNECT_PIN PE_3

#define CS_CARD_PIN1 PD_15
#define CS_CARD_PIN2 PD_14
#define CS_CARD_PIN3 PD_13
#define CS_CARD_PIN4 PD_12
#define CS_CARD_PIN5 PD_11
#define CS_CARD_PIN6 PD_10
#define CS_CARD_PIN7 PD_9
#define CS_CARD_PIN8 PD_8

#define ON_OFF_PIN PE_8
#define RAILWAY_PIN PE_9
#define WALKING_PIN PE_10
#define T1_CS_PIN PE_11
#define T2_CS_PIN PE_12
#define T3_CS_PIN PE_13
#define T4_CS_PIN PE_14
#define T5_CS_PIN PE_15

/* thangnm custom phase dependent*/
#define GREEN_DEPENDENT 1
#define YELLOW_DEPENDENT 2
#define RED_DEPENDENT 3

// Trang thai ket noi
typedef enum _mtfc_cpu_type_t {
  mtfc_ac = 0,
  mtfc_dc_24,
  mtfc_dc_36,
  mtfc_dc_48,
  mtfc_solar
} _mtfc_cpu_type_t;

typedef enum _mtfc_com_select_t {
  mtfc_gate_gateway = 0,
  mtfc_gate_display,
  mtfc_gate_communication,
} _mtfc_com_select_t;

// Trang thai ket noi
typedef enum _mtfc_connect_t {
  mtfc_offline = 0,
  mtfc_online,
  mtfc_connect,
  mtfc_disconnect
} _mtfc_connect_t;

// Thu tu bit xuat tin hieu den
typedef enum _pin_out_profile_ {
  pin_green = 0,
  pin_yellow,
  pin_red,
  pin_walking_green,
  pin_walking_red,
  pin_option_1,
  pin_option_2,
  pin_option_3
} _pin_out_profile_;

// che do khi chon switch off se tat den hay chop vang
typedef enum _mtfc_mode_off_ {
  mtfc_mode_off_is_off = 0,
  mtfc_mode_off_is_flashing,
} _mtfc_mode_off_;

// Macro cho cai dat chien luoc
typedef enum _set_state_ {
  mtfc_unset = 0,
  mtfc_unused_form = 255,
  mtfc_unused_time = 255
} _set_state_;

// Trang thai chinh khi moi khoi dong
typedef enum _mtfc_main_state_ {
  mtfc_startup_state = 0,
  mtfc_normal_state,
} _mtfc_main_state_;

typedef enum _mtfc_main_work_state_ {
  mtfc_state_off = 0,
  mtfc_state_auto,
  mtfc_state_manual,
  mtfc_state_railway,
  mtfc_state_walking,
} _mtfc_main_work_state_;

typedef enum _gate_request_date_t {
  mtfc_gateway_request_schedule = 1,
  mtfc_gateway_request_config,
  mtfc_gateway_request_coordinate,
  mtfc_gateway_request_details,
  mtfc_gateway_request_cpu_type,
} _gate_request_date_t;

// /**
//  * @brief Cai dat che do di bo
//  *
//  * @return typedef struct
//  */
// typedef struct __attribute__((packed))
// {
//     uint16_t walking_time;
//     uint16_t walking_time_re_contructions;
// } type_walking_config_para_t;

//---------------------------------------------------------CAI DAT
//CARD---------------------------------------
/**
 * @brief Du lieu cua tung card
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  char sn[32];
  uint32_t imei;
  uint8_t phase;
  uint8_t is_railway_enabled;
  uint8_t is_walking_enabled;
  uint8_t is_dependent_phase;
  uint8_t time_delay_dependent_phase;
} type_one_cardConfig_t;

/**
 * @brief Cau truc luu cai dat cau hinh card cua tu dieu khien
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_one_cardConfig_t index[MAX_SIDE];
} type_cardConfig_t;

//------------------------------------------------------------------------------------------------------------

/**
 * @brief Cau truc luu thoi gian lam viec cua CPU
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_hm_time_t tbegin;
  type_hm_time_t tend;
} type_active_lamp_t;

//------------------------------------------------------BIEU MAU CHIEN
//LUOC----------------------------------
/**
 * @brief Du lieu thoi gian dem nguoc cua 1 pha den
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  uint8_t t_green;        // thoi gian xanh
  uint8_t t_yellow;       // thoi gian vang
  uint8_t t_red;          // thoi gin do
  uint8_t t_start_green;  // thoi gian bat dau xanh
  uint8_t t_end_green;    // thoi gian end xanh
  uint8_t t_start_yellow; // thoi gian bat dau vang
  uint8_t t_end_yellow;   // thoi gian ket thuc vang
} type_one_side_lamp_time_t;

typedef struct __attribute__((packed)) {
  int16_t t_start_flashing; // Thoi gian bat dau chop den di bo
  int16_t t_end_Wk_green;   // Thoi gian ket thuc
} type_green_flashing_t;

typedef struct __attribute__((packed)) {
  type_green_flashing_t side[MAX_SIDE];
} type_time_walking_green_flashing_t;

/**
 * @brief Thoi gian hoat dong cua 1 chien luoc den
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  uint8_t num_side_used;
  uint8_t period_crossroads;
  type_one_side_lamp_time_t side[MAX_SIDE];
  uint8_t clearance_time_crossroads;
} type_one_cycle_lamp_time_t;

/**
 * @brief So luong bieu mau luu chien luoc dem nguoc den (5 bieu mau)
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_one_cycle_lamp_time_t index[MAX_CYCLE_FORM];
} type_cycle_lamp_form_t;

//-----------------------------------------------------------------------------------------------------------

//----------------------------------------------------BIEU MAU THOI
//GIAN-------------------------------------
/**
 * @brief cau truc cua 1 timeline
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_hm_time_t t_begin_apply;
  uint8_t index_cycle_form;
} type_one_timeline_t;

/**
 * @brief cau truc bieu 1 bieu mau thoi gian
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_one_timeline_t select_point[MAX_TIME_LINE];
} type_day_time_line_t;

/**
 * @brief cau truc bieu mau thoi gian
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  type_day_time_line_t index[MAX_TIME_FORM];
} type_time_lamp_form_t;
//-----------------------------------------------------------------------------------------------------------

//----------------------------------------------CHIEN LUOC TRONG
//TUAN----------------------------------------
/**
 * @brief Bieu mau luu chien luoc thoi gian trong tuan
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  uint8_t index[MAX_DAYS_WORKING];
} type_days_lamp_form_t;
//----------------------------------------------------------------------------------------------------------

/**
 * @brief Bien tong luu toan bo chien luoc cua den
 *
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  uint8_t num_side;               // So pha cua CPU
  type_active_lamp_t active_time; // Thoi gian hoat dong cua CPU
  type_cycle_lamp_form_t cycle;   // Luu cac chien luoc
  type_time_lamp_form_t time;     // Bieu mau thoi gian
  type_days_lamp_form_t days;     // Bieu mau tuan
} type_mtfc_schedule_t;

typedef struct __attribute__((packed)) {
  uint8_t num_side;          // So pha cua CPU
  uint8_t yellow_t;          // Thoi gian den vang
  uint8_t clearance_t;       // Thoi gian giai toa
  uint8_t green_t[MAX_SIDE]; // Thoi gian xanh cua tung pha
} type_mtfc_quick_cycle_config_t;

// Cau truc goi tin detail truyen len server
typedef struct __attribute__((packed)) {
  uint8_t type_cpu;
  uint8_t num_side;
  char imei[32];
  char firmware_build_version[32];
} type_mtfc_details_message_t;

//
typedef struct __attribute__((packed)) {
  double latitude;
  double longitude;
} type_mtfc_coordinates_message_t;

// goi tin 1s gui len server
typedef struct __attribute__((packed)) {
  uint8_t num_phase;
  uint8_t walking_signal;
  uint8_t railway_signal;
  uint8_t cpu_active_mode;
  uint8_t input_hardware;
  uint8_t input_software;
  uint8_t idx_time_form;
  uint8_t idx_timeline;
  uint8_t idx_cycle_form;
  uint8_t signal[MAX_SIDE];
  uint8_t mapping[MAX_SIDE];
  uint8_t countdown[MAX_SIDE];
  uint8_t output_sensor[MAX_SIDE];
  time_t rtc_epoch;
} type_mtfc_working_message_t;

// Thong so cai pin xuat tin hieu
typedef struct __attribute__((packed)) {
  uint8_t pin[8];
} type_mtfc_phase_gate_config_t;

typedef struct __attribute__((packed)) {
  uint8_t type_cpu;         // loai CPU
  bool walking_mode_enable; // Che do di bo duoc cho phep hay khong
  bool railway_mode_enable; // che do ket noi duong sat duoc cho phep hay khong
  bool power_meters_enable; // bo do cong suat nguon
  bool locked_hard_control_enable;  // khoa dieu khien khi che do ket noi
  bool is_flashing;                 // che do tat la chop vang hay tat ngo ra
  bool is_enabale_out485_lamp_port; // cho phep xuat du lieu ra cong RS485
  uint8_t railway_delay_on_time;  // thoi gian tre chuyen sang che do duong sat
  uint8_t railway_delay_off_time; // thoi gian delay tat tre tin hieu duong sat
  uint8_t walking_walking_time;   // thoi gian che do di bo
  double latitude;                // toa do
  double longitude;               // toa do
  type_mtfc_phase_gate_config_t
      phase_pin_config[MAX_SIDE]; // du lieu cai dat tung pin tin hieu den
} type_mtfc_config_part1_t;

typedef struct __attribute__((packed)) {      // toa do
  type_one_cardConfig_t index_card[MAX_SIDE]; // du lieu cai dat card
} type_mtfc_config_part2_t;

typedef struct __attribute__((packed)) {
  uint8_t type_cpu;         // loai CPU
  bool walking_mode_enable; // Che do di bo duoc cho phep hay khong
  bool railway_mode_enable; // che do ket noi duong sat duoc cho phep hay khong
  bool power_meters_enable; // bo do cong suat nguon
  bool locked_hard_control_enable;  // khoa dieu khien khi che do ket noi
  bool is_flashing;                 // che do tat la chop vang hay tat ngo ra
  bool is_enabale_out485_lamp_port; // cho phep xuat du lieu ra cong RS485
  uint8_t railway_delay_on_time;  // thoi gian tre chuyen sang che do duong sat
  uint8_t railway_delay_off_time; // thoi gian delay tat tre tin hieu duong sat
  uint8_t walking_walking_time;   // thoi gian che do di bo
  double latitude;                // toa do
  double longitude;               // toa do
  uint8_t dependent_phase_time;   // thoi gian che do di bo
  type_one_cardConfig_t index_card[MAX_SIDE]; // du lieu cai dat card
  type_mtfc_phase_gate_config_t
      phase_pin_config[MAX_SIDE]; // du lieu cai dat tung pin tin hieu den
} type_mtfc_config_t;

typedef struct __attribute__((packed)) {
  uint16_t t_seek;     // bien diem che do dem nguoc
  uint16_t t;          // tong chu ky dem nguoc
  uint8_t num_side;    // so pha dang dieu khien
  uint8_t t_clearance; // Thoi gian giai toa
  type_one_side_lamp_time_t
      side[MAX_SIDE];          // thoi gian xanh vang do dang thuc thi
  uint8_t signal[MAX_SIDE];    // trang thai tin hieu ra card
  uint8_t mapping[MAX_SIDE];   // anh xa ngo ra
  uint8_t countdown[MAX_SIDE]; // bien dem cua den dem nguoc
  uint8_t sensor[MAX_SIDE];
  type_time_walking_green_flashing_t
      t_flashing_wg; // Thoi gian bat dau chop tin hieu den vàng
  uint8_t railway_connect_phase_config[MAX_SIDE]; // Pha nao co ket noi duong
                                                  // sat thi = 1 nguoc lai = 0
  uint8_t current_phase_is_green; // pha nao dang o den xanh =0 nghia la dang
                                  // chop vang hoac off
  uint8_t railway_signal;           // Tin hieu ket noi duong sat
  uint8_t walking_signal;           // Tin hieu den di bo
  uint8_t is_device_conneted_soft;  // trang thai ket noi voi phan mem
  uint8_t is_device_online;         // Ket noi voi cloud
  uint8_t index_side_manual_select; // pha tin hieu duoc chon trong che do
                                    // manual
  volatile uint8_t
      mtfc_main_state; // Trang thai dang khoi dong hay vao che do hd chinh
  volatile uint8_t mtfc_working_state; // Trang thai trong che do chinh
  uint8_t railway_delay_on_time;  // thoi gian tre chuyen sang che do duong sat
  uint8_t railway_delay_off_time; // thoi gian delay tat tre tin hieu duong sat
  uint8_t walking_walking_time;   // thoi gian chuyen
  uint8_t index_card_check;       // chi so check card
  uint8_t input_hard;             // ngo vao dieu khien hardware
  uint8_t input_soft;             // ngo vao dieu khien software
  uint8_t idx_time_form;
  uint8_t idx_timeline;
  uint8_t idx_cycle_form;
  volatile bool is_active_time;
  uint8_t is_mtfc_ready; // Trang thái sẵn sàng hoạt động của thiết bị
  uint8_t index_page_display;
  uint8_t index_send_cycle_form;
  uint8_t is_sd_inserted;           // Trang thai the nho
  float temperature;                // Nhiet do ben trong cpu
  uint8_t is_ex_com_card_connected; // ket noi voi ex card com
  uint8_t is_startup_state;         // CPU moi khoi dong
  uint8_t ex_com_import_feedback;   // Phan hoi trang thai import
  uint8_t ex_com_export_feedback;   // Phan hoi trang thai export
  uint8_t index_send_schedule;      // Bien dem tien trinh send schedule
  uint8_t usb_console_txt[64];      // dung de luu console usb
  uint8_t gps_message_txt[64];      // Dung de luu message
  uint8_t idx_check_sensor; // chi so check gia tri cam bien cua tung card
  uint8_t idx_app_schedule; // chi so schedule ma
  uint32_t millis_ping_cycle_form; //
} type_mtfc_cycle_working_package_t;

typedef struct __attribute__((packed)) {
  uint8_t countdown[MAX_SIDE];
  uint8_t tm_dependent[MAX_SIDE];
  uint8_t status_lamp_dependent[MAX_SIDE]; // x 1 v 2 d 3
} type_time_mtfc_xvdi_t;

typedef struct __attribute__((packed)) {
  volatile bool is_railway_event_on_handle;  // su kien on tin hieu duong sat
  volatile bool is_railway_event_off_handle; // Su kien off tin hieu duong sat
  uint32_t signal_on_counter;

} type_mtfc_railway_work_base_t;

typedef struct __attribute__((packed)) {
  float power_consum_in_day[MAX_BOX_IN_CROSSROAD];
  float power_consum_total[MAX_BOX_IN_CROSSROAD];
  float u[MAX_BOX_IN_CROSSROAD];
  float pow_charger_in_day[MAX_BOX_IN_CROSSROAD];
} type_mtfc_power_meters_message_t;

typedef struct __attribute__((packed)) {
  uint8_t ouput[MAX_SIDE];
  uint8_t mapping[MAX_SIDE];
  uint8_t pair_signal;
  type_date_time_t rtc_time;
  type_time_mtfc_xvdi_t tm_cycle;
} type_mtfc_card_data_t;

typedef struct __attribute__((packed)) {
  uint8_t signal[MAX_SIDE];    // trang thai tin hieu ra card
  uint8_t mapping[MAX_SIDE];   // anh xa ngo ra
  uint8_t countdown[MAX_SIDE]; // bien dem cua den dem nguoc
  uint8_t railway_signal;
  uint8_t walking_signal;
} type_mtfc_lamp485_t;

// Goi tin cai dat den dem nguoc rs485
typedef struct __attribute__((packed)) {
  uint8_t phase;
  uint8_t is_railway;
} type_dn_config_t;

typedef struct __attribute__((packed)) {
  type_date_time_t rtc_time;
  uint8_t server_status;
  uint8_t app_status;
  uint8_t mtfc_working_state; // Trang thai trong che do chinh
  uint8_t side_select;
  uint8_t sd_mount;
  float temperature;
} type_mtfc_page_one_t;

typedef struct __attribute__((packed)) {
  uint16_t t_seek;                // bien diem che do dem nguoc
  uint16_t t;                     // tong chu ky dem nguoc
  uint8_t num_side;               // so pha dang dieu khien
  type_active_lamp_t active_time; // Thoi gian hoat dong cua CPU
  uint8_t t_clearance;            // Thoi gian giai toa
  type_one_side_lamp_time_t
      side[MAX_SIDE]; // thoi gian xanh vang do dang thuc thi
  uint8_t idx_time_form;
  uint8_t idx_timeline;
  uint8_t idx_cycle_form;
} type_mtfc_page_two_t;

typedef struct __attribute__((packed)) {
  uint8_t raiway_enable;
  uint8_t walking_enable;
} type_mtfc_page_iv_t;

typedef struct __attribute__((packed)) {
  uint8_t num_side;
  uint8_t slot_phase[MAX_SIDE];
  uint8_t railway_enable[MAX_SIDE];
} type_mtfc_page_v_t;

typedef struct __attribute__((packed)) {
  uint8_t slot;
  uint8_t phase;
  uint8_t is_walking;
  uint8_t is_railway;
  uint8_t is_dependent_phase;
} type_mtfc_one_card_config_t;

typedef struct __attribute__((packed)) {
  type_date_time_t rtc_time;
  double lati;
  double longi;
} type_mtfc_time_location_t;

typedef struct __attribute__((packed)) {
  uint8_t phase;
  uint8_t mask_output;
  uint32_t imei;
  char sms[48];
} type_sensor_pkg_t;

typedef struct __attribute__((packed)) {
  uint8_t sen_data[MAX_SIDE];
  uint8_t mask_data[MAX_SIDE];
  uint32_t imei[MAX_SIDE];
} type_sensor_recei_pkg_t;

typedef struct __attribute__((packed)) {
  uint8_t phase;
  uint32_t imei;
} type_check_pkg_t;

typedef struct __attribute__((packed)) {
  uint8_t phase;
  uint8_t time_delay_dependent_phase;
  uint8_t time_delay_on_railway;
  uint8_t time_delay_off_railway;
  uint8_t time_walking;
} time_setting_t;

typedef struct __attribute__((packed)) {
  uint8_t time_form;
  uint8_t time_line;
  uint8_t time_index;
  uint8_t time_begin_hour_apply;
  uint8_t time_begin_minute_apply;
  uint8_t t;
  uint8_t num_side; // phase number
  uint8_t yellow;
  uint8_t giaitoa;
  uint8_t green_1;
  uint8_t red_1;
  uint8_t green_2;
  uint8_t red_2;
  uint8_t green_3;
  uint8_t red_3;
  uint8_t green_4;
  uint8_t red_4;
} time_current_t;

typedef struct __attribute__((packed)) {
  uint8_t day[7];
  type_time_lamp_form_t time;
  uint8_t cycle_1[10];
  uint8_t cycle_2[10];
  uint8_t cycle_3[10];
  uint8_t cycle_4[10];
  uint8_t cycle_5[10];
} cycle_t;

/**
 * @brief bien luu co cua he thong
 * @param ticker_one_seconds co bao tran tin hieu 1 seconds
 * @return typedef struct
 */
typedef struct __attribute__((packed)) {
  volatile bool is_mtfc_reset_system; // Reset lai cpu
  volatile bool
      is_ticker_one_seconds_seek; // dùng cho tiến trình đếm hệ thống đèn
  volatile bool
      is_ticker_one_seconds_gateway; // dùng cho xử lý tiến trình 1s gateway
  volatile bool is_ticker_one_seconds_main; // dùng xử lý tiến trình 1s hàm main
  volatile bool
      is_ticker_one_seconds_glcd; // Dung xu ly cac tien trinh 1s cua glcd

  volatile bool
      is_receive_card_config_feedback; // dữ liệu cài đặt phản hồi từ card

  volatile bool is_soft_connected;     // event soft kết nối vào
  volatile bool is_soft_disconnected;  // event soft disconnect
  volatile bool is_cloud_connected;    // event kết nối với cloud
  volatile bool is_cloud_disconnected; // event cloud ngắt kết nối

  volatile bool is_update_rtc; // Cập nhật thời gian

  volatile bool is_gateway_consle_message; // cmd gateway gui xuong
  volatile bool
      is_update_schedule_to_gateway; // truyền dữ liêu schedule cho gateway
  volatile bool is_update_mtfc_config_to_gateway; // Truyền dữ liệu cài đặt
                                                  // thiết bị đến gateway
  volatile bool
      is_update_mtfc_coordinate_to_gateway; // Truyền tọa độ của thiết bị
  volatile bool
      is_update_mtfc_details_to_gateway; // truyền thông tin cơ bản của thiết bị
  volatile bool is_update_mtfc_type_cpu_to_gateway; // Truyền loại cpu
  volatile bool is_update_config_to_cloud; // Cap nhat thong so lai len cloud
  volatile bool
      is_update_schedule_to_cloud;     // update thoi gian hoat dong len gateway
  volatile bool is_app_control;        // Lenh dieu khien tu app
  volatile bool is_quick_cycle_config; // Cau hinh nhanh cycle tu app
  volatile bool is_all_cycle_form_update;     // Update toan bo cycle form
  volatile bool is_save_new_schedule;         // luu cau dat moi schedule
  volatile bool is_save_new_schedule_form_sd; // luu cai dat schedule tu
  volatile bool is_gateway_update_gps;        // yeu cau lay toa do tu gateway

  volatile bool is_card_config_request; // Yeu cau lay thong tin card tu phan
                                        // cung cua mtfc
  volatile bool is_card_config_search; // yeu cau lay du lieu tu card online
  volatile bool is_card_update_card_parameters; // Luu thong so cai dat cho card
  volatile bool is_card_one_slot_update;        // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p2;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p3;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p4;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p5;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p6;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p7;     // Luu thong so cua 1 card
  volatile bool is_card_one_slot_update_p8;     // Luu thong so cua 1 card
  volatile bool is_card_fb_config; // Co trang thai phan hoi config card

  volatile bool
      is_time_location_load; // Yeu cau cap nhat thoi gian len man hinha
  volatile bool is_time_location_update;  // yeu cau luu thoi gian xuong mtfc
  volatile bool is_time_location_syn_gps; // Yeu cau lay toa do gps
  volatile bool is_update_location;       // update vi tri len gateway
  volatile bool is_update_gps_request;    // yeu cau lay location tu gateway

  volatile bool is_active_time_load;   // yeu cau cap nhat thoi gian hoat dong
  volatile bool is_active_time_update; // yeu cau luu thoi gian hoat dong

  volatile bool is_details_loaded;          // yeu cau gui detail len man hinh
  volatile bool is_device_loaded;           // Yeu cau lay device config
  volatile bool is_device_update;           // Gui cai dat
  volatile bool is_device_default_schedule; // Cai dat  ve mac dinh schedule

  volatile bool is_connection_load;   // yeu cau lay thong so module connections
  volatile bool is_connection_update; // thong so cai dat connections gủi xuong

  volatile bool is_day_form_load;   // yeu cau lay thong so day form
  volatile bool is_day_form_update; // yeu cau luu day form

  volatile bool is_time_form_load;   // yeu cau lay time form
  volatile bool is_time_form_update; // yeu cau luu time form

  volatile bool is_cycle_form_load_event; // yeu cau lay cycle form
  volatile bool is_cycle_form_load;       // yeu cau lay cycle form
  volatile bool is_cycle_form_update;     // yeu cau luu cycle form
  volatile bool is_cycle_form_default;    // Yeu cau set default cycle form
  volatile bool is_cycle_form_pre_load;
  // volatile bool is_watchdog_executable;//Dung de test hoat dong cua watch dog
  volatile bool
      is_import_sd_feedback; // phan hoi trang thai import tu module ex card
  volatile bool
      is_export_sd_feedback; // phan hoi trang thai export tu module ex card
  volatile bool is_ex_card_update_schedule;
  volatile bool is_import_schedule_sd;    // nhap du lieu cai dat tu the nho
  volatile bool is_export_schedule_sd;    // xuat du lieu cai dat ra the nho
  volatile bool is_sd_mounted_event;      // su kien the nho plugin
  volatile bool is_usb_console;           // su kien nhan du lieu console tu usb
  volatile bool is_require_update_rtc_ex; // yeu cau update lai thoi gian thiet
                                          // bi do bi loi
  volatile bool is_gps_local_request;   // yeu cau lay gps local
  volatile bool is_gps_local_message;   // cac console gps
  volatile bool is_gps_local_data;      // du lieu gps loacal
  volatile bool is_sensor_update;       // du lieu cam bien tu card cuat gui ve
  volatile bool is_send_config_to_lamp; // gui cai dat den den

  volatile bool is_sever_set_schedule; // Lenh cai schedule tu server
  volatile bool
      is_server_trasfer_one_cycle_form; // Nhan 1 cycle form tu phia server
  volatile bool
      is_save_schedule_form_server; // Luu chien lược hoạt động từ phía server
  volatile bool time_setting_config;
  volatile bool start;
  volatile bool manual;
  volatile bool checkmode;
} type_flag_system_t;

/**
 * @brief bien reset mem mcu khi cai dat pha, thangnm thay doi cho nay
 */
typedef struct __attribute__((packed)) {
  uint8_t is_check_soft_reset;
} type_check_flag_reset_mcu_t;

typedef struct __attribute__((packed)) {
  uint8_t is_check_card_insert;
} type_check_flag_card_inpendent;

typedef struct __attribute__((packed)) {
  uint8_t num_count_phase_dependent[MAX_SIDE];
  uint8_t card_insert_now;
} type_count_card_dependent;
#endif
