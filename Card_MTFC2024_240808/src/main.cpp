#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <bsp_config.h>
#include <mspSerial.h>
#include <IWatchdog.h>

#include "flash.h"

#define setPin(n) digitalWrite(n, HIGH)
#define clearPin(n) digitalWrite(n, LOW)

/**
 * @brief Khai bao OOP
 * 
 */
HardwareSerial debugComPort(DEBUG_RX_PIN, DEBUG_TX_PIN);
HardwareSerial cardComPort(CARD_RX_PIN, CARD_TX_PIN);
mspSerial cardCom(&cardComPort, 115200);

#define debug(condition, fmt, ...)           \
  do                                         \
  {                                          \
    if (condition)                           \
      debugComPort.printf(fmt, __VA_ARGS__); \
  } while (0)

//------------------------------------------------------------------------------------

#pragma region KHAI BAO HAM MACRO
/**
 * @brief Khai bao macro ham
 * 
 */

/**
 * @brief Ham ghi bo nho
 * 
 * @param pos vi tri 
 * @param dat du lieu
 * @param siz kich thuoc
 */
void mtfc_read_eeprom_api(uint8_t pos, uint8_t *dat, uint16_t siz);

/**
 * @brief Ham doc bo nho
 * 
 * @param pos vi tri
 * @param dat du lieu
 * @param siz kich thuoc
 */
void mtfc_write_eeprom_api(uint8_t pos, uint8_t *dat, uint16_t siz);

/**
 * @brief Ham dieu khien trang lai led bao
 * 
 */
void mtfc_led_processing(void);

/**
 * @brief Ham doc  cai dat cua thiet bi
 * 
 */
int8_t mtfc_memory_startup_load(void);

/**
 * @brief Chon card vao che do cmd
 * [true] selected
 */
bool mtfc_hardware_select_card(void);

/**
 * @brief Doc che do hoat dong cua CPU
 * 
 */
void mtfc_read_mode_active(void);

/**
 * @brief Ham callback sau khi du lieu cong giao tiep nhan du farme truyen
 * 
 */
void mtfc_card_com_event_handler(void);

/**
 * @brief Doc trang thai ngo ra
 * 
 * @return uint8_t 
 */
uint8_t mtfc_read_sensor(void);

/**
 * @brief xuat du lieu ra phan cung
 * 
 * @param dat vung du lieu
 * @param config cai dat 
 */
void mtfc_output_buffer_to_hardware(type_mtfc_card_data_t *dat, type_one_cardConfig_t *config);

/**
 * @brief Xuat du lieu ra phan cung
 * 
 * @param dat du lieu xuat ra
 */
void mtfc_output_hardware(uint8_t dat);

/**
 * @brief Tat ngo ra card cong suat
 * 
 */
void mtfc_output_off(void);

/**
 * @brief Ham xu lieu tu cpu gui den
 * 
 */
void mtfc_card_data_processing(void);

/**
 * @brief Doc cam bien ngo ra va tim loi
 * 
 */
void mtfc_sensor_detect_processing(type_mtfc_workbase_t *mtfc_wk);

/**
 * @brief Ham timeout ket noi card voi cpu chinh
 * 
 * @return int8_t #0 error
 */
int8_t mtfc_detect_lost_control_signal(void);
/**
 * @brief Ham xuat ngo ra re phai
 */
void mtfc_output_right(uint8_t dat);
/**
 * @brief Khoi tao flash
 * Write, Read flash
 * @param addr dia chi o nho
 * @param buf du lieu
 * @param len do dai du lieu
 */
void settings_storage(void);
void mtfc_write_flash_api(uint32_t addr, const void *buf, uint32_t len);
void mtfc_read_flash_api(uint32_t addr, void *buf, uint32_t len);
//------------------------------------------------------------------------------------
#pragma endregion

#pragma region KHAI BAO CAC BIEN
/**
 * @brief Khai bao cac bien
 * 
 */
