#include <mbed.h>
#include <stdio.h>
#include <bsp_bits.h>
#include <bsp_config.h>
#include <debug.h>
#include <device_memory_api.h>
#include <msp_serial_api.h>
#include <state_controller_api.h>
#include <main_api.h>
#include <millis.h>
#include <stdarg.h>
#include <card_control_api.h>
#include <rtc_api_cus.h>
#include <serial_msp_code.h>
#include <declare_function.h>
#include <iwdg.h>

#define DEBUG_EXECUTION_MAIN_LOOP 0

#pragma region DEFINE MACRO

#pragma endregion

#pragma region BAO OOP
// UART_1
//----------Using ex communication card
// UART_2
//----------Using for gateway
// UART_3
//----------Using lamp com port
// UART_4
//----------Using gplc com
// UART_5
Serial dbCom(UART5_TX_PIN, UART5_RX_PIN, 115200);
// UART_6
//----------Using cho card cong suat
rtc mtfc_rtc(PB_7, PB_6, PB_5);
device_memory mtfc_mem(PA_7, PA_6, PA_5, PA_4, PB_0); // mosi,  miso,  sclk,  cs
msp_serial rf_com(UART2_TX_PIN, UART2_RX_PIN);
msp_serial mtfc_lamp_com(UART3_TX_PIN, UART3_RX_PIN);
msp_serial main_rs232_com(UART1_TX_PIN, UART1_RX_PIN);
// Tạm thời không dùng
// msp_serial    mtfc_glcd_com(UART4_TX_PIN, UART4_RX_PIN);
DigitalOut ledAct(PD_3);

state_controller check_in_main_com(ON_OFF_PIN,
                                   RAILWAY_PIN,
                                   WALKING_PIN,
                                   T1_CS_PIN,
                                   T2_CS_PIN,
                                   T3_CS_PIN,
                                   T4_CS_PIN,
                                   T5_CS_PIN);
card_control_api check_card_com(UART6_TX_PIN, UART6_RX_PIN,
                                CS_CARD_PIN1,
                                CS_CARD_PIN2,
                                CS_CARD_PIN3,
                                CS_CARD_PIN4,
                                CS_CARD_PIN5,
                                CS_CARD_PIN6,
                                CS_CARD_PIN7,
                                CS_CARD_PIN8);

DigitalOut mtfc_led_cpu(LED_ACT_PIN);
DigitalOut mtfc_led_connect(LED_CONNECT_PIN);
Ticker mtfc_one_seconds_ticker;
Timer mtfc_timer_debug;
Timer mtfc_timer_main;
Timeout mtfc_railway_delay_on;
Timeout mtfc_railway_delay_off;

uint8_t mode_cross = 2;
uint8_t checkphase = 0;
uint8_t checkmode = 0;
#pragma endregion

#pragma region DECLARE STRUCTURE
// Thong so cai dat doc tu card
type_cardConfig_t type_cardConfig;
// Thong so cai dat doc tu card
type_cardConfig_t mtfc_card_config_search;
// Bo diem nhan thong so cua card
type_one_cardConfig_t mtfc_card_config_buff;
// Chien luoc xuat tin hieu den
type_mtfc_schedule_t mtfc_schedule;
// Chien luoc xuat tin hieu den xu dung cho edit
type_mtfc_schedule_t mtfc_schedule_buffer;
// Chien luoc xuat tin hieu den tu phia server
type_mtfc_schedule_t mtfc_schedule_server;
// Bo luu du lieu lam viec hien tai
type_mtfc_cycle_working_package_t mtfc_working_current_base;
// Bo luu du lieu moi dang duoc cap nhat
type_mtfc_cycle_working_package_t mtfc_working_buffer_update_base;
// Bo luu du lieu moi dang duoc cap nhat len man hinh
type_mtfc_cycle_working_package_t mtfc_working_buffer_update_qt;
// Du lieu xuat ra card
type_mtfc_card_data_t mtfc_card_data_out;
// Co he thong chinh
type_flag_system_t mtfc_flag_system;
// Bien cau hinh cho module dieu khien trang thai lam viec
type_state_module_config_t mtfc_device_state_config;
// bien luu cai dat cua thiet bi
type_mtfc_config_t mtfc_config;
type_mtfc_config_t mtfc_config_update;
// goi tin dinh ky 1s gui len soft
type_mtfc_working_message_t mtfc_working_message;
// luu thoi gian cai dat gio he thong
type_date_time_t mtfc_rtc_new_update;
// set time
type_date_time_t mtfc_rtc_set;
// luu thoi gian hien tai cua he thong
type_date_time_t mtfc_rtc_val;
type_hm_time_t mtfc_hm_present;
// Luu thong tin co ban cua CPU
type_mtfc_details_message_t mtfc_detail_message;
// Bien luu goi tin cai dat nhanh chien luoc tu app
type_mtfc_quick_cycle_config_t mtfc_quick_cycle_config;

type_mtfc_railway_work_base_t mtfc_railway_work_base;

type_mtfc_coordinates_message_t mtfc_coordinate_gateway_message;

// Bien xuat du lieu diem nguoc ra cong RS485
type_mtfc_lamp485_t mtfc_lamp485;
// Cai dat pha cho tung den
type_dn_config_t dn_config;

// Goi tin gia tri cam bien
type_sensor_pkg_t mtfc_sen_pkg;
type_sensor_recei_pkg_t mtfc_sen_recei_pkg;
type_check_pkg_t mtfc_check_pkg;

// Cac bien man hinh hien thi
type_mtfc_page_one_t mtfc_page_one_data;
type_mtfc_page_two_t mtfc_page_two_data;
type_mtfc_page_iv_t mtfc_page_iv_data;
type_mtfc_page_v_t mtfc_page_v_data;

type_mtfc_page_v_t mtfc_card_config_data_glcd;
type_mtfc_one_card_config_t mtfc_one_card_config;
type_mtfc_one_card_config_t mtfc_one_card_config_card1;
type_mtfc_time_location_t mtfc_time_location;
type_mtfc_config_part1_t mtfc_config_part1;
/////////////////////
// Chien luoc xuat tin hieu den tu phia server
type_mtfc_schedule_t schedule_update_from_screen;
time_setting_t time_setting;

time_current_t time_current;
cycle_t cycle;

type_check_flag_reset_mcu_t check_reset_mcu;
type_check_flag_card_inpendent check_card_inpendent;
type_count_card_dependent check_card_num_card_dependent;
#pragma endregion

#pragma region DECLARE VARIABLE
char mtfc_gate_consol_message[GATEWAY_CONSOLE_MESSAGE_LENGTH] = "";
char mtfc_gate_control_message[GATEWAY_CONSOLE_MESSAGE_LENGTH] = "";
#pragma endregion

#pragma region XU LY LED HIEN THI TRANG THAI-----------------------------------------------
void mtfc_led_status_processing(void)
{
  static uint32_t time_led_status = 0;
  if (millis() - time_led_status > 1000)
  {
    ledAct = !ledAct;
    time_led_status = millis();
    mtfc_led_cpu = !mtfc_led_cpu;
    if ((mtfc_working_current_base.is_device_online == mtfc_online) && (mtfc_working_current_base.is_device_conneted_soft == mtfc_connect))
      mtfc_led_connect = mtfc_led_cpu;
    else if (mtfc_working_current_base.is_device_online == mtfc_online)
      mtfc_led_connect = !mtfc_led_cpu;
    else
      mtfc_led_connect = 0;
    time_led_status = millis();
  }
}
#pragma endregion

#pragma region CAC HAM CHINH [STARTUP FC,memory, MAIN]------------------------------------

void mtfc_watdog_init(float time_wd)
{
  debug(USING_WATCHDOG_TIMER, "\r\nmtfc watchdog reload: %0.1f (s)", time_wd);
  uint16_t time_wd_reload = (uint16_t)((time_wd * 40000.0) / 256);
  debug(USING_WATCHDOG_TIMER, "\r\nmtfc watchdog init reload: %d", time_wd_reload);
}

void mtfc_memory_startup_config(void)
{
  memset((bool *)&mtfc_flag_system, false, sizeof(type_flag_system_t));
  memset((uint8_t *)&mtfc_card_config_search, 0, sizeof(type_cardConfig_t));
  memset((uint8_t *)&mtfc_card_config_buff, 0, sizeof(type_one_cardConfig_t));
  memset((uint8_t *)&mtfc_schedule, 0, sizeof(type_mtfc_schedule_t));
  memset((uint8_t *)&mtfc_schedule_buffer, 0, sizeof(type_mtfc_schedule_t));
  memset((uint8_t *)&mtfc_working_current_base, 0, sizeof(type_mtfc_cycle_working_package_t));
  memset((uint8_t *)&mtfc_working_buffer_update_base, 0, sizeof(type_mtfc_cycle_working_package_t));
  memset((uint8_t *)&mtfc_working_buffer_update_qt, 0, sizeof(type_mtfc_cycle_working_package_t));
  memset((uint8_t *)&mtfc_card_data_out, 0, sizeof(type_mtfc_card_data_t));
  memset((uint8_t *)&mtfc_device_state_config, 0, sizeof(type_state_module_config_t));
  memset((uint8_t *)&mtfc_config, 0, sizeof(type_mtfc_config_t));
  memset((uint8_t *)&mtfc_working_message, 0, sizeof(type_mtfc_working_message_t));
  memset((uint8_t *)&mtfc_rtc_val, 0, sizeof(type_date_time_t));
  memset((uint8_t *)&mtfc_page_one_data, 0, sizeof(type_mtfc_page_one_t));
  memset((uint8_t *)&mtfc_card_config_data_glcd, 0, sizeof(type_mtfc_page_v_t));
  memset((uint8_t *)&mtfc_time_location, 0, sizeof(type_mtfc_time_location_t));
  memset((uint8_t *)&mtfc_quick_cycle_config, 0, sizeof(type_mtfc_quick_cycle_config_t));
  memset((uint8_t *)&mtfc_railway_work_base, 0, sizeof(type_mtfc_railway_work_base_t));
  memset((uint8_t *)&mtfc_coordinate_gateway_message, 0, sizeof(type_mtfc_coordinates_message_t));
  memset((uint8_t *)&mtfc_lamp485, 0, sizeof(type_mtfc_lamp485_t));
  memset((uint8_t *)&mtfc_schedule_server, 0, sizeof(type_mtfc_schedule_t));
  debug(MAIN_DEBUG, "%s", "\r\nMemory init..............");
  mtfc_mem.mem_init();

  if (mtfc_mem.mem_is_device_fist_run()) // kiem da co phai thiet bi lan dau khoi dong
  {
    wait(1);
    mtfc_glcd_console("Set mftc to default");
    mtfc_contruct_mtfc_config_default(&mtfc_config);
    mtfc_contruct_schedule_default(&mtfc_schedule, DEFAULT_NUM_PHASES);
    mtfc_timer_debug.start();
    mtfc_mem.mem_save_data(BLOCK_1, (char *)&mtfc_config, sizeof(type_mtfc_config_t));
    debug(MAIN_DEBUG, "\r\nmtfc_config write:%d (us)", mtfc_timer_debug.read_us());
    mtfc_timer_debug.stop();
    mtfc_timer_debug.reset();
    mtfc_timer_debug.start();
    mtfc_mem.mem_save_data(BLOCK_2, (char *)&mtfc_schedule, sizeof(type_mtfc_schedule_t));
    debug(MAIN_DEBUG, "\r\nmtfc_schedule write:%d (us)", mtfc_timer_debug.read_us());
    mtfc_timer_debug.stop();
    mtfc_timer_debug.reset();
  }
  wait(1);
  debug(DEBUG_GLCD, "%s\r\n", "Load memory config.........");
  mtfc_timer_debug.start();
  mtfc_mem.mem_read_data(BLOCK_1, (char *)&mtfc_config, sizeof(type_mtfc_config_t));
  mtfc_mem.mem_read_data(BLOCK_1, (char *)&mtfc_config_update, sizeof(type_mtfc_config_t));
  debug(MAIN_DEBUG, "\r\nmtfc_config read:%d (us)", mtfc_timer_debug.read_us());
  mtfc_timer_debug.stop();
  mtfc_timer_debug.reset();
  mtfc_timer_debug.start();
  mtfc_mem.mem_read_data(BLOCK_2, (char *)&mtfc_schedule, sizeof(type_mtfc_schedule_t));
  mtfc_mem.mem_read_data(BLOCK_2, (char *)&schedule_update_from_screen, sizeof(type_mtfc_schedule_t));
  mtfc_mem.mem_read_data(BLOCK_3, (char *)&check_reset_mcu, sizeof(type_check_flag_reset_mcu_t));

  debug(MAIN_DEBUG, "\r\nmtfc_schedule read:%d (us) [%02d] phase", mtfc_timer_debug.read_us(), mtfc_schedule.num_side);
  debug(DEBUG_CYCLE, "\r\nmtfc_schedule.time.index[0].select_point[0].index_cycle_form):%d", mtfc_schedule.time.index[0].select_point[0].index_cycle_form);
  debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule.time.index[0].select_point[1].index_cycle_form);
  debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_red:%d", mtfc_schedule.time.index[0].select_point[2].index_cycle_form);
  debug(MAIN_DEBUG, "\r\ngot value 123: %d", mtfc_schedule.cycle.index[0].side[0].t_red);
  debug(MAIN_DEBUG, "\r\ngot value 123: %d", mtfc_schedule.cycle.index[0].side[1].t_red);
  debug(MAIN_DEBUG, "\r\ngot value 123: %d", mtfc_schedule.cycle.index[0].side[2].t_red);
  debug(MAIN_DEBUG, "\r\ngot value 123: %d", mtfc_schedule.cycle.index[0].side[3].t_red);

  cycle.time = mtfc_schedule.time;
  for (int i = 0; i < 10; i++)
  {
    if (i < 7)
      cycle.day[i] = mtfc_schedule.days.index[i];
    if (i < 8)
    {
      cycle.cycle_1[i] = mtfc_schedule.cycle.index[0].side[i].t_green;
      cycle.cycle_2[i] = mtfc_schedule.cycle.index[1].side[i].t_green;
      cycle.cycle_3[i] = mtfc_schedule.cycle.index[2].side[i].t_green;
      cycle.cycle_4[i] = mtfc_schedule.cycle.index[3].side[i].t_green;
      cycle.cycle_5[i] = mtfc_schedule.cycle.index[4].side[i].t_green;
    }
    if (i == 8)
    {
      cycle.cycle_1[8] = mtfc_schedule.cycle.index[0].side[0].t_yellow;
      cycle.cycle_2[8] = mtfc_schedule.cycle.index[1].side[0].t_yellow;
      cycle.cycle_3[8] = mtfc_schedule.cycle.index[2].side[0].t_yellow;
      cycle.cycle_4[8] = mtfc_schedule.cycle.index[3].side[0].t_yellow;
      cycle.cycle_5[8] = mtfc_schedule.cycle.index[4].side[0].t_yellow;
    }
    if (i == 9)
    {
      cycle.cycle_1[9] = mtfc_schedule.cycle.index[0].clearance_time_crossroads;
      cycle.cycle_2[9] = mtfc_schedule.cycle.index[1].clearance_time_crossroads;
      cycle.cycle_3[9] = mtfc_schedule.cycle.index[2].clearance_time_crossroads;
      cycle.cycle_4[9] = mtfc_schedule.cycle.index[3].clearance_time_crossroads;
      cycle.cycle_5[9] = mtfc_schedule.cycle.index[4].clearance_time_crossroads;
    }
  }
  mtfc_timer_debug.stop();
  mtfc_timer_debug.reset();
  debug(SIZEOF_DEBUG, "\r\nsiseof(mtfc_config):%d", sizeof(mtfc_config));
  debug(SIZEOF_DEBUG, "\r\nsiseof(mtfc_schedule):%d", sizeof(mtfc_schedule));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_one_cycle_lamp_time_t):%d", sizeof(type_one_cycle_lamp_time_t));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_time_lamp_form_t):%d", sizeof(type_time_lamp_form_t));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_mtfc_working_message_t):%d", sizeof(type_mtfc_working_message_t));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_mtfc_details_message_t):%d", sizeof(type_mtfc_details_message_t));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_mtfc_config_part1_t):%d", sizeof(type_mtfc_config_part1_t));
  debug(SIZEOF_DEBUG, "\r\nsiseof(type_mtfc_config_part2_t):%d", sizeof(type_mtfc_config_part2_t));
}

void mtfc_initialize_parameters(type_mtfc_cycle_working_package_t *obj, type_mtfc_working_message_t *mess, type_mtfc_schedule_t *schedule, type_mtfc_config_t *mtf_config)
{
  obj->t_seek = 1; // Khoi tao gia tri max de lan chay dau tien65535
  obj->num_side = schedule->num_side;
  obj->mtfc_main_state = mtfc_startup_state;
  obj->is_device_online = mtfc_offline;
  obj->is_device_conneted_soft = mtfc_disconnect;
  obj->index_card_check = 1; // Fist card slot check
  mess->num_phase = schedule->num_side;

  obj->idx_timeline = 255;
  obj->idx_time_form = 255;
  obj->idx_cycle_form = 255;
  obj->is_startup_state = false;
  type_device_factory_details_t mtfc_mem_details = mtfc_mem.mem_get_device_details_t();
  mtfc_detail_message.type_cpu = mtf_config->type_cpu;
  mtfc_detail_message.num_side = schedule->num_side;
  for (uint8_t i = 0; i < 32; i++)
  {
    mtfc_detail_message.imei[i] = mtfc_mem_details.imei[i];
    mtfc_detail_message.firmware_build_version[i] = mtfc_mem_details.firmwar_version[i];
  }
  mtfc_check_phase_connect_railway(obj, mtf_config, schedule);
  wait(1);
  obj->current_phase_is_green = 1;
}

void mtfc_startUp_config(void)
{
  for (uint8_t i = 0; i < 20; i++)
  {
    mtfc_led_cpu = !mtfc_led_cpu;
    mtfc_led_connect = !mtfc_led_connect;
    wait(0.2);
  }
  mtfc_glcd_console("mtfc begin startup system");
  // mtfc_glcd_com.send_byte(MSP_MTFC_NOTIFY_STARTUP, 1);
  wait(1);
  mtfc_glcd_console("RTC module init.........");
  if (mtfc_rtc.rtc_ic_init() != 0)
  {
    wait(1);
    mtfc_glcd_console("RTC startup failed");
    wait(1);
  }
  wait(1);
  // Doc cau hinh lam viec cua thiet bi
  mtfc_memory_startup_config();
  wait(1);
  check_in_main_com.init_para_setting(&mtfc_device_state_config);
  wait(1);
  mtfc_glcd_console("mtfc finish config....");
  check_card_com.set_callbackRx(mtfc_card_rx_event_handler);
  rf_com.set_callbackRx(mtfc_gateway_rx_event_handler);
  // mtfc_glcd_com.set_callbackRx(mtfc_glcd_rx_event_handler);
  main_rs232_com.set_callbackRx(mtfc_ex_card_com_rx_event_handler);
  mtfc_lamp_com.baud(9600);
  dbCom.attach(&mtfc_debug_console_rx_event_handler, SerialBase::RxIrq);
  rf_com.printf("\r\nGateway com port hello!!!");
}
void waitForAEquals1()
{
  mtfc_flag_system.start = 0; // get tin hieu tu CM4 khoi dong chay he thong
  if (check_reset_mcu.is_check_soft_reset == 1)
  {
    check_reset_mcu.is_check_soft_reset = 0;
    mtfc_mem.mem_save_data(BLOCK_3, (char *)&check_reset_mcu, sizeof(type_check_flag_reset_mcu_t));
    mtfc_flag_system.start = 1;
  }
  while (mtfc_flag_system.start != 1)
  {

    // debug(MAIN_DEBUG, "%s", "Waiting for a to be 1...\n");
    wait(1);
  }
  wait(1);
  debug(MAIN_DEBUG, "%s", "a has become 1. Continuing...\n");
}

