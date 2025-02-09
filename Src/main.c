/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SLIX2_BLOCKS         80
#define SLIX2_INVENTORY_LEN   9
#define SLIX2_SYSINFO_LEN    14
#define SLIX2_SIGNATURE_LEN  32

/////////////////////////////////////////////////////////
// DMO emulation data (default emulation after power up)
//

//choose between one of the dumped SKU (roll types) for emulation
#define DMO_SKU  S0722430 // 54 x 101 mm, 220 pcs.
//#define DMO_SKU  S0722550 // 19 x  51 mm, 500 pcs.
//#define DMO_SKU  S0722400 // 36 x  89 mm, 50 pcs.
//#define DMO_SKU  1744907  // 102 x 152 mm, 220 pcs.

//choose one of the dumped original tags for UID + signature emulation. It does not have to match the dumped data
#define SLIX2_TAG_EMU 1
//#define SLIX2_TAG_EMU 2
//#define SLIX2_TAG_EMU 3
//#define SLIX2_TAG_EMU 4

#if SLIX2_TAG_EMU == 1
static const uint8_t DMO_TAG_SLIX2_INVENTORY[SLIX2_INVENTORY_LEN] = {0x01,0xBA,0x6C,0x60,0x3D,0x08,0x01,0x04,0xE0};
static const uint8_t DMO_TAG_SLIX2_SYSINFO[SLIX2_SYSINFO_LEN]     = {0x0F,0xBA,0x6C,0x60,0x3D,0x08,0x01,0x04,0xE0,0x01,0x3D,0x4F,0x03,0x01};
static const uint8_t DMO_TAG_SLIX2_SIGNATURE[SLIX2_SIGNATURE_LEN] = {0x33,0x4A,0x63,0x63,0xD0,0x13,0x49,0xDB,0xA0,0x9E,0xEE,0x15,0x1E,0xF8,0xF8,0xF3,0xFA,0x15,0xF5,0x77,0xE4,0x4D,0x75,0x9B,0x78,0x14,0xCA,0xD3,0x7E,0x02,0xEF,0x10};
#elif SLIX2_TAG_EMU == 2
static const uint8_t DMO_TAG_SLIX2_INVENTORY[SLIX2_INVENTORY_LEN] = {0x01,0x36,0x50,0x93,0x44,0x08,0x01,0x04,0xE0};
static const uint8_t DMO_TAG_SLIX2_SYSINFO[SLIX2_SYSINFO_LEN]     = {0x0F,0x36,0x50,0x93,0x44,0x08,0x01,0x04,0xE0,0x01,0x3D,0x4F,0x03,0x01};
static const uint8_t DMO_TAG_SLIX2_SIGNATURE[SLIX2_SIGNATURE_LEN] = {0x63,0x3D,0xFD,0xD9,0x2E,0xF4,0xEC,0xDC,0xD0,0xF0,0x62,0xF2,0xE3,0x12,0x15,0x66,0x54,0xC7,0xA0,0xEA,0xEB,0x95,0xD2,0x60,0x48,0xAA,0x1D,0x9A,0x4F,0x66,0x2B,0xD7};
#elif SLIX2_TAG_EMU == 3
static const uint8_t DMO_TAG_SLIX2_INVENTORY[SLIX2_INVENTORY_LEN] = {0x01,0xA0,0xC6,0x7D,0x3D,0x08,0x01,0x04,0xE0};
static const uint8_t DMO_TAG_SLIX2_SYSINFO[SLIX2_SYSINFO_LEN]     = {0x0F,0xA0,0xC6,0x7D,0x3D,0x08,0x01,0x04,0xE0,0x01,0x3D,0x4F,0x03,0x01};
static const uint8_t DMO_TAG_SLIX2_SIGNATURE[SLIX2_SIGNATURE_LEN] = {0x0E,0x0F,0x76,0x62,0x33,0xB6,0x5B,0xA8,0xA0,0xAE,0x61,0xB4,0x57,0x23,0x51,0x4D,0xF5,0x12,0x5B,0x2C,0xFC,0x73,0xFD,0x0C,0xBA,0x4A,0x01,0x79,0xA9,0x53,0x2C,0x21};
#elif SLIX2_TAG_EMU == 4
static const uint8_t DMO_TAG_SLIX2_INVENTORY[SLIX2_INVENTORY_LEN] = {0x01,0x5C,0x61,0x8E,0x3D,0x08,0x01,0x04,0xE0};
static const uint8_t DMO_TAG_SLIX2_SYSINFO[SLIX2_SYSINFO_LEN]     = {0x0F,0x5C,0x61,0x8E,0x3D,0x08,0x01,0x04,0xE0,0x01,0x3D,0x4F,0x03,0x01};
static const uint8_t DMO_TAG_SLIX2_SIGNATURE[SLIX2_SIGNATURE_LEN] = {0xA2,0x8F,0x44,0x95,0x27,0xEC,0x27,0x85,0x92,0xC4,0x28,0x42,0x97,0x87,0xC8,0x27,0xCD,0x29,0x9A,0xFA,0x67,0x04,0x14,0xE5,0x4A,0xD7,0x11,0xEF,0x36,0x40,0xD8,0x10};
#endif

