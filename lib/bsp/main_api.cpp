
#include <bsp_config.h>
#include <serial_msp_code.h>
#include <debug.h>

extern void mtfc_debug_console_part_data(uint8_t *str, uint8_t size);

//default thoi gian xanh cua tung pha
uint8_t default_green_time[MAX_SIDE] = {25,25,25,25,25,25,25,25};
enum
{
     CONSOLE_IDLE,
     CONSOLE_START,
     CONSOLE_GET,
     CONSOLE_END
};

volatile uint8_t mtf_debug_console = CONSOLE_END;
uint8_t mtfc_console_buff[255];
uint8_t mtfc_console_idx = 0;


const char string_name_days[7][5] = 
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
};
void mtfc_debug_console_rx_event_handler(void)
{

    if (dbCom.readable())
    {
        char c = dbCom.getc();
        if (mtf_debug_console == CONSOLE_IDLE)
        {
            mtf_debug_console = (c == '>') ? CONSOLE_START : CONSOLE_IDLE;
        }
        else if (mtf_debug_console == CONSOLE_START)
        {
            mtf_debug_console = (c == '_') ? CONSOLE_GET : CONSOLE_IDLE;
        }
        else if (mtf_debug_console == CONSOLE_GET)
        {
            if (c == ':')
            {
                mtfc_console_idx = 0;
                mtf_debug_console = CONSOLE_END;
            }
            else
            {
                mtf_debug_console = CONSOLE_IDLE;
            }
        }
        else if (mtf_debug_console == CONSOLE_END && c != '$')
        {
            mtfc_console_buff[mtfc_console_idx++] = c;
        }
        else if (mtf_debug_console == CONSOLE_END && c == '$')
        {
            mtfc_debug_console_part_data(mtfc_console_buff, mtfc_console_idx);
            mtf_debug_console = CONSOLE_IDLE;
        }
    }
}

void mtfc_caclator_one_cycle_form(type_one_cycle_lamp_time_t *dat)
{
//  Cong thuc tinh tong chu ky
//  T = Tg1+....Tgn + n*(Ty + Tclearance)
    for (uint8_t i = 0; i < dat->num_side_used; i++)
    {
        dat->period_crossroads += dat->side[i].t_green;
    }
    dat->period_crossroads = dat->period_crossroads + dat->num_side_used*(dat->clearance_time_crossroads+dat->side[0].t_yellow);
//Tinh thoi gian den RED tred = T - (Tg+Ty);
    for (uint8_t i = 0; i < dat->num_side_used; i++)
    {
        dat->side[i].t_red = dat->period_crossroads - (dat->side[i].t_green + dat->side[i].t_yellow);
    }
//Tinh cac thong so phu
//.Tinh cua pha 1
    if (dat->num_side_used > 0)
    {
        dat->side[0].t_start_green = 0;
        dat->side[0].t_end_green = dat->side[0].t_green;
        dat->side[0].t_start_yellow = dat->side[0].t_end_green;
        dat->side[0].t_end_yellow = dat->side[0].t_start_yellow + dat->side[0].t_yellow;
    }
//.Tinh cac pha khac
    for (uint8_t i = 1; i < dat->num_side_used; i++)
    {
        dat->side[i].t_start_green = dat->side[i - 1].t_end_yellow + dat->clearance_time_crossroads;
        dat->side[i].t_end_green = dat->side[i].t_start_green + dat->side[i].t_green;
        dat->side[i].t_start_yellow = dat->side[i].t_end_green;
        dat->side[i].t_end_yellow = dat->side[i].t_start_yellow + dat->side[i].t_yellow;
    }
//Xoa cac pha khong su dung
    for (int i = dat->num_side_used; i < MAX_SIDE; i++)
    {
        dat->side[i].t_start_green = 0;
        dat->side[i].t_end_green = 0;
        dat->side[i].t_start_yellow = 0;
        dat->side[i].t_end_yellow = 0;
    }
}