int main()
{
  mtfc_startUp_config();
  mtfc_initialize_parameters(&mtfc_working_current_base, &mtfc_working_message, &mtfc_schedule, &mtfc_config);
  millisStart();
  mtfc_one_seconds_ticker.attach(&mtfc_one_seconds_ticker_event_handler, 1.0);
  // mtfc_glcd_console("mtfc startup flashing...");
  // mtfc_glcd_com.send_byte(MSP_MTFC_NOTIFY_STARTUP, 0);
  mtfc_rtc_val = mtfc_rtc.rtc_get_time();
  if (mtfc_rtc_val.year < 2021)
  {
    mtfc_glcd_console("Error rtc time Check hardware?");
    mtfc_flag_system.is_require_update_rtc_ex = true;
  }
  debug(MAIN_DEBUG, "\r\nSET RTC: %02d %02d %02d - %02d/%02d/%04d",
        mtfc_rtc_val.hour,
        mtfc_rtc_val.minute,
        mtfc_rtc_val.seconds,
        mtfc_rtc_val.date,
        mtfc_rtc_val.month,
        mtfc_rtc_val.year);
  // mtfc_debug_print_cycle_form(&mtfc_schedule.cycle);
#ifdef USING_WATCHDOG_TIMER
  // mtfc_watdog_init(3.0);
#endif
  waitForAEquals1();
  mtfc_card_detect(&mtfc_working_current_base);
  while (true)
  {
#if DEBUG_EXECUTION_MAIN_LOOP == 1
    mtfc_timer_main.stop();
    mtfc_timer_main.reset();
    mtfc_timer_main.start();
#endif
    mtfc_main_ticker_one_seconds(&mtfc_working_current_base);
    mtfc_update_mtfc_parameters(&mtfc_schedule, &mtfc_working_current_base, &mtfc_config);
    switch (mtfc_working_current_base.mtfc_main_state)
    {
    case mtfc_startup_state:
    {
      mtfc_working_current_base.mtfc_main_state = mtfc_startup_cpu_processing();
      break;
    }
    case mtfc_normal_state:
    {
      mtfc_normal_cpu_processing();
      break;
    }
    default:
      break;
    }
    // mtfc_gateway_processing(&mtfc_working_current_base);
    mtfc_led_status_processing();
    mtfc_rtc_update(&mtfc_rtc_new_update);
    mtfc_glcd_processing(&mtfc_schedule, &mtfc_working_current_base, &mtfc_config, &mtfc_rtc_val);
    mtfc_ex_card_communication(&mtfc_working_current_base, &mtfc_config, &mtfc_schedule);
    mtfc_sensor_detect_processing(&mtfc_working_current_base, &mtfc_config);
#if DEBUG_EXECUTION_MAIN_LOOP == 1
    debug(MAIN_DEBUG, "\r\ndebug_main_time:%d", mtfc_timer_main.read_us());
#endif
#ifdef USING_WATCHDOG_TIMER
    // iwdg_feed();
#endif
  }
}

void mtfc_update_mtfc_parameters(type_mtfc_schedule_t *schedule, type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config)
{
  if (mtfc_flag_system.is_card_update_card_parameters)
  {
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      config->index_card[i] = mtfc_card_config_search.index[i];
      debug(MAIN_DEBUG, "\r\nmtfc_card_config_search.index[i]:%d", mtfc_card_config_search.index[i].is_railway_enabled);
      // config->index_card[1] = mtfc_config_update.index_card[1];
      // config->index_card[2] = mtfc_config_update.index_card[2];
    }
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    debug(DEBUG_GLCD, "%s\r\n", "Card saved all config!");
    mtfc_check_phase_connect_railway(mtfc_wk_base, config, schedule);
    mtfc_flag_system.is_update_config_to_cloud = true;
    mtfc_flag_system.is_card_update_card_parameters = false;
  }
  if (mtfc_flag_system.is_update_location)
  {
    config->latitude = mtfc_time_location.lati;
    config->longitude = mtfc_time_location.longi;
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    debug(DEBUG_GLCD, "%s\r\n", "save new location!");
    mtfc_flag_system.is_update_config_to_cloud = true;
    mtfc_flag_system.is_update_location = false;
  }
  if (mtfc_flag_system.is_gateway_update_gps)
  {
    mtfc_flag_system.is_gateway_update_gps = false;
    config->latitude = mtfc_coordinate_gateway_message.latitude;
    config->longitude = mtfc_coordinate_gateway_message.longitude;
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    debug(DEBUG_GLCD, "%s\r\n", "Save gps successful!!!");
    mtfc_flag_system.is_update_config_to_cloud = true;
    mtfc_flag_system.is_time_location_load = true; // gui lai thong so len man hinh
  }
  if (mtfc_flag_system.is_active_time_update)
  {
    schedule->active_time = mtfc_schedule_buffer.active_time;
    mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
    debug(DEBUG_GLCD, "%s\r\n", "save new active time!");
    mtfc_flag_system.is_update_schedule_to_cloud = true;
    mtfc_flag_system.is_active_time_update = false;
  }
  if (mtfc_flag_system.is_quick_cycle_config)
  {
    if (mtfc_check_cycle_config_data(&mtfc_quick_cycle_config, &mtfc_schedule) != 0)
    {
      debug(DEBUG_GLCD, "%s\r\n", "Error! Invalid data cycle");
      debug(MAIN_DEBUG, "\r\n%s", "Error! Invalid data cycle");
    }
    else
    {
      type_one_cycle_lamp_time_t temp_cycle;
      memset((uint8_t *)&temp_cycle, 0, sizeof(temp_cycle));
      mtfc_one_cycle_set_config(&temp_cycle, &mtfc_quick_cycle_config);
      if ((mtfc_wk_base->idx_cycle_form < MAX_CYCLE_FORM) && (mtfc_wk_base->idx_cycle_form >= 0))
      {
        schedule->cycle.index[mtfc_wk_base->idx_cycle_form] = temp_cycle;
        mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
        debug(DEBUG_GLCD, "%s\r\n", "Sucessful! Update cycle [%01d]", (mtfc_wk_base->idx_cycle_form + 1));
        debug(MAIN_DEBUG, "\r\n%s", "Sucessful! Update cycle [%01d]", (mtfc_wk_base->idx_cycle_form + 1));
        mtfc_flag_system.is_update_schedule_to_cloud = true;
      }
    }

    mtfc_flag_system.is_quick_cycle_config = false;
  }
  if (mtfc_flag_system.is_device_update)
  {
    config->is_flashing = mtfc_config_part1.is_flashing;
    config->power_meters_enable = mtfc_config_part1.power_meters_enable;
    config->is_enabale_out485_lamp_port = mtfc_config_part1.is_enabale_out485_lamp_port;
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    debug(DEBUG_GLCD, "%s\r\n", "device updated!");
    mtfc_flag_system.is_update_config_to_cloud = true;
    mtfc_flag_system.is_device_update = false;
  }
  if (mtfc_flag_system.is_connection_update)
  {
    config->railway_mode_enable = mtfc_config_part1.railway_mode_enable;
    config->railway_delay_on_time = mtfc_config_part1.railway_delay_on_time;
    config->railway_delay_off_time = mtfc_config_part1.railway_delay_off_time;
    config->walking_mode_enable = mtfc_config_part1.walking_mode_enable;
    config->walking_walking_time = mtfc_config_part1.walking_walking_time;
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    wait(0.02);
    debug(DEBUG_GLCD, "%s\r\n", "connection update!");
    mtfc_flag_system.is_update_config_to_cloud = true;
    mtfc_flag_system.is_connection_update = false;
  }
  if (mtfc_flag_system.is_save_new_schedule || mtfc_flag_system.is_save_new_schedule_form_sd || mtfc_flag_system.is_save_schedule_form_server)
  {
    *schedule = mtfc_schedule_buffer;
    mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
    if (mtfc_flag_system.is_save_new_schedule)
    {
      debug(DEBUG_GLCD, "%s\r\n", "Recovery default all schedule");
    }
    else if (mtfc_flag_system.is_save_new_schedule_form_sd)
    {
      debug(DEBUG_GLCD, "%s\r\n", "Successful import schedule");
    }
    else
    {
      debug(DEBUG_GLCD, "%s\r\n", "Successful update schedule form app");
    }
    mtfc_flag_system.is_update_schedule_to_cloud = true;
    mtfc_flag_system.is_save_new_schedule = false;
    mtfc_flag_system.is_save_new_schedule_form_sd = false;
    mtfc_flag_system.is_save_schedule_form_server = false;
  }
  if (mtfc_flag_system.is_day_form_update)
  {
    schedule->days = mtfc_schedule_buffer.days;
    mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
    debug(DEBUG_GLCD, "%s\r\n", "Update days form");
    main_rs232_com.send_byte(CMD_MASTER_TO_CPU_FB, CMD_MASTER_TO_CPU_FB_DAY_FORM);
    debug(MAIN_DEBUG, "\r\n%s", "Update days form");
    mtfc_flag_system.is_update_schedule_to_cloud = true;
    mtfc_flag_system.is_day_form_update = false;
    /* test case*/
    for (int i = 0; i <= MAX_DAYS_WORKING; i++)
    {
      debug(MAIN_DEBUG, "got value day form update: %d\r\n", schedule->days);
    }
  }

  if (mtfc_flag_system.is_time_form_update) // Cai dat bieu mau thoi gian
  {
    schedule->time = mtfc_schedule_buffer.time;
    mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
    debug(DEBUG_GLCD, "%s\r\n", "Update time form");
    main_rs232_com.send_byte(CMD_MASTER_TO_CPU_FB, CMD_MASTER_TO_CPU_FB_TIME_FORM);
    debug(MAIN_DEBUG, "\r\n%s", "Update time form");
    mtfc_flag_system.is_update_schedule_to_cloud = true;
    mtfc_flag_system.is_time_form_update = false;
  }

  if (mtfc_flag_system.is_all_cycle_form_update)
  {
    schedule->cycle = mtfc_schedule_buffer.cycle;
    mtfc_mem.mem_save_data(BLOCK_2, (char *)schedule, sizeof(type_mtfc_schedule_t));
    debug(DEBUG_GLCD, "%s\r\n", "Update all cycle form");
    main_rs232_com.send_byte(CMD_MASTER_TO_CPU_FB, CMD_MASTER_TO_CPU_FB_CYCLE_FORM);
    debug(MAIN_DEBUG, "\r\n%s", "Update all cycle form");
    if (mtfc_flag_system.is_cycle_form_pre_load) // Gui lai len man hinh
    {
      mtfc_flag_system.is_cycle_form_load_event = true;
      mtfc_flag_system.is_cycle_form_load = true;
      mtfc_flag_system.is_cycle_form_pre_load = false;
    }
    mtfc_flag_system.is_update_schedule_to_cloud = true;
    mtfc_flag_system.is_all_cycle_form_update = false;
  }
  if (mtfc_flag_system.is_update_config_to_cloud) // Thay doi config mtfc thi gui lai len cloud
  {
    if (mtfc_wk_base->is_device_online == mtfc_online)
    {
      mtfc_flag_system.is_update_mtfc_config_to_gateway = true;
    }
    mtfc_flag_system.is_update_config_to_cloud = false;
  }
  if (mtfc_flag_system.is_update_schedule_to_cloud) // Thay doi schedule thi gui lai len cloud
  {
    if (mtfc_wk_base->is_device_online == mtfc_online)
    {
      mtfc_flag_system.is_update_schedule_to_gateway = true;
    }
    mtfc_flag_system.is_update_schedule_to_cloud = false;
  }
  if (mtfc_flag_system.time_setting_config) // Thay doi time setting
  {
    check_card_com.send_struct(CMD_MASTER_TO_SLAVE_TIME_SETTING, (uint8_t *)&time_setting, sizeof(time_setting_t));
    config->railway_delay_on_time = time_setting.time_delay_on_railway;
    config->railway_delay_off_time = time_setting.time_delay_off_railway;
    config->walking_walking_time = time_setting.time_walking;
    config->dependent_phase_time = time_setting.time_delay_dependent_phase;
    debug(MAIN_DEBUG, "\r\nTime setting phase: %d\r\n", time_setting.phase);

    for(int i = 0 ; i < check_card_num_card_dependent.card_insert_now ; i ++) // so 4 doi thanh so card hien hanh
    {
      if(mtfc_card_config_search.index[i].phase == time_setting.phase)
      {
        mtfc_card_config_search.index[i].time_delay_dependent_phase = time_setting.time_delay_dependent_phase;
      }
    }
    // debug(MAIN_DEBUG, "\r\nTime setting time on railway: %d\r\n", time_setting.time_delay_on_railway);
    
    mtfc_mem.mem_save_data(BLOCK_1, (char *)config, sizeof(type_mtfc_config_t));
    wait(0.02);
    mtfc_flag_system.time_setting_config = false;
  }
}
#pragma endregion

#pragma region TICKER 1s DUNG XU LY CAC TAC VU DINH KY
void mtfc_main_ticker_one_seconds(type_mtfc_cycle_working_package_t *mtfc_wk_base)
{
  unsigned long mtfc_send_error_time = millis();
  if (mtfc_flag_system.is_ticker_one_seconds_main)
  {
    mtfc_rtc_val = mtfc_rtc.rtc_get_time();
    mtfc_hm_present.hour = mtfc_rtc_val.hour;
    mtfc_hm_present.minute = mtfc_rtc_val.minute;
    // debug(MAIN_DEBUG, "\r\n%s:%d:%d:%d", "Time", mtfc_hm_present.hour, mtfc_hm_present.minute, mtfc_rtc_val.seconds);
    if ((mtfc_rtc_val.hour == 1) && (mtfc_rtc_val.minute == 1) && (mtfc_rtc_val.seconds == 1))
    {
      mtfc_rtc.rtc_load_ex_rtc_to_internal();
    }
    mtfc_flag_system.is_ticker_one_seconds_main = false;
  }
  if (mtfc_rtc_val.year < 2021)
  {
    if (millis() - mtfc_send_error_time >= 5000)
    {
      mtfc_glcd_console("Error rtc time Check hardware?");
      mtfc_flag_system.is_require_update_rtc_ex = true;
      mtfc_send_error_time = millis();
    }
  }
}
#pragma endregion

#pragma region CAC HAM GUI CAC GOI TIN -->Gateway, -->Glcd, -->communication-------------

void mtfc_send_schedule(msp_serial *p, type_mtfc_schedule_t *mtfc_schedule)
{
  p->send_struct(MSP_SCHEDULE_ACTIVE_TIME, (uint8_t *)&(mtfc_schedule->active_time), sizeof(type_active_lamp_t));
  p->send_struct(MSP_SCHEDULE_TIME_FORM, (uint8_t *)&(mtfc_schedule->time), sizeof(type_time_lamp_form_t));
  p->send_struct(MSP_SCHEDULE_DAY_FORM, (uint8_t *)&(mtfc_schedule->days), sizeof(type_days_lamp_form_t));
  p->send_byte(MSP_SCHEDULE_NUM_SIDE, mtfc_schedule->num_side);
  for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
  {
    p->send_struct(MSP_SCHEDULE_CYCLE_FORM, (uint8_t *)&(mtfc_schedule->cycle.index[i]), sizeof(type_one_cycle_lamp_time_t));
  }
}

int8_t mtfc_send_schedule(msp_serial *p, type_mtfc_schedule_t *mtfc_schedule, type_mtfc_cycle_working_package_t *mtfc_wk)
{
  static uint32_t mtfc_millis_send_schedule = 0;
  static bool mtfc_fist_call = true;
  if (mtfc_fist_call)
  {
    mtfc_millis_send_schedule = millis();
    mtfc_fist_call = false;
  }
  if (millis() - mtfc_millis_send_schedule >= 100)
  {
    switch (mtfc_wk->index_send_schedule)
    {
    case 0:
      p->send_struct(MSP_SCHEDULE_ACTIVE_TIME, (uint8_t *)&(mtfc_schedule->active_time), sizeof(type_active_lamp_t));
      debug(MAIN_DEBUG, "\r\n%s", "Send active time");
      mtfc_wk->index_send_schedule = 1;
      break;
    case 1:
      p->send_struct(MSP_SCHEDULE_TIME_FORM, (uint8_t *)&(mtfc_schedule->time), sizeof(type_time_lamp_form_t));
      debug(MAIN_DEBUG, "\r\n%s", "Send time form");
      mtfc_wk->index_send_schedule = 2;
      break;
    case 2:
      p->send_struct(MSP_SCHEDULE_DAY_FORM, (uint8_t *)&(mtfc_schedule->days), sizeof(type_days_lamp_form_t));
      debug(MAIN_DEBUG, "\r\n%s", "Send day form");
      mtfc_wk->index_send_schedule = 3;
      break;
    case 3:
      p->send_byte(MSP_SCHEDULE_NUM_SIDE, mtfc_schedule->num_side);
      debug(MAIN_DEBUG, "\r\n%s", "Send num side");
      mtfc_wk->index_send_schedule = 4;
      break;
    case 4:
    {
      p->send_struct(MSP_SCHEDULE_CYCLE_FORM, (uint8_t *)&(mtfc_schedule->cycle.index[mtfc_wk->index_send_cycle_form]), sizeof(type_one_cycle_lamp_time_t));
      (*mtfc_wk).index_send_cycle_form = (*mtfc_wk).index_send_cycle_form + 1;
      if (mtfc_wk->index_send_cycle_form >= MAX_CYCLE_FORM)
      {
        mtfc_wk->index_send_cycle_form = 0;
        mtfc_wk->index_send_schedule = 0;
        mtfc_fist_call = true;
        return 0;
      }
      break;
    }
    default:
      break;
    }
    mtfc_millis_send_schedule = millis();
  }
  return 1;
}

void mtfc_send_coordinates(msp_serial *p, type_mtfc_coordinates_message_t *coor)
{
  p->send_struct(MSP_COORDINATE, (uint8_t *)coor, sizeof(type_mtfc_coordinates_message_t));
}

void mtfc_send_mtfc_details(msp_serial *p, type_mtfc_details_message_t *details)
{
  p->send_struct(MSP_DETAILS, (uint8_t *)details, sizeof(type_mtfc_details_message_t));
}

void mtfc_send_mtfc_working_one_seconds_message(msp_serial *p, type_mtfc_working_message_t *mes)
{
  p->send_struct(MSP_ONE_SECOND_MESSAGE, (uint8_t *)mes, sizeof(type_mtfc_working_message_t));
}