#if DMO_SKU == S0722430
static const uint8_t DMO_TAG_SLIX2_BLOCKS[SLIX2_BLOCKS*sizeof(uint32_t)] = { //S0722430 54 x 101 mm, 220 pcs.
  0x03,0x0A,0x82,0xED,0x86,0x39,0x61,0xD2,0x03,0x14,0x1E,0x32,0xB6,0xCA,0x00,0x3C,0x27,0xB3,0x98,0xBA,0x53,0x30,0x37,0x32,0x32,0x34,0x33,0x30,0x00,0x00,0x00,0x00,0x00,0xFF,0x04,0x01,0x01,0x00,0x00,0x00,
  0x22,0x04,0x1E,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0xF0,0x03,0x1C,0x02,0x00,0x00,0x00,0x00,0x46,0x02,0xDC,0x00,0x78,0x2D,0x16,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xD7,0xFA,0x00,0x1C,0x10,0x17,0x6E,0x3B,0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0xA9,0x75,0x6B,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x32,0x8C,0x00,0x30,0xBA,0x47,0x99,0xB3,0x00,0x00,0x00,0x00,0xAC,0x87,0xFF,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x11,0xF3,0x00,0x2C,0xDD,0xC3,0x3E,0x91,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0xFF,0x00,0x01
};
#elif DMO_SKU == S0722550
static const uint8_t DMO_TAG_SLIX2_BLOCKS[SLIX2_BLOCKS*sizeof(uint32_t)] = { //S0722550 19 x 51 mm, 500 pcs.
  0x03,0x0A,0x82,0xED,0x86,0x39,0x61,0xD2,0x03,0x14,0x1E,0x32,0xB6,0xCA,0x00,0x3C,0xEE,0x0D,0xBF,0xDF,0x53,0x30,0x37,0x32,0x32,0x35,0x35,0x30,0x00,0x00,0x00,0x00,0x00,0xFF,0x06,0x01,0x01,0x00,0x00,0x00,
  0x29,0x02,0x1E,0x00,0x25,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0xFC,0x01,0x17,0x01,0x00,0x00,0x00,0x00,0x37,0x01,0xF4,0x01,0xF5,0x35,0x32,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xD7,0xFA,0x00,0x1C,0x20,0x61,0xF3,0x3C,0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x91,0x47,0x14,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x32,0x8C,0x00,0x30,0x00,0xD5,0x75,0xCA,0x00,0x00,0x00,0x00,0x8D,0xBB,0x3A,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x11,0xF3,0x00,0x2C,0xDD,0xC3,0x3E,0x91,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD9,0xFD,0x00,0x01
};
#elif DMO_SKU == S0722400
static const uint8_t DMO_TAG_SLIX2_BLOCKS[SLIX2_BLOCKS*sizeof(uint32_t)] = { //S0722400 36 x 89 mm, 50 pcs.
  0x03,0x0A,0x82,0xED,0x86,0x39,0x61,0xD2,0x03,0x14,0x1E,0x32,0xB6,0xCA,0x00,0x3C,0x36,0x42,0x0C,0x33,0x53,0x30,0x37,0x32,0x32,0x34,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0xFF,0x04,0x01,0x01,0x00,0x00,0x00,
  0xA3,0x03,0x1E,0x00,0x26,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x76,0x03,0x65,0x01,0x00,0x00,0x00,0x00,0x85,0x01,0x34,0x00,0x75,0x09,0x05,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xD7,0xFA,0x00,0x1C,0x14,0xC2,0x5D,0xBC,0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x3D,0x3C,0xEA,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x32,0x8C,0x00,0x30,0x3E,0x50,0xEC,0x31,0x00,0x00,0x00,0x00,0x2D,0x07,0xA6,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x11,0xF3,0x00,0x2C,0xDD,0xC3,0x3E,0x91,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0xFF,0x00,0x01
};
#elif DMO_SKU == 1744907
static const uint8_t DMO_TAG_SLIX2_BLOCKS[SLIX2_BLOCKS*sizeof(uint32_t)] = { //1744907 102 x 152 mm, 220 pcs.
  0x03,0x0A,0x82,0xED,0x86,0x39,0x61,0xD2,0x03,0x14,0x1E,0x32,0xB6,0xCA,0x00,0x3C,0x86,0x50,0xB6,0x72,0x31,0x37,0x34,0x34,0x39,0x30,0x37,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x24,0x01,0x01,0x00,0x00,0x00,
  0x73,0x06,0x1E,0x00,0x26,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x3A,0x06,0x14,0x04,0x00,0x00,0x00,0x00,0x33,0x04,0xDC,0x00,0xF1,0x46,0x16,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xD7,0xFA,0x00,0x1C,0x72,0xB2,0x23,0x87,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x85,0x30,0xE1,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x32,0x8C,0x00,0x30,0x67,0x63,0xC8,0x4D,0x00,0x00,0x00,0x00,0x2C,0x16,0x75,0xF5,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x11,0xF3,0x00,0x2C,0xDD,0xC3,0x3E,0x91,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0xFF,0x00,0x01
};
#endif