int8_t mtfc_check_cycle_config_data(type_mtfc_quick_cycle_config_t *cycle, type_mtfc_schedule_t *schedule)
{
    if (cycle->num_side > schedule->num_side)
        return -1;
    for (uint8_t i = 0; i < cycle->num_side; i++)
    {
        if ((cycle->green_t[i] < MIN_GREEN_TIME) || (cycle->green_t[i] > MAX_GREEN_TIME))
            return -1;
    }
    if (cycle->yellow_t == 0 || cycle->yellow_t > MAX_YELLOW_TIME)
        return -1;
    if (cycle->clearance_t > MAX_CLEARANCE_TIME)
        return -1;
    return 0;
}

int8_t mtfc_one_cycle_set_config(type_one_cycle_lamp_time_t *temp, type_mtfc_quick_cycle_config_t *cycle)
{
    temp->num_side_used = cycle->num_side;
    temp->clearance_time_crossroads = cycle->clearance_t;
    temp->period_crossroads = 0;
    for (uint8_t j = 0; j < cycle->num_side; j++)
    {
        temp->side[j].t_green = cycle->green_t[j];
        temp->side[j].t_yellow = cycle->yellow_t;
    }
    //Xoa thoi gian xanh vang do cua cac pha den khong dung den
    for (uint8_t j = cycle->num_side; j < MAX_SIDE; j++)
    {
        temp->side[j].t_green = mtfc_unset;
        temp->side[j].t_yellow = mtfc_unset;
        temp->side[j].t_red = mtfc_unset;
    }
    mtfc_caclator_one_cycle_form(temp);
    return 0;
}

void mtfc_contruct_schedule_default(type_mtfc_schedule_t *dat, uint8_t num_phase)
{
    //CAI DAT MAC DINH 4 TRUONG DU LIEU
    //[1.MAC DINH THOI GIAN LAM VIEC]
    dat->num_side = num_phase;
    dat->active_time.tbegin.hour = DEFAULT_ACTIVE_BEGIN_H;
    dat->active_time.tbegin.minute = DEFAULT_ACTIVE_BEGIN_M;
    dat->active_time.tend.hour = DEFAULT_ACTIVE_END_H;
    dat->active_time.tend.minute = DEFAULT_ACTIVE_END_M;
    //[2.MAC DINH BIEU MAU CHIEN LUOC]
    for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
    {
        dat->cycle.index[i].num_side_used = num_phase;
        dat->cycle.index[i].clearance_time_crossroads = DEFAULT_CLEARANCE_TIME;
        dat->cycle.index[i].period_crossroads = 0;
        //Load cac gia tri xanh vang cho tung pha den
        for (uint8_t j = 0; j < num_phase; j++)
        {
            dat->cycle.index[i].side[j].t_green = default_green_time[j];
            dat->cycle.index[i].side[j].t_yellow = DEFAULT_YELLOW_TIME;
        }
        //Xoa thoi gian xanh vang do cua cac pha den khong dung den
        for (uint8_t j = num_phase; j < MAX_SIDE; j++)
        {
            dat->cycle.index[i].side[j].t_green = mtfc_unset;
            dat->cycle.index[i].side[j].t_yellow = mtfc_unset;
        }
        //Tinh toan cac gia tri con lai
        mtfc_caclator_one_cycle_form(&dat->cycle.index[i]);
    }
    //[3.MAC DINH BIEU MAU THOI GIAN]
    //.Xoa tat ca cac cycle form
    for (uint8_t i = 0; i < MAX_TIME_FORM; i++)
        for (uint8_t j = 0; j < MAX_TIME_LINE; j++)
        {
            dat->time.index[i].select_point[j].index_cycle_form = mtfc_unused_form;
            dat->time.index[i].select_point[j].t_begin_apply.hour = mtfc_unused_form;
            dat->time.index[i].select_point[j].t_begin_apply.minute = mtfc_unused_form;
        }
    //.Gan gia tri cho bieu mau thoi gian dau tien
    dat->time.index[0].select_point[0].index_cycle_form = DEFAULT_CYCLE_APPLY;
    dat->time.index[0].select_point[0].t_begin_apply.hour = 1; //chien luoc mat dinh duoc ap dung tu
    dat->time.index[0].select_point[0].t_begin_apply.minute = 0;
    //[4.MAC DINH CAC NGAY TRONG TUAN]
    for(uint8_t i = 0; i < MAX_DAYS_WORKING; i++)
    {
        dat->days.index[i] = DEFAULT_TIME_FORM_APPLY;
    }
}