void mtfc_send_mtfc_type(msp_serial *p, type_mtfc_config_t *mtfc_config)
{
  p->send_byte(MSP_MTFC_TYPE_CPU, mtfc_config->type_cpu);
}

void mtfc_send_mtfc_config(msp_serial *p, type_mtfc_config_t *mtfc_config)
{
  type_mtfc_config_part1_t part1;
  type_mtfc_config_part2_t part2;
  part1.type_cpu = mtfc_config->type_cpu;
  part1.walking_mode_enable = mtfc_config->walking_mode_enable;
  part1.railway_mode_enable = mtfc_config->railway_mode_enable;
  part1.power_meters_enable = mtfc_config->power_meters_enable;
  part1.locked_hard_control_enable = mtfc_config->locked_hard_control_enable;
  part1.is_flashing = mtfc_config->is_flashing;
  part1.is_enabale_out485_lamp_port = mtfc_config->is_enabale_out485_lamp_port;
  part1.railway_delay_on_time = mtfc_config->railway_delay_on_time;
  part1.railway_delay_off_time = mtfc_config->railway_delay_off_time;
  part1.walking_mode_enable = mtfc_config->walking_mode_enable;
  part1.latitude = mtfc_config->latitude;
  part1.longitude = mtfc_config->longitude;
  for (uint8_t i = 0; i < MAX_SIDE; i++)
  {
    part2.index_card[i] = mtfc_config->index_card[i];
  }
  for (uint8_t i = 0; i < MAX_SIDE; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      part1.phase_pin_config[i].pin[j] = mtfc_config->phase_pin_config[i].pin[j];
    }
  }
  p->send_struct(MSP_MTFC_CONFIG_PART1, (uint8_t *)&part1, sizeof(type_mtfc_config_part1_t));
  wait(0.02);
  for (uint8_t i = 0; i < MAX_SIDE; i++)
  {
    p->send_struct(MSP_MTFC_CONFIG_PART2, (uint8_t *)&part2.index_card[i], sizeof(type_one_cardConfig_t));
  }
}
#pragma endregion

#pragma region GIAO TIEP GATEWAY---------------------------------------------------------

void mtfc_gateway_keepalive(type_mtfc_cycle_working_package_t *mtfc_wk_base)
{
  static unsigned long mtfc_mill_keepalive = millis();
  if (mtfc_wk_base->is_device_conneted_soft == mtfc_connect)
  {
    // rf_com.send_byte(MSP_KEEPALIVE_PQ,1);
    return;
  }
  else if (mtfc_wk_base->is_device_online == mtfc_online)
  {
    if (millis() - mtfc_mill_keepalive >= KEEPALIVE_TIME)
    {
      rf_com.send_byte(MSP_KEEPALIVE_PQ, 1);
      mtfc_mill_keepalive = millis();
    }
    return;
  }
}

#pragma region Event connect gateway
void mtfc_gateway_connect_cloud_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller)
{
  mtfc_wk_base->is_device_online = mtfc_online;
  // Xử lý nếu không bận thì vào trạng thái kết nối
  mtfc_glcd_console("Online");
}

void mtfc_gateway_disconnect_cloud_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller)
{
  mtfc_wk_base->is_device_online = mtfc_offline;
  mtfc_wk_base->is_device_conneted_soft = mtfc_disconnect;
  state_controller->soft_controler_input = CLEAR_VALUE;
  mtfc_glcd_console("Offline");
}

void mtfc_gateway_connect_soft_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller)
{
  if (mtfc_wk_base->is_device_online == mtfc_online)
  {
    // Khi lần đầu vào thì đọc phần cứng cập nhật vào gói message
    state_controller->soft_controler_input = CLEAR_VALUE;
    mtfc_wk_base->input_hard = check_in_main_com.get_all_signal_input();
    mtfc_wk_base->input_soft = mtfc_wk_base->input_hard;
    mtfc_device_state_config.soft_controler_input = mtfc_wk_base->input_soft;
    state_controller->soft_controler_input = mtfc_wk_base->input_soft;
    mtfc_wk_base->is_device_conneted_soft = mtfc_connect;
    // Xử lý nếu không bận thì vào trạng thái kết nối
    mtfc_glcd_console("App connected");
  }
}

void mtfc_gateway_disconnect_soft_event(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *config, type_state_module_config_t *state_controller)
{
  mtfc_wk_base->is_device_conneted_soft = mtfc_disconnect;
  state_controller->soft_controler_input = CLEAR_VALUE;
  mtfc_glcd_console("App disconnected");
}
#pragma endregion

void mtfc_gateway_rx_event_handler(uint8_t siz)
{
  switch (rf_com.get_cmd())
  {
  case MSP_GATEWAY_SOFT_CONNECT:
  {
    rf_com.readstruct((uint8_t *)&mtfc_gate_consol_message, GATEWAY_CONSOLE_MESSAGE_LENGTH);
    mtfc_flag_system.is_soft_connected = true;
    break;
  }
  case MSP_GATEWAY_CLOUD_CONNECT:
  {
    rf_com.readstruct((uint8_t *)&mtfc_gate_consol_message, GATEWAY_CONSOLE_MESSAGE_LENGTH);
    mtfc_flag_system.is_cloud_connected = true;
    break;
  }
  case MSP_GATEWAY_REQUEST_DATA:
  {
    rf_com.readstruct((uint8_t *)&mtfc_gate_consol_message, GATEWAY_CONSOLE_MESSAGE_LENGTH);
    mtfc_flag_system.is_gateway_consle_message = true;
    break;
  }
  case MSP_APP_MESS_CONTROL:
  {
    rf_com.readstruct((uint8_t *)&mtfc_gate_control_message, GATEWAY_CONSOLE_MESSAGE_LENGTH);
    mtfc_flag_system.is_app_control = true;
    break;
  }
  case MSP_APP_SYN_RTC_TIME: //  ;
  {
    rf_com.readstruct((uint8_t *)&mtfc_rtc_new_update, sizeof(type_date_time_t));
    mtfc_flag_system.is_update_rtc = true;
    break;
  }
  case MSP_QUICK_CYCLE_CONFIG:
  {
    rf_com.readstruct((uint8_t *)&mtfc_quick_cycle_config, sizeof(type_mtfc_quick_cycle_config_t));
    mtfc_flag_system.is_quick_cycle_config = true;
    break;
  }
  case MSP_APP_ACTIVE_TIME:
  {
    rf_com.readstruct((uint8_t *)&mtfc_schedule_buffer.active_time, sizeof(type_active_lamp_t));
    mtfc_flag_system.is_active_time_update = true;
    break;
  }
  case MSP_GPS_GATEWAY:
  {
    rf_com.readstruct((uint8_t *)&mtfc_coordinate_gateway_message, sizeof(type_mtfc_coordinates_message_t));
    mtfc_flag_system.is_time_location_syn_gps = true;
    break;
  }
  case MSP_APP_DAYS_FORM: // type_mtfc_schedule_t mtfc_schedule_server;
  {
    rf_com.readstruct((uint8_t *)&mtfc_schedule_server.days, sizeof(type_days_lamp_form_t));
    mtfc_working_current_base.idx_app_schedule = 0;
    debug(MAIN_DEBUG, "\r\n%s", "App trasferred days form");
    break;
  }
  case MSP_APP_TIME_FORM:
  {
    rf_com.readstruct((uint8_t *)&mtfc_schedule_server.time, sizeof(type_time_lamp_form_t));
    mtfc_working_current_base.idx_app_schedule = 0;
    debug(MAIN_DEBUG, "\r\n%s", "App trasferred  time form");
    break;
  }
  case MSP_APP_CYCLE_FORM:
  {
    rf_com.readstruct((uint8_t *)&mtfc_schedule_server.cycle.index[mtfc_working_current_base.idx_app_schedule], sizeof(type_one_cycle_lamp_time_t));
    debug(MAIN_DEBUG, "\r\nApp trasferred  idx cycle form [%d]", mtfc_working_current_base.idx_app_schedule);
    mtfc_working_current_base.idx_app_schedule += 1;
    mtfc_flag_system.is_server_trasfer_one_cycle_form = true;
    break;
  }
  default:
    break;
  }
}

// #define MSP_APP_DAYS_FORM               42//Gui dau tien
// #define MSP_APP_TIME_FORM               43//Gui thu 2
// #define MSP_APP_CYCLE_FORM              44 //Gui 32 lan moi lan delay 200ms

void mtfc_gateway_processing(type_mtfc_cycle_working_package_t *mtfc_wk_base)
{
  // Gui goi tin keepalive
#pragma region KEEPALIVE
  mtfc_gateway_keepalive(mtfc_wk_base);
#pragma endregion

#pragma region GUI GOI TIN 1S
  // Ở trạng thái kết nối định kỳ gủi 1 s
  if (mtfc_wk_base->is_device_conneted_soft == mtfc_connect)
  {
    // if (mtfc_flag_system.is_ticker_one_seconds_gateway) //Khi co ket noi voi phan mem thi gui thong tin dinh ky 1s
    // {
    //   mtfc_working_message.num_phase = mtfc_wk_base->num_side;
    //   mtfc_working_message.rtc_epoch = mtfc_rtc.epoch();
    //   time_t t = mtfc_working_message.rtc_epoch ;
    //   if (mtfc_config.railway_mode_enable)
    //     mtfc_working_message.railway_signal = mtfc_wk_base->railway_signal;
    //   else
    //     mtfc_working_message.railway_signal = 0;
    //   if (mtfc_config.walking_mode_enable)
    //     mtfc_working_message.walking_signal = mtfc_wk_base->walking_signal;
    //   else
    //     mtfc_working_message.walking_signal = 0;

    //   mtfc_wk_base->input_soft = mtfc_device_state_config.soft_controler_input;
    //   mtfc_working_message.cpu_active_mode = mtfc_wk_base->mtfc_working_state;
    //   mtfc_working_message.input_hardware = mtfc_wk_base->input_hard;
    //   mtfc_working_message.input_software = mtfc_wk_base->input_soft;
    //   mtfc_working_message.idx_time_form = mtfc_wk_base->idx_time_form;
    //   mtfc_working_message.idx_timeline = mtfc_wk_base->idx_timeline;
    //   mtfc_working_message.idx_cycle_form = mtfc_wk_base->idx_cycle_form;
    //   for (uint8_t i = 0; i < MAX_SIDE; i++)
    //   {
    //     mtfc_working_message.signal[i] = mtfc_wk_base->signal[i];
    //     mtfc_working_message.mapping[i] = mtfc_wk_base->mapping[i];
    //     mtfc_working_message.countdown[i] = mtfc_wk_base->countdown[i];
    //     mtfc_working_message.output_sensor[i] = 0;
    //   }
    //   for(uint8_t i = 0; i < MAX_SIDE; i++)
    //   {
    //     mtfc_working_message.output_sensor[i] = mtfc_wk_base->sensor[i];
    //   }
    //   mtfc_send_mtfc_working_one_seconds_message(&rf_com, &mtfc_working_message);
    //   mtfc_flag_system.is_ticker_one_seconds_gateway = false;
    //   return;
    // }
  }
#pragma endregion

#pragma region CAC THONG SO LAM VIEC CUA GATEWAY LEN SERVER
  // Gủi các gói tin thông số làm việc của bộ gateway
  if (mtfc_flag_system.is_gateway_consle_message)
  {
    if (strstr((char *)mtfc_gate_consol_message, "schedule")) // test com gateway
    {
      mtfc_flag_system.is_update_schedule_to_gateway = true;
    }
    else if (strstr((char *)mtfc_gate_consol_message, "config"))
    {
      mtfc_flag_system.is_update_mtfc_config_to_gateway = true;
    }
    else if (strstr((char *)mtfc_gate_consol_message, "coordinate"))
    {
      mtfc_flag_system.is_update_mtfc_coordinate_to_gateway = true;
    }
    else if (strstr((char *)mtfc_gate_consol_message, "details"))
    {
      debug(MAIN_DEBUG, "\r\n%s", "Update details to gw");
      mtfc_flag_system.is_update_mtfc_details_to_gateway = true;
    }
    else if (strstr((char *)mtfc_gate_consol_message, "cputype"))
    {
      mtfc_flag_system.is_update_mtfc_type_cpu_to_gateway = true;
    }
    memset((char *)mtfc_gate_consol_message, '\0', 32);
    mtfc_flag_system.is_gateway_consle_message = false;
  }

  if (mtfc_flag_system.is_update_schedule_to_gateway)
  {
    mtfc_send_schedule(&rf_com, &mtfc_schedule);
    mtfc_glcd_console("Update schedule to gateway");
    debug(DEBUG_CONSOLE, "\r\n%s", "Send schedule gateway");
    mtfc_flag_system.is_update_schedule_to_gateway = false;
  }
  else if (mtfc_flag_system.is_update_mtfc_config_to_gateway)
  {
    mtfc_send_mtfc_config(&rf_com, &mtfc_config);
    mtfc_glcd_console("Update config to gateway");
    mtfc_flag_system.is_update_mtfc_config_to_gateway = false;
  }
  else if (mtfc_flag_system.is_update_mtfc_coordinate_to_gateway)
  {
    type_mtfc_coordinates_message_t coords_message;
    coords_message.latitude = mtfc_config.latitude;
    coords_message.longitude = mtfc_config.longitude;
    mtfc_send_coordinates(&rf_com, &coords_message);
    mtfc_glcd_console("Update coordinate to gateway");
    mtfc_flag_system.is_update_mtfc_coordinate_to_gateway = false;
  }
  else if (mtfc_flag_system.is_update_mtfc_details_to_gateway)
  {
    mtfc_send_mtfc_details(&rf_com, &mtfc_detail_message);
    mtfc_glcd_console("Update details to gateway");
    mtfc_flag_system.is_update_mtfc_details_to_gateway = false;
  }
  else if (mtfc_flag_system.is_update_mtfc_type_cpu_to_gateway)
  {
    mtfc_send_mtfc_type(&rf_com, &mtfc_config);
    mtfc_glcd_console("Update typeCPU to gateway");
    mtfc_flag_system.is_update_mtfc_type_cpu_to_gateway = false;
  }
#pragma endregion

#pragma region CAC GOI TIN KET NOI
  // Các gói tin thông báo sự kiện kết nối
  if (mtfc_flag_system.is_soft_connected) // SU KIEN SOFT KET NOI
  {
    if (strstr((char *)mtfc_gate_consol_message, "app_connect")) // test com gateway
    {
      mtfc_gateway_connect_soft_event(&mtfc_working_current_base, &mtfc_config, &mtfc_device_state_config);
    }
    else if (strstr((char *)mtfc_gate_consol_message, "app_disconnect"))
    {
      mtfc_gateway_disconnect_soft_event(&mtfc_working_current_base, &mtfc_config, &mtfc_device_state_config);
    }
    memset((char *)mtfc_gate_consol_message, '\0', 32);
    mtfc_flag_system.is_soft_connected = false;
  }

  if (mtfc_flag_system.is_cloud_connected) // SU KIEN CONNECTED CLOUD
  {
    if (strstr((char *)mtfc_gate_consol_message, "cloud_connect")) // test com gateway
    {
      mtfc_gateway_connect_cloud_event(&mtfc_working_current_base, &mtfc_config, &mtfc_device_state_config);
    }
    else if (strstr((char *)mtfc_gate_consol_message, "cloud_disconnect"))
    {
      mtfc_gateway_disconnect_cloud_event(&mtfc_working_current_base, &mtfc_config, &mtfc_device_state_config);
    }
    memset((char *)mtfc_gate_consol_message, '\0', 32);
    mtfc_flag_system.is_cloud_connected = false;
  }
#pragma endregion

#pragma region CAC LENH DIEU KHIEN
  if (mtfc_flag_system.is_app_control)
  {
    if ((mtfc_wk_base->is_device_conneted_soft == mtfc_connect) && (mtfc_wk_base->is_device_online == mtfc_online)) // Thiet bi online va ket noi voi app
    {
      if (strstr((char *)mtfc_gate_control_message, "mtfc_on")) // test com gateway
      {
        bit_set(mtfc_device_state_config.soft_controler_input, pin_switch_on);
        mtfc_glcd_console("App pressed on");
      }
      else if (strstr((char *)mtfc_gate_control_message, "mtfc_off"))
      {
        bit_clear(mtfc_device_state_config.soft_controler_input, pin_switch_on);
        mtfc_glcd_console("App pressed off");
      }
      else if (strstr((char *)mtfc_gate_control_message, "mtfc_select_free"))
      {
        mtfc_device_state_config.soft_controler_input = (mtfc_device_state_config.soft_controler_input & 0x87);
        mtfc_glcd_console("App select free");
      }
      if (bit_check(mtfc_device_state_config.soft_controler_input, pin_switch_on))
      {
        if (strstr((char *)mtfc_gate_control_message, "mtfc_select_t1"))
        {
          mtfc_device_state_config.soft_controler_input = (mtfc_device_state_config.soft_controler_input & 0x87);
          bit_set(mtfc_device_state_config.soft_controler_input, pin_switch_t1);
          mtfc_glcd_console("App select T1");
        }
        else if (strstr((char *)mtfc_gate_control_message, "mtfc_select_t2"))
        {

          mtfc_device_state_config.soft_controler_input = (mtfc_device_state_config.soft_controler_input & 0x87);
          bit_set(mtfc_device_state_config.soft_controler_input, pin_switch_t2);
          mtfc_glcd_console("App select T2");
        }
        else if (strstr((char *)mtfc_gate_control_message, "mtfc_select_t3"))
        {
          if (mtfc_wk_base->num_side > 2)
          {
            mtfc_device_state_config.soft_controler_input = (mtfc_device_state_config.soft_controler_input & 0x87);
            bit_set(mtfc_device_state_config.soft_controler_input, pin_switch_t3);
            mtfc_glcd_console("App select T3");
          }
          else
          {
            mtfc_device_state_config.soft_controler_input = mtfc_device_state_config.soft_controler_input & 0xDF;
          }
        }
        else if (strstr((char *)mtfc_gate_control_message, "mtfc_select_t4"))
        {
          if (mtfc_wk_base->num_side > 3)
          {
            mtfc_device_state_config.soft_controler_input = (mtfc_device_state_config.soft_controler_input & 0x87);
            bit_set(mtfc_device_state_config.soft_controler_input, pin_switch_t4);
            mtfc_glcd_console("App select T4");
          }
          else
          {
            mtfc_device_state_config.soft_controler_input = mtfc_device_state_config.soft_controler_input & 0xBF;
          }
        }
      }
    }
    if (strstr((char *)mtfc_gate_control_message, "mtfc_reset")) // test com gateway
    {
      mtfc_flag_system.is_mtfc_reset_system = true;
    }
    memset((char *)mtfc_gate_control_message, '\0', GATEWAY_CONSOLE_MESSAGE_LENGTH);
    mtfc_flag_system.is_app_control = false;
  }
  if (mtfc_flag_system.is_mtfc_reset_system)
  {
    mtfc_glcd_console("App mtfc reset");
    NVIC_SystemReset();
    mtfc_flag_system.is_mtfc_reset_system = false;
  }
  if (mtfc_flag_system.is_time_location_syn_gps)
  {
    mtfc_flag_system.is_time_location_syn_gps = false;
    debug(MAIN_DEBUG, "\n\rNew gps updated\r\nLatitude:%f\r\nLongitude:%f", mtfc_coordinate_gateway_message.latitude, mtfc_coordinate_gateway_message.longitude);
    mtfc_flag_system.is_gateway_update_gps = true;
  }
#pragma endregion

#pragma region CAI DAT SCHEDULE
  if (mtfc_flag_system.is_sever_set_schedule) // Xu ly khi nhan du du lieu
  {

    mtfc_flag_system.is_sever_set_schedule = false;
    debug(MAIN_DEBUG, "\r\n%s", "Recieved all data form app");
    // mtfc_debug_print_time_form(&mtfc_schedule_server.time);
    // mtfc_debug_print_days_form(&mtfc_schedule_server.days);
    // mtfc_debug_print_cycle_form(&mtfc_schedule_server.cycle);
    // debug(MAIN_DEBUG, "\r\n%s", "Caculator cycle form app setting");
    mtfc_schedule_server.num_side = mtfc_schedule.num_side;
    mtfc_contruct_all_cycle_user_config(&mtfc_schedule_server.cycle, mtfc_schedule.num_side);
    mtfc_schedule_server.active_time = mtfc_schedule.active_time;
    mtfc_schedule_buffer = mtfc_schedule_server;
    mtfc_flag_system.is_save_schedule_form_server = true;
    // mtfc_debug_print_cycle_form(&mtfc_schedule_server.cycle);
  }

  if (mtfc_flag_system.is_server_trasfer_one_cycle_form) // Moi cycle form gui phan hoi ve lai gateway
  {
    if (mtfc_working_current_base.idx_app_schedule >= MAX_CYCLE_FORM)
    {
      mtfc_flag_system.is_server_trasfer_one_cycle_form = false;
      mtfc_flag_system.is_sever_set_schedule = true;
    }
    else
    {
      if ((millis() - mtfc_working_current_base.millis_ping_cycle_form) >= TIME_PING_CYCLE_FORM)
      {
        rf_com.send_byte(MSP_APP_CYCLE_IDX, mtfc_working_current_base.idx_app_schedule);
        mtfc_working_current_base.millis_ping_cycle_form = millis();
      }
    }
  }

#pragma endregion
}