/////////////////////////////////////////////////////////
// I2C to real CLRC688 for reading presented tags
//

#define I2C_TIMEOUT 100
#define CLRC688_ADDR (0x28 << 1)

bool CLRC688_WriteRegister(I2C_HandleTypeDef* phi2c, const uint8_t addr, const uint8_t reg, const uint8_t val) {
  return( HAL_OK == HAL_I2C_Mem_Write(phi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&val, sizeof(uint8_t), I2C_TIMEOUT) );
}

bool CLRC688_ReadRegister(I2C_HandleTypeDef* phi2c, const uint8_t addr, const uint8_t reg, uint8_t* pval) {
  return( HAL_OK == HAL_I2C_Mem_Read(phi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, pval, sizeof(uint8_t), I2C_TIMEOUT) );
}

bool CLRC688_Transceive(I2C_HandleTypeDef* phi2c, const uint8_t addr, const uint8_t* send, const uint8_t sendlen, uint8_t* recv, uint8_t* precvlen) {
  if( 
      !CLRC688_WriteRegister(phi2c, addr, 0x0F,0x11) ||
      !CLRC688_WriteRegister(phi2c, addr, 0x10,0xFF) ||
      !CLRC688_WriteRegister(phi2c, addr, 0x11,0xFF) ||
      !CLRC688_WriteRegister(phi2c, addr, 0x08,0x04) ||    //setup IRQ0EN
      !CLRC688_WriteRegister(phi2c, addr, 0x09,0x41) ||    //setup IRQ1EN
      !CLRC688_WriteRegister(phi2c, addr, 0x06,0x7F) ||    //clear all IRQ0
      !CLRC688_WriteRegister(phi2c, addr, 0x07,0x7F) ||    //clear all IRQ1
      !CLRC688_WriteRegister(phi2c, addr, 0x00,0x00) ||    //execute command nop/clear
      !CLRC688_WriteRegister(phi2c, addr, 0x02,0x10)       //FIFOCtrl
    ) {
    return false;
  }

  if( HAL_OK != HAL_I2C_Mem_Write(phi2c, addr, 0x05, I2C_MEMADD_SIZE_8BIT, (uint8_t*)send, sendlen, I2C_TIMEOUT) ) {
    return false;
  }

  if( !recv || !precvlen)
    return true;

  if( !CLRC688_WriteRegister(phi2c, addr, 0x00,0x07) ) {                      //execute command: transmit to + receive from tag
    return false;
  }

  for( uint32_t ticks = HAL_GetTick() + I2C_TIMEOUT; ticks>HAL_GetTick(); ) { //poll IRQ signal via reading IRQ1 register (also could use IRQ line instead...)
    uint8_t r7;
    if( !CLRC688_ReadRegister(phi2c, addr, 0x07, &r7) ) {
      return false;
    }
    
    if( r7 & 0x40 )
      break;

    HAL_Delay(10);
  }

  uint8_t r0A;
  if( !CLRC688_ReadRegister(phi2c, addr, 0x0A, &r0A) || (0 != r0A) ) {  //read and check error register
    return false;
  }
  
  uint8_t r04;
  if( !CLRC688_ReadRegister(phi2c, addr, 0x04, &r04) ) {                //read response length
    return false;
  }

  if( r04 > (*precvlen+1) )
    return false;

  uint8_t tmp[256] = {0xFF};
  if( (HAL_OK == HAL_I2C_Mem_Read(phi2c, addr, 0x05, I2C_MEMADD_SIZE_8BIT, tmp, r04, I2C_TIMEOUT)) && (0x00==tmp[0]) ) {
    if( r04>1 ) {
      memcpy(recv, tmp+1, r04-1);
    }
    *precvlen = r04-1;
    return true;
  }

  *precvlen=0;
  return false;
}

