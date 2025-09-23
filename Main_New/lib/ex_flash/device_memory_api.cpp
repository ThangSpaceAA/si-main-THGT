#include "device_memory_api.h"
#include "debug.h"

device_memory::device_memory (PinName di_pin, PinName do_pin, PinName clk_pin, PinName cs_pin, PinName samp)
:SpiFlash25(di_pin,do_pin,clk_pin,cs_pin),SampleAdc(samp)
{
    device_fist_run = false;
}

void device_memory::mem_init(void)
{
    mem_size = SpiFlash25::read_memsize() / (1024 * 1024);
    mem_JEDEC_ID = SpiFlash25::read_id();
    if (mem_get_details() != 0) //Lan dau khoi dong
    {
        type_device_factory_details_t temp;
        temp = mem_load_default_device_details();
        mem_set_device_details_t(temp);
    }
}

type_device_factory_details_t device_memory::mem_get_device_details_t(void)
{
    return device_details;
}

void device_memory::mem_set_device_details_t(type_device_factory_details_t dat)
{
    device_details = dat;
}

int8_t device_memory::mem_get_details(void)
{
    mem_read_data(BLOCK_0,(char *)&device_details,sizeof(type_device_factory_details_t));
    // debug(MAIN_DEBUG, "got value device details : %s\r\n", device_details.firmwar_version);
    // debug(MAIN_DEBUG, "got value device details : %ld\r\n", device_details.core_clock_speed);
    // debug(MAIN_DEBUG, "got value device details : %s\r\n", device_details.imei);
    // debug(MAIN_DEBUG, "got value device details : %d\r\n", device_details.memory_size);
    // debug(MAIN_DEBUG, "got value device details : %s\r\n", device_details.license);
    if(strstr((char*)&device_details.license,"SIGNAL TECH CO.,LTD") == NULL)
    {
        device_fist_run = true;
        return -1;
    }
    return 0;
}

type_device_factory_details_t device_memory::mem_load_default_device_details(void)
{
    type_device_factory_details_t temp;
    sprintf(temp.firmwar_version,"[%s %s]",__DATE__,__TIME__);
    srand (time(NULL));
    uint32_t rannum = rand();
    if(rannum == 0)
    {
        wait(0.1);
        srand (time(NULL));
        uint32_t rannum = rand();
    }
    sprintf(temp.imei,"Si-mftc-%08X",rannum);
    sprintf(temp.license,"%s","SIGNAL TECH CO.,LTD");
    temp.core_clock_speed = SystemCoreClock;
    temp.memory_size = mem_size;
    mem_save_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    memset((uint8_t*)&temp,0,sizeof(type_device_factory_details_t));
    wait(0.5);
    mem_read_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    return temp;
}

void device_memory::mem_re_initialize_imei(void)
{
    type_device_factory_details_t temp;
    temp = mem_get_device_details_t();
    srand (time(NULL));
    uint32_t rannum = rand();
    if(rannum == 0)
    {
        srand (time(NULL));
       // srand(SampleAdc.read_u16());
        wait(0.1);
        uint32_t rannum = rand();
    }
    sprintf(temp.imei,"Si-mftc-%08X",rannum);
    mem_save_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    memset((uint8_t*)&temp,0,sizeof(type_device_factory_details_t));
    wait(0.5);
    mem_read_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    mem_set_device_details_t(temp);
}

void device_memory::mem_re_initialize_firmware(void)
{
    type_device_factory_details_t temp;
    temp = mem_get_device_details_t();
    sprintf(temp.firmwar_version, "[%s %s]", __DATE__, __TIME__);
    mem_save_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    memset((uint8_t*)&temp,0,sizeof(type_device_factory_details_t));
    wait(0.5);
    mem_read_data(BLOCK_0,(char *)&temp,sizeof(type_device_factory_details_t));
    mem_set_device_details_t(temp);
}

void device_memory::mem_save_data(int add,const char *dat,int siz)
{
    SpiFlash25::clear_sector(add);
    SpiFlash25::write(add,siz,dat);    
}

void device_memory::mem_read_data(int add,char *data, int siz )
{
    SpiFlash25::read(add,siz,data);
}

bool  device_memory::mem_is_device_fist_run(void)
{
    return device_fist_run;
}