#pragma endregion

#pragma region XU LY CAC CAI DAT CARD DIEU KHIEN-----------------------------------------

void mtfc_check_phase_connect_railway(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config, type_mtfc_schedule_t *mtfc_schedule)
{
  // mtfc_mem.mem_read_data(BLOCK_3, (char *)&mtfc_one_card_config_card1, sizeof(type_one_cardConfig_t));
  for (uint8_t j = 0; j < MAX_SIDE; j++)
  {
    mtfc_wk->railway_connect_phase_config[j] = 0;
    if (mtfc_config->index_card[j].is_railway_enabled == 1)
      mtfc_wk->railway_connect_phase_config[mtfc_config->index_card[j].phase - 1] = 1;
  }
  debug(MAIN_DEBUG, "\r\nrailway_connect_phase_config:[%02d][%02d][%02d][%02d][%02d][%02d][%02d][%02d]",
        mtfc_wk->railway_connect_phase_config[0],
        mtfc_wk->railway_connect_phase_config[1],
        mtfc_wk->railway_connect_phase_config[2],
        mtfc_wk->railway_connect_phase_config[3],
        mtfc_wk->railway_connect_phase_config[4],
        mtfc_wk->railway_connect_phase_config[5],
        mtfc_wk->railway_connect_phase_config[6],
        mtfc_wk->railway_connect_phase_config[7]);
}

int8_t mtfc_check_card_connect_railway(type_cardConfig_t *card)
{
  uint8_t res = 0;
  for (uint8_t i = 0; i < MAX_SIDE; i++)
  {
    if (card->index[i].is_railway_enabled == 1)
      res++;
  }
  return res;
}

int8_t mtfc_check_num_card_insert(type_cardConfig_t *card)
{
  uint8_t res = 0;
  for (uint8_t i = 0; i < MAX_SIDE; i++)
  {
    if (card->index[i].imei != 0)
      res++;
  }
  return res;
}

uint8_t mtfc_card_detect(type_mtfc_cycle_working_package_t *obj)
{
  static uint32_t time_periode_detect_card = 0;
  static unsigned long time_out_process = millis();
  volatile bool mtfc_enter_mode_search = true;
  volatile bool mtfc_is_card_feetback = false;
  wait(1);
  obj->index_card_check = 1;
  time_periode_detect_card = millis();
  while (mtfc_enter_mode_search)
  {
    if (((millis() - time_periode_detect_card) >= 2000) || (mtfc_is_card_feetback == true))
    {
      if (obj->index_card_check >= 1 && obj->index_card_check <= 8)
      {
        check_card_com.free();
        wait(0.5);
        check_card_com.cs_slot(obj->index_card_check);
        wait(0.5);
        debug(MAIN_DEBUG, "\r\nCard check idx: [%02d]", obj->index_card_check);
        check_card_com.send_byte(MSP_CARD_READ_CONFIG, obj->index_card_check);
        main_rs232_com.send_byte(CMD_MASTER_TO_CPU_CHECK_CARD, obj->index_card_check);
        main_rs232_com.send_struct(CMD_MASTER_TO_CPU_CARD_INFOR, (uint8_t *)&mtfc_card_config_search.index[obj->index_card_check - 1], sizeof(type_one_cardConfig_t));

        (*obj).index_card_check += 1;
        time_out_process = millis();
      }
      mtfc_is_card_feetback = false;
      time_periode_detect_card = millis();
    }
    if (obj->index_card_check > 8)
    {
      if (millis() - time_out_process > 3000)
      {
        check_card_com.free();

        uint8_t res = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
          // mtfc_printOut_card_config(&mtfc_card_config_search.index[i], (i + 1));
        }
        if ((memcmp(&mtfc_card_config_search, mtfc_config.index_card, sizeof(type_cardConfig_t))) != 0)
        {
          debug(MAIN_DEBUG, "\r\n%s", "Card changed 111111111????");
          res = mtfc_check_num_card_insert(&mtfc_card_config_search);
          check_card_num_card_dependent.card_insert_now = mtfc_check_num_card_insert(&mtfc_card_config_search);
          debug(MAIN_DEBUG, "Sucessfull! Card ready [%02d]", res);
          wait(3);
        }
        else
        {
          debug(MAIN_DEBUG, "\r\n%s", "Card changed 22222222222????");
          if (mtfc_config.railway_mode_enable)
          {
            res = mtfc_check_card_connect_railway(&mtfc_card_config_search);
            debug(MAIN_DEBUG, "Found [%02d] configured railway", res);
            wait(2);
          }
          res = mtfc_check_num_card_insert(&mtfc_card_config_search);
          check_card_num_card_dependent.card_insert_now = mtfc_check_num_card_insert(&mtfc_card_config_search);
          if (res)
            debug(MAIN_DEBUG, "Sucessfull! Card ready [%02d]", res);
          else
            debug(MAIN_DEBUG, "%s\r\n", "Warning! No card insert");
          wait(2);
        }
        mtfc_enter_mode_search = false;
        return 0;
      }
    }
    if (mtfc_flag_system.is_receive_card_config_feedback)
    {
      uint8_t t = check_card_com.read_slot_slect_index();
      // debug(MAIN_DEBUG, "got card slot: %d\r\n")
      if (t >= 1)
      {
        mtfc_card_config_search.index[t - 1] = mtfc_card_config_buff;
        main_rs232_com.send_struct(CMD_MASTER_TO_CPU_CARD_INFOR, (uint8_t *)&mtfc_card_config_search.index[t - 1], sizeof(type_one_cardConfig_t));
      }
      else
        debug(CMD_MASTER_TO_CPU_CARD_INFOR, "\r\n%s", "Loi");
      mtfc_is_card_feetback = true;
      mtfc_flag_system.is_receive_card_config_feedback = false;
      check_card_com.free();
    }
  }
}

uint8_t count_card = 0;
void mtfc_card_rx_event_handler(uint8_t siz)
{
  uint8_t t = 0;
  debug(MAIN_DEBUG, "\r\ncheck_card_com.get_cmd(): [%d]", check_card_com.get_cmd());
  switch (check_card_com.get_cmd())
  {
  case MSP_CARD_READ_CONFIG:
    check_card_com.readstruct((uint8_t *)&mtfc_card_config_buff, sizeof(type_one_cardConfig_t));
    debug(MAIN_DEBUG, "imei card %ld\r\n", mtfc_card_config_buff.imei);
    debug(MAIN_DEBUG, "phase %d\r\n", mtfc_card_config_buff.phase);
    debug(MAIN_DEBUG, "railway %d\r\n", mtfc_card_config_buff.is_railway_enabled);
    debug(MAIN_DEBUG, "walking %d\r\n", mtfc_card_config_buff.is_walking_enabled);
    debug(MAIN_DEBUG, "dependent phase %d\r\n", mtfc_card_config_buff.is_dependent_phase);
    debug(MAIN_DEBUG, "time delay dependent phase %d\r\n", mtfc_card_config_buff.time_delay_dependent_phase);
    if(mtfc_card_config_buff.is_dependent_phase == 1)
    {
      mtfc_card_config_search.index[mtfc_working_current_base.index_card_check - 2].time_delay_dependent_phase = mtfc_card_config_buff.time_delay_dependent_phase;
      debug(MAIN_DEBUG, "card current: %d\r\n", mtfc_card_config_search.index[mtfc_working_current_base.index_card_check - 2].time_delay_dependent_phase);
    }
    mtfc_flag_system.is_receive_card_config_feedback = true;
    break;
  case MSP_CARD_FB_CONFIG:
    t = check_card_com.read8();
    main_rs232_com.send_byte(CMD_MASTER_TO_CPU_FB, CMD_MASTER_TO_CPU_FB_CARD_CONFIG);
    if (t == 255) // Loi
      mtfc_flag_system.is_card_fb_config = false;
    break;
  case MSP_CARD_DATA_SENSOR:
    check_card_com.readstruct((uint8_t *)&mtfc_sen_pkg, sizeof(type_sensor_pkg_t));
    mtfc_flag_system.is_sensor_update = true;
    break;
  default:
    break;
  }
}

#pragma endregion
#pragma region LIEN QUAN DEN XU LY HIEN THI GIAO TIEP MAN HINH [HIEN THI, CAI DAT,....]--------------------------
void mtfc_glcd_console(const char *format, ...)
{
  char buf[32];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  // mtfc_glcd_com.send_struct(MSP_GLCD_CONSOLE,(uint8_t*)&buf,sizeof(buf));
  va_end(ap);
}

void mtfc_glcd_processing(type_mtfc_schedule_t *schedule, type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *mtfc_config, type_date_time_t *rtc)
{
  static uint32_t mtfc_glcd_time = millis();
  static volatile bool is_send_config_data = false;
  static uint8_t temp_update_time = 0;
#pragma region update du lieu len man hinh chinh
  if ((millis() - mtfc_glcd_time >= 1200) && (mtfc_wk_base->index_page_display <= 6)) // Cac goi tin gui 1.2s len man hinh
  {
    if (mtfc_wk_base->index_page_display == 0 || mtfc_wk_base->index_page_display == 6)
    {
      mtfc_page_one_data.rtc_time = *rtc;
      mtfc_page_one_data.server_status = mtfc_wk_base->is_device_online;
      mtfc_page_one_data.app_status = mtfc_wk_base->is_device_conneted_soft;
      mtfc_page_one_data.mtfc_working_state = mtfc_wk_base->mtfc_working_state;
      mtfc_page_one_data.side_select = mtfc_wk_base->index_side_manual_select;
      mtfc_page_one_data.sd_mount = mtfc_wk_base->is_sd_inserted;
      mtfc_page_one_data.temperature = mtfc_wk_base->temperature;
      // mtfc_glcd_com.send_struct(MSP_GLCD_PAGE_ONE, (uint8_t *)&mtfc_page_one_data, sizeof(type_mtfc_page_one_t));
      if (mtfc_wk_base->index_page_display == 6)
      {
        temp_update_time++;
        if (temp_update_time >= 3)
        {
          main_rs232_com.send_byte(MSP_MTFC_TEMPRATURE, 1);
          temp_update_time = 0;
        }
      }
    }
    else if (mtfc_wk_base->index_page_display == 1 || mtfc_wk_base->index_page_display == 2)
    {
      mtfc_page_two_data.num_side = mtfc_wk_base->num_side;
      mtfc_page_two_data.t_seek = mtfc_wk_base->t_seek;
      mtfc_page_two_data.t = mtfc_wk_base->t;
      mtfc_page_two_data.t_clearance = mtfc_wk_base->t_clearance;
      mtfc_page_two_data.active_time = schedule->active_time;
      mtfc_page_two_data.idx_time_form = mtfc_wk_base->idx_time_form;
      mtfc_page_two_data.idx_timeline = mtfc_wk_base->idx_timeline;
      mtfc_page_two_data.idx_cycle_form = mtfc_wk_base->idx_cycle_form;
      for (uint8_t i = 0; i < mtfc_wk_base->num_side; i++)
        mtfc_page_two_data.side[i] = mtfc_wk_base->side[i];
      // mtfc_glcd_com.send_struct(MSP_GLCD_PAGE_TWO, (uint8_t *)&mtfc_page_two_data, sizeof(type_mtfc_page_two_t));
    }
    else if (mtfc_wk_base->index_page_display == 3)
    {
      mtfc_page_iv_data.raiway_enable = mtfc_config->railway_mode_enable;
      mtfc_page_iv_data.walking_enable = mtfc_config->walking_mode_enable;
      // mtfc_glcd_com.send_struct(MSP_GLCD_PAGE_IV, (uint8_t *)&mtfc_page_iv_data, sizeof(type_mtfc_page_iv_t));
    }
    else if (mtfc_wk_base->index_page_display == 4)
    {
      mtfc_page_v_data.num_side = mtfc_wk_base->num_side;
      for (uint8_t i = 0; i < MAX_SIDE; i++)
      {
        mtfc_page_v_data.slot_phase[i] = mtfc_card_config_search.index[i].phase;
        mtfc_page_v_data.railway_enable[i] = mtfc_card_config_search.index[i].is_railway_enabled;
      }

      // mtfc_glcd_com.send_struct(MSP_GLCD_PAGE_V, (uint8_t *)&mtfc_page_v_data, sizeof(type_mtfc_page_v_t));
    }
    mtfc_glcd_time = millis();
  }
#pragma endregion

#pragma region Cai dat thong so cua card dieu khien
  if (mtfc_flag_system.is_card_config_request) // yeu cau lay thong so card luu trong flash
  {
    mtfc_card_config_data_glcd.num_side = mtfc_wk_base->num_side;
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      mtfc_card_config_data_glcd.slot_phase[i] = mtfc_config->index_card[i].phase;
      mtfc_card_config_data_glcd.railway_enable[i] = mtfc_config->index_card[i].is_railway_enabled;
    }
    // mtfc_glcd_com.send_struct(MSP_CARD_CONFIG_LOAD, (uint8_t *)&mtfc_card_config_data_glcd, sizeof(type_mtfc_page_v_t));
    if ((memcmp(&mtfc_card_config_search, mtfc_config->index_card, sizeof(type_cardConfig_t))) != 0)
    {
      mtfc_glcd_console("Please!Update Card Parameter");
    }
    mtfc_flag_system.is_card_config_request = false;
  }
  else if (mtfc_flag_system.is_card_config_search) // yeu cau lay thong so card detect luc khoi dong
  {
    mtfc_card_config_data_glcd.num_side = mtfc_wk_base->num_side;
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      mtfc_card_config_data_glcd.slot_phase[i] = mtfc_card_config_search.index[i].phase;
      mtfc_card_config_data_glcd.railway_enable[i] = mtfc_card_config_search.index[i].is_railway_enabled;
    }
    // mtfc_glcd_com.send_struct(MSP_CARD_CONFIG_LOAD, (uint8_t *)&mtfc_card_config_data_glcd, sizeof(type_mtfc_page_v_t));
    mtfc_glcd_console("Card parameter Updated!!!");
    mtfc_flag_system.is_card_config_search = false;
  }

  if (mtfc_flag_system.is_card_one_slot_update)
  {
    if ((millis() - mtfc_glcd_time) >= TIME_LOOP_SEND_CONFIG_CARD) // xu ly tien trinh theo chu ky
    {
      if (mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].imei == 0) // slot khong ton tai card
      {
        debug(MAIN_DEBUG, "%s\r\n", "Error! not found card!!");
        mtfc_flag_system.is_card_one_slot_update = false; // Loi ko tim thay card. Xoa tien trinh cau hinh card
      }
      else
      {
        if (check_card_com.read_slot_slect_index() == 0) // Slot chua duoc chon. Nen chon slot truoc. 1s sau se gui cau hinh
        {
          check_card_com.cs_slot(mtfc_one_card_config.slot);
          is_send_config_data = false;
        }
        else if ((check_card_com.read_slot_slect_index() == mtfc_one_card_config.slot) && (is_send_config_data == false)) // Ca
        {
          debug(MAIN_DEBUG, "one card config: %d\r\n", mtfc_one_card_config.slot);
          for (uint8_t i; i < 32; i++)
            mtfc_card_config_buff.sn[i] = mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].sn[i];
          mtfc_card_config_buff.imei = mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].imei;
          mtfc_card_config_buff.phase = mtfc_one_card_config.phase;
          mtfc_card_config_buff.is_railway_enabled = mtfc_one_card_config.is_railway;
          mtfc_card_config_buff.is_dependent_phase = mtfc_one_card_config.is_dependent_phase;
          mtfc_card_config_buff.is_walking_enabled = mtfc_one_card_config.is_walking;
          check_card_com.send_struct(MSP_CARD_WRITE_CONFIG, (uint8_t *)&mtfc_card_config_buff, sizeof(type_one_cardConfig_t));
          mtfc_flag_system.is_card_fb_config = true;
          is_send_config_data = true;
        }
        else if (is_send_config_data == true)
        {
          if (mtfc_flag_system.is_card_fb_config == false)
          {
            debug(MAIN_DEBUG, "Error! Config Slot [%01d]!!\r\n", mtfc_one_card_config.slot);
          }
          else
          {
            // Luu thanh cong copy lai cac thong so vao vung dem
            mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].phase = mtfc_card_config_buff.phase;
            mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].is_railway_enabled = mtfc_card_config_buff.is_railway_enabled;
            mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].is_dependent_phase = mtfc_card_config_buff.is_dependent_phase;
            mtfc_card_config_search.index[mtfc_one_card_config.slot - 1].is_walking_enabled = mtfc_card_config_buff.is_walking_enabled;
            mtfc_flag_system.is_card_update_card_parameters = true;
            debug(MAIN_DEBUG, "Sucessfull! Config Slot [%01d]!!\r\n", mtfc_one_card_config.slot);
          }
          check_card_com.free();
          mtfc_flag_system.is_card_one_slot_update = false;
          is_send_config_data = false;
        }
      }
      mtfc_glcd_time = millis();
    }
  }
