/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t3_strings.h - strings used by t3_app menu and event notifications
 */

#pragma region Notifications
static char str_e_SW1_PRESSED[]        = "SW1 Pressed\n";
static char str_e_SW2_PRESSED[]        = "SW2 Pressed\n";
static char str_e_SW3_PRESSED[]        = "SW3 Pressed\n";
static char str_e_LED1_ON[]            = "LED1 ON\n";
static char str_e_LED2_ON[]            = "LED2 ON\n";
static char str_e_LED3_ON[]            = "LED3 ON\n";
static char str_e_RPG_TURNING[]        = "RPG Turning\n";
static char str_e_RPG_CW[]             = "RPG CW\n";
static char str_e_RPG_CCW[]            = "RPG CCW\n";
static char str_e_RPG_SLOW[]           = "RPG SLOW\n";
static char str_e_RPG_MEDIUM[]         = "RPG MEDIUM\n";
static char str_e_RPG_FAST[]           = "RPG FAST\n";
static char str_e_LAST_RPG_COUNTER[]   = "RPG LAST COUNTER\n";
static char str_e_RPG_COUNTER[]        = "RPG COUNTER\n";
static char str_e_RPG_VELOCITY[]       = "RPG VELOCITY\n";

#pragma endregion

#pragma region Menu

// Menu States
enum MENU_STATE { TOP_MENU, 
                  DBL_PRESS_SETTINGS, 
                  LED_FLASH_SETTINGS, 
                  RPG_THRESH_SETTINGS,
                  SW1_PERIOD,
                  SW2_PERIOD,
                  SW3_PERIOD };

// Top Level Menu Items
static char str_m_DBL_PRESS_SETTINGS[] = "1: Double Press Settings\n";
static char str_m_LED_FLASH_SETTINGS[] = "2: LED Flash Settings\n";
static char str_m_RPG_THRESH_SETTINGS[] = "3: RPG Threshold Settings\n";

// Double Press Settings Menu Items
static char str_mdp_SW1_PERIOD[]       = "1: SW1 Period\n";
static char str_mdp_SW2_PERIOD[]       = "2: SW2 Period\n";
static char str_mdp_SW3_PERIOD[]       = "3: SW3 Period\n";

// LED Flash Settings Menu Items
static char str_mled_LED1_PERIOD[]     = "1: LED1 Flash Period\n";
static char str_mled_LED2_PERIOD[]     = "2: LED2 Flash Period\n";
static char str_mled_LED3_PERIOD[]     = "3: LED3 Flash Period\n";

// RPG Threshold Settings
static char str_mrpg_RPG_SLOW[]        = "1: RPG Slow Threshold\n";
static char str_mrpg_RPG_MED[]         = "2: RPG Med Threshold\n";
static char str_mrpg_RPG_FAST[]        = "3: RPG Fast Threshold\n";

// SW1 Period Settings
static char str_sw1p_CUR_VALUE[]       = "SW1 Double Press Period (+/-): ";
static char str_sw1p_NEW_VALUE[]       = "(ESC to go back)\n";

// Menu Common
#define ESC_KEY 0x1B
static char str_m_INSTR[]              = "Make a Selection: ";
static char str_m_BACK[]               = "ESC: Back\n";
static char str_m_SEPARATOR[]          = "------------------------------\n";

#pragma endregion