bool CLRC688_Init(I2C_HandleTypeDef* phi2c, const uint8_t addr) {
  HAL_GPIO_WritePin(OUT_PWDN_READER_GPIO_Port, OUT_PWDN_READER_Pin, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(OUT_PWDN_READER_GPIO_Port, OUT_PWDN_READER_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);

  const uint8_t zerobuf[] = {0x00,0x00};

  if( !CLRC688_WriteRegister(phi2c, addr, 0x43,0x40) ||    //LPCD_Q_Result
      !CLRC688_WriteRegister(phi2c, addr, 0x02,0x10) ||    //FIFOCtrl
      !CLRC688_WriteRegister(phi2c, addr, 0x03,0xFE) ||    //FIFOWaterLevel
      !CLRC688_WriteRegister(phi2c, addr, 0x0C,0x80) ||    //RxBitCtrl
      !CLRC688_WriteRegister(phi2c, addr, 0x28,0x80) ||    //DrvMode
      !CLRC688_WriteRegister(phi2c, addr, 0x2A,0x01) ||    //DrvCon
      !CLRC688_WriteRegister(phi2c, addr, 0x2B,0x05) ||    //Txl
      !CLRC688_WriteRegister(phi2c, addr, 0x34,0x00) ||    //RxSofD
      !CLRC688_WriteRegister(phi2c, addr, 0x38,0x12) ||    //RxAna
      !CLRC688_Transceive(phi2c, addr, zerobuf, sizeof(zerobuf), NULL, NULL) ||
      !CLRC688_WriteRegister(phi2c, addr, 0x00,0x0D) ||    //load protocol
      !CLRC688_WriteRegister(phi2c, addr, 0x2C,0x7B) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x2D,0x7B) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x2E,0x08) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x2F,0x00) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x30,0x00) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x31,0x00) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x33,0x0F) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x35,0x02) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x37,0x4E) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x39,0x07) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x36,0x8C) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x31,0xC0) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x32,0x00) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x29,0x10) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x28,0x81) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x08,0x00) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x28,0x89) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x00,0x40) ||   
      !CLRC688_WriteRegister(phi2c, addr, 0x00,0x00) 
    ) {
    HAL_GPIO_WritePin(OUT_PWDN_READER_GPIO_Port, OUT_PWDN_READER_Pin, GPIO_PIN_SET);
    return false;
  }

  return true;
}

