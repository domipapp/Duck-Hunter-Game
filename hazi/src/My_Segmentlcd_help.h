
/*****************************************************************************
 * Ez a fájl tartalmazza a szükséges defineokat és struktűra létrehozásokat
 * amelyek lehetővé teszik a 7 szegmens egyszerű kezelését
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup SegmentLcd
 * @brief Segment LCD driver
 *
 * @details
 *  This driver implements symbol and string write support for EFM32 kits with
 *  segment LCD.
 * @{
 ******************************************************************************/

#if defined(LCD_MODULE_CE322_1001)
#define NUM_DIGITS              6
#define NUM_VAL_MAX             999999
#define NUM_VAL_MIN             -99999
#define NUM_VAL_MAX_HEX         0xFFFFFF
#define ALPHA_DIGITS            8
#define ALPHA_VAL_MAX           99999999
#define ALPHA_VAL_MIN           -9999999
#else
#define NUM_DIGITS              4
#define NUM_VAL_MAX             9999
#define NUM_VAL_MIN             -999
#define NUM_VAL_MAX_HEX         0xFFFF
#define ALPHA_DIGITS            7
#define ALPHA_VAL_MAX           9999999
#define ALPHA_VAL_MIN           -9999999
#endif

/**************************************************************************//**
 * @brief
 * Defines each text symbol's segment in terms of COM and BIT numbers,
 * in a way that we can enumerate each bit for each text segment in the
 * following bit pattern:
 * @verbatim
 *  -------0------
 *
 * |   \7  |8  /9 |
 * |5   \  |  /   |1
 *
 *  --6---  ---10--
 *
 * |    /  |  \11 |
 * |4  /13 |12 \  |2
 *
 *  -------3------
 * @endverbatim
 * E.g.: First text character bit pattern #3 (above) is
 * Segment 1D for Display
 * Location COM 3, BIT 0
 *****************************************************************************/
typedef struct {
  uint8_t com[14]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[14]; /**< LCD bit number */
} CHAR_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields numeric display
 *****************************************************************************/
typedef struct {
  uint8_t com[7]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[7]; /**< LCD bit number */
} NUMBER_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for Energy Modes on display
 *****************************************************************************/
typedef struct {
  uint8_t com[5]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[5]; /**< LCD bit number */
} EM_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for A-wheel (suited for Anim)
 *****************************************************************************/
typedef struct {
  uint8_t com[8]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[8]; /**< LCD bit number */
} ARING_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for Battery (suited for Anim)
 *****************************************************************************/
typedef struct {
  uint8_t com[4]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[4]; /**< LCD bit number */
} BATTERY_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for array
 *****************************************************************************/
typedef struct {
  uint8_t com[35]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[35]; /**< LCD bit number */
} ARRAY_TypeDef;

/**************************************************************************//**
 * @brief Defines segment COM and BIT fields for top and bottom row blocks.
 * The bit pattern shown above for characters can be split into upper and lower
 * portions for animation purposes. There are separate COM and BIT numbers
 * which together represent a set of stacked blocks which can be shown on two
 * rows in the segmented LCD screen.
 *
 * There are four blocks shown on the top row:
 * @verbatim
 *  -------0------
 *
 * |   \5  |6  /7 |
 * |2   \  |  /   |1
 *
 *  --3---  ---4--
 * @endverbatim
 *
 * There are four blocks shown on the bottom row :
 * @verbatim
 *  --3---  ---4--
 *
 * |    /  |  \5  |
 * |2  /7  |6  \  |0
 *
 *  -------1------
 * @endverbatim
 *
 * The upper row has four blocks which are placed directly above the four
 * blocks on the lower row. Each block can be in one of three states:
 * blank - outer five segments off, inner three segments off
 * outer - outer five segments on,  inner three segments off
 * inner - outer five segments off, inner three segments on
 * full  - outer five segments on,  inner three segments on
 *
 * @verbatim
 * Top row:    0 1 2 3 4 5 6 7
 * Bottom row: 0 1 2 3 4 5 6 7
 * @endverbatim
 *****************************************************************************/
typedef struct {
  uint8_t com[8]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[8]; /**< LCD bit number */
} Block_TypeDef;

#if defined(_SILICON_LABS_32B_SERIES_0)
typedef struct {
  CHAR_TypeDef    Text[ALPHA_DIGITS];      /**< Text on display */
  NUMBER_TypeDef  Number[NUM_DIGITS];      /**< Numbers on display */
  EM_TypeDef      EMode;                   /**< Display energy mode */
  ARING_TypeDef   ARing;                   /**< Display ring */
  BATTERY_TypeDef Battery;                 /**< Display battery */
  Block_TypeDef   TopBlocks[ALPHA_DIGITS]; /**< Display top blocks */
  Block_TypeDef   BotBlocks[ALPHA_DIGITS]; /**< Display bottom blocks */
} MCU_DISPLAY;
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
typedef struct {
  CHAR_TypeDef    Text[ALPHA_DIGITS];      /**< Text on display */
  NUMBER_TypeDef  Number[NUM_DIGITS];      /**< Numbers on display */
  ARRAY_TypeDef   Array;                   /**< Display array */
  Block_TypeDef   TopBlocks[ALPHA_DIGITS]; /**< Display top blocks */
  Block_TypeDef   BotBlocks[ALPHA_DIGITS]; /**< Display bottom blocks */
} MCU_DISPLAY;
#endif
