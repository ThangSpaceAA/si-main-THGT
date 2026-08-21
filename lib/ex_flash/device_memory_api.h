#ifndef DEVICE_MEMORY_API_H
#define DEVICE_MEMORY_API_H
#include <mbed.h>
#include "SpiFlash25.h"

typedef struct __attribute__((packed))
{
  char      firmwar_version[32];
  char      imei[32];
  char      license[32];
  uint32_t  core_clock_speed;  
  uint16_t  memory_size;  
}type_device_factory_details_t;

class device_memory : public SpiFlash25
{
public:
  char *mem_JEDEC_ID;
  uint16_t mem_size;
  /**
   * @brief Construct a new device memory object
   * 
   * @param di_pin 
   * @param do_pin 
   * @param clk_pin 
   * @param cs_pin 
   */
  device_memory (PinName di_pin, PinName do_pin, PinName clk_pin, PinName cs_pin, PinName samp);
  /**
   * @brief Cau hinh bo nho
   * 
   */
  void mem_init(void);
  /**
   * @brief Get thong tin xuat xuong cua thiet bi
   * 
   * @return type_device_factory_details_t 
   */
  type_device_factory_details_t mem_get_device_details_t(void);
  
  /**
   * @brief Save ex flash memory
   * 
   * @param add 
   * @param dat 
   * @param siz 
   */
  void mem_save_data(int add, const char *dat, int siz);
  /**
   * @brief Read ex flash memory
   * 
   * @param add 
   * @param data 
   * @param siz 
   */
  void mem_read_data(int add, char *data, int siz);
  /**
   * @brief Kiem tra thiet bi co phai lan dau khoi dong
   * 
   * @return true 
   * @return false 
   */
  bool mem_is_device_fist_run(void);
  /**
   * @brief Cai dat lai dia chi imei
   * 
   */
  void mem_re_initialize_imei(void);
  /**
   * @brief Cai dat lai firmwar_version
   * 
   */
  void mem_re_initialize_firmware(void);

private:
  AnalogIn  SampleAdc;
  volatile bool device_fist_run;
  type_device_factory_details_t device_details;
  int8_t mem_get_details(void);
  type_device_factory_details_t mem_load_default_device_details(void);
  void mem_set_device_details_t(type_device_factory_details_t dat);
};

#endif