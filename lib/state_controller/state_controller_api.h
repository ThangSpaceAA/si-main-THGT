#ifndef STATE_CONTROLLER_API_H
#define STATE_CONTROLLER_API_H
#include <mbed.h>


#define CLEAR_VALUE     0x00

typedef enum _switch_state_
{
    SWITCH_ACTIVE = 0,
    SITHCH_INACTIVE
}_switch_state_;

typedef struct __attribute__((packed)) {
  volatile uint8_t num_side;
  volatile uint8_t soft_controler_input;
} type_state_module_config_t;



//Thu tu chan swicth dieu khien
typedef enum _pin_switch_profile_
{
    pin_switch_on = 0,
    pin_switch_railway,
    pin_switch_walking,
    pin_switch_t1,
    pin_switch_t2,
    pin_switch_t3,
    pin_switch_t4,
    pin_switch_op
} _pin_switch_profile_;

class state_controller 
{

public:
  state_controller(PinName _on_off_pin, PinName _railway_pin,
                   PinName _walking_pin, PinName _t1_pin, PinName _t2_pin,
                   PinName _t3_pin, PinName _t4_pin, PinName _opt_pin);
  void init_para_setting(type_state_module_config_t *conf);


//Doc phan cung
  /**
   * @brief Doc gia tri port dieu khien
   * 
   * @return * uint8_t 
   */
  uint8_t get_all_signal_input(void);

  /**
   * @brief Doc tin hieu on/off
   * 
   * @return true Tich cuc
   * @return false 
   */
  bool read_on_switch(void);


 /**
   * @brief doc tin hieu phan cung
   * 
   * @return true tich cuc
   * @return false 
   */
  bool get_hard_railway_switch(void);



  /**
   * @brief Tin hieu ket noi duong sat
   * 
   * @return true tich cuc
   * @return false 
   */
  bool get_railway_switch(void);

  
  /**
   * @brief Set the railway switch object
   * 
   * @param state 
   */
  void set_railway_switch(bool state);

  /**
   * @brief 
   * 
   * @return true 
   * @return false 
   */
  bool is_change_railway_switch(void);

  /**
   * @brief Tin hieu di bo
   * 
   * @return true Tich cuc
   * @return false 
   */
  bool read_walking_switch(void);

  /**
   * @brief Doc so pha dieu khien
   * 
   * @return uint8_t So pha
   */
  uint8_t read_phase_switch(void);

//Doc phan mem
  /**
   * @brief Doc tin hieu on/off
   * 
   * @return true Tich cuc
   * @return false 
   */
  bool read_on_app(void);

  /**
   * @brief Doc so pha dieu khien
   * 
   * @return uint8_t So pha
   */
  uint8_t read_phase_app(void);

  /**
   * @brief 
   * 
   */
  void clear_input_app(void);

  

private:
  DigitalIn on_off_pin;
  DigitalIn railway_pin;
  DigitalIn walking_pin;
  DigitalIn t1_pin;
  DigitalIn t2_pin;
  DigitalIn t3_pin;
  DigitalIn t4_pin;
  DigitalIn opt_pin;
  type_state_module_config_t *device_conf;
  //Bien luu trang thai input hard
  volatile uint8_t input_hard;
  //Bien luu trang thai input_soft
  volatile uint8_t input_soft;

  bool rail_signal;

};

#endif