void CLRC688_DeInit(I2C_HandleTypeDef* phi2c) {
  HAL_GPIO_WritePin(OUT_PWDN_READER_GPIO_Port, OUT_PWDN_READER_Pin, GPIO_PIN_SET);
}

bool CLRC688_ReadOutSLIX2(I2C_HandleTypeDef* phi2c, const uint8_t addr, uint8_t inventory[SLIX2_INVENTORY_LEN], uint8_t sysinfo[SLIX2_SYSINFO_LEN], uint8_t signature[SLIX2_SIGNATURE_LEN], uint32_t blocks[SLIX2_BLOCKS]) {
  static const uint8_t inventorycmd[] = {0x36,0x01,0x00,0x00};
  uint8_t inventorylen = SLIX2_INVENTORY_LEN;
  if( !CLRC688_Transceive(phi2c, addr, inventorycmd, sizeof(inventorycmd), inventory, &inventorylen) || (inventorylen<9) ) {
    return false;
  }  
  uint8_t uid[] = {inventory[1],inventory[2],inventory[3],inventory[4],inventory[5],inventory[6],inventory[7],inventory[8]};

  uint8_t sysinfocmd[] = {0x22,0x2B,uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]};
  uint8_t sysinfolen = SLIX2_SYSINFO_LEN;
  if( !CLRC688_Transceive(phi2c, addr, sysinfocmd, sizeof(sysinfocmd), sysinfo, &sysinfolen) || (sysinfolen<14)) {
    return false;
  }  

  uint8_t signaturecmd[] = {0x22,0xBD,0x04,uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]};
  uint8_t signaturelen = SLIX2_SIGNATURE_LEN;
  if( !CLRC688_Transceive(phi2c, addr, signaturecmd, sizeof(signaturecmd), signature, &signaturelen) || (signaturelen<32)) {
    return false;
  }  

  for(uint8_t block=0; block<SLIX2_BLOCKS; block+=20){
    uint8_t readblockscmd[] = {0x22,0x23,uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7],block,20-1};
    uint8_t readblockslen = 20*sizeof(uint32_t);
    if( !CLRC688_Transceive(phi2c, addr, readblockscmd, sizeof(readblockscmd), (uint8_t*)&blocks[block], &readblockslen) || (readblockslen!=20*sizeof(uint32_t))) {
      return false;
    }
  }

  return true;
}

bool CLRC688_CheckPresense(I2C_HandleTypeDef* phi2c, const uint8_t addr, uint8_t inventory[SLIX2_INVENTORY_LEN]) {
  uint8_t uid[] = {inventory[1],inventory[2],inventory[3],inventory[4],inventory[5],inventory[6],inventory[7],inventory[8]};
  uint8_t resettoreadycmd[] = {0x22,0x26,uid[0],uid[1],uid[2],uid[3],uid[4],uid[5],uid[6],uid[7]};
  uint8_t resettoreadylen = 0;
  return( CLRC688_Transceive(phi2c, addr, resettoreadycmd, sizeof(resettoreadycmd), inventory, &resettoreadylen) );
}

/////////////////////////////////////////////////////////
// I2C emulation of CLRC688 + SLIX2 tag
// (with auto reset counter on power down signal)
//

static uint8_t  EMU_SLIX2_INVENTORY[SLIX2_INVENTORY_LEN];
static uint8_t  EMU_SLIX2_SYSINFO[SLIX2_SYSINFO_LEN];
static uint8_t  EMU_SLIX2_SIGNATURE[SLIX2_SIGNATURE_LEN];
static uint32_t EMU_SLIX2_BLOCKS[SLIX2_BLOCKS];
static uint16_t EMU_SLIX2_COUNTER;
static bool     EMU_SLIX2_TAG_PRESENT;

void EMU_SLIX2_CounterReset() {
  uint16_t amount_of_labels = EMU_SLIX2_BLOCKS[0x0F]>>16;
  uint16_t counter_margin = EMU_SLIX2_BLOCKS[0x10]>>16;
  EMU_SLIX2_COUNTER = 0xFFFF - amount_of_labels - counter_margin;
}

