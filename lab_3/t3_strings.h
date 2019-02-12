/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t3_strings.h - strings used by t3_app menu and event notifications
 */

#ifndef T3_STRINGS
#define T3_STRINGS

#pragma region Notifications
static char str_e_SW1_PRESSED[] = "SW1 Pressed\n";
static char str_e_SW2_PRESSED[] = "SW2 Pressed\n";
static char str_e_SW3_PRESSED[] = "SW3 Pressed\n";
static char str_e_LED1_ON[] = "LED1 ON\n";
static char str_e_LED2_ON[] = "LED2 ON\n";
static char str_e_LED3_ON[] = "LED3 ON\n";
static char str_e_RPG_TURNING[] = "RPG Turning\n";
static char str_e_RPG_CW[] = "RPG CW\n";
static char str_e_RPG_CCW[] = "RPG CCW\n";
static char str_e_RPG_SLOW[] = "RPG SLOW\n";
static char str_e_RPG_MEDIUM[] = "RPG MEDIUM\n";
static char str_e_RPG_FAST[] = "RPG FAST\n";
static char str_e_LAST_RPG_COUNTER[] = "RPG LAST COUNTER\n";
static char str_e_RPG_COUNTER[] = "RPG COUNTER\n";
static char str_e_RPG_VELOCITY[] = "RPG VELOCITY\n";

#pragma endregion

#pragma region Menu

// Menu States
enum MENU_STATE {
    TOP_MENU,
    DBL_PRESS_SETTINGS,
    RPG_THRESH_SETTINGS,
    SW1_PERIOD,
    SW2_PERIOD,
    SW3_PERIOD,
    RPG_SLOW,
    RPG_MED,
    RPG_FAST
};

// Top Level Menu Items
static char str_m_DBL_PRESS_SETTINGS[] = "1: Double Press Settings\n";
static char str_m_RPG_THRESH_SETTINGS[] = "2: RPG Threshold Settings\n";

// Double Press Settings Menu Items
static char str_mdp_SW1_PERIOD[] = "1: SW1 Double Press Period\n";
static char str_mdp_SW2_PERIOD[] = "2: SW2 Double Press Period\n";
static char str_mdp_SW3_PERIOD[] = "3: SW3 Double Press Period\n";

// RPG Threshold Settings Menu Items
static char str_mrpg_RPG_SLOW[] = "1: RPG Slow Threshold\n";
static char str_mrpg_RPG_MED[] = "2: RPG Medium Threshold\n";
static char str_mrpg_RPG_FAST[] = "3: RPG Fast Threshold\n";

// SW Period Settings
static char str_sw1p_CUR_VALUE[] = "SW1 Double Press Period (+/-): ";
static char str_sw2p_CUR_VALUE[] = "SW2 Double Press Period (+/-): ";
static char str_sw3p_CUR_VALUE[] = "SW3 Double Press Period (+/-): ";

// RPG Threshold Settings
static char str_rpgs_CUR_VALUE[] = "RPG Slow Threshold (+/-): ";
static char str_rpgm_CUR_VALUE[] = "RPG Medium Threshold (+/-): ";
static char str_rpgf_CUR_VALUE[] = "RPG Fast Threshold (+/-): ";

// Menu Common
#define ESC_KEY 0x1B
static char str_m_BACK[] = "ESC: Back\n";
static char str_m_SEPARATOR[] = "------------------------------\n";

// Key Bindings
#define ESC_KEY 0x1B
#define PLUS_KEY 0x2B
#define MINUS_KEY 0x2D

#pragma endregion

#endif