#pragma endregion

#pragma region Cai dat thoi gian he thong rtc
  if (mtfc_flag_system.is_time_location_load)
  {
    mtfc_time_location.rtc_time = mtfc_rtc_val;
    mtfc_time_location.lati = mtfc_config->latitude;
    mtfc_time_location.longi = mtfc_config->longitude;
    // mtfc_glcd_com.send_struct(MSP_TIME_LOCATION_LOAD, (uint8_t *)&mtfc_time_location, sizeof(type_mtfc_time_location_t));
    mtfc_glcd_console("Time & Location loaded!");
    mtfc_flag_system.is_time_location_load = false;
  }
  else if (mtfc_flag_system.is_time_location_update)
  {
    mtfc_rtc_new_update = mtfc_time_location.rtc_time;
    mtfc_flag_system.is_update_rtc = true;
    if ((mtfc_time_location.lati != mtfc_config->latitude) || (mtfc_time_location.longi != mtfc_config->longitude))
    {
      mtfc_flag_system.is_update_location = true;
    }
    mtfc_flag_system.is_time_location_update = false;
  }
  else if (mtfc_flag_system.is_update_gps_request)
  {
    mtfc_glcd_console("Wait...gps update");
    rf_com.send_byte(MSP_GPS_GATEWAY, 1);
    mtfc_flag_system.is_update_gps_request = false;
  }
#pragma endregion

#pragma region Cai dat active time
  if (mtfc_flag_system.is_active_time_load)
  {
    // mtfc_glcd_com.send_struct(MSP_ACTIVE_TIME_LOAD, (uint8_t *)&schedule->active_time, sizeof(type_active_lamp_t));
    mtfc_glcd_console("active time loaded!");
    mtfc_flag_system.is_active_time_load = false;
  }
#pragma endregion

#pragma region menu details
  if (mtfc_flag_system.is_details_loaded)
  {
    // mtfc_glcd_com.send_struct(MSP_DETAILS_LOAD, (uint8_t *)&mtfc_detail_message, sizeof(type_mtfc_details_message_t));
    mtfc_flag_system.is_details_loaded = false;
  }
#pragma endregion

#pragma region menu device
  if (mtfc_flag_system.is_device_loaded || mtfc_flag_system.is_connection_load)
  {
    // mtfc_send_mtfc_config(&mtfc_glcd_com, mtfc_config);
    mtfc_flag_system.is_connection_load = false;
    mtfc_flag_system.is_device_loaded = false;
  }
  else if (mtfc_flag_system.is_device_default_schedule)
  {
    if (schedule->num_side >= 2 && schedule->num_side <= 8)
      mtfc_contruct_schedule_default(&mtfc_schedule_buffer, schedule->num_side);
    else
      mtfc_contruct_schedule_default(&mtfc_schedule_buffer, 2);
    mtfc_flag_system.is_save_new_schedule = true;
    mtfc_flag_system.is_device_default_schedule = false;
  }
#pragma endregion

#pragma region day form
  if (mtfc_flag_system.is_day_form_load)
  {
    // mtfc_glcd_com.send_struct(MSP_DAY_FORM_LOAD,(uint8_t *)&mtfc_schedule.days,sizeof(type_days_lamp_form_t));
    mtfc_flag_system.is_day_form_load = false;
  }
#pragma endregion

#pragma region time form
  if (mtfc_flag_system.is_time_form_load)
  {
    // mtfc_glcd_com.send_struct(MSP_TIME_FORM_LOAD,(uint8_t *)&mtfc_schedule.time,sizeof(type_time_lamp_form_t));
    mtfc_glcd_console("Time form loaded");
    mtfc_flag_system.is_time_form_load = false;
  }
#pragma endregion

#pragma region cycle form
  if (mtfc_flag_system.is_cycle_form_load_event) // Bat su kien yeu cau gui cycle form
  {
    // mtfc_glcd_com.send_byte(MSP_CYCLE_FORM_BEGIN_TRANS, MAX_CYCLE_FORM); //send reset counter;
    mtfc_glcd_console("Pls! Wait load cycle form");
    mtfc_wk_base->index_send_cycle_form = 0;
    mtfc_flag_system.is_cycle_form_load = true;
    mtfc_flag_system.is_cycle_form_load_event = false;
  }
  else if (mtfc_flag_system.is_cycle_form_load) // Tien trinh gui cycle form len man hinh
  {
    if ((millis() - mtfc_glcd_time >= 100))
    {
      // mtfc_glcd_com.send_struct(MSP_CYCLE_FORM_LOAD, (uint8_t *)&schedule->cycle.index[mtfc_wk_base->index_send_cycle_form], sizeof(type_one_cycle_lamp_time_t));
      mtfc_wk_base->index_send_cycle_form++;
      mtfc_glcd_time = millis();
    }
    if (mtfc_wk_base->index_send_cycle_form >= MAX_CYCLE_FORM)
    {
      mtfc_wk_base->index_send_cycle_form = 0;
      mtfc_glcd_console("Cycle form loaded");
      mtfc_flag_system.is_cycle_form_load = false;
    }
  }
  else if (mtfc_flag_system.is_cycle_form_default)
  {
    mtfc_contruct_all_cycle_default(&mtfc_schedule_buffer.cycle, schedule->num_side);
    mtfc_flag_system.is_all_cycle_form_update = true;
    mtfc_flag_system.is_cycle_form_pre_load = true;
    mtfc_flag_system.is_cycle_form_default = false;
  }
  else if (mtfc_flag_system.is_cycle_form_update)
  {
    mtfc_contruct_all_cycle_user_config(&mtfc_schedule_buffer.cycle, schedule->num_side);
    mtfc_flag_system.is_all_cycle_form_update = true;
    mtfc_flag_system.is_cycle_form_pre_load = true;
    mtfc_flag_system.is_cycle_form_update = false;
    mtfc_working_current_base.index_send_cycle_form = 0;
    // mtfc_debug_print_active_time(&mtfc_schedule.active_time);
    // mtfc_debug_print_time_form(&mtfc_schedule.time);
    //  mtfc_debug_print_days_form(&mtfc_schedule.days);
    mtfc_debug_print_cycle_form(&mtfc_schedule.cycle);
  }
#pragma endregion

#pragma region cai dat pha cho den dem nguoc RS485
  if (mtfc_flag_system.is_send_config_to_lamp)
  {
    mtfc_lamp_com.send_struct(MSP_LAMP485_CONFIG, (uint8_t *)&dn_config, sizeof(type_dn_config_t));
    mtfc_glcd_console("Set all Lamp to id [%01d]", dn_config.phase);
    mtfc_flag_system.is_send_config_to_lamp = false;
  }
#pragma endregion
}

void mtfc_rtc_update(type_date_time_t *t)
{
  static unsigned long mtfc_mill_update_time = millis();
  if (mtfc_flag_system.is_update_rtc)
  {
    // if (mtfc_rtc_new_update.day == -1)
    //   mtfc_rtc_new_update.day = 6;
    // else
    //   mtfc_rtc_new_update.day = mtfc_rtc_new_update.day - 1;
    type_date_time_t temp = *t;
    if (mtfc_rtc.rtc_set_time(temp))
    {
      debug(MAIN_DEBUG, "%s\r\n", "Rtc update failed!!!");
    }
    else
    {
      debug(MAIN_DEBUG, "%s\r\n", "Rtc update successful!!!");
    }

    debug(DEBUG_CONSOLE, "\r\nSET RTC: %02d %02d %02d - %d %02d/%02d/%04d",
          mtfc_rtc_new_update.hour,
          mtfc_rtc_new_update.minute,
          mtfc_rtc_new_update.seconds,
          mtfc_rtc_new_update.day,
          mtfc_rtc_new_update.date,
          mtfc_rtc_new_update.month,
          mtfc_rtc_new_update.year);
    mtfc_flag_system.is_require_update_rtc_ex = mtfc_flag_system.is_require_update_rtc_ex ? false : mtfc_flag_system.is_require_update_rtc_ex;
    mtfc_flag_system.is_update_rtc = false;
  }
  if (mtfc_flag_system.is_require_update_rtc_ex)
  {
    if (millis() - mtfc_mill_update_time >= 3000)
    {
      rf_com.send_byte(MSP_APP_SYN_RTC_TIME, 1);
      mtfc_mill_update_time = millis();
    }
  }
}

#pragma endregion

#pragma region XU LY GIAO TIEP VOI CARD GIAO TIEP MO RONG
void mtfc_ex_card_communication(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config, type_mtfc_schedule_t *mtfc_schedule)
{
  if (!mtfc_wk->is_startup_state) // Kiem tra cac thong so ex com khi moi khoi dong
  {
    mtfc_wk->is_startup_state = true;
    debug(MAIN_DEBUG, "\r\n%s", "Check ex com card ???");
    main_rs232_com.send_byte(MSP_CHECK_MODULE, 1);
    wait(0.02);
    debug(MAIN_DEBUG, "\r\n%s", "Check sd card mounted ???");
    main_rs232_com.send_byte(MSP_SD_MOUNT, 1);
  }
  if (mtfc_flag_system.is_sd_mounted_event)
  {
    mtfc_flag_system.is_sd_mounted_event = false;
    mtfc_glcd_console("%s", (mtfc_wk->is_sd_inserted == 1) ? "Sd inserted" : "Sd removed");
  }
  if (mtfc_flag_system.is_import_sd_feedback)
  {
    mtfc_flag_system.is_import_sd_feedback = false;
    mtfc_glcd_console("%s",
                      (mtfc_wk->ex_com_import_feedback == 0) ? "Sd imported all data" : (mtfc_wk->ex_com_import_feedback == 1) ? "Can't read file:(("
                                                                                    : (mtfc_wk->ex_com_import_feedback == 2)   ? "Data mismatch:(("
                                                                                    : (mtfc_wk->ex_com_import_feedback == 3)   ? "Schedule parameter illegal:(("
                                                                                                                               : "Unknow import message:((");
  }
  if (mtfc_flag_system.is_export_sd_feedback)
  {
    mtfc_flag_system.is_export_sd_feedback = false;
    mtfc_glcd_console("%s",
                      (mtfc_wk->ex_com_export_feedback == 0) ? "Export to sd successfull!" : "Export to sd failed :((");
  }
  if (mtfc_flag_system.is_ex_card_update_schedule) // update toan bo schedule
  {
    debug(MAIN_DEBUG, "\r\n%s", "Received schedule form sd card");
    // mtfc_debug_print_active_time(&mtfc_schedule_buffer.active_time);
    // mtfc_debug_print_time_form(&mtfc_schedule_buffer.time);
    // mtfc_debug_print_days_form(&mtfc_schedule_buffer.days);
    // mtfc_debug_print_cycle_form(&mtfc_schedule_buffer.cycle);
    mtfc_schedule_buffer.num_side = mtfc_schedule->num_side; // reload num phase for schedule buffer
    mtfc_flag_system.is_save_new_schedule_form_sd = true;
    mtfc_flag_system.is_ex_card_update_schedule = false;
  }
  if (mtfc_flag_system.is_import_schedule_sd)
  {
    mtfc_flag_system.is_import_schedule_sd = false;
    main_rs232_com.send_byte(MSP_IMPORT_SCHEDULE_SD, mtfc_schedule->num_side);
    debug(MAIN_DEBUG, "\r\n%s", "Send request import schedule sd");
  }
  else if (mtfc_flag_system.is_export_schedule_sd)
  {
    if (!mtfc_send_schedule(&main_rs232_com, mtfc_schedule, mtfc_wk))
    {
      mtfc_flag_system.is_export_schedule_sd = false;
      debug(MAIN_DEBUG, "\r\n%s", "Finish send schedule to sd card");
    }
  }
  if (mtfc_flag_system.is_usb_console)
  {
    if ((strstr((char *)mtfc_wk->usb_console_txt, "mtfc_reset_imei")))
    {
      debug(MAIN_DEBUG, "\r\n%s", "Reset IMEI mtfc");
      mtfc_glcd_console("%s", "Reset IMEI");
      mtfc_mem.mem_re_initialize_imei();
      mtfc_glcd_console("%s", "ReStartUp mtfc");
      while (1)
        ;
    }
    else if ((strstr((char *)mtfc_wk->usb_console_txt, "mtfc_reset_firmware")))
    {
      debug(MAIN_DEBUG, "\r\n%s", "Reset firmware mtfc");
      mtfc_glcd_console("%s", "Reset firmware");
      mtfc_mem.mem_re_initialize_firmware();
      mtfc_glcd_console("%s", "ReStartUp mtfc");
      while (1)
        ;
    }
    else if ((strstr((char *)mtfc_wk->usb_console_txt, "mtfc_config_phase")))
    {
      char *temp = strtok((char *)mtfc_wk->usb_console_txt, "=");
      temp = strtok(NULL, "p");
      uint8_t new_phase = (uint8_t)atoi(temp);
      if (new_phase >= 2 && new_phase <= 8)
      {
        debug(MAIN_DEBUG, "\r\nSet num phase [%02d]", new_phase);
        mtfc_glcd_console("Set num phase [%02d]", new_phase);
        mtfc_contruct_schedule_default(&mtfc_schedule_buffer, new_phase);
        *mtfc_schedule = mtfc_schedule_buffer;
        mtfc_mem.mem_save_data(BLOCK_2, (char *)mtfc_schedule, sizeof(type_mtfc_schedule_t));
        mtfc_glcd_console("%s", "ReStartUp mtfc");
        while (1)
          ;
      }
      else
      {
        mtfc_glcd_console("%s", "Set num phase error :((");
      }
    }
    memset((char *)&mtfc_wk->usb_console_txt, '\0', 64);
    mtfc_flag_system.is_usb_console = false;
  }
  if (mtfc_flag_system.is_gps_local_request) // yeu cau lay local gps
  {
    main_rs232_com.send_byte(MSP_GET_GPS_EX, 1);
    mtfc_flag_system.is_gps_local_request = false;
  }
  if (mtfc_flag_system.is_gps_local_message)
  {
    mtfc_glcd_console("%s", mtfc_wk->gps_message_txt);
    mtfc_flag_system.is_gps_local_message = false;
  }
  if (mtfc_flag_system.is_gps_local_data)
  {
    mtfc_flag_system.is_gps_local_data = false;
    debug(MAIN_DEBUG, "\n\rNew gps local updated\r\nLatitude:%f\r\nLongitude:%f", mtfc_coordinate_gateway_message.latitude, mtfc_coordinate_gateway_message.longitude);
    mtfc_flag_system.is_gateway_update_gps = true;
  }
}
int aqw = 0;
void mtfc_ex_card_com_rx_event_handler(uint8_t siz)
{
  debug(MAIN_DEBUG, "\r\nmain_rs232_com.get_cmd(): [%d]", main_rs232_com.get_cmd());
  switch (main_rs232_com.get_cmd())
  {
  case CMD_CPU_TO_MASTER_SOPHA:
    debug(MAIN_DEBUG, "\r\n%d", schedule_update_from_screen.num_side);
    schedule_update_from_screen.num_side = main_rs232_com.read8();
    mtfc_contruct_schedule_default(&schedule_update_from_screen, schedule_update_from_screen.num_side);
    mtfc_mem.mem_save_data(BLOCK_2, (char *)&schedule_update_from_screen, sizeof(type_mtfc_schedule_t));
    check_reset_mcu.is_check_soft_reset = 1;
    mtfc_mem.mem_save_data(BLOCK_3, (char *)&check_reset_mcu, sizeof(type_check_flag_reset_mcu_t));
    debug(MAIN_DEBUG, "\r\n %d", schedule_update_from_screen.num_side);
    // debug(MAIN_DEBUG, "\r\nactive_time.tbegin %d", schedule_update_from_screen.active_time.tbegin);
    // debug(MAIN_DEBUG, "\r\nactive_time.tend %d", schedule_update_from_screen.active_time.tend);
    system_reset(); // thangnm thay doi cho nay
    break;
  case CMD_CPU_TO_MASTER_TIME_ACTIVE:
    main_rs232_com.readstruct((uint8_t *)&mtfc_schedule_buffer.active_time, sizeof(type_active_lamp_t));
    mtfc_flag_system.is_active_time_update = true;
    break;

  case CMD_CPU_TO_MASTER_CYCLE_FORM:
    main_rs232_com.readstruct((uint8_t *)&mtfc_quick_cycle_config, sizeof(type_mtfc_quick_cycle_config_t));
    debug(MAIN_DEBUG, "\r\nmtfc_quick_cycle_config %d", mtfc_quick_cycle_config.num_side);
    debug(MAIN_DEBUG, "\r\nmtfc_quick_cycle_config %d", mtfc_quick_cycle_config.green_t[0]);
    debug(MAIN_DEBUG, "\r\nmtfc_quick_cycle_config %d", mtfc_quick_cycle_config.green_t[1]);
    debug(MAIN_DEBUG, "\r\nmtfc_quick_cycle_config %d", mtfc_quick_cycle_config.yellow_t);
    debug(MAIN_DEBUG, "\r\nmtfc_quick_cycle_config %d", mtfc_quick_cycle_config.clearance_t);
    mtfc_flag_system.is_quick_cycle_config = true;
    break;

  case CMD_CPU_TO_MASTER_SET_RTC_MANUAL:
    main_rs232_com.readstruct((uint8_t *)&mtfc_rtc_new_update, sizeof(type_date_time_t));
    mtfc_flag_system.is_update_rtc = true;
    break;

  case CMD_CPU_TO_MASTER_MODE_CROSS:
    mode_cross = main_rs232_com.read8();
    // debug(MAIN_DEBUG, "\r\nmode_cross %d",mode_cross);
    break;

  case CMD_CPU_TO_MASTER_CHANGE_PHASE_1:
    main_rs232_com.readstruct((uint8_t *)&mtfc_one_card_config, sizeof(type_mtfc_one_card_config_t));
    debug(MAIN_DEBUG, "\r\nOne slot card update: [%01d] [%01d] [%01d] [%01d] [%01d]", mtfc_one_card_config.slot, mtfc_one_card_config.phase, mtfc_one_card_config.is_railway, mtfc_one_card_config.is_walking, mtfc_one_card_config.is_dependent_phase);
    mtfc_flag_system.is_card_one_slot_update = true;
    break;

  case CMD_CPU_TO_MASTER_SCHEDULE:
    main_rs232_com.send_struct(200, (uint8_t *)&cycle, sizeof(cycle_t));
    break;
  case CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE:
  {
    main_rs232_com.readstruct((uint8_t *)&cycle.time, sizeof(type_time_lamp_form_t));

    mtfc_schedule_buffer.time = cycle.time;
    mtfc_flag_system.is_time_form_update = true;
    // mtfc_debug_print_time_form(&mtfc_schedule_buffer.time);
    break;
  }
  case 55:
  {
    aqw = main_rs232_com.read8();
    debug(MAIN_DEBUG, "\r\n%s: %d", "Truoc khi read chien luoc: ", mtfc_working_current_base.index_send_cycle_form);
    break;
  }
  case MSP_APP_CYCLE_FORM:
  {
    main_rs232_com.readstruct((uint8_t *)&cycle, sizeof(cycle_t));
    for (int i = 0; i < 10; i++)
    {
      if (i < 8)
      {
        mtfc_schedule_buffer.cycle.index[0].side[i].t_green = cycle.cycle_1[i];
        mtfc_schedule_buffer.cycle.index[1].side[i].t_green = cycle.cycle_2[i];
        mtfc_schedule_buffer.cycle.index[2].side[i].t_green = cycle.cycle_3[i];
        mtfc_schedule_buffer.cycle.index[3].side[i].t_green = cycle.cycle_4[i];
        mtfc_schedule_buffer.cycle.index[4].side[i].t_green = cycle.cycle_5[i];

        mtfc_schedule_buffer.cycle.index[0].side[i].t_yellow = cycle.cycle_1[8];
        mtfc_schedule_buffer.cycle.index[1].side[i].t_yellow = cycle.cycle_2[8];
        mtfc_schedule_buffer.cycle.index[2].side[i].t_yellow = cycle.cycle_3[8];
        mtfc_schedule_buffer.cycle.index[3].side[i].t_yellow = cycle.cycle_4[8];
        mtfc_schedule_buffer.cycle.index[4].side[i].t_yellow = cycle.cycle_5[8];
      }
      if (i == 9)
      {
        mtfc_schedule_buffer.cycle.index[0].clearance_time_crossroads = cycle.cycle_1[9];
        mtfc_schedule_buffer.cycle.index[1].clearance_time_crossroads = cycle.cycle_2[9];
        mtfc_schedule_buffer.cycle.index[2].clearance_time_crossroads = cycle.cycle_3[9];
        mtfc_schedule_buffer.cycle.index[3].clearance_time_crossroads = cycle.cycle_4[9];
        mtfc_schedule_buffer.cycle.index[4].clearance_time_crossroads = cycle.cycle_5[9];
      }
    }
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.num_side:%d", mtfc_schedule.num_side);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].num_side_used:%d", mtfc_schedule_buffer.cycle.index[0].num_side_used);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[1].num_side_used:%d", mtfc_schedule_buffer.cycle.index[1].num_side_used);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[0].side[0].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[0].side[0].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[0].side[1].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[0].side[1].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[0].side[2].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[0].side[2].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[0].side[3].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[0].side[3].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[1].side[0].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[1].side[0].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[1].side[1].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[1].side[1].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[1].side[2].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[1].side[2].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[1].side[3].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[1].side[3].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[2].side[0].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[2].side[0].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[2].side[1].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[2].side[1].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[2].side[2].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[2].side[2].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[2].side[3].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[2].side[3].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[3].side[0].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[3].side[0].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[3].side[1].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[3].side[1].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[3].side[2].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[3].side[2].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[3].side[3].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[3].side[3].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[4].side[0].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[4].side[0].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[4].side[1].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[4].side[1].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[4].side[2].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[4].side[2].t_yellow);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_green:%d", mtfc_schedule_buffer.cycle.index[4].side[3].t_green);
    debug(DEBUG_CYCLE, "\r\nmtfc_schedule.cycle.index[0].side[0].t_yellow:%d", mtfc_schedule_buffer.cycle.index[4].side[3].t_yellow);

    mtfc_flag_system.is_cycle_form_update = true;

    break;
  }

  case CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE:
    {
      main_rs232_com.readstruct((uint8_t *)&mtfc_schedule_buffer.days, sizeof(type_days_lamp_form_t));
      cycle.day[0] = mtfc_schedule_buffer.days.index[0];
      cycle.day[1] = mtfc_schedule_buffer.days.index[1];
      cycle.day[2] = mtfc_schedule_buffer.days.index[2];
      cycle.day[3] = mtfc_schedule_buffer.days.index[3];
      cycle.day[4] = mtfc_schedule_buffer.days.index[4];
      cycle.day[5] = mtfc_schedule_buffer.days.index[5];
      cycle.day[6] = mtfc_schedule_buffer.days.index[6];
      debug(DEBUG_CYCLE, "\r\ncycle.day[0]:%d", cycle.day[0]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[1]:%d", cycle.day[1]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[2]:%d", cycle.day[2]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[3]:%d", cycle.day[3]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[4]:%d", cycle.day[4]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[5]:%d", cycle.day[5]);
      debug(DEBUG_CYCLE, "\r\ncycle.day[6]:%d", cycle.day[6]);

      mtfc_flag_system.is_day_form_update = true;
      break;
    }

  case CMD_CPU_TO_MASTER_TIME_SETTING:
    {
      main_rs232_com.readstruct((uint8_t *)&time_setting, sizeof(time_setting_t));
      mtfc_flag_system.time_setting_config = true;
      break;
    }

  case CMD_CPU_TO_MASTER_START:
  {
    mtfc_flag_system.start = true;
    break;
  }

  case 69:
  {
    checkphase = main_rs232_com.read8();
    if (checkphase)
      mtfc_flag_system.manual = true;
    else
      mtfc_flag_system.manual = false;
    break;
  }
  case 70:
  {
    checkmode = main_rs232_com.read8();
    if (checkmode)
      mtfc_flag_system.checkmode = true;
    else
      mtfc_flag_system.checkmode = false;
    break;
  }
  default:
    break;
  }
}
#pragma endregion

