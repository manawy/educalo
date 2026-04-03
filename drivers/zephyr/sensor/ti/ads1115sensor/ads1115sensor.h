/*
 * Copyright (c) 2026 Fabien Georget <fabien.georget@usherbrooke.ca>
 * SPDX-Licence-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <zephyr/drivers/i2c.h>


#define ADS1115_APOINTER_MASK          0x03 // Pointer to register
#define ADS1115_APOINTER_CONVERSION    0x00 // Last Conversion register
#define ADS1115_APOINTER_CONFIG        0x01 // Configuration register
#define ADS1115_APOINTER_LOWTHRESHOLD  0x02 // Low threshold value register
#define ADS1115_APOINTER_HIGHTHRESHOLD 0x03 // High threshold value register

#define ADS1115_CONFIG_MSB_OS_MASK     0x80 // Operational status
#define ADS1115_CONFIG_MSB_OS_TRIGGER  0x80 // W: single conversion trigger
#define ADS1115_CONFIG_MSB_OS_BUSY     0x00 // R: Busy status
#define ADS1115_CONFIG_MSB_OS_NOTBUSY  0x00 // R: Not busy status

#define ADS1115_CONFIG_MSB_MUX_MASK  0x70 // Multiplexer configuration
#define ADS1115_CONFIG_MSB_MUX_01    0x00 // AINp = AIN0 and AINn = AIN1
#define ADS1115_CONFIG_MSB_MUX_03    0x10 // AINp = AIN0 and AINn = AIN3
#define ADS1115_CONFIG_MSB_MUX_13    0x20 // AINp = AIN1 and AINn = AIN3
#define ADS1115_CONFIG_MSB_MUX_23    0x30 // AINp = AIN2 and AINn = AIN3
#define ADS1115_CONFIG_MSB_MUX_0G    0x40 // AINp = AIN0 and AINn = GND
#define ADS1115_CONFIG_MSB_MUX_1G    0x50 // AINp = AIN1 and AINn = GND
#define ADS1115_CONFIG_MSB_MUX_2G    0x60 // AINp = AIN2 and AINn = GND
#define ADS1115_CONFIG_MSB_MUX_3G    0x70 // AINp = AIN3 and AINn = GND

#define ADS1115_CONFIG_MSB_PGA_MASK       0x0E // Programmable gain configuration
#define ADS1115_CONFIG_MSB_PGA_FSR_6_144  0x00 // FSR = +/- 6.144V
#define ADS1115_CONFIG_MSB_PGA_FSR_4_096  0x02 // FSR = +/- 4.096V
#define ADS1115_CONFIG_MSB_PGA_FSR_2_048  0x04 // FSR = +/- 2.048V
#define ADS1115_CONFIG_MSB_PGA_FSR_1_024  0x06 // FSR = +/- 1.024V
#define ADS1115_CONFIG_MSB_PGA_FSR_0_512  0x08 // FSR = +/- 0.512V
#define ADS1115_CONFIG_MSB_PGA_FSR_0_256  0x0A // FSR = +/- 0.256V

#define ADS1115_CONFIG_MSB_MODE_MASK        0x01 // Operating mode configuration
#define ADS1115_CONFIG_MSB_MODE_CONTINUOUS  0x00 // Continuous conversion mode
#define ADS1115_CONFIG_MSB_MODE_SINGLE      0x01 // Single shot mode

#define ADS1115_CONFIG_LSB_DR_MASK      0xE0 // Operating mode configuration
#define ADS1115_CONFIG_LSB_DR_8SPS      0x00 // Data rate :   8 SPS
#define ADS1115_CONFIG_LSB_DR_16SPS     0x20 // Data rate :  16 SPS
#define ADS1115_CONFIG_LSB_DR_32SPS     0x40 // Data rate :  32 SPS
#define ADS1115_CONFIG_LSB_DR_64SPS     0x60 // Data rate :  64 SPS
#define ADS1115_CONFIG_LSB_DR_128SPS    0x80 // Data rate : 128 SPS
#define ADS1115_CONFIG_LSB_DR_250SPS    0xA0 // Data rate : 250 SPS
#define ADS1115_CONFIG_LSB_DR_475SPS    0xC0 // Data rate : 475 SPS
#define ADS1115_CONFIG_LSB_DR_860SPS    0xE0 // Data rate : 860 SPS

#define ADS1115_CONFIG_LSB_COMPMODE_MASK         0x10 // Comparator mode configuration
#define ADS1115_CONFIG_LSB_COMPMODE_TRADITIONAL  0x00 // Comparator mode configuration
#define ADS1115_CONFIG_LSB_COMPMODE_WINDOW       0x10 // Comparator mode configuration

#define ADS1115_CONFIG_LSB_COMPPOL_MASK   0x08 // Comparator polarity configuration
#define ADS1115_CONFIG_LSB_COMPPOL_LOW    0x00 // Active low (default)
#define ADS1115_CONFIG_LSB_COMPPOL_HIGH   0x08 // Active high

#define ADS1115_CONFIG_LSB_COMPLATCH_MASK  0x04 // Comparator latching configuration
#define ADS1115_CONFIG_LSB_COMPLATCH_FALSE 0x00 // Non latching (default)
#define ADS1115_CONFIG_LSB_COMPLATCH_TRUE  0x04 // Latching

#define ADS1115_CONFIG_LSB_COMPALERT_MASK     0x03 // Comparator alert and queue configuration
#define ADS1115_CONFIG_LSB_COMPALERT_1        0x00 // Assert after 1 conversion
#define ADS1115_CONFIG_LSB_COMPALERT_2        0x01 // Assert after 2 conversions
#define ADS1115_CONFIG_LSB_COMPALERT_3        0x02 // Assert after 3 conversions
#define ADS1115_CONFIG_LSB_COMPALERT_DISABLE  0x03 // Disabled (default)

enum ADS1115Addr : uint8_t
{
    ADS1115Addr_GND = 0x48,
    ADS1115Addr_VDD = 0x49,
    ADS1115Addr_SDA = 0x50,
    ADS1115Addr_SCL = 0x51
};


  // The possible data rates
  enum ADS1115DataRate :uint8_t
  {
    ADS1115DataRate_8   = ADS1115_CONFIG_LSB_DR_8SPS,
    ADS1115DataRate_16  = ADS1115_CONFIG_LSB_DR_16SPS,
    ADS1115DataRate_32  = ADS1115_CONFIG_LSB_DR_32SPS,
    ADS1115DataRate_64  = ADS1115_CONFIG_LSB_DR_64SPS,
    ADS1115DataRate_128 = ADS1115_CONFIG_LSB_DR_128SPS,
    ADS1115DataRate_250 = ADS1115_CONFIG_LSB_DR_250SPS,
    ADS1115DataRate_475 = ADS1115_CONFIG_LSB_DR_475SPS,
    ADS1115DataRate_860 = ADS1115_CONFIG_LSB_DR_860SPS,
  };

  // The possible gains
  enum ADS1115Gain :uint8_t
  {
    ADS1115DGain_TWO_THIRD = ADS1115_CONFIG_MSB_PGA_FSR_6_144,
    ADS1115DGain_ONE       = ADS1115_CONFIG_MSB_PGA_FSR_4_096,
    ADS1115DGain_TWO       = ADS1115_CONFIG_MSB_PGA_FSR_2_048,
    ADS1115DGain_FOUR      = ADS1115_CONFIG_MSB_PGA_FSR_1_024,
    ADS1115DGain_EIGHT     = ADS1115_CONFIG_MSB_PGA_FSR_0_512,
    ADS1115DGain_SIXTEEN   = ADS1115_CONFIG_MSB_PGA_FSR_0_256,
  };

  // The possible mutliplexer configurations
  //
  // ADS1115MUX_XY : differential read between pin X and Y
  // If Y = G: G is ground
  enum ADS1115Mux :uint8_t
  {
    ADS1115MUX_01 = ADS1115_CONFIG_MSB_MUX_01, // AINp = AIN0 and AINn = AIN1 (default)
    ADS1115MUX_03 = ADS1115_CONFIG_MSB_MUX_03, // AINp = AIN0 and AINn = AIN3
    ADS1115MUX_13 = ADS1115_CONFIG_MSB_MUX_13, // AINp = AIN1 and AINn = AIN3
    ADS1115MUX_23 = ADS1115_CONFIG_MSB_MUX_23, // AINp = AIN2 and AINn = AIN3
    ADS1115MUX_0G = ADS1115_CONFIG_MSB_MUX_0G, // AINp = AIN0 and AINn = GND
    ADS1115MUX_1G = ADS1115_CONFIG_MSB_MUX_1G, // AINp = AIN1 and AINn = GND
    ADS1115MUX_2G = ADS1115_CONFIG_MSB_MUX_2G, // AINp = AIN1 and AINn = GND
    ADS1115MUX_3G = ADS1115_CONFIG_MSB_MUX_3G, // AINp = AIN3 and AINn = GND
  };



struct ads1115sensor_data {
    int16_t reg_val;
    uint8_t config_buf[3];
};

struct ads1115sensor_config {
    struct i2c_dt_spec i2c;
};