void mtfc_contruct_all_cycle_default(type_cycle_lamp_form_t *dat, uint8_t num_phase)
{
    for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
    {
        dat->index[i].num_side_used = num_phase;
        dat->index[i].clearance_time_crossroads = DEFAULT_CLEARANCE_TIME;
        dat->index[i].period_crossroads = 0;
        //Load cac gia tri xanh vang cho tung pha den
        for (uint8_t j = 0; j < num_phase; j++)
        {
            dat->index[i].side[j].t_green = default_green_time[j];
            dat->index[i].side[j].t_yellow = DEFAULT_YELLOW_TIME;
        }
        //Xoa thoi gian xanh vang do cua cac pha den khong dung den
        for (uint8_t j = num_phase; j < MAX_SIDE; j++)
        {
            dat->index[i].side[j].t_green = mtfc_unset;
            dat->index[i].side[j].t_yellow = mtfc_unset;
            dat->index[i].side[j].t_red = mtfc_unset;
        }
        //Tinh toan cac gia tri con lai
        mtfc_caclator_one_cycle_form(&dat->index[i]);
    }
}

void mtfc_contruct_all_cycle_user_config(type_cycle_lamp_form_t *dat, uint8_t num_phase)
{
    for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
    {
        dat->index[i].num_side_used = num_phase;
        dat->index[i].period_crossroads = 0;
        //Xoa thoi gian xanh vang do cua cac pha den khong dung den
        for (uint8_t j = num_phase; j < MAX_SIDE; j++)
        {
            dat->index[i].side[j].t_green = mtfc_unset;
            dat->index[i].side[j].t_yellow = mtfc_unset;
            dat->index[i].side[j].t_red = mtfc_unset;
        }
        //Tinh toan cac gia tri con lai
        mtfc_caclator_one_cycle_form(&dat->index[i]);
    }
}


void mtfc_contruct_card_config_default(type_one_cardConfig_t *obj)
{
    for (uint8_t i = 0; i < 32; i++)
    {
        obj->sn[i] = '\0';
    }
    obj->imei = 0;
    obj->phase = 0;
    obj->is_railway_enabled = 0;
}

void mtfc_contruct_pin_config_default(type_mtfc_phase_gate_config_t *obj)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        obj->pin[i] = i;
    }
}

void mtfc_contruct_mtfc_config_default(type_mtfc_config_t *obj)
{
    obj->type_cpu = mtfc_ac;
    obj->walking_mode_enable = false;
    obj->railway_mode_enable = true;
    obj->power_meters_enable = false;
    obj->locked_hard_control_enable = true;
    obj->is_flashing = true;
    obj->is_enabale_out485_lamp_port = false;
    obj->railway_delay_on_time = 10;
    obj->railway_delay_off_time = 20;
    obj->walking_walking_time = 0;
    obj->latitude = 10.823099;
    obj->longitude = 106.629662;
    //Cau hinh thong so card
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
        mtfc_contruct_card_config_default((type_one_cardConfig_t*)&obj->index_card[i]);
    }
    //Cau hinh thong so chan xuat tin hieu
    for (uint8_t i = 0; i < MAX_SIDE; i++)
    {
        mtfc_contruct_pin_config_default((type_mtfc_phase_gate_config_t*)&obj->phase_pin_config[i]);
    }
}