//Bien trang lai lam viec cua cpu
cardWorkingState_t cardWorkingState = ACTIVE_STATE;
//Bien luu cau hinh card
type_one_cardConfig_t mtfc_card_config;
//Du lieu tu cpu gui den cai dat card
type_one_cardConfig_t mtfc_card_config_update;
//Bien danh dau card duoc cau hinh hay chua
bool mtfc_card_installed = false;
//Bien luu du lieu dieu khien
type_cardDataInOut_t cardData;
//Bien luu phan hoi phan cung
type_cardDataInOut_t cardDataSensor;
//Bien xu ly chuong trinh
type_sysFlag_t mtfc_sys_flag;
//Bien luu du lieu tu CPU gui xuong
type_mtfc_card_data_t mtfc_card_data_ouput;
//Bien luu qua trinh lam viec
type_mtfc_workbase_t mtfc_wk;

uint32_t mtfc_ticker_time_connection = 0;
volatile uint8_t led_active_state = 0;
type_sensor_pkg_t mtfc_sensor_pkg;
type_check_pkg_t mtfc_check_pkg;
uint32_t flag_red = 0;
time_setting_t time_setting;
//------------------------------------------------------------------------------------
#pragma endregion

#pragma region CAC HAM CHINH
void mtfc_startup_config(void)
{
  //Port output configuration
  pinMode(OUT_GREEN_GATE, OUTPUT);
  pinMode(OUT_YELLOW_GATE, OUTPUT);
  pinMode(OUT_RED_GATE, OUTPUT);
  pinMode(OUT_WALKING_GREEN_GATE, OUTPUT);
  pinMode(OUT_WALKING_RED_GATE, OUTPUT);
  pinMode(OUT_OP1_GATE, OUTPUT);
  pinMode(OUT_OP2_GATE, OUTPUT);
  pinMode(OUT_OP3_GATE, OUTPUT);
  //Port input configuration
  pinMode(FB_GREEN_GATE, INPUT_PULLUP);
  pinMode(FB_YELLOW_GATE, INPUT_PULLUP);
  pinMode(FB_RED_GATE, INPUT_PULLUP);
  pinMode(FB_WALKING_GREEN_GATE, INPUT_PULLUP);
  pinMode(FB_WALKING_RED_GATE, INPUT_PULLUP);
  pinMode(FB_OP1_GATE, INPUT_PULLUP);
  pinMode(FB_OP2_GATE, INPUT_PULLUP);
  pinMode(FB_OP3_GATE, INPUT_PULLUP);
  //Select pin configuration
  pinMode(CS_CARD_GATE, INPUT);
  //Led pin configuration
  pinMode(LED_STATUS_GATE, OUTPUT);
  //Type pin configuration
  pinMode(TYPE_CARD_GATE, INPUT);
  //Tat tat ca output
  mtfc_output_off();
  digitalWrite(OUT_OP1_GATE, LOW);
  debugComPort.begin(115200);
  cardComPort.begin(115200);
  if(digitalRead(TYPE_CARD_GATE) == AC_TYPE_CARD)
  {
    mtfc_wk.card_type = AC_TYPE;
  }else
  {
    mtfc_wk.card_type = DC_TYPE;
  }
  debug(MAIN_DEBUG, "\r\nCard finish config hardware!\r\n[%s]", (mtfc_wk.card_type == AC_TYPE)?"AC TYPE":"DC TYPE");
}

void mtfc_flash_statup_led()
{
  for (uint8_t i = 0; i < 10; i++)
  {
    digitalWrite(LED_STATUS_GATE, !digitalRead(LED_STATUS_GATE));
    delay(50);
  }
}
void setup()
{
  SPI.setMISO(PB14);
  SPI.setMOSI(PB15);
  SPI.setSCLK(PB13);

  mtfc_startup_config();
  
  settings_storage();

  mtfc_flash_statup_led();
  if (mtfc_memory_startup_load() != -1)
  {
    mtfc_card_installed = true;
  }
  cardCom.attachInterupt(mtfc_card_com_event_handler);
  digitalWrite(LED_STATUS_GATE, HIGH);
    debug(MAIN_DEBUG, "\r\nsn: %s\r\nimei: %ld\r\nphase: %d\r\nis railway enable: %d\r\nis dependent enable: %d\r\ntime dephent delay: %d",
        mtfc_card_config.sn,
        mtfc_card_config.imei,
        mtfc_card_config.phase,
        mtfc_card_config.is_railway_enabled,
        mtfc_card_config.is_dependent_phase,
        mtfc_card_config.time_delay_dependent_phase);
  IWatchdog.begin(10000000);
}

