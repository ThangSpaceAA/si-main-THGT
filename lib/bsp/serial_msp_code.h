#ifndef SERIAL_MSP_CODE_H
#define SERIAL_MSP_CODE_H


//NHOM CAC LENH GUI SCHEDULE
//Gui so pha len man hinh
#define MSP_SCHEDULE_NUM_SIDE       0
#define MSP_SCHEDULE_ACTIVE_TIME    1
#define MSP_SCHEDULE_CYCLE_FORM     2
#define MSP_SCHEDULE_TIME_FORM      3
#define MSP_SCHEDULE_DAY_FORM       4

#define MSP_GATEWAY_REQUEST_DATA    5

#define MSP_QUICK_CYCLE_CONFIG      6

#define MSP_MTFC_BEGIN_STARTUP      7
#define MSP_MTFC_FINISH_STARTUP     8

//Ma lenh ket noi mtfc voi software 1 ket noi, 0 ngat ket noi
#define MSP_GATEWAY_SOFT_CONNECT		10
//Ma lenh ket noi mtf voi cloud 1 ket noi , 0 ngat ket noi
#define MSP_GATEWAY_CLOUD_CONNECT		11
//Ma phan hoi ket noi software voi cpu
#define MSP_GATEWAY_MESSAGE_CONTROL_LOG	12
//Lenh gui phan hoi loi thiet bi len server
#define MSP_GATEWAY_MESSAGE_ERROR_LOG   13
//Lenh gui phan hoi warning thiet bi len server
#define MSP_GATEWAY_MESSAGE_WARNING_LOG 14



//Truyen toa do cua thiet bi
#define MSP_COORDINATE					20
//Thong so 
#define MSP_DETAILS	     	    	    21
//Goi tin lam viec gui dinh ky 1 s 
#define MSP_ONE_SECOND_MESSAGE			22
//Goi tin truyen cau hinh cai dat part 1
#define MSP_MTFC_CONFIG_PART1			23
//Goi tin truyen cau hinh cai dat part 2
#define MSP_MTFC_CONFIG_PART2		    24
//Gui goi tin loai CPU
#define MSP_MTFC_TYPE_CPU		    	25

//Goi tin dieu khien mtfc gui tu app
#define MSP_APP_MESS_CONTROL            30

//Goi tin dong bo thoi gian tu app
#define MSP_APP_SYN_RTC_TIME            40
#define MSP_APP_ACTIVE_TIME             41

//Truyen du lieu Cycle form
#define MSP_APP_DAYS_FORM               42//Gui dau tien
#define MSP_APP_TIME_FORM               43//Gui thu 2
#define MSP_APP_CYCLE_FORM              44 //Gui 32 lan moi lan delay 200ms
#define MSP_APP_CYCLE_IDX               45






//---------------------------------------GIAO TIEP VOI CARD----------------------------------
//Xuat du lieu ra card
#define MSP_CARD_WRITE_DATA     	0
//Du lieu cam bien ngo ra card cong suat
#define MSP_CARD_FB_DATA        	1
//Lenh doc cai dat tu card
#define MSP_CARD_READ_CONFIG    	2
//Lenh ghi cai dat cho card
#define MSP_CARD_WRITE_CONFIG   	3
//Lenh xoa cai dat cua card
#define MSP_CARD_DELETE         	4
//Phan hoi cai dat cua card
#define MSP_CARD_FB_CONFIG      	5
//Thong so cam bien cua card
#define MSP_CARD_DATA_SENSOR        6
//Ma lenh yeu cau lay du lieu cua card 
#define MSP_CARD_CHECK_SENSOR       7 

//--------------------------------------GIAO TIEP VOI MAN HINH-----------------------------
//Gui lenh console len man hinh hien thi
#define MSP_GLCD_CONSOLE        	0
#define MSP_MTFC_NOTIFY_STARTUP     1
#define MSP_STARTED_MAIN_MCU        2

#define MSP_GLCD_PAGE_IDX	    	9
#define MSP_GLCD_PAGE_ONE	    	10
#define MSP_GLCD_PAGE_TWO	    	11
#define MSP_GLCD_PAGE_IV	    	12
#define MSP_GLCD_PAGE_V	    	    13
#define MSP_GLCD_PAGE_VI	    	14

#define MSP_TIME_LOCATION_LOAD	    20
#define MSP_TIME_LOCATION_UPDATE	21
#define MSP_TIME_LOCATION_SYN	    22

#define MSP_CYCLE_FORM_LOAD			25
#define MSP_CYCLE_FORM_UPDATE		26
#define MSP_CYCLE_FORM_BEGIN_TRANS	27
#define MSP_CYCLE_FORM_DEFAUTF	    28