void mtfc_debug_print_active_time(type_active_lamp_t *act_time)
{
    debug(MAIN_DEBUG, "\r\n%s", "------------------Active Time------------------------");
    debug(MAIN_DEBUG, "\r\nActive Time: [%02d %02d] -> [%02d %02d]", act_time->tbegin.hour,act_time->tbegin.minute, act_time->tend.hour, act_time->tend.minute);
    debug(MAIN_DEBUG, "\r\n%s", "-----------------------------------------------------");
}

void mtfc_debug_print_time_form(type_time_lamp_form_t *time_form)
{
    debug(MAIN_DEBUG, "\r\n%s", "------------------Time Form------------------------");
    debug(MAIN_DEBUG, "\r\n%s", "Index\tTimeline\tBegin\t\tCycleForm");
    for (uint8_t i = 0; i < MAX_TIME_FORM; i++)
    {
        debug(MAIN_DEBUG, "\r\n[%02d]", (i + 1));
        for (uint8_t j = 0; j < MAX_TIME_LINE; j++)
        {
            debug(MAIN_DEBUG, "\t[%02d]", (j + 1));
            debug(MAIN_DEBUG, "\t[%03d %03d]\t\t[%02d]", time_form->index[i].select_point[j].t_begin_apply.hour, time_form->index[i].select_point[j].t_begin_apply.minute,time_form->index[i].select_point[j].index_cycle_form);
            debug(MAIN_DEBUG, "%s", "\r\n");
        }
    }
}

void mtfc_debug_print_days_form(type_days_lamp_form_t *days_form)
{
    debug(MAIN_DEBUG, "\r\n%s", "------------------Days Form--------------------------");
    debug(MAIN_DEBUG, "\r\n%s", "Days\tIndex Time Form");
    for (uint8_t i = 0; i < MAX_DAYS_WORKING; i++)
    {
        debug(MAIN_DEBUG, "\r\n[%s]\t[%03d]", string_name_days[i], days_form->index[i]);
    }
    debug(MAIN_DEBUG, "\r\n%s", "-----------------------------------------------------");
}

void mtfc_debug_print_cycle_form(type_cycle_lamp_form_t *dat, uint8_t idx)
{
    debug(MAIN_DEBUG, "\r\n%s", "Idex\tNumLine\tCycle\tClearanceT\tLine\tGreen\tRed\tYellow\tSgreen\tEgreen\tSyellow\tEyellow");
    debug(MAIN_DEBUG, "\r\n[%02d]", idx + 1);
    debug(MAIN_DEBUG, "\t%02d\t%03d\t%02d",
          dat->index[idx].num_side_used,
          dat->index[idx].period_crossroads,
          dat->index[idx].clearance_time_crossroads);
    for (uint8_t j = 0; j < MAX_SIDE; j++)
    {
        if (j != 0)
            debug(MAIN_DEBUG, "\t\t\t\t\t%02d", j + 1);
        else
            debug(MAIN_DEBUG, "\t\t%02d", j + 1);
        debug(MAIN_DEBUG, "\t%03d\t%03d\t%03d",
              dat->index[idx].side[j].t_green,
              dat->index[idx].side[j].t_red,
              dat->index[idx].side[j].t_yellow);
        debug(MAIN_DEBUG, "\t%03d\t%03d",
              dat->index[idx].side[j].t_start_green,
              dat->index[idx].side[j].t_end_green);
        debug(MAIN_DEBUG, "\t%03d\t%03d\r\n",
              dat->index[idx].side[j].t_start_yellow,
              dat->index[idx].side[j].t_end_yellow);
    }
}

void mtfc_debug_print_cycle_form(type_cycle_lamp_form_t *dat)
{
    for (uint8_t i = 0; i < MAX_CYCLE_FORM; i++)
    {
        mtfc_debug_print_cycle_form(dat,i);
    }
}