/**
 * @brief Chan led trang thai hoat dong
 * [soild]: cmd mode
 * [fast flash 200ms]: no config 
 * [slow flash 1s]: active mode
 */
uint32_t mill_cnt = millis();
uint8_t cnt = 0;
void loop()
{
  mtfc_sensor_detect_processing(&mtfc_wk);
  mtfc_read_mode_active();
  mtfc_led_processing();
  cardCom.isr_event();
  mtfc_card_data_processing();
  if (mtfc_detect_lost_control_signal() != 0)
  {
    mtfc_output_off();
  }
  else
  {
    //Xuat du lieu ra card
    if (mtfc_sys_flag.is_out_data_to_output)
    {
      mtfc_output_buffer_to_hardware(&mtfc_card_data_ouput, &mtfc_card_config);
      mtfc_sys_flag.is_out_data_to_output = false;
      led_active_state = mtfc_card_data_ouput.pair_signal;
    }
  }
  if (flag_red)
    digitalWrite(OUT_RED_GATE, 1);
  IWatchdog.reload();
}

int8_t mtfc_detect_lost_control_signal(void)
{
  static long long mtfc_time_ticker = 0;
  if (millis() - mtfc_time_ticker > 1000)
  {
    mtfc_ticker_time_connection++;
    mtfc_ticker_time_connection = (mtfc_ticker_time_connection > 5) ? 5 : mtfc_ticker_time_connection;
    mtfc_time_ticker = millis();
  }
  if (mtfc_ticker_time_connection > TIME_OUT_CONNECTIONS)
  {
    return -1;
  }
  return 0;
}

#pragma endregion