void EMU_SLIX2_Communication(const uint8_t* pindata, const uint8_t inlength, uint8_t* poutdata, uint8_t* poutlength) {
  switch(pindata[1]) { //emulate command for tag
    case 0x01: { //inventory
      EMU_SLIX2_CounterReset();
      poutdata[0]=0;
      memcpy(poutdata+1, EMU_SLIX2_INVENTORY, SLIX2_INVENTORY_LEN);
      *poutlength = 1 + SLIX2_INVENTORY_LEN;
      EMU_SLIX2_TAG_PRESENT = true;
      break;
    }
    case 0x21: { //write single block
      if( 0x4F == pindata[10] ) { //write counter
        if( (1==pindata[11]) && (0==pindata[12]) && (0==pindata[13]) && (0==pindata[14]) ) { //only allow increment by 1
          EMU_SLIX2_COUNTER++;
        }
        else {
          poutdata[0]=0x01; poutdata[0]=0x0F; *poutlength=2; //set error
          break;
        }
      }
      poutdata[0]=0; *poutlength=1; 
      break;
    }
    case 0x23: { //read multiple block
      uint8_t blk = pindata[10];
      if( blk >= SLIX2_BLOCKS ) {
        poutdata[0]=0x01; poutdata[0]=0x0F; *poutlength=2; //set error
        break;
      }
      uint8_t cnt = pindata[11]+1;
      if( blk+cnt > SLIX2_BLOCKS )
        cnt = SLIX2_BLOCKS-blk;
      poutdata[0]=0;
      memcpy(poutdata+1, &EMU_SLIX2_BLOCKS[blk], cnt*4);
      *poutlength = 1 + cnt*4;

      //special case counter
      if( (79==blk) || (blk+cnt>=79) ) {
        poutdata[1 + (79-blk)*4 + 0] = (uint8_t)EMU_SLIX2_COUNTER;
        poutdata[1 + (79-blk)*4 + 1] = (uint8_t)(EMU_SLIX2_COUNTER>>8);
        poutdata[1 + (79-blk)*4 + 2] = 0;
        poutdata[1 + (79-blk)*4 + 3] = 1;
      }
      break;
    }
    case 0x26: { //reset to ready (used to check if tag still present)
      if( EMU_SLIX2_TAG_PRESENT ) {
        poutdata[0]=0; *poutlength=1; 
      } else {
        poutdata[0]=0x01; poutdata[0]=0x0F; *poutlength=2; //set error
      }
      break;
    }
    case 0x2B: poutdata[0]=0; memcpy(poutdata+1, EMU_SLIX2_SYSINFO, SLIX2_SYSINFO_LEN); *poutlength=1+SLIX2_SYSINFO_LEN; break;                          //sysinfo
    case 0xB2: poutdata[0]=0; poutdata[1]=HAL_GetTick(); poutdata[2]=HAL_GetTick()>>8; *poutlength=3; break;                                             //random
    case 0xB3: poutdata[0]=0; *poutlength=1; break;                                                                                                      //set password => just signal success
    case 0xBD: poutdata[0]=0; memcpy(poutdata+1, EMU_SLIX2_SIGNATURE, SLIX2_SIGNATURE_LEN); *poutlength=1+SLIX2_SIGNATURE_LEN; break;                    //signature
    default:   poutdata[0]=0; *poutlength=1; break;                                                                                                      //always signal success
  }
}

void EMU_CLRC688_IRQSet(uint8_t irq) {
  HAL_GPIO_WritePin(OUT_IRQ_GPIO_Port, OUT_IRQ_Pin, irq?GPIO_PIN_SET:GPIO_PIN_RESET);
}