void mtfc_printOut_card_config(type_one_cardConfig_t *obj, uint8_t slot)
{
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nCard config slot -------------------[%02d]",slot);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nsn:[%s]",obj->sn);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nimei:[%d]",obj->imei);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nPhase:[%02d]",obj->phase);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nis_railway_enabled:[%d]",obj->is_railway_enabled);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nis_walking_enable:[%d]",obj->is_walking_enabled);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\nis_dependent_phase:[%d]",obj->is_dependent_phase);
    debug(DEBUG_PRINTF_CARD_SEARCH, "\r\ntime_delay_dependent_phase:[%d]",obj->time_delay_dependent_phase);
    debug(DEBUG_PRINTF_CARD_SEARCH, "%s","\r\n----------------------------------------------");
}

void mtfc_printOut_pin_config(type_mtfc_phase_gate_config_t *phase_pin)
{
    for(uint8_t i = 0; i <MAX_SIDE; i++)
    {
        debug(MAIN_DEBUG,     "\r\nphase pin [%02d]: ",(i+1));
        for(uint8_t j = 0; j <8; j++)
        {
            debug(MAIN_DEBUG,     " %02d",phase_pin->pin[j]);
        }
        phase_pin++;
        debug(MAIN_DEBUG, "%s","\r\n----------------------------------"); 
    }  
}


float mtfc_time_to_float(type_hm_time_t t)
{
    float res = 0;
    res = (float)t.hour + (float)t.minute/100.0;
    return res;
}


bool mtfc_check_active_time (type_hm_time_t t_present, type_hm_time_t t_begin, type_hm_time_t t_end)
{
    float f_present = 0, f_begin = 0, f_end = 0;
    if (t_begin.hour == 255 || t_begin.minute == 255 || t_end.hour == 255 || t_end.minute == 255)
    {
        return true;
    }
    f_present = mtfc_time_to_float(t_present);
    f_begin = mtfc_time_to_float(t_begin);
    f_end = mtfc_time_to_float(t_end);
    debug(RTC_DEBUG, "%s:%f\r\n","f_present",f_present);
    debug(RTC_DEBUG, "%s:%f\r\n","f_begin",f_begin);
    debug(RTC_DEBUG, "%s:%f\r\n","f_end",f_end);
    debug(RTC_DEBUG, "%s","\r\n----------------------------------");  
    if (f_present >= f_begin && f_present <= f_end)
    {
        return true;
    }
    return false;
}

uint8_t mtfc_check_day_form(type_days_lamp_form_t *day_form, uint8_t day_present)
{
    //debug(MAIN_DEBUG, "\r\nday_form->index[day_present]:%d", day_form->index[day_present]);
    if((day_form->index[day_present] >=0) && (day_form->index[day_present] <=(MAX_TIME_FORM -1)) )
    {
        return day_form->index[day_present];
    }
    return 255;
}

uint8_t mtfc_check_time_form(type_time_lamp_form_t *time_form, uint8_t index ,type_hm_time_t t_present,type_one_timeline_t *time_line)
{
    float f_present = 0;
    float f_time_line = 0;

    time_line->t_begin_apply.hour = 255;
    time_line->t_begin_apply.minute = 255;
    time_line->index_cycle_form = 255;
    if (index >= MAX_TIME_FORM)
        return 255;
    f_present = mtfc_time_to_float(t_present);
    //debug(MAIN_DEBUG, "\r\nf_present:%f", f_present);
    for (int8_t i = (MAX_TIME_FORM - 1); i >=0; i--)
    {
        //time_form->index[index].select_point[i].t_begin_apply.hour = 8;
        //time_form->index[index].select_point[i].t_begin_apply.minute = 0;
        f_time_line = mtfc_time_to_float(time_form->index[index-1].select_point[i].t_begin_apply);
        //debug(MAIN_DEBUG, "\r\nf_time_line:%f", f_time_line);
        if (f_present >= f_time_line)
        {
            *time_line = time_form->index[index-1].select_point[i];
            return i;
        }
        //debug(MAIN_DEBUG,     "\r\nSearch [%02d]: ",i);
    }
    return 255;
}