#pragma region XU LY DETECT LOI NGO ra
void mtfc_sensor_detect_processing(type_mtfc_workbase_t *mtfc_wk)
{
  static uint32_t mill_printf_out = 0;
  if (mtfc_sys_flag.is_enable_process_readSensor)
  {
    if (millis() - mtfc_wk->mill_time_get_sample >= 1000)
    {
      mtfc_sys_flag.is_enable_process_readSensor = false;
      mtfc_wk->sensor_readback = mtfc_read_sensor();

      //Neu tat ca cac ngo ra dieu = 0 nghia la CPU dang tat
      if(mtfc_wk->sensor_readback == 0x00)
      {
        return;
      }
      //Do tim su khac biet giua tin hieu xuat ra va gia tri cam bien doc ve
      if (mtfc_wk->sensor_readback != mtfc_wk->output_data) //<<--------------------Co loi xay ra
      {
        //Check pin
        uint8_t out = mtfc_wk->output_data;
        uint8_t read = mtfc_wk->sensor_readback;
        memset((char *)&mtfc_sensor_pkg.sms, '\0', sizeof(mtfc_sensor_pkg.sms));
        if (read == 0x00)
        {
          debug(ERROR_DEBUG, "\r\n%s", "Power turn off");
          //return;
        }
        mtfc_sys_flag.is_error_output = true;
        strcat((char *)&mtfc_sensor_pkg.sms, "Error");
        //Green gate
        if (bitRead(out, GREEN_BIT))
        {
          if (bitRead(read, GREEN_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Green:dead");
            bitSet(mtfc_sensor_pkg.mask_output, GREEN_BIT);
          }
        }
        else
        {
          if (bitRead(read, GREEN_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Green:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, GREEN_BIT);
          }
        }
        //Yellow gate
        if (bitRead(out, YELLOW_BIT))
        {
          if (bitRead(read, YELLOW_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Yellow:dead");
            bitSet(mtfc_sensor_pkg.mask_output, YELLOW_BIT);
          }
        }
        else
        {
          if (bitRead(read, YELLOW_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Yellow:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, YELLOW_BIT);
          }
        }
        //Red gate
        if (bitRead(out, RED_BIT))
        {
          if (bitRead(read, RED_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Red:dead");
            bitSet(mtfc_sensor_pkg.mask_output, RED_BIT);
          }
        }
        else
        {
          if (bitRead(read, RED_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Red:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, RED_BIT);
          }
        }
        //WG gate
        if (bitRead(out, WALKING_GREEN_BIT))
        {
          if (bitRead(read, WALKING_GREEN_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Walking green:dead");
            bitSet(mtfc_sensor_pkg.mask_output, WALKING_GREEN_BIT);
          }
        }
        else
        {
          if (bitRead(read, WALKING_GREEN_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Walking green:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, WALKING_GREEN_BIT);
          }
        }
        //WR gate
        if (bitRead(out, WALKING_RED_BIT))
        {
          if (bitRead(read, WALKING_RED_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Walking red:dead");
            bitSet(mtfc_sensor_pkg.mask_output, WALKING_RED_BIT);
          }
        }
        else
        {
          if (bitRead(read, WALKING_RED_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Walking red:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, WALKING_RED_BIT);
          }
        }
        //OP1 gate
        if (bitRead(out, OP1_BIT))
        {
          if (bitRead(read, OP1_BIT) == 0)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Option 1:dead");
            bitSet(mtfc_sensor_pkg.mask_output, OP1_BIT);
          }
        }
        else
        {
          if (bitRead(read, OP1_BIT) == 1)
          {
            strcat((char *)&mtfc_sensor_pkg.sms, ",Option 1:short circuit");
            bitSet(mtfc_sensor_pkg.mask_output, OP1_BIT);
          }
        }
      }
      else //<<--------------------Khong co loi
      {
        mtfc_sys_flag.is_error_output = false;
        mtfc_sensor_pkg.mask_output = 0;
        memset((char *)&mtfc_sensor_pkg.sms, '\0', sizeof(mtfc_sensor_pkg.sms));
        strcat((char *)&mtfc_sensor_pkg.sms, "OK");
      }
      mtfc_wk->mill_time_get_sample = millis();
    }
  }
  //Yeu cau lay du lieu detect ngo ra cua card
  if (mtfc_sys_flag.is_request_get_sensor)
  {
    mtfc_sys_flag.is_request_get_sensor = false;
    if (mtfc_check_pkg.imei != mtfc_card_config.imei)
    {
      return;
    }
    if (mtfc_check_pkg.phase != mtfc_card_config.phase)
    {
      return;
    }
    //Xu ly gui du lieu thong bao loi 1 lan
    if (mtfc_sensor_pkg.mask_output == 0)//KHONG CO LOI
    {
     // if (mtfc_wk->error_cash[0])
     //{
        for (uint8_t i = 0; i < 10; i++)
        {
          mtfc_wk->error_cash[i] = 0;
        }
     //s }
      mtfc_sensor_pkg.phase = mtfc_card_config.phase;
      mtfc_sensor_pkg.imei = mtfc_card_config.imei;
      //cardCom.send_struct(MSP_CARD_DATA_SENSOR, (uint8_t *)&mtfc_sensor_pkg, sizeof(type_sensor_pkg_t));
    }
    else//CO LOI NGO RA
    {
      uint8_t error_check_cash = 0;
      for (uint8_t i = 0; i < 10; i++)
      {
        if (mtfc_sensor_pkg.mask_output == mtfc_wk->error_cash[i])
          error_check_cash = 1;
      }
      debug(ERROR_DEBUG, "\r\nerror check cash[%d]", error_check_cash);
      if (error_check_cash) //LOI DA TON TAI
      {
        memset((char *)&mtfc_sensor_pkg.sms, '\0', sizeof(mtfc_sensor_pkg.sms));
        strcat((char *)&mtfc_sensor_pkg.sms, "unchanged");
        mtfc_sensor_pkg.phase = mtfc_card_config.phase;
        mtfc_sensor_pkg.imei = mtfc_card_config.imei;
        //cardCom.send_struct(MSP_CARD_DATA_SENSOR, (uint8_t *)&mtfc_sensor_pkg, sizeof(type_sensor_pkg_t));
      }
      else //LOI MOI PHAT SINH
      {
        for (uint8_t i = 0; i < 10; i++)
        {
          if (mtfc_wk->error_cash[i] == 0)
          {
            mtfc_wk->error_cash[i] = mtfc_sensor_pkg.mask_output;
            break;
          }
        }
        mtfc_sensor_pkg.phase = mtfc_card_config.phase;
        mtfc_sensor_pkg.imei = mtfc_card_config.imei;
        //cardCom.send_struct(MSP_CARD_DATA_SENSOR, (uint8_t *)&mtfc_sensor_pkg, sizeof(type_sensor_pkg_t));
      }
    }
    debug(ERROR_DEBUG, "\r\nms:[%s]-%02X", mtfc_sensor_pkg.sms, mtfc_sensor_pkg.mask_output);
  }
}
#pragma endregion

#pragma region XU LY DU LIEU NHAN DUOC

void mtfc_card_data_processing(void)
{
  //Ghi cau hinh phan cung
  if (mtfc_sys_flag.isWriteConfig)
  {
    if ((cardWorkingState == CMD_STATE) || (mtfc_sys_flag.time_setting))
    {
      mtfc_sys_flag.time_setting = false;
      
      type_one_cardConfig_t temp = mtfc_card_config_update;


        debug(MAIN_DEBUG, "\r\nsn: %s\r\nimei: %ld\r\nphase: %d\r\nis railway enable: %d\r\nis walking enable: %d\r\nis dependent enable: %d\r\ntime dephent delay: %d",
        mtfc_card_config_update.sn,
        mtfc_card_config_update.imei,
        mtfc_card_config_update.phase,
        mtfc_card_config_update.is_railway_enabled,
        mtfc_card_config_update.is_walking_enabled,
        mtfc_card_config_update.is_dependent_phase,
        mtfc_card_config_update.time_delay_dependent_phase);

      // mtfc_write_eeprom_api(0, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      mtfc_write_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      delay(200);
      memset((uint8_t *)&temp, 0, sizeof(type_one_cardConfig_t));
      // mtfc_read_eeprom_api(0, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      mtfc_read_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t*)&temp, sizeof(type_one_cardConfig_t));
      if ((temp.phase != mtfc_card_config_update.phase))
      {
        //Phan hoi error
        cardCom.send_byte(MSP_CARD_FB_CONFIG, 255);
        mtfc_card_installed = false;
      }
      else
      {
        //Phan hoi sucessfull
        mtfc_card_config = mtfc_card_config_update;
        cardCom.send_byte(MSP_CARD_FB_CONFIG, mtfc_card_config.phase);
        mtfc_card_installed = true;
      }
      if (temp.phase == 0)
      {
        mtfc_card_installed = false;
      }
    }
      debug(MAIN_DEBUG, "\r\nsn: %s\r\nimei: %ld\r\nphase: %d\r\nis railway enable: %d\r\nis walking enable: %d\r\nis dependent enable: %d\r\ntime dephent delay: %d",
        mtfc_card_config.sn,
        mtfc_card_config.imei,
        mtfc_card_config.phase,
        mtfc_card_config.is_railway_enabled,
        mtfc_card_config.is_walking_enabled,
        mtfc_card_config.is_dependent_phase,
        mtfc_card_config.time_delay_dependent_phase);
      mtfc_sys_flag.isWriteConfig = false;
  }
  //Xoa du lieu cai dat phan cung
  if (mtfc_sys_flag.isDeleteConfig)
  {
    if (cardWorkingState == CMD_STATE)
    {
      type_one_cardConfig_t temp = mtfc_card_config_update;
      temp.phase = 255;
      // mtfc_write_eeprom_api(0, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      mtfc_write_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      delay(200);
      memset((uint8_t *)&temp, 0, sizeof(type_one_cardConfig_t));
      // mtfc_read_eeprom_api(0, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      mtfc_read_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&temp, sizeof(type_one_cardConfig_t));
      if (temp.phase != 255)
      {
        //Phan hoi error
        cardCom.send_byte(MSP_CARD_FB_CONFIG, 255);
        if (temp.phase >= 1 && temp.phase <= 8)
        {
          mtfc_card_installed = true;
        }
        else
        {
          mtfc_card_installed = false;
        }
      }
      else
      {
        //Phan hoi sucessfull
        mtfc_card_config = temp;
        cardCom.send_byte(MSP_CARD_FB_CONFIG, mtfc_card_config.phase);
        mtfc_card_installed = false;
      }
    }
    mtfc_sys_flag.isDeleteConfig = false;
  }
  //Doc cau cardCom
  if (mtfc_sys_flag.isRequestConfig)
  {
    if (cardWorkingState == CMD_STATE)
    {
      debug(MAIN_DEBUG, "\r\n%s", "Send back Config");
      cardCom.send_struct(MSP_CARD_READ_CONFIG, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
      debug(MAIN_DEBUG, "\r\nsn: %s\r\nimei: %ld\r\nphase: %d\r\nis railway enable: %d\r\nis walking enable: %d\r\nis dependent enable: %d\r\n time dephent delay: %d",
        mtfc_card_config.sn,
        mtfc_card_config.imei,
        mtfc_card_config.phase,
        mtfc_card_config.is_railway_enabled,
        mtfc_card_config.is_walking_enabled,
        mtfc_card_config.is_dependent_phase,
        mtfc_card_config.time_delay_dependent_phase);
    }
    mtfc_sys_flag.isRequestConfig = false;
  }
}

#pragma endregion

#pragma region INTERRUP READ STEAM DATA CART_RX
void mtfc_card_com_event_handler(void)
{
  //debug(MAIN_DEBUG, "\r\ncardCom.getMSP():%d\n", cardCom.getMSP());
  switch (cardCom.getMSP())
  {
  case MSP_CARD_WRITE_DATA:
    cardCom.readstruct((uint8_t *)&mtfc_card_data_ouput, sizeof(type_mtfc_card_data_t));
    mtfc_ticker_time_connection = 0;
    mtfc_sys_flag.is_out_data_to_output = true;
    break;
  case MSP_CARD_WRITE_CONFIG:
    cardCom.readstruct((uint8_t *)&mtfc_card_config_update, sizeof(type_one_cardConfig_t));
    mtfc_card_config_update.time_delay_dependent_phase = time_setting.time_delay_dependent_phase;
    mtfc_sys_flag.isWriteConfig = true;
    break;
  case MSP_CARD_DELETE:
    mtfc_sys_flag.isDeleteConfig = true;
    break;
  case MSP_CARD_READ_CONFIG:
    mtfc_sys_flag.isRequestConfig = true;
    break;
  case MSP_CARD_CHECK_SENSOR:
    cardCom.readstruct((uint8_t *)&mtfc_check_pkg, sizeof(type_check_pkg_t));
    mtfc_sys_flag.is_request_get_sensor = true;
    break;
  case CMD_MASTER_TO_SLAVE_TIME_SETTING:
    cardCom.readstruct((uint8_t *)&time_setting, sizeof(time_setting_t));
    debug(MAIN_DEBUG, "\r\time_setting.phase: %d", time_setting.phase);
    debug(MAIN_DEBUG, "\r\tmtfc_card_config.phase: %d", mtfc_card_config.phase);
    debug(MAIN_DEBUG, "\r\time_setting.time_delay_dependent_phase: %d", time_setting.time_delay_dependent_phase);
    if (mtfc_card_config.phase == time_setting.phase)
    {
      // mtfc_read_eeprom_api(0, (uint8_t *)&mtfc_card_config_update, sizeof(type_one_cardConfig_t));
      mtfc_read_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&mtfc_card_config_update, sizeof(type_one_cardConfig_t));
      mtfc_card_config_update.time_delay_dependent_phase = time_setting.time_delay_dependent_phase;
      mtfc_sys_flag.isWriteConfig = true;
      mtfc_sys_flag.time_setting = true;
    }
    break;
  default:
    break;
  }
}
#pragma endregion

#pragma region CAC HAM KHAC
void mtfc_read_mode_active(void)
{
  static cardWorkingState_t cardWorkingStateOLD = NO_CONFIG_STATE;
  if (mtfc_hardware_select_card())
    cardWorkingState = CMD_STATE;
  else if (mtfc_card_installed)
    cardWorkingState = ACTIVE_STATE;
  else
    cardWorkingState = NO_CONFIG_STATE;
  if (cardWorkingStateOLD != cardWorkingState)
  {
    debug(MAIN_DEBUG, "\r\ncardWorkingState : %d", cardWorkingState);
    cardWorkingStateOLD = cardWorkingState;
  }
}

bool mtfc_hardware_select_card(void)
{
  if (digitalRead(CS_CARD_GATE) == 0)
  {
    delay(20);
    if (digitalRead(CS_CARD_GATE) == 0)
    {
      return true;
    }
  }
  return false;
}

void mtfc_led_processing(void)
{
  static uint32_t led_status_time_count = 0;
  if (cardWorkingState == NO_CONFIG_STATE) //fast flash
  {
    if ((millis() - led_status_time_count) > FLASH_FAST_TIME)
    {
      digitalWrite(LED_STATUS_GATE, !digitalRead(LED_STATUS_GATE));
      led_status_time_count = millis();
    }
  }
  else if (cardWorkingState == ACTIVE_STATE) //slow flash
  {
    /*     if ((millis() - led_status_time_count) > FLASH_LOW_TIME)
    {
      digitalWrite(LED_STATUS_GATE, !digitalRead(LED_STATUS_GATE));
      led_status_time_count = millis();
    } */
    digitalWrite(LED_STATUS_GATE, !led_active_state);
  }
  else //soild
  {
    digitalWrite(LED_STATUS_GATE, LOW);
  }
}

#pragma endregion

#pragma region HAM LIEN QUAN BO NHO

int8_t mtfc_memory_startup_load(void)
{
  memset((uint8_t *)&cardData, 0, sizeof(type_cardDataInOut_t));
  memset((bool *)&mtfc_sys_flag, false, sizeof(type_sysFlag_t));
  memset((uint8_t *)&mtfc_card_config, 0, sizeof(type_one_cardConfig_t));
  memset((uint8_t *)&mtfc_card_config_update, 0, sizeof(type_one_cardConfig_t));
  memset((uint8_t *)&cardDataSensor, 0, sizeof(type_cardDataInOut_t));
  // mtfc_read_eeprom_api(0, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
  mtfc_read_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
  if (strstr((char *)&mtfc_card_config.sn, "Si") == NULL)
  {
    debug(MAIN_DEBUG, "\r\n%s", "Fist active random number imei");
    srand(analogRead(PA1));
    uint32_t i = rand();
    mtfc_card_config.imei = i;
    if(digitalRead(TYPE_CARD_GATE) == AC_TYPE_CARD)
    {
      sprintf((char *)&mtfc_card_config.sn, "Si-AC%08X", i);
    }else
    {
      sprintf((char *)&mtfc_card_config.sn, "Si-DC%08X", i);
    }

    mtfc_card_config.phase = 0;
    mtfc_card_config.is_railway_enabled = 0;
    //deviceConfig.phase = NO_CONFIG_PHASE;
    // mtfc_write_eeprom_api(0, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
    mtfc_write_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
    delay(200);
    // mtfc_read_eeprom_api(0, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
    mtfc_read_flash_api(ADDR_STORAGE_INFO_CARD, (uint8_t *)&mtfc_card_config, sizeof(type_one_cardConfig_t));
  }
  if ((mtfc_card_config.phase >= 1) && (mtfc_card_config.phase <= 8))
  {
    return 0;
  }
  return -1;
}

void mtfc_write_eeprom_api(uint8_t pos, uint8_t *dat, uint16_t siz)
{
  uint8_t temp = 0;
  uint16_t address = pos;
  while (siz--)
  {
    temp = *dat;
    delay(100);
    EEPROM.write(address++, temp);
    dat++;
  }
}

void mtfc_read_eeprom_api(uint8_t pos, uint8_t *dat, uint16_t siz)
{
  uint16_t address = pos;
  for (uint16_t i = 0; i < siz; i++)
  {
    *dat = EEPROM.read(address++);
    dat++;
  }
}
#pragma endregion

#pragma region HAM XUAT NHAP DU LIEU CARD

void mtfc_output_buffer_to_hardware(type_mtfc_card_data_t *dat, type_one_cardConfig_t *config)
{
  uint8_t out = 0;
  if (config->phase >= 1 && config->phase <= 8)
  {
    out = dat->ouput[config->phase - 1];
    if (config->is_railway_enabled)
    {
      if (dat->ouput[config->phase - 1] != dat->mapping[config->phase - 1])
      {
        out = dat->mapping[config->phase - 1];
      }
    }
    mtfc_output_hardware(out);
    // mtfc_output_right(out);
    debug(OUPUT_DEBUG, "\r\ndata output: [%02X]", out);
  }
  else
  {
    mtfc_output_off();
  }
}

void mtfc_output_off(void)
{
  mtfc_output_hardware(0);
}
static uint32_t led_status_delay_count = 0;
void mtfc_output_hardware(uint8_t dat)
{
  static unsigned long mtfc_send_error_time = millis();
  if (dat & 0x01)
  {
    if (mtfc_card_config.is_dependent_phase == 1)
    {
      if ((led_status_delay_count ) < mtfc_card_config.time_delay_dependent_phase)
      {
        debug(MAIN_DEBUG, "\r\n %s", "Dang cho delay xanh");
        debug(MAIN_DEBUG, "\r\nled_status_time_count : %d", led_status_delay_count);
        flag_red = 1;
        led_status_delay_count++;
      }
      else
      {
        debug(MAIN_DEBUG, "\r\n %s", "Chua co gi xay ra");
        debug(MAIN_DEBUG, "\r\nmillis : %d", millis());
        debug(MAIN_DEBUG, "\r\nled_status_time_count : %d", led_status_delay_count);
        flag_red = 0;
        clearPin(OUT_RED_GATE);
        setPin(OUT_GREEN_GATE);
      }
    }
    else
    {
      flag_red = 0;
      setPin(OUT_GREEN_GATE);
    }
  }
  else
    clearPin(OUT_GREEN_GATE);
  if (dat & 0x02)
  {
    setPin(OUT_YELLOW_GATE);
    if(millis() - mtfc_send_error_time > 1100)
    {
      mtfc_sys_flag.is_turn_right = true;
    }
    else
    {
      mtfc_sys_flag.is_turn_right = false;
      mtfc_send_error_time = millis();
      clearPin(OUT_YELLOW_GATE);
    }
    flag_red = 0;
  }
  else
    clearPin(OUT_YELLOW_GATE);
  if (dat & 0x04)
  {
    setPin(OUT_RED_GATE);
    led_status_delay_count = 0;
  }
  else
    clearPin(OUT_RED_GATE);
  if (dat & 0x08)
    setPin(OUT_WALKING_GREEN_GATE);
  else
    clearPin(OUT_WALKING_GREEN_GATE);
  if (dat & 0x10)
    setPin(OUT_WALKING_RED_GATE);
  else
    clearPin(OUT_WALKING_RED_GATE);
  if (dat & 0x20)
    setPin(OUT_OP1_GATE);
  else
    clearPin(OUT_OP1_GATE);
  if (dat & 0x40)
    setPin(OUT_OP2_GATE);
  else
    clearPin(OUT_OP2_GATE);
  if (dat & 0x80)
    setPin(OUT_OP3_GATE);
  else
    clearPin(OUT_OP3_GATE);  

  mtfc_wk.output_data = dat;
  mtfc_wk.mill_time_get_sample = millis();
  mtfc_sys_flag.is_enable_process_readSensor = true;
}

// yêu cầu mũi tên rẻ phải (thangnm)
void mtfc_output_right(uint8_t dat)
{
  if ((dat & 0x04) || ((dat & 0x02) && mtfc_sys_flag.is_turn_right))
  {
    setPin(OUT_OP1_GATE);
  }
  else
  {
    clearPin(OUT_OP1_GATE);
  }
}

uint8_t mtfc_read_sensor(void)
{
  uint8_t sen = 0;
  if (digitalRead(FB_GREEN_GATE) == 0)
  {
    bitSet(sen, GREEN_BIT);
  }

  if (digitalRead(FB_YELLOW_GATE) == 0)
  {
    bitSet(sen, YELLOW_BIT);
  }

  if (digitalRead(FB_RED_GATE) == 0)
  {
    bitSet(sen, RED_BIT);
  }

  if (digitalRead(FB_WALKING_GREEN_GATE) == 0)
  {
    bitSet(sen, WALKING_GREEN_BIT);
  }

  if (digitalRead(FB_WALKING_RED_GATE) == 0)
  {
    bitSet(sen, WALKING_RED_BIT);
  }

  return sen;
}

#pragma endregion

#pragma region STORAGE

void settings_storage(void)
{
  if (SerialFlash.begin(FLASH_CHIP_SELECT))
  {
    debug(MAIN_DEBUG, "%s\r\n", "OK");
    // digitalWrite(LED_STATUS_GATE, HIGH);
    // while(1);
  }
  else
  {
    debug(MAIN_DEBUG, "%s\r\n", "No OK");
  }
}

void mtfc_write_flash_api(uint32_t addr, const void *buf, uint32_t len)
{
  SerialFlash.eraseBlock(addr);
  delay(2);
  SerialFlash.write(addr, buf, len);
}

void mtfc_read_flash_api(uint32_t addr, void *buf, uint32_t len)
{
  SerialFlash.read(addr, buf, len);
}
#pragma endregion