#define MSP_TIME_FORM_LOAD	        30
#define MSP_TIME_FORM_UPDATE	    31

#define MSP_DAY_FORM_LOAD	        35
#define MSP_DAY_FORM_UPDATE	        36

#define MSP_CARD_CONFIG_LOAD	    40
#define MSP_CARD_CONFIG_UPDATE	    41
#define MSP_CARD_CONFIG_SEARCH	    42
#define MSP_CARD_CONFIG_UPDATE_SLOT	43


#define MSP_PIN_CONFIG_LOAD	        45
#define MSP_PIN_CONFIG_UPDATE	    46

#define MSP_CONNECTION_LOAD	        50
#define MSP_CONNECTION_UPDATE	    51

#define MSP_IMPORT_SCHEDULE	        55
#define MSP_EXPORT_SCHEDULE	        56

#define MSP_DEVICE_LOAD	            60
#define MSP_DEVICE_UPDATE	        61
#define MSP_DEVICE_DEFAULT_SCHEDULE	62

#define MSP_DETAILS_LOAD	        65

#define MSP_ACTIVE_TIME_LOAD        70
#define MSP_ACTIVE_TIME_UPDATE	    71

#define MSP_LAMP485_CONFIG          80

#define MSP_GPS_GATEWAY             160
#define MSP_KEEPALIVE_PQ            255

//--------------------------------------GIAO TIEP CARD COMMUNICATION-----------------------------

#define MSP_MTFC_TEMPRATURE         147
#define MSP_SD_MOUNT                148
#define MSP_CHECK_MODULE            149
#define MSP_IMPORT_SCHEDULE_SD      150
#define MSP_EXPORT_SCHEDULE_SD      151
#define MSP_IMPORT_ERROR_FB         152
#define MSP_EXPORT_ERROR_FB         153
#define MSP_EX_COM_CONSOLE          154
#define MSP_GET_GPS_EX              155
#define MSP_GET_GPS_MESSAGE         156
#define MSP_GPS_EX_DATA             157


/////////////CPU moi/////////////////////////////

#define CMD_CPU_TO_MASTER_SOPHA                     9
#define CMD_CPU_TO_MASTER_TIME_ACTIVE               11
#define CMD_CPU_TO_MASTER_CYCLE_FORM                12
#define CMD_CPU_TO_MASTER_SET_RTC_MANUAL            16
#define CMD_MASTER_TO_CPU_CARD_INFOR                17
#define CMD_MASTER_TO_CPU_MODE_ACTIVE	          	19
#define CMD_MASTER_TO_CPU_SELECT_PHASE	            20
#define CMD_CPU_TO_MASTER_MODE_CROSS          		24
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_1       		27
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_2       		28
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_3       		29
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_4       		30
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_5        	31
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_6            32
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_7            33
#define CMD_CPU_TO_MASTER_CHANGE_PHASE_8            34
#define CMD_QT_TO_CPU_SCHEDULE                      35
#define CMD_CPU_TO_MASTER_SCHEDULE                  36
#define CMD_MASTER_TO_CPU_SCHEDULE_TIME             37
#define CMD_MASTER_TO_CPU_SCHEDULE_DAYS             38
#define CMD_CPU_TO_MASTER_SCHEDULE_TIME_UPDATE      42
#define CMD_MASTER_TO_CPU_SCHEDULE_CYCLE            43
#define CMD_QT_TO_CPU_SCHEDULE_DAYS_UPDATE		    47
#define CMD_CPU_TO_MASTER_SCHEDULE_DAYS_UPDATE	    48
#define CMD_QT_TO_CPU_TIME_SETTING				    49
#define CMD_CPU_TO_MASTER_TIME_SETTING			    50
#define CMD_MASTER_TO_SLAVE_TIME_SETTING		    51
#define CMD_MASTER_TO_CPU_CHECK_CARD                52
#define CMD_MASTER_TO_CPU_CURRENT                   54
#define CMD_MASTER_TO_CPU_FB                  		56
#define CMD_CPU_TO_QT_FB	                  		57
#define CMD_MASTER_TO_CPU_FB_DAY_FORM              	58
#define CMD_MASTER_TO_CPU_FB_TIME_FORM            	59
#define CMD_MASTER_TO_CPU_FB_CYCLE_FORM             60
#define CMD_MASTER_TO_CPU_PHASE                     66
#define CMD_MASTER_TO_CPU_FB_CARD_CONFIG            67
#define CMD_CPU_TO_MASTER_START						68
#define CMD_MASTER_TO_CPU_RAILWAY_SIGNAL             72
#define CMD_MASTER_TO_CPU_WALKING_SIGNAL             73
#endif