#pragma region XU LY CAC CHE DO HOAT DONG CUA HE THONG DEN-------------------------------

void mtfc_timeout_railway_delay_on_callback(void)
{
  mtfc_railway_work_base.is_railway_event_on_handle = true;
}

void mtfc_timeout_railway_delay_off_event_callback(void)
{
  mtfc_railway_work_base.is_railway_event_off_handle = true;
}

uint8_t mtfc_read_signal_control(type_mtfc_cycle_working_package_t *mtfc_device_state, type_mtfc_config_t *config)
{
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>CONNECT TO CLOUD && APP
  if ((mtfc_device_state->is_device_online == mtfc_online) && (mtfc_device_state->is_device_conneted_soft == mtfc_connect))
  {
    if (config->locked_hard_control_enable) // Khoa phan cung khong doc tin hieu tu switch tru tin hieu duong sat
    {
      if (check_in_main_com.read_on_app()) // APP DANG DIEU KHIEN ON
      {

        mtfc_device_state->index_side_manual_select = check_in_main_com.read_phase_app();
        if ((mtfc_device_state->index_side_manual_select >= 1) && (mtfc_device_state->index_side_manual_select <= 4))
          return mtfc_state_manual;
        else
          return mtfc_state_auto;
      }
      else
      {
        return mtfc_state_off;
      }
    }
    else
    {
      // Tam thoi return trang thai off sau nay se update them
      return mtfc_state_off;
    }
  }
  else //->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>OFFLINE MODE
  {
    // debug(MAIN_DEBUG, "\r\n%s","CONNECT TO APP");
    if (check_in_main_com.read_on_switch())
    {
      if (mtfc_flag_system.manual)
      {
        mtfc_device_state->index_side_manual_select = checkphase;
        if (mtfc_device_state->index_side_manual_select != 0)
          debug(MAIN_DEBUG, "\r\nAPP->index_side_manual_select %d", mtfc_device_state->index_side_manual_select);
        if ((mtfc_device_state->index_side_manual_select >= 1) && (mtfc_device_state->index_side_manual_select <= 4))
          return mtfc_state_manual;
        else
          return mtfc_state_auto;
      }
      else if (mtfc_flag_system.checkmode)
      {
        return mtfc_state_off;
      }
      else
      {
        mtfc_device_state->index_side_manual_select = check_in_main_com.read_phase_switch();
        if (mtfc_device_state->index_side_manual_select != 0)
          debug(MAIN_DEBUG, "\r\nmtfc_device_state->index_side_manual_select %d", mtfc_device_state->index_side_manual_select);
        if ((mtfc_device_state->index_side_manual_select >= 1) && (mtfc_device_state->index_side_manual_select <= 4))
          return mtfc_state_manual;
        else
          return mtfc_state_auto;
      }
    }

    else
    {
      return mtfc_state_off;
    }
  }
}

void mtfc_railway_processing(type_mtfc_cycle_working_package_t *mtfc_wk_base, type_mtfc_config_t *mtfc_config, type_mtfc_railway_work_base_t *mtfc_railway)
{
  static uint32_t counter_signal_raiway = millis();
  if ((mtfc_wk_base->mtfc_working_state != mtfc_state_off) && (mtfc_config->railway_mode_enable))
  {
    if (check_in_main_com.is_change_railway_switch())
    {
      debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway input hardware changed to:");
      if (check_in_main_com.get_railway_switch())
      {
        debug(DEBUG_RAILWAY_SIGNAL, "%s", " 1");
        if (mtfc_config->railway_delay_on_time)
        {
          mtfc_railway_delay_on.attach(&mtfc_timeout_railway_delay_on_callback, mtfc_config->railway_delay_on_time);
          debug(DEBUG_RAILWAY_SIGNAL, "\r\nRailway signal will on %03d(s)", mtfc_config->railway_delay_on_time);
        }
        else
        {
          mtfc_wk_base->railway_signal = 1;
          debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway signal on now");
        }
      }
      else
      {
        debug(DEBUG_RAILWAY_SIGNAL, "%s", " 0");
        if (mtfc_railway->signal_on_counter >= TIME_NOISE_RAILWAY_SIGNAL)
        {
          if (mtfc_config->railway_delay_off_time)
          {
            mtfc_railway_delay_off.attach(&mtfc_timeout_railway_delay_off_event_callback, mtfc_config->railway_delay_off_time);
            debug(DEBUG_RAILWAY_SIGNAL, "\r\nRailway signal will off %03d(s)", mtfc_config->railway_delay_off_time);
          }
          else
          {
            mtfc_wk_base->railway_signal = 0;
            mtfc_railway->signal_on_counter = 0;
            debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway signal off now");
          }
        }
        else
        {
          mtfc_railway_delay_on.detach();
          mtfc_railway_delay_off.detach();
          mtfc_wk_base->railway_signal = 0;
          mtfc_railway->signal_on_counter = 0;
          debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway signal noise");
        }
      }
    }
    if (check_in_main_com.get_railway_switch())
    {
      if ((millis() - counter_signal_raiway) >= 1000)
      {
        (*mtfc_railway).signal_on_counter += 1;
        counter_signal_raiway = millis();
      }
    }
    if (mtfc_railway->is_railway_event_on_handle)
    {
      mtfc_wk_base->railway_signal = 1;
      mtfc_railway->is_railway_event_on_handle = false;
      debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway signal event on");
    }
    else if (mtfc_railway->is_railway_event_off_handle)
    {
      mtfc_wk_base->railway_signal = 0;
      mtfc_railway->signal_on_counter = 0;
      mtfc_railway->is_railway_event_off_handle = false;
      debug(DEBUG_RAILWAY_SIGNAL, "\r\n%s", "Railway signal event off");
    }
  }
  else
  {
    if (mtfc_wk_base->railway_signal == 1)
    {
      mtfc_wk_base->railway_signal = 0;
      mtfc_railway_delay_on.detach();
      mtfc_railway_delay_off.detach();
      mtfc_railway->signal_on_counter = 0;
      mtfc_railway->is_railway_event_on_handle = false;
      mtfc_railway->is_railway_event_off_handle = false;
    }
  }
}

void mtfc_normal_cpu_processing(void)
{
  static uint32_t mtfc_ticker_normal_cpu = millis();
  // DOC CHE DO HOAT DONG
  mtfc_working_current_base.mtfc_working_state = mtfc_read_signal_control(&mtfc_working_current_base, &mtfc_config);
  // TIN HIEU DUONG SAT
  mtfc_railway_processing(&mtfc_working_current_base, &mtfc_config, &mtfc_railway_work_base);
  // CAC CHE DO HOAT DONG
  if (mtfc_working_buffer_update_qt.mtfc_working_state != mtfc_working_current_base.mtfc_working_state)
  {
    mtfc_working_buffer_update_qt.mtfc_working_state = mtfc_working_current_base.mtfc_working_state;
    main_rs232_com.send_byte(CMD_MASTER_TO_CPU_MODE_ACTIVE, mtfc_working_buffer_update_qt.mtfc_working_state);
    if (mtfc_working_buffer_update_qt.mtfc_working_state == 2)
      main_rs232_com.send_byte(CMD_MASTER_TO_CPU_SELECT_PHASE, mtfc_working_current_base.index_side_manual_select);
  }

  if (mtfc_working_current_base.mtfc_working_state == mtfc_state_off) //<<<<<<<<<<<<<<<<<OFF MODE>>>>>>>
  {
    if (mtfc_config.is_flashing) // che do chop vang khi tat
    {
      mtfc_yellow_lamp_flashing_processing(&mtfc_working_current_base);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "off mode is flashing");
    }
    else // if (checkmode) // che do chop vang khi tat
    {
      mtfc_yellow_lamp_flashing_processing(&mtfc_working_current_base);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "off mode is flashing");
    }
    // else // off tat ca cac ngo ra
    // {
    //   mtfc_off_lamp_processing(&mtfc_working_current_base);
    //   debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "off mode is off all");
    // }
  }
  else if (mtfc_working_current_base.mtfc_working_state == mtfc_state_auto) //<<<<<<AUTO MODE>>>>>>
  {
    if ((millis() - mtfc_ticker_normal_cpu) >= 1000) // Dinh ky 1s vao search
    {
      uint8_t time_form_apply = 255;
      type_one_timeline_t time_line_apply;
      uint8_t index_time_line_app = 255;
      time_line_apply.t_begin_apply.hour = 255;
      time_line_apply.t_begin_apply.minute = 255;
      time_line_apply.index_cycle_form = 255;
      mtfc_working_current_base.is_active_time = mtfc_check_active_time(mtfc_hm_present, mtfc_schedule.active_time.tbegin, mtfc_schedule.active_time.tend);
      if (mtfc_working_current_base.is_active_time)
      {
        time_form_apply = mtfc_check_day_form(&mtfc_schedule.days, mtfc_rtc_val.day);
        if (time_form_apply < MAX_TIME_FORM)
        {
          index_time_line_app = mtfc_check_time_form(&mtfc_schedule.time, time_form_apply, mtfc_hm_present, &time_line_apply);
        }
      }
      debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "-------------------------------------------");
      debug(DEBUG_SEARCH_SCHEDULER, "\r\nActive_time_apply: %s", mtfc_working_current_base.is_active_time ? "True" : "false");
      debug(DEBUG_SEARCH_SCHEDULER, "\r\nTime_form_index: %d", time_form_apply);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\nTime_line: %d %d %d", time_line_apply.index_cycle_form, time_line_apply.t_begin_apply.hour, time_line_apply.t_begin_apply.minute);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\nTime_line_index: %d", index_time_line_app);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "-------------------------------------------");
      mtfc_working_current_base.idx_time_form = time_form_apply;
      mtfc_working_current_base.idx_timeline = index_time_line_app;
      mtfc_working_current_base.idx_cycle_form = time_line_apply.index_cycle_form;
      time_current.time_form = mtfc_working_current_base.idx_time_form;
      time_current.time_line = mtfc_working_current_base.idx_timeline;
      time_current.time_index = mtfc_working_current_base.idx_cycle_form;
      time_current.time_begin_hour_apply = time_line_apply.t_begin_apply.hour;
      time_current.time_begin_minute_apply = time_line_apply.t_begin_apply.minute;
      main_rs232_com.send_struct(CMD_MASTER_TO_CPU_CURRENT, (uint8_t *)&time_current, sizeof(time_current_t));
      mtfc_ticker_normal_cpu = millis();
      // wait(5);
    }
    if (mtfc_auto_lamp_countdown_processing(&mtfc_schedule, &mtfc_working_current_base, &mtfc_working_buffer_update_base, &mtfc_working_current_base.t_seek, mtfc_working_current_base.is_active_time, mtfc_working_current_base.idx_cycle_form) != 0)
    {
      // debug(MAIN_DEBUG, "\r\n%s", "mtfc_auto_lamp_countdown_processing = -1");
      mtfc_yellow_lamp_flashing_processing(&mtfc_working_current_base);
      debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "Auto search null");
    }
    // debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "Auto");
  }
  else if (mtfc_working_current_base.mtfc_working_state == mtfc_state_manual)
  {
    mftc_transfer_lamp_side(&mtfc_working_current_base, mtfc_working_current_base.index_side_manual_select);
    debug(DEBUG_SEARCH_SCHEDULER, "\r\n%s", "Manual");
  }
}

uint8_t mtfc_startup_cpu_processing(void)
{
  static uint32_t mtfc_time_flashing_startup = millis();
  if (millis() < (STARTUP_TIME_FLASH + mtfc_time_flashing_startup))
  {
    volatile bool on_signal = check_in_main_com.read_on_switch();
    if (on_signal || mtfc_config.is_flashing)
    {
      int8_t res = mtfc_yellow_lamp_flashing_processing(&mtfc_working_current_base);
      mtfc_working_current_base.is_mtfc_ready = 0;
      return mtfc_startup_state;
    }
    else
    {
      mtfc_working_current_base.is_mtfc_ready = 1;
      return mtfc_normal_state;
    }
  }
  else
  {
    debug(MAIN_DEBUG, "\r\n%s", "mtfc finish startup");
    mtfc_working_current_base.is_mtfc_ready = 1;
    return mtfc_normal_state;
  }
}

#pragma endregion

#pragma region HAM XU LY TIN HIEU DEN----------------------------------------------------

void mtfc_debug_output(uint8_t *signal, uint8_t *mapping, uint8_t *countdown, uint8_t num_side, uint8_t is_green)
{
  for (uint8_t i = 0; i < num_side; i++)
  {
    debug(OUTPUT_DEBUG, "\r\nside [%02d]: %02X -%02X - %02d", (i + 1), (*signal), (*mapping), (*countdown));
    signal++;
    mapping++;
    countdown++;
  }
  debug(OUTPUT_DEBUG, "\r\ncurrent phase is green: %d", is_green);
}

uint8_t mtfc_output_signal_position_hardware_convert(uint8_t signal, uint8_t idx)
{
  uint8_t temp = signal;
  return temp;
}

uint8_t mtfc_set_side_to_red(void)
{
  uint8_t temp = 0;
  bit_set(temp, pin_red);
  bit_set(temp, pin_walking_green);
  return temp;
}

uint8_t mtfc_output_signal_phase_dependent(uint8_t signal, uint8_t idx)
{
  uint8_t temp = signal;
  return temp;
}

