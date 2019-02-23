/* Embedded Systems - Spring 2019

 * Christian Bush (cbb330@msstate.edu), Spencer Callicott (sc2257@msstate.edu)
 * Will Carroll (woc17@msstate.edu), Landon Casey (lec426@msstate.edu)
 * Jack Fletcher (jdf469@msstate.edu)
 *
 * t4_strings.h - strings used by t3_app menu and event notifications
 */

#ifndef T4_STRINGS
#define T4_STRINGS

#pragma region Menu

// Top Level Menu Items
static char str_m_PROCESS_SETTINGS[] = "1: Processing Mode Settings\n";
static char str_m_SAMPLE_SETTINGS[] = "2: Sample Settings\n";

// ADC Processing Settings Menu Items
static char str_madc_PROCESS_ONE[] = "1: One-Shot\n";
static char str_madc_PROCESS_AVG[] = "2: Average\n";
static char str_madc_PROCESS_MIN[] = "3: Minimum\n";
static char str_madc_PROCESS_MAX[] = "4: Maximum\n";
static char str_madc_PROCESS_MED[] = "5: Median\n";

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