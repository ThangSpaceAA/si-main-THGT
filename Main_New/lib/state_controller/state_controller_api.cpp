#include <state_controller_api.h>
#include <bsp_bits.h>


state_controller::state_controller(PinName _on_off_pin, PinName _railway_pin,
                                   PinName _walking_pin, PinName _t1_pin,
                                   PinName _t2_pin, PinName _t3_pin,
                                   PinName _t4_pin, PinName _opt_pin)
    : on_off_pin(_on_off_pin), railway_pin(_railway_pin),
      walking_pin(_walking_pin), t1_pin(_t1_pin), t2_pin(_t2_pin),
      t3_pin(_t3_pin), t4_pin(_t4_pin), opt_pin(_opt_pin)

{
  device_conf = NULL;
}


void state_controller::init_para_setting(type_state_module_config_t *conf) {
  device_conf = conf;
  input_hard = CLEAR_VALUE;
  input_soft = CLEAR_VALUE;
  rail_signal = false;
}

uint8_t state_controller::get_all_signal_input(void) {
  uint8_t temp = 0;
  if (on_off_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_on);
  if (railway_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_railway);
  if (walking_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_walking);
  if (t1_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_t1);
  if (t2_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_t2);
  if (t3_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_t3);
  if (t4_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_t4);
  if (opt_pin == SWITCH_ACTIVE)
    bit_set(temp, pin_switch_op);
  return temp;
}


bool state_controller::read_on_switch(void)
{
  bool on_signal = false;
  input_hard = get_all_signal_input();
  if (on_off_pin == SWITCH_ACTIVE)
    on_signal = true;
  if (bit_check(input_hard, pin_switch_on) != on_signal)
  {
    if (on_off_pin == SWITCH_ACTIVE)
      on_signal = true;
    else
      on_signal = false;
  }
  return on_signal;
}


bool state_controller::get_railway_switch(void)
{
  return rail_signal;
}
  

void state_controller::set_railway_switch(bool state)
{
  rail_signal = state;
}

bool state_controller::is_change_railway_switch(void)
{
  bool temp = get_hard_railway_switch();
  if (temp != get_railway_switch())
  {
    set_railway_switch(temp);
    return true;
  }
  return false;
}

bool state_controller::get_hard_railway_switch(void)
{
  bool temp = false;
  if (railway_pin == SWITCH_ACTIVE)
    temp = true;
  return temp;
}


bool state_controller::read_walking_switch(void)
{
  bool walking_signal = false;
  input_hard = get_all_signal_input();
  if (walking_pin == SWITCH_ACTIVE)
    walking_signal = true;
  if (bit_check(input_hard, pin_switch_walking) != walking_signal)
  {
    if (walking_pin == SWITCH_ACTIVE)
      walking_signal = true;
    else
      walking_signal = false;
  }
  return walking_signal;
}

uint8_t state_controller::read_phase_switch(void)
{
  input_hard = get_all_signal_input();
  if (t1_pin == SWITCH_ACTIVE)
    return 1;
  if (t2_pin == SWITCH_ACTIVE)
    return 2;
  if (t3_pin == SWITCH_ACTIVE)
    return 3;
  if (t4_pin == SWITCH_ACTIVE)
    return 4;
  return 0;
}

//Doc phan mem

bool state_controller::read_on_app(void)
{
  uint8_t temp = device_conf->soft_controler_input;
  input_soft = device_conf->soft_controler_input;
  if (bit_check(temp, pin_switch_on))
  {
    return true;
  }
  return false;
}

uint8_t state_controller::read_phase_app(void)
{
  input_soft = device_conf->soft_controler_input;
  if (bit_check(input_soft, pin_switch_t1))
    return 1;
  if (bit_check(input_soft, pin_switch_t2))
    return 2;
  if (bit_check(input_soft, pin_switch_t3))
    return 3;
  if (bit_check(input_soft, pin_switch_t4))
    return 4;
  return 0;
}


void state_controller::clear_input_app(void)
{
  input_soft = CLEAR_VALUE;
}