void mtfc_output_signal_data_to_card(type_mtfc_cycle_working_package_t *mtfc, uint8_t *signal, uint8_t num_side)
{
  //(1)Chuyen doi chinh du lieu ra port chinh---------------------------------------------------
  for (uint8_t i = 0; i < num_side; i++)
  {
    mtfc->mapping[i] = *signal;
    mtfc_card_data_out.ouput[i] = mtfc_output_signal_position_hardware_convert(*signal, i);
    mtfc_card_data_out.mapping[i] = mtfc_card_data_out.ouput[i];
    signal++;
  }
  //(2)xu ly khi co tin hieu duong sat ket noi-------------------------------------------------
  if (mtfc->railway_signal)
  {
    //(2'')Chuyen doi chinh du lieu ra port anh xa
    for (uint8_t i = 0; i < num_side; i++)
    {
      if (mtfc->railway_connect_phase_config[i] != 0)
      {
        // Chuyen doi truoc khi dich cai dat
        mtfc_card_data_out.mapping[i] = mtfc_set_side_to_red();
        //
        mtfc->mapping[i] = mtfc_card_data_out.mapping[i];
        // Chuyen doi khi cai dat
        mtfc_card_data_out.mapping[i] = mtfc_output_signal_position_hardware_convert(mtfc_card_data_out.mapping[i], i);
        debug(MAIN_DEBUG, "\r\n%s", "Railway!");
      }
    }
  }
  // //(3)xoa du lieu cac phase khong su dung---------------------------------------------------
  // for (uint8_t i = num_side; i < MAX_SIDE; i++)
  // {
  //   mtfc_card_data_out.ouput[i] = 0;
  //   mtfc_card_data_out.mapping[i] = 0;
  // }
  //(4)xuat du lieu ra card------------------------------------------------------------------
  mtfc_card_data_out.pair_signal = ~mtfc_card_data_out.pair_signal;
  mtfc_card_data_out.rtc_time.hour = mtfc_rtc_val.hour;
  mtfc_card_data_out.rtc_time.minute = mtfc_rtc_val.minute;
  mtfc_card_data_out.rtc_time.seconds = mtfc_rtc_val.seconds;

  main_rs232_com.send_struct(MSP_CARD_WRITE_DATA, (uint8_t *)&mtfc_card_data_out, sizeof(type_mtfc_card_data_t));
  // debug(MAIN_DEBUG, "%d:%d:%d\r\n", mtfc_card_data_out.rtc_time.hour, mtfc_card_data_out.rtc_time.minute, mtfc_card_data_out.rtc_time.seconds);
  rf_com.send_struct(MSP_CARD_WRITE_DATA, (uint8_t *)&mtfc_card_data_out, sizeof(type_mtfc_card_data_t));
  check_card_com.send_struct(MSP_CARD_WRITE_DATA, (uint8_t *)&mtfc_card_data_out, sizeof(type_mtfc_card_data_t));

  if (mtfc_config.is_enabale_out485_lamp_port == true)
  {
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      mtfc_lamp485.signal[i] = mtfc->signal[i];
      mtfc_lamp485.mapping[i] = mtfc->mapping[i];
      mtfc_lamp485.countdown[i] = mtfc->countdown[i];
    }
    mtfc_lamp485.railway_signal = mtfc_config.railway_mode_enable ? mtfc->railway_signal : 0;
    mtfc_lamp485.walking_signal = mtfc_config.walking_mode_enable ? mtfc->walking_signal : 0;
    mtfc_lamp_com.send_struct(MSP_CARD_WRITE_DATA, (uint8_t *)&mtfc_lamp485, sizeof(type_mtfc_lamp485_t));
  }
}

int count_tmp;

int8_t mtfc_seek_lamp_signal_convert(type_mtfc_cycle_working_package_t *mtfc)
{
  uint8_t mtfc_walking_red_cus = 0;
  debug(DEBUG_SEEK_COUNTER, "\r\nconvert output t_seek - t: %03d - %03d", mtfc->t_seek, mtfc->t);

  // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->num_side: %d", mtfc->num_side);
  time_current.t = mtfc->t - 1;
  time_current.num_side = mtfc->num_side;
  // PHAN SIGNAL---------------------------------------------
  for (uint8_t i = 0; i < mtfc->num_side; i++)
  {
#pragma region XU LY PHAN SIGNAL XVD
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_start_green: %d", i, mtfc->side[i].t_start_green);
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_end_green: %d", i, mtfc->side[i].t_end_green);
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_start_yellow: %d", i, mtfc->side[i].t_start_yellow);
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_end_yellow: %d", i, mtfc->side[i].t_end_yellow);

    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_green: %d", i, mtfc->side[i].t_green);
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_red: %d", i, mtfc->side[i].t_red);
    // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_yellow: %d", i, mtfc->side[i].t_yellow);
    time_current.yellow = mtfc->side[0].t_yellow;
    if (mtfc_config.index_card[0].is_dependent_phase)
    {
      time_current.red_1 = mtfc->side[0].t_red + mtfc_config.dependent_phase_time;
      time_current.green_1 = mtfc->side[0].t_green - mtfc_config.dependent_phase_time;
    }
    else
    {
      time_current.red_1 = mtfc->side[0].t_red;
      time_current.green_1 = mtfc->side[0].t_green;
    }
    // if(mtfc_config.index_card[1].is_dependent_phase)
    // {
    //   time_current.red_2 = mtfc->side[1].t_red+mtfc_config.dependent_phase_time;
    //   time_current.green_2 = mtfc->side[1].t_green-mtfc_config.dependent_phase_time;
    // }
    // else
    {
      time_current.red_2 = mtfc->side[1].t_red;
      time_current.green_2 = mtfc->side[1].t_green;
    }
    if (mtfc_config.index_card[2].is_dependent_phase)
    {
      time_current.red_3 = mtfc->side[2].t_red + mtfc_config.dependent_phase_time;
      time_current.green_3 = mtfc->side[2].t_green - mtfc_config.dependent_phase_time;
    }
    else
    {
      time_current.red_3 = mtfc->side[2].t_red;
      time_current.green_3 = mtfc->side[2].t_green;
    }

    if (mtfc_config.index_card[3].is_dependent_phase)
    {
      time_current.red_4 = mtfc->side[3].t_red + mtfc_config.dependent_phase_time;
      time_current.green_4 = mtfc->side[3].t_green - mtfc_config.dependent_phase_time;
    }
    else
    {
      time_current.red_4 = mtfc->side[3].t_red;
      time_current.green_4 = mtfc->side[3].t_green;
    }

    // XANH - VANG - DO--------------------------------------

    if ((mtfc->t_seek >= mtfc->side[i].t_start_green) && (mtfc->t_seek < mtfc->side[i].t_end_green))
    {
      debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_start_green 2: %d", i, mtfc->side[i].t_start_green);
      debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_end_green 2: %d", i, mtfc->side[i].t_end_green);
      debug(DEBUG_SEEK_COUNTER, "\r\ntime t seek: %d", mtfc->t_seek);
      bit_set(mtfc->signal[i], pin_green);
      bit_clear(mtfc->signal[i], pin_yellow);
      bit_clear(mtfc->signal[i], pin_red);
      bit_clear(mtfc->signal[i], pin_option_1);
      mtfc->current_phase_is_green = i + 1;
    }
    else if ((mtfc->t_seek >= mtfc->side[i].t_start_yellow) && (mtfc->t_seek < (mtfc->side[i].t_end_yellow))) 
    {
      debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_start_yellow 2: %d", i, mtfc->side[i].t_start_yellow);
      bit_set(mtfc->signal[i], pin_yellow);
      bit_clear(mtfc->signal[i], pin_green);
      bit_clear(mtfc->signal[i], pin_red);
      // debug(MAIN_DEBUG, "\r\nmode_cross: %d",mode_cross);
      if (mode_cross == 2)
      {
        // debug(MAIN_DEBUG, "\r\nmode_cross: %d",mode_cross);
        bit_set(mtfc->signal[i], pin_option_1);
      }
      else
        bit_clear(mtfc->signal[i], pin_option_1);
    }
    else
    {
      // debug(MAIN_DEBUG,"\r\n time red: %d, gia tri i: %d" ,mtfc->side[i].t_red, i);
      // debug(DEBUG_SEEK_COUNTER, "\r\nmtfc->side[%d].t_start_red 2", i);
      bit_set(mtfc->signal[i], pin_red);
      bit_clear(mtfc->signal[i], pin_yellow);
      bit_clear(mtfc->signal[i], pin_green);
      bit_set(mtfc->signal[i], pin_option_1);
    }
#pragma endregion

#pragma region XU LY BO XANH DO
    // DEN DI BO XANH----------------------------------------
    if (bit_check(mtfc->signal[i], pin_red))
    {
      if (mtfc->t_seek < mtfc->t_flashing_wg.side[i].t_end_Wk_green)
      {
        if (i == 0)
        {
          if (mtfc->t_seek > mtfc->t_flashing_wg.side[i].t_start_flashing)
            bit_flip(mtfc->signal[i], pin_walking_green);
          else
            bit_set(mtfc->signal[i], pin_walking_green);
        }
        else
        {

          if ((mtfc->t_seek > mtfc->t_flashing_wg.side[i].t_start_flashing) && (mtfc->t_seek < mtfc->side[i].t_end_yellow))
            bit_flip(mtfc->signal[i], pin_walking_green);
          else
            bit_set(mtfc->signal[i], pin_walking_green);
        }
      }
      else
      {
        bit_clear(mtfc->signal[i], pin_walking_green);
        mtfc_walking_red_cus = 1;
      }
    }
    else
    {
      bit_clear(mtfc->signal[i], pin_walking_green);
    }

    // DEN DI BO DO------------------------------------------
    if (mtfc_walking_red_cus)
    {
      if (bit_check(mtfc->signal[i], pin_green) || bit_check(mtfc->signal[i], pin_yellow))
      {
        bit_set(mtfc->signal[i], pin_walking_red);
      }
      else
      {
        bit_clear(mtfc->signal[i], pin_walking_red);
        bit_set(mtfc->signal[i], pin_walking_green);
      }
    }
    else
    {
      if (bit_check(mtfc->signal[i], pin_green) || bit_check(mtfc->signal[i], pin_yellow))
      {
        bit_set(mtfc->signal[i], pin_walking_red);
      }
      else
      {
        bit_clear(mtfc->signal[i], pin_walking_red);
      }
    }
#pragma endregion
  }

#pragma region XU lY COUNTDOWN
  // XU LY PHAN BIEN DEM NGUOC------------------------------------------------------
  for (uint8_t i = 0; i < mtfc->num_side; i++) // 0 1 2
  {
    if (bit_check(mtfc->signal[i], pin_green))
    {
      // COUNTDOWN GREEN------------------------------------------------------------
      mtfc_card_data_out.tm_cycle.countdown[i] = mtfc->countdown[i] = abs((mtfc->t_seek) - (mtfc->side[i].t_end_green));
      debug(DEBUG_MONITOR_COUNTDOWN, "countdown green : %d gia tri i : %d\r\n", mtfc_card_data_out.tm_cycle.countdown[i], i);
    }
    else if (bit_check(mtfc->signal[i], pin_yellow))
    {
      // COUNTDOWN YELLOW-----------------------------------------------------------
      mtfc_card_data_out.tm_cycle.countdown[i] = mtfc->countdown[i] = abs((mtfc->t_seek) - (mtfc->side[i].t_end_yellow));
      debug(DEBUG_MONITOR_COUNTDOWN, "countdown yellow : %d gia tri i : %d\r\n", mtfc_card_data_out.tm_cycle.countdown[i], i);
    }
    else if (bit_check(mtfc->signal[i], pin_red))
    {
      // COUNTDOWN RED--------------------------------------------------------------
      if (mtfc->t_seek < mtfc->side[i].t_end_green)
      {
        mtfc_card_data_out.tm_cycle.countdown[i] = mtfc->countdown[i] = abs(((mtfc->t_seek) - mtfc->side[i].t_start_green));
        debug(DEBUG_MONITOR_COUNTDOWN, "countdown red 3 : %d gia tri i : %d\r\n", mtfc_card_data_out.tm_cycle.countdown[i], i);
      }
      else
      {
        if (i == 0)
        {
          mtfc_card_data_out.tm_cycle.countdown[i] = mtfc->countdown[i] = abs((mtfc->t - mtfc->t_seek));
          debug(DEBUG_MONITOR_COUNTDOWN, "countdown red 1 : %d gia tri i : %d\r\n", mtfc_card_data_out.tm_cycle.countdown[i], i);
        }
        else
        {
          mtfc_card_data_out.tm_cycle.countdown[i] = mtfc->countdown[i] = abs((mtfc->t - mtfc->t_seek)) + mtfc->side[i].t_start_green;
          debug(DEBUG_MONITOR_COUNTDOWN, "countdown red 2 : %d gia tri i : %d\r\n", mtfc_card_data_out.tm_cycle.countdown[i], i);
        }
      }
    }
#if COUNTDOWN_TO_ZERO == 1
    mtfc->countdown[i] = (mtfc->countdown[i] > 0) ? (mtfc->countdown[i] - 1) : mtfc->countdown[i];
#endif
  }
  // debug(MAIN_DEBUG, "%s\r\n", "<<<======================================================================>>>");
  for (int i = 0; i < check_card_num_card_dependent.card_insert_now; i++) // so 4 can thay doi theo so card dang hien hanh
  {
    if (mtfc_card_config_search.index[i].time_delay_dependent_phase != 0 &&
        mtfc_card_config_search.index[i].phase != 0 && mtfc_card_config_search.index[i].is_dependent_phase == 1)
    {
      // debug(MAIN_DEBUG, "time delay phase dependent: %d, %d\r\n", mtfc_card_config_search.index[i].time_delay_dependent_phase, i);
      if (bit_check(mtfc->signal[mtfc_card_config_search.index[i].phase - 1], pin_green))
      {
        if (check_card_num_card_dependent.num_count_phase_dependent[i] < mtfc_card_config_search.index[i].time_delay_dependent_phase) // van con dang o red
        {
          mtfc_card_data_out.tm_cycle.status_lamp_dependent[i] = RED_DEPENDENT;
          if (mtfc->t_seek < mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_end_green + mtfc_card_config_search.index[i].time_delay_dependent_phase)
          {
            mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs((mtfc->t_seek)- (mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_start_green + mtfc_card_config_search.index[i].time_delay_dependent_phase));
          }
          else
          {
            if (i == 0)
            {
              mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs((mtfc->t - mtfc->t_seek) + mtfc_card_config_search.index[i].time_delay_dependent_phase);
            }
            else
            {
              mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs(mtfc->t - mtfc->t_seek) + (mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_start_green + mtfc_card_config_search.index[i].time_delay_dependent_phase);
            }
          }
          
          debug(DEBUG_MONITOR_COUNTDOWN_CARD_INDEPENDENT, "countdown tmp RED 1 : %d, %d\r\n", mtfc_card_data_out.tm_cycle.tm_dependent[i], i);
          check_card_num_card_dependent.num_count_phase_dependent[i]++;
        }
        else
        {
          mtfc_card_data_out.tm_cycle.status_lamp_dependent[i] = GREEN_DEPENDENT;
          mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs(mtfc->t_seek - (mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_end_green));
          debug(DEBUG_MONITOR_COUNTDOWN_CARD_INDEPENDENT, "countdown tmp GREEN : %d\r\n", mtfc_card_data_out.tm_cycle.tm_dependent[i]);
        }
      }
      else if (bit_check(mtfc->signal[mtfc_card_config_search.index[i].phase - 1], pin_yellow))
      {
        mtfc_card_data_out.tm_cycle.status_lamp_dependent[i] = YELLOW_DEPENDENT;
        mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs(mtfc->t_seek - mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_end_yellow);
        debug(DEBUG_MONITOR_COUNTDOWN_CARD_INDEPENDENT, "countdown tmp YELLOW : %d\r\n", mtfc_card_data_out.tm_cycle.tm_dependent[i]);
      }
      else if (bit_check(mtfc->signal[mtfc_card_config_search.index[i].phase - 1], pin_red))
      {
        mtfc_card_data_out.tm_cycle.status_lamp_dependent[i] = RED_DEPENDENT;
        if (mtfc->t_seek < mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_end_green)
        {
          mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs((mtfc->t_seek) - (mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_start_green + mtfc_card_config_search.index[i].time_delay_dependent_phase));
        }
        else
        {
          if (i == 0)
          {
            mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs((mtfc->t - mtfc->t_seek) + mtfc_card_config_search.index[i].time_delay_dependent_phase);
          }
          else
          {
            mtfc_card_data_out.tm_cycle.tm_dependent[i] = abs(mtfc->t - mtfc->t_seek) + (mtfc->side[mtfc_card_config_search.index[i].phase - 1].t_start_green + mtfc_card_config_search.index[i].time_delay_dependent_phase);
          }
        }
        debug(DEBUG_MONITOR_COUNTDOWN_CARD_INDEPENDENT, "countdown tmp RED 2 : %d, %d\r\n", mtfc_card_data_out.tm_cycle.tm_dependent[i], i);
        check_card_num_card_dependent.num_count_phase_dependent[i] = 0;
      }
    }
  }
#pragma endregion

#pragma region XOA CAC BIEN
  // XOA CAC BYTE OUTPUT KHONG DUNG DEN-----------------------
  for (uint8_t i = mtfc->num_side; i < MAX_SIDE; i++)
  {
    mtfc->signal[i] = 0;
    mtfc->countdown[i] = 255;
  }
  // debug(DEBUG_SEEK_COUNTER, "\r\n%s","Done");
#pragma endregion
  return true;
}

int8_t mtfc_cycle_load(type_mtfc_schedule_t *src, type_mtfc_cycle_working_package_t *dat, uint8_t idx)
{
  if (src->cycle.index[idx].period_crossroads == 0)
  {
    return -1;
  }
  dat->t = src->cycle.index[idx].period_crossroads;
  dat->num_side = src->cycle.index[idx].num_side_used;
  dat->t_clearance = src->cycle.index[idx].clearance_time_crossroads;
  for (uint8_t i = 0; i < dat->num_side; i++)
  {
    dat->side[i] = src->cycle.index[idx].side[i];
  }
  return 0;
}

void mtfc_seek_counter_reload(type_mtfc_cycle_working_package_t *dat)
{
  debug(MAIN_DEBUG, "%s", "\r\nmtfc reload t_seek");
  (*dat).t_seek = 0;
}

void mtfc_green_walking(type_mtfc_cycle_working_package_t *mtfc)
{
  for (uint8_t i = 0; i < mtfc->num_side; i++)
  {
    mtfc->t_flashing_wg.side[i].t_end_Wk_green = mtfc->side[i].t_start_green - TIME_OFFSET_OFF_WALKING_GREEN;
    if (mtfc->t_flashing_wg.side[i].t_end_Wk_green < 0)
      mtfc->t_flashing_wg.side[i].t_end_Wk_green = mtfc->t - TIME_OFFSET_OFF_WALKING_GREEN;

    mtfc->t_flashing_wg.side[i].t_start_flashing = mtfc->t_flashing_wg.side[i].t_end_Wk_green - TIME_FLASHING_GREEN_WALKING;
  }
  for (uint8_t i = mtfc->num_side; i < MAX_SIDE; i++)
  {
    mtfc->t_flashing_wg.side[i].t_start_flashing = 0xFFFF;
    mtfc->t_flashing_wg.side[i].t_end_Wk_green = 0xFFFF;
  }
}

uint8_t mtfc_set_side_to_green(type_mtfc_cycle_working_package_t *obj, uint8_t side)
{
  if (side >= 1 && side <= MAX_SIDE)
  {
    for (uint8_t i = 0; i < obj->num_side; i++)
    {
      if (i == (side - 1))
      {
        bit_set(obj->signal[i], pin_green);
        bit_set(obj->signal[i], pin_walking_red);
        bit_clear(obj->signal[i], pin_red);
        bit_clear(obj->signal[i], pin_yellow);
        bit_clear(obj->signal[i], pin_walking_green);
        bit_clear(obj->signal[i], pin_option_1);
      }
      else
      {
        bit_set(obj->signal[i], pin_red);
        bit_set(obj->signal[i], pin_walking_green);
        bit_clear(obj->signal[i], pin_green);
        bit_clear(obj->signal[i], pin_yellow);
        bit_clear(obj->signal[i], pin_walking_red);
        bit_set(obj->signal[i], pin_option_1);
      }
    }
    return side;
  }
  return 0;
}

