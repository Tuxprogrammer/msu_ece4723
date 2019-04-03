#include "t7synth.h"
#include "t7sensors.h"
#include "t7menu.h"
#include "t7ecan.h"

ESOS_USER_INTERRUPT(ESOS_IRQ_PIC24_T4)
{
    write_DAC(wvform_data[u8_wvform_idx]);
    u8_wvform_idx = ++u8_wvform_idx % 128;
    ESOS_MARK_PIC24_USER_INTERRUPT_SERVICED(ESOS_IRQ_PIC24_T4);
}

void user_init()
{
    __esos_unsafe_PutString(HELLO_MSG);

    // This is all called in esos_menu_init
    // config_esos_uiF14();
    // esos_lcd44780_configDisplay();
    // esos_lcd44780_init();
    esos_menu_init();
    esos_pic24_configI2C1(400);
    configSPI1();
    __esos_ecan_hw_config_ecan();
    initialize_network();
    // initialize_network_menu_list();
    network_menu.ast_items[MY_ID].b_hidden = 0;

    esos_RegisterTask(lcd_menu);
    esos_RegisterTask(set_led);
    esos_RegisterTask(update_lm60);
    esos_RegisterTask(update_ds1631);
    esos_RegisterTask(CANFactory);
    esos_RegisterTask(ecan_receiver);
    esos_RegisterTask(ecan_beacon_network);
    esos_RegisterTask(ecan_clean_network);
    esos_RegisterTask(request_temp);
    CHANGE_MODE_ECAN1(ECAN_MODE_NORMAL);

    ESOS_REGISTER_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4, ESOS_USER_IRQ_LEVEL2, _T4Interrupt);
    CONFIG_FCNSYN_TIMER();
    ESOS_ENABLE_PIC24_USER_INTERRUPT(ESOS_IRQ_PIC24_T4);
}