void EMU_CLRC688_Communication(const uint8_t* pindata, const uint8_t inlength, uint8_t* poutdata, uint8_t* poutlength) {
  static uint8_t clrc668_fifo_buffer[256];
  static uint8_t clrc668_fifo_length = 0;

  *poutlength = 0;
  if( inlength>0 ) {
    switch( pindata[0] ) {
      case 0x00: { //command for reader ic
        switch( pindata[1] ) { 
          case 0x07: {  //handle xfer to_/from tag
            EMU_SLIX2_Communication(clrc668_fifo_buffer, clrc668_fifo_length, clrc668_fifo_buffer, &clrc668_fifo_length);
            EMU_CLRC688_IRQSet(1);   //signal IRQ
            break;
          }
        }
        if( pindata[1] & 0x80 ) //reset the counter when software standby mode is entered
          EMU_SLIX2_CounterReset();
        break;
      } 

      case 0x04: { //fifolen
        if( 1 == inlength ) {
          poutdata[0] = clrc668_fifo_length;
          *poutlength = 1;
        }
        break;
      }

      case 0x05: { //fifodata
        if( inlength>1 ) { //incoming data
          memcpy(clrc668_fifo_buffer, pindata+1, inlength-1);
        }
        else { //outgoing data
          if( clrc668_fifo_length )
            memcpy(poutdata, clrc668_fifo_buffer, clrc668_fifo_length);
          *poutlength = clrc668_fifo_length;
          clrc668_fifo_length = 0;
        }
        break;
      }

      case 0x06: //irq0 register
      case 0x07: //irq1 register
      { 
        if( 1 == inlength ) { //is it a read?
          poutdata[0] = 0x7F; //signal all ints
          *poutlength = 1;
        }
        else
          EMU_CLRC688_IRQSet(0);  //clear interrupts / stop signalling IRQ
        break;
      }

      case 0x0A: { //error register
        poutdata[0] = 0x00; //no error
        *poutlength = 1;
        break;
      }

      default: //ignore all other register writes
        break;
    }
  }
}

void InitEmulationWithDefaultData(void) {
  //copy default tag data
  memcpy(EMU_SLIX2_INVENTORY, DMO_TAG_SLIX2_INVENTORY, SLIX2_INVENTORY_LEN); 
  memcpy(EMU_SLIX2_SYSINFO,   DMO_TAG_SLIX2_SYSINFO, SLIX2_SYSINFO_LEN); 
  memcpy(EMU_SLIX2_SIGNATURE, DMO_TAG_SLIX2_SIGNATURE, SLIX2_SIGNATURE_LEN); 
  memcpy(EMU_SLIX2_BLOCKS,    DMO_TAG_SLIX2_BLOCKS, SLIX2_BLOCKS*sizeof(uint32_t));
}

void UpdateEmulationDataFromRealTag(void) {
  //read data from tag and copy to emulation buffer
  if( CLRC688_Init(&hi2c2, CLRC688_ADDR) ) {
    uint8_t inventory[9];
    uint8_t sysinfo[32];
    uint8_t signature[32];
    uint32_t blocks[80];
    if( CLRC688_ReadOutSLIX2(&hi2c2, CLRC688_ADDR, inventory, sysinfo, signature, blocks) ) {
      //check for DMO prefix and DMO magic
      if( (0xed820a03==blocks[0]) && (0xd2613986==blocks[1]) && (0x321e1403==blocks[2]) && (0x3c00cab6==blocks[3]) ) {
        //copy tag data to emulation and set counter to maximum
        memcpy(EMU_SLIX2_INVENTORY, inventory, SLIX2_INVENTORY_LEN);
        memcpy(EMU_SLIX2_SYSINFO, sysinfo, SLIX2_SYSINFO_LEN); 
        memcpy(EMU_SLIX2_SIGNATURE, signature, SLIX2_SIGNATURE_LEN); 
        memcpy(EMU_SLIX2_BLOCKS, blocks, SLIX2_BLOCKS*sizeof(uint32_t));
        //simulate tag removal so new content will be fetched
        EMU_SLIX2_TAG_PRESENT = false;
      }

      //wait for tag removal (roll change)
      for(uint32_t notpresent=0;notpresent<10;) {
        HAL_Delay(100);
        if( !CLRC688_CheckPresense(&hi2c2, CLRC688_ADDR, inventory) )
          notpresent++;
        else
          notpresent=0;
      }
    }
    CLRC688_DeInit(&hi2c2);
  }
}