uint8_t mtfc_set_side_to_yellow(type_mtfc_cycle_working_package_t *obj, uint8_t side)
{
  if (side >= 1 && side <= MAX_SIDE)
  {
    for (uint8_t i = 0; i < obj->num_side; i++)
    {
      if (i == (side - 1))
      {
        bit_set(obj->signal[i], pin_yellow);
        bit_clear(obj->signal[i], pin_green);
        bit_clear(obj->signal[i], pin_red);
        bit_clear(obj->signal[i], pin_walking_red);
        bit_clear(obj->signal[i], pin_walking_green);
      }
      else
      {
        bit_set(obj->signal[i], pin_red);
        bit_set(obj->signal[i], pin_walking_green);
        bit_clear(obj->signal[i], pin_green);
        bit_clear(obj->signal[i], pin_yellow);
        bit_clear(obj->signal[i], pin_walking_red);
      }
    }
  }
  return 0;
}

int8_t mftc_transfer_lamp_side(type_mtfc_cycle_working_package_t *obj, uint8_t side_change)
{
  if (side_change >= 1 && side_change <= 8)
  {
    if (mtfc_flag_system.is_ticker_one_seconds_seek)
    {
      if (obj->current_phase_is_green == 0) // Dang chop vang thi chuyen tuc thi
      {
        obj->current_phase_is_green = mtfc_set_side_to_green(obj, side_change);
      }
      else
      {
        if (obj->current_phase_is_green == side_change) // Pha hien hanh cung la pha duoc chon
        {
          obj->current_phase_is_green = mtfc_set_side_to_green(obj, side_change);
        }
        else // Pha xanh hien hanh khac pha duoc chon.Luc nay co hieu ung chuyen pha
        {
          obj->current_phase_is_green = mtfc_set_side_to_yellow(obj, obj->current_phase_is_green);
        }
      }
      for (uint8_t i = 0; i < MAX_SIDE; i++)
      {
        obj->countdown[i] = 255;
      }
      // mtfc_output_signal_data_to_card(obj->signal, obj->num_side);
      mtfc_output_signal_data_to_card(obj, (uint8_t *)obj->signal, obj->num_side);
      mtfc_debug_output(obj->signal, obj->mapping, obj->countdown, obj->num_side, obj->current_phase_is_green);
      mtfc_flag_system.is_ticker_one_seconds_seek = false;
    }
  }
  return true;
}

int8_t mtfc_yellow_lamp_flashing_processing(type_mtfc_cycle_working_package_t *obj)
{
  // KHAI BAO CAC BIEN TOAN CUC
  static bool mtfc_yellow_signal = false;
  static uint32_t mtfc_time_flashing_yellow_lamp = 0;
  // NEU LAN DAU CHUYEN SANG CHE DO CHOP DEN VANG
  if (obj->current_phase_is_green != 0 || obj->countdown[0] != 255)
  {
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      obj->countdown[i] = 255; // Xoa cac gia tri dem nguoc
    }
    obj->current_phase_is_green = 0;
  }
  // SAU THOI GIAN DUTY_TIME_FLASHING_YELLOW_LAMP THAY DOI TRANG THAI DEN VANG
  if (millis() - mtfc_time_flashing_yellow_lamp > DUTY_TIME_FLASHING_YELLOW_LAMP)
  {
    mtfc_timer_debug.start();
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      obj->signal[i] = 0; // Xoa cac gia tri dem nguoc
    }
    mtfc_yellow_signal = !mtfc_yellow_signal;
    // NEU mtfc_yellow_signal = true thi bat den vang
    if (mtfc_yellow_signal)
    {
      for (uint8_t i = 0; i < obj->num_side; i++)
      {
        bit_set(obj->signal[i], pin_yellow);
      }
    }
    // mtfc_output_signal_data_to_card(obj->signal, obj->num_side);
    mtfc_output_signal_data_to_card(obj, (uint8_t *)obj->signal, obj->num_side);
    mtfc_debug_output(obj->signal, obj->mapping, obj->countdown, obj->num_side, obj->current_phase_is_green);
    mtfc_time_flashing_yellow_lamp = millis();
    debug(EXECUTE_FLASHING_SHOW_TIME_DEBUG, "\r\nmtfc_seek_yellow_flashing_processing in: %d (us)", mtfc_timer_debug.read_us());
    mtfc_timer_debug.stop();
    mtfc_timer_debug.reset();
    return 0;
  }
  return -1;
}

int8_t mtfc_auto_lamp_countdown_processing(type_mtfc_schedule_t *mtfc_schedule, type_mtfc_cycle_working_package_t *mtfc_current_cycle, type_mtfc_cycle_working_package_t *mtfc_new_cycle, uint16_t *t_seek, bool active_time, uint8_t idx)
{
  if (active_time == false) // NGOÀI THỜI GIAN HOẠT ĐỘNG
  {
    // debug(MAIN_DEBUG, "\r\n%s", "No active time");
    return -1;
  }
  if (idx > (MAX_CYCLE_FORM - 1)) // CHỈ SỐ CYCLE FORM KHÔNG HỢP LỆ
  {
    debug(MAIN_DEBUG, "\r\n%s", "No CYCLE_FORM");
    return -1;
  }
  if (mtfc_cycle_load(mtfc_schedule, mtfc_new_cycle, idx) != 0) // LỖI KHI XẢY RA KHI LOAD CYCLE FORM
  {
    debug(MAIN_DEBUG, "\r\n%s", "Mtfc do not load cycle form");
    return -1;
  }
  // debug(MAIN_DEBUG,"\r\nmtfc_new_cycle->t :%d %d %d",mtfc_new_cycle->t,mtfc_new_cycle->num_side,mtfc_new_cycle->side[0].t_green);
  if (mtfc_flag_system.is_ticker_one_seconds_seek) // HOẠT ĐỘNG VỚI CHU KỲ 1S
  {
    mtfc_timer_debug.start();
    if (mtfc_current_cycle->t_seek >= mtfc_current_cycle->t) // KẾT THÚC CHU KỲ ĐẾM NGƯỢC
    {
      debug(DEBUG_SEEK_COUNTER, "\r\nt_seek - mtfc_current_cycle :%d-%d", mtfc_current_cycle->t_seek, mtfc_current_cycle->t);
      if (memcmp(mtfc_current_cycle->side, mtfc_new_cycle->side, (sizeof(type_one_side_lamp_time_t) * MAX_SIDE)) != 0) // SO SÁNH CÓ CẬP NHẬT MỚI KHÔNG
      {
        mtfc_current_cycle->t = mtfc_new_cycle->t;
        mtfc_current_cycle->num_side = mtfc_new_cycle->num_side;
        mtfc_current_cycle->t_clearance = mtfc_new_cycle->t_clearance;
        for (uint8_t i = 0; i < MAX_SIDE; i++)
          mtfc_current_cycle->side[i] = mtfc_new_cycle->side[i];
        mtfc_green_walking(mtfc_current_cycle);
        debug(MAIN_DEBUG, "%s", "\r\nmtfc update new cycle form");
      }
      mtfc_seek_counter_reload(mtfc_current_cycle);
    }
    mtfc_seek_lamp_signal_convert(mtfc_current_cycle);
    // debug(MAIN_DEBUG, "%s: %d", "\r\nseek done",mtfc_current_cycle->t_seek);
    // mtfc_output_signal_data_to_card((uint8_t *)mtfc_current_cycle->signal, mtfc_current_cycle->num_side);
    mtfc_output_signal_data_to_card(mtfc_current_cycle, (uint8_t *)mtfc_current_cycle->signal, mtfc_current_cycle->num_side);
    (*mtfc_current_cycle).t_seek++;
    debug(EXECUTE_AUTO_SHOW_TIME_DEBUG, "\r\nmtfc_auto_lamp_countdown_processing in: %d (us)", mtfc_timer_debug.read_us());
    mtfc_debug_output(mtfc_current_cycle->signal, mtfc_current_cycle->mapping, mtfc_current_cycle->countdown, mtfc_current_cycle->num_side, mtfc_current_cycle->current_phase_is_green);
    mtfc_timer_debug.stop();
    mtfc_timer_debug.reset();
    mtfc_flag_system.is_ticker_one_seconds_seek = false;
  }
  return 0;
}

int8_t mtfc_off_lamp_processing(type_mtfc_cycle_working_package_t *obj)
{
  static uint32_t mtfc_count_time_off = 0;
  if ((millis() - mtfc_count_time_off) >= 1000)
  {
    obj->current_phase_is_green = 0;
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      obj->countdown[i] = 0;
      obj->signal[i] = 0;
    }
    // mtfc_output_signal_data_to_card(obj->signal, obj->num_side);
    mtfc_output_signal_data_to_card(obj, (uint8_t *)obj->signal, obj->num_side);
    mtfc_count_time_off = millis();
    return 0;
  }
}

#pragma endregion

#pragma region CARD KIEM TRA LOI

void mtfc_gateway_message_error_log(const char *format, ...)
{
  char buf[64];
  memset((char *)buf, '\0', 64);
  va_list ap;
  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  rf_com.send_struct(MSP_GATEWAY_MESSAGE_ERROR_LOG, (uint8_t *)&buf, sizeof(buf));
  va_end(ap);
}

void mtfc_search_and_save_sensor(type_sensor_recei_pkg_t *sen, uint8_t dat, uint8_t phase, uint32_t imei)
{
  if ((phase > 0) && (phase <= 8))
  {
    for (uint8_t i = (phase - 1); i < MAX_SIDE; i++)
    {
      if ((sen->mask_data[i] == 0) || (sen->imei[i] == imei))
      {
        sen->sen_data[i] = dat;
        sen->mask_data[i] = 255;
        sen->imei[i] = imei;
        break;
      }
    }
  }
}

int8_t mtfc_check_imei(uint8_t idx, type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config)
{
  if ((mtfc_config->index_card[idx].imei == 0) || (mtfc_check_pkg.phase = mtfc_config->index_card[idx].phase == 0))
  {
    return -1;
  }
  return 0;
}

void mtfc_sensor_detect_processing(type_mtfc_cycle_working_package_t *mtfc_wk, type_mtfc_config_t *mtfc_config)
{
  static unsigned long mtfc_get_card_sensor_timeline = millis();
  if (mtfc_wk->mtfc_main_state != mtfc_normal_state)
  {
    return;
  }
  if (millis() - mtfc_get_card_sensor_timeline >= 1500)
  {
    if (mtfc_wk->idx_check_sensor == 0)
    {
      for (uint8_t i = 0; i < MAX_SIDE; i++)
      {
        mtfc_sen_recei_pkg.mask_data[i] = 0;
      }
    }
    while (mtfc_check_imei(mtfc_wk->idx_check_sensor, mtfc_wk, mtfc_config) != 0)
    {
      (*mtfc_wk).idx_check_sensor++;
    }
    if ((mtfc_wk->idx_check_sensor >= 0) && (mtfc_wk->idx_check_sensor <= 7))
    {
      mtfc_check_pkg.imei = mtfc_config->index_card[mtfc_wk->idx_check_sensor].imei;
      mtfc_check_pkg.phase = mtfc_config->index_card[mtfc_wk->idx_check_sensor].phase;
      check_card_com.send_struct(MSP_CARD_CHECK_SENSOR, (uint8_t *)&mtfc_check_pkg, sizeof(type_check_pkg_t));
      debug(DEBUG_SENSOR_READ_BACKET, "\r\nIdx card check:[%02d]", mtfc_wk->idx_check_sensor);
    }
    (*mtfc_wk).idx_check_sensor++;
    if (mtfc_wk->idx_check_sensor > 7)
    {
      mtfc_wk->idx_check_sensor = 0;
    }
    mtfc_get_card_sensor_timeline = millis();
  }
  if (mtfc_flag_system.is_sensor_update)
  {
    mtfc_flag_system.is_sensor_update = false;
    mtfc_search_and_save_sensor(&mtfc_sen_recei_pkg, mtfc_sen_pkg.mask_output, mtfc_sen_pkg.phase, mtfc_sen_pkg.imei);
    debug(DEBUG_SENSOR_READ_BACKET, "\r\n%s", "Sen array:");
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
      mtfc_wk->sensor[i] = mtfc_sen_recei_pkg.sen_data[i];
      debug(DEBUG_SENSOR_READ_BACKET, " [%02X]", mtfc_wk->sensor[i]);
    }
    debug(DEBUG_SENSOR_READ_BACKET, "%s", "\r\n");
    if (strstr((char *)&mtfc_sen_pkg.sms, "Error"))
    {
      if (mtfc_wk->is_device_online == mtfc_online)
      {
        mtfc_gateway_message_error_log("%s-->Card Phase [%02d]", mtfc_sen_pkg.sms, mtfc_sen_pkg.phase);
      }
      debug(MAIN_DEBUG, "\r\nCard read back:%s --%02X--Phase[%02d]", mtfc_sen_pkg.sms, mtfc_sen_pkg.mask_output, mtfc_sen_pkg.phase);
    }
  }
}

#pragma endregion

#pragma region HAM NGAT CHU KY ONE SECONDS-----------------------------------------------
void mtfc_one_seconds_ticker_event_handler()
{
  mtfc_flag_system.is_ticker_one_seconds_seek = (mtfc_flag_system.is_ticker_one_seconds_seek == false) ? true : mtfc_flag_system.is_ticker_one_seconds_seek;
  // mtfc_flag_system.is_ticker_one_seconds_gateway = (mtfc_flag_system.is_ticker_one_seconds_gateway == false) ? true : mtfc_flag_system.is_ticker_one_seconds_seek;
  mtfc_flag_system.is_ticker_one_seconds_main = (mtfc_flag_system.is_ticker_one_seconds_main == false) ? true : mtfc_flag_system.is_ticker_one_seconds_main;
  mtfc_flag_system.is_ticker_one_seconds_glcd = (mtfc_flag_system.is_ticker_one_seconds_glcd == false) ? true : mtfc_flag_system.is_ticker_one_seconds_glcd;
}
#pragma endregion

#pragma region TEST CAC CHE DO XUAT TIN HIEU den-----------------------------------------
void mtfc_test_state_active(void)
{
  uint8_t enable_process_idx = 1;
  uint8_t side = 0;
  if (enable_process_idx == 1)
  {
    mtfc_auto_lamp_countdown_processing(&mtfc_schedule, &mtfc_working_current_base, &mtfc_working_buffer_update_base, &mtfc_working_current_base.t_seek, true, 0);
  }
  else if (enable_process_idx == 2)
  {
    int8_t res = mtfc_yellow_lamp_flashing_processing(&mtfc_working_current_base);
  }
  else if (mtfc_flag_system.is_ticker_one_seconds_seek && (enable_process_idx == 3))
  {
    debug(MAIN_DEBUG, "\r\n%s", "mtfc is manual mode");
    mftc_transfer_lamp_side(&mtfc_working_current_base, side);
    mtfc_flag_system.is_ticker_one_seconds_seek = false;
  }
  /*   if (check_in_main_com.is_input_phase_hardware_changed())
    {
      side = check_in_main_com.get_input_phase_hardware();
      debug(MAIN_DEBUG, "\r\nmtfc state controller phase changed: %d", side);
    } */
}
#pragma endregion

#pragma region SU LY NHAN DU LIEU TU CONG COM debug
void mtfc_debug_console_part_data(uint8_t *str, uint8_t size)
{
  if (strstr((char *)str, "now")) // xuat man hinh chien luoc hoat donng
  {
    time_t now = mtfc_rtc.epoch();
    debug(DEBUG_CONSOLE, "\r\nTime as a basic string = %s", ctime(&now));
  }
  else if (strstr((char *)str, "rtc_set"))
  {
    char *temp = strtok((char *)str, ",");
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.hour = (uint8_t)atoi(temp);
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.minute = (uint8_t)atoi(temp);
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.seconds = (uint8_t)atoi(temp);
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.date = (uint8_t)atoi(temp);
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.month = (uint8_t)atoi(temp);
    temp = strtok(NULL, ",");
    mtfc_rtc_new_update.year = (uint16_t)atoi(temp);
    mtfc_flag_system.is_update_rtc = true;
  }
  else if (strstr((char *)str, "ping_com_gateway")) // test com gateway
  {
    rf_com.printf("\r\nGateway com port hello!!!");
  }
  else if (strstr((char *)str, "send_schedule")) // Gui chien luoc (1)
  {
    mtfc_flag_system.is_update_schedule_to_gateway = true;
  }
  else if (strstr((char *)str, "send_config")) // gui cai dat
  {
    mtfc_flag_system.is_update_mtfc_config_to_gateway = true; //(2)
  }
  else if (strstr((char *)str, "send_coordinate")) // Gui toa do//(3)
  {
    mtfc_flag_system.is_update_mtfc_coordinate_to_gateway = true;
  }
  else if (strstr((char *)str, "send_details")) // Gui thong tin co ban cua cpu//(4)
  {
    mtfc_flag_system.is_update_mtfc_details_to_gateway = true;
  }
  else if (strstr((char *)str, "send_cpu_type")) // Gui loai CPU(5)
  {
    mtfc_flag_system.is_update_mtfc_type_cpu_to_gateway = true;
  }
  else if (strstr((char *)str, "mtfc_online"))
  {
    sprintf(mtfc_gate_consol_message, "%s", "cloud_connect");
    mtfc_flag_system.is_cloud_connected = true;
  }
  else if (strstr((char *)str, "mtfc_offline"))
  {
    sprintf(mtfc_gate_consol_message, "%s", "cloud_disconnect");
    mtfc_flag_system.is_cloud_connected = true;
  }
  else if (strstr((char *)str, "mtfc_connect_soft"))
  {
    sprintf(mtfc_gate_consol_message, "%s", "app_connect");
    mtfc_flag_system.is_soft_connected = true;
  }
  else if (strstr((char *)str, "mtfc_disconnect_soft"))
  {
    sprintf(mtfc_gate_consol_message, "%s", "app_disconnect");
    mtfc_flag_system.is_soft_connected = true;
  }
  else if (strstr((char *)str, "mtfc_on"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_on");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_off"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_off");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_t1"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_select_t1");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_t2"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_select_t2");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_t3"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_select_t3");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_t4"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_select_t4");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_free"))
  {
    sprintf(mtfc_gate_control_message, "%s", "mtfc_select_free");
    mtfc_flag_system.is_app_control = true;
  }
  else if (strstr((char *)str, "mtfc_reset"))
  {
    mtfc_flag_system.is_mtfc_reset_system = true;
  }
  else if (strstr((char *)str, "test_watchdog"))
  {
    debug(MAIN_DEBUG, "\r\n%s", "Watchdog execution....");
    while (true)
      ;
  }
  memset((char *)str, '\0', 255);
}
#pragma endregion