/////////////////////////////////////////////////////////
// STM32 HAL function implementation for 
// - EXTI power down line monitoring
// - I2C slave emulation

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  static uint8_t clrc668_pwr_state = 1;

  uint8_t pdown = HAL_GPIO_ReadPin(EXTI0_IN_PWDN_GPIO_Port,EXTI0_IN_PWDN_Pin);
  if( pdown == clrc668_pwr_state )
    return;

  if( !pdown )
    HAL_I2C_EnableListen_IT(&hi2c1);  //power up
  else {
    HAL_I2C_DisableListen_IT(&hi2c1); //power down
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSTANDBYMode();
  }
  clrc668_pwr_state = pdown;
}

#define I2C_RCV_BUF_SIZE 257
#define I2C_SND_BUF_SIZE 257

static uint8_t I2CSlaveRecvBuf[I2C_RCV_BUF_SIZE];
static uint8_t I2CSlaveRecvBufLen;

static uint8_t I2CSlaveSendBuf[I2C_SND_BUF_SIZE];
static uint8_t I2CSlaveSendBufLen;

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t transferDirection, uint16_t addrMatchCode) {
  if (transferDirection == I2C_DIRECTION_RECEIVE) { //DIRECTION_RECEIVE refers to master => for us (slave) means sending 
      EMU_CLRC688_Communication(I2CSlaveRecvBuf, I2CSlaveRecvBufLen, I2CSlaveSendBuf, &I2CSlaveSendBufLen);
    HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2CSlaveSendBuf, I2CSlaveSendBufLen, I2C_LAST_FRAME);
  } else {
    I2CSlaveRecvBufLen = 0;
    HAL_I2C_Slave_Seq_Receive_IT(hi2c, I2CSlaveRecvBuf, 1, I2C_NEXT_FRAME);
  }
  HAL_GPIO_WritePin(OUT_LED_GPIO_Port, OUT_LED_Pin, GPIO_PIN_RESET);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  I2CSlaveRecvBufLen++;
  if( I2CSlaveRecvBufLen < I2C_RCV_BUF_SIZE )
    HAL_I2C_Slave_Seq_Receive_IT(hi2c, I2CSlaveRecvBuf + I2CSlaveRecvBufLen, 1, I2C_NEXT_FRAME);
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c) {
  EMU_CLRC688_Communication(I2CSlaveRecvBuf, I2CSlaveRecvBufLen, I2CSlaveSendBuf, &I2CSlaveSendBufLen);
  HAL_I2C_EnableListen_IT(hi2c);
  HAL_GPIO_WritePin(OUT_LED_GPIO_Port, OUT_LED_Pin, GPIO_PIN_SET);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  HAL_I2C_EnableListen_IT(hi2c);
}
//
//////////////////////////////////////////////
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  //////////////////////////////////////////////
  //
  InitEmulationWithDefaultData();

  //start listening as I2C slave
  HAL_I2C_EnableListen_IT(&hi2c1);

  //main loop
  for(;;) {
    UpdateEmulationDataFromRealTag();
    HAL_Delay(100);
  }
  //
  //////////////////////////////////////////////
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 80;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OUT_LED_GPIO_Port, OUT_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_IRQ_Pin|OUT_PULLUP_I2C2_SDA_Pin|OUT_PULLUP_I2C2_SCL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OUT_PWDN_READER_GPIO_Port, OUT_PWDN_READER_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : OUT_LED_Pin */
  GPIO_InitStruct.Pin = OUT_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OUT_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : EXTI0_IN_PWDN_Pin */
  GPIO_InitStruct.Pin = EXTI0_IN_PWDN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EXTI0_IN_PWDN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_IRQ_Pin OUT_PULLUP_I2C2_SDA_Pin OUT_PULLUP_I2C2_SCL_Pin */
  GPIO_InitStruct.Pin = OUT_IRQ_Pin|OUT_PULLUP_I2C2_SDA_Pin|OUT_PULLUP_I2C2_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA6 PA7
                           PA8 PA9 PA10 PA11
                           PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB14
                           PB15 PB4 PB5 PB8
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_IRQ_READER_Pin */
  GPIO_InitStruct.Pin = IN_IRQ_READER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_IRQ_READER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OUT_PWDN_READER_Pin */
  GPIO_InitStruct.Pin = OUT_PWDN_READER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OUT_PWDN_READER_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
