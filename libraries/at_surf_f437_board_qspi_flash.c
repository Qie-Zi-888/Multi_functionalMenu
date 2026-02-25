/**
  **************************************************************************
  * @file     at_surf_f437_board_qspi_flash.c
  * @version  v2.0.0
  * @date     2020-11-02
  * @brief    the driver library of the qspi flash flash
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to 
  * download from Artery official website is the copyrighted work of Artery. 
  * Artery authorizes customers to use, copy, and distribute the BSP 
  * software and its related documentation for the purpose of design and 
  * development in conjunction with Artery microcontrollers. Use of the 
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
  
#include "at_surf_f437_board_qspi_flash.h"

qspi_cmd_type qspi_flash_cmd_config;

void qspi_flash_busy_check(void);
void qspi_flash_write_enable(void);

/**
  * @brief  initializes quad spi flash.
  * @param  none
  * @retval none
  */
void qspi_flash_init(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the qspi flash clock */
  crm_periph_clock_enable(QSPI_FLASH_QSPI_CLK, TRUE);

  /* enable the pin clock */
  crm_periph_clock_enable(QSPI_FLASH_CS_GPIO_CLK, TRUE);
  crm_periph_clock_enable(QSPI_FLASH_CLK_GPIO_CLK, TRUE);
  crm_periph_clock_enable(QSPI_FLASH_D0_GPIO_CLK, TRUE);
  crm_periph_clock_enable(QSPI_FLASH_D1_GPIO_CLK, TRUE);
  crm_periph_clock_enable(QSPI_FLASH_D2_GPIO_CLK, TRUE);
  crm_periph_clock_enable(QSPI_FLASH_D3_GPIO_CLK, TRUE);
  
  /* configure gpio */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_CS_GPIO_PIN;
  gpio_init(QSPI_FLASH_CS_GPIO_PORT, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_CLK_GPIO_PIN;
  gpio_init(QSPI_FLASH_CLK_GPIO_PORT, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_D0_GPIO_PIN;
  gpio_init(QSPI_FLASH_D0_GPIO_PORT, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_D1_GPIO_PIN;
  gpio_init(QSPI_FLASH_D1_GPIO_PORT, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_D2_GPIO_PIN;
  gpio_init(QSPI_FLASH_D2_GPIO_PORT, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = QSPI_FLASH_D3_GPIO_PIN;
  gpio_init(QSPI_FLASH_D3_GPIO_PORT, &gpio_init_struct);
  
  /* gpio mux config */
  gpio_pin_mux_config(QSPI_FLASH_CS_GPIO_PORT,  QSPI_FLASH_CS_GPIO_PINS_SOURCE,  QSPI_FLASH_CS_GPIO_MUX); 
  gpio_pin_mux_config(QSPI_FLASH_CLK_GPIO_PORT, QSPI_FLASH_CLK_GPIO_PINS_SOURCE, QSPI_FLASH_CLK_GPIO_MUX);
  gpio_pin_mux_config(QSPI_FLASH_D0_GPIO_PORT,  QSPI_FLASH_D0_GPIO_PINS_SOURCE,  QSPI_FLASH_D0_GPIO_MUX); 
  gpio_pin_mux_config(QSPI_FLASH_D1_GPIO_PORT,  QSPI_FLASH_D1_GPIO_PINS_SOURCE,  QSPI_FLASH_D1_GPIO_MUX); 
  gpio_pin_mux_config(QSPI_FLASH_D2_GPIO_PORT,  QSPI_FLASH_D2_GPIO_PINS_SOURCE,  QSPI_FLASH_D2_GPIO_MUX); 
  gpio_pin_mux_config(QSPI_FLASH_D3_GPIO_PORT,  QSPI_FLASH_D3_GPIO_PINS_SOURCE,  QSPI_FLASH_D3_GPIO_MUX); 
  
  /* switch to cmd port */
  qspi_xip_enable(QSPI_FLASH_QSPIx, FALSE);

  /* set sclk */
  qspi_clk_division_set(QSPI_FLASH_QSPIx, QSPI_CLK_DIV_4);

  /* set sck idle mode 0 */
  qspi_sck_mode_set(QSPI_FLASH_QSPIx, QSPI_SCK_MODE_0);

  /* set wip in bit 0 */
  qspi_busy_config(QSPI_FLASH_QSPIx, QSPI_BUSY_OFFSET_0);
}


/** @addtogroup 437_QSPI_xip_port_read_flash
  * @{
  */

#define FLASH_PAGE_PROGRAM_SIZE          256

qspi_cmd_type w25q128_cmd_config;
qspi_xip_type w25q128_xip_init;

void qspi_busy_check(void);
void qspi_write_enable(void);

/**
  * @brief  w25q128 cmd read config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @param  addr: read start address
  * @param  counter: read data counter
  * @retval none
  */
void w25q128_cmd_read_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr, uint32_t counter)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0xEB;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = addr;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
  qspi_cmd_struct->data_counter = counter;
  qspi_cmd_struct->second_dummy_cycle_num = 6;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_144;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = FALSE;
}

/**
  * @brief  w25q128 cmd write config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @param  addr: write start address
  * @param  counter: write data counter
  * @retval none
  */
void w25q128_cmd_write_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr, uint32_t counter)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x32;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = addr;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
  qspi_cmd_struct->data_counter = counter;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_114;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = TRUE;
}

/**
  * @brief  w25q128 cmd erase config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @param  addr: erase address
  * @retval none
  */
void w25q128_cmd_erase_config(qspi_cmd_type *qspi_cmd_struct, uint32_t addr)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x20;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = addr;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_3_BYTE;
  qspi_cmd_struct->data_counter = 0;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = TRUE;
}

/**
  * @brief  w25q128 cmd wren config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @retval none
  */
void w25q128_cmd_wren_config(qspi_cmd_type *qspi_cmd_struct)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x06;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = 0;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
  qspi_cmd_struct->data_counter = 0;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = TRUE;
}

/**
  * @brief  w25q128 cmd rdsr config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @retval none
  */
void w25q128_cmd_rdsr_config(qspi_cmd_type *qspi_cmd_struct)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x05;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = 0;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
  qspi_cmd_struct->data_counter = 0;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = TRUE;
  qspi_cmd_struct->write_data_enable = FALSE;
}

/**
  * @brief  qspi dma set
  * @param  dir: dma transfer direction
  * @param  buf: the pointer for dma data
  * @param  length: data length
  * @retval none
  */
void qspi_dma_set(dma_dir_type dir, uint8_t* buf, uint32_t length)
{
  dma_init_type dma_init_struct;
  dma_reset(DMA2_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = length / 4;  /* using word unit */
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.direction = dir;
  dma_init_struct.memory_base_addr = (uint32_t)buf;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_WORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)(&(QSPI1->dt));
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_WORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;

  dma_init(DMA2_CHANNEL1, &dma_init_struct);

  dmamux_init(DMA2MUX_CHANNEL1, DMAMUX_DMAREQ_ID_QSPI1);
  dmamux_enable(DMA2, TRUE);

  dma_channel_enable(DMA2_CHANNEL1, TRUE);
}

/**
  * @brief  qspi read data
  * @param  addr: the address for read
  * @param  total_len: the length for read
  * @param  buf: the pointer for read data
  * @retval none
  */
void qspi_data_read(uint32_t addr, uint32_t total_len, uint8_t* buf)
{
  /* config qspi's dma mode */
  qspi_dma_enable(QSPI1, TRUE);
  qspi_dma_rx_threshold_set(QSPI1, QSPI_DMA_FIFO_THOD_WORD08);

  /* config and enable dma */
  qspi_dma_set(DMA_DIR_PERIPHERAL_TO_MEMORY, buf, total_len);

  /* kick command */
  w25q128_cmd_read_config(&w25q128_cmd_config, addr, total_len);
  qspi_cmd_operation_kick(QSPI1, &w25q128_cmd_config);

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

  /* wait dma completed */
  while(dma_flag_get(DMA2_FDT1_FLAG) == RESET);
  dma_flag_clear(DMA2_FDT1_FLAG);
  qspi_dma_enable(QSPI1, FALSE);
}

/**
  * @brief  qspi write data for one time
  * @param  addr: the address for write
  * @param  sz: the length for write
  * @param  buf: the pointer for write data
  * @retval none
  */
static void qspi_data_once_write(uint32_t addr, uint32_t sz, uint8_t* buf)
{
  qspi_write_enable();

  /* config qspi's dma mode */
  qspi_dma_enable(QSPI1, TRUE);
  qspi_dma_tx_threshold_set(QSPI1, QSPI_DMA_FIFO_THOD_WORD08);

  /* config and enable dma */
  qspi_dma_set(DMA_DIR_MEMORY_TO_PERIPHERAL, buf, sz);

  /* kick command */
  w25q128_cmd_write_config(&w25q128_cmd_config, addr, sz);
  qspi_cmd_operation_kick(QSPI1, &w25q128_cmd_config);
  

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

  /* wait dma completed */
  while(dma_flag_get(DMA2_FDT1_FLAG) == RESET);
  dma_flag_clear(DMA2_FDT1_FLAG);
  qspi_dma_enable(QSPI1, FALSE);

  qspi_busy_check();
}

/**
  * @brief  qspi write data
  * @param  addr: the address for write
  * @param  total_len: the length for write
  * @param  buf: the pointer for write data
  * @retval none
  */
void qspi_data_write(uint32_t addr, uint32_t total_len, uint8_t* buf)
{
  uint32_t blk_sz;
  do
  { /* send up to 256 bytes at one time */
    if(total_len > FLASH_PAGE_PROGRAM_SIZE)
    {
      blk_sz = FLASH_PAGE_PROGRAM_SIZE;
    }
    else
    {
      blk_sz = total_len;
    }
    qspi_data_once_write(addr, blk_sz, buf);
    addr += blk_sz;
    buf += blk_sz;
    total_len -= blk_sz;
  }while(total_len > 0);
}

/**
  * @brief  qspi erase data
  * @param  sec_addr: the sector address for erase
  * @retval none
  */
void qspi_erase(uint32_t sec_addr)
{
  qspi_write_enable();

  w25q128_cmd_erase_config(&w25q128_cmd_config, sec_addr);
  qspi_cmd_operation_kick(QSPI1, &w25q128_cmd_config);

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);

  qspi_busy_check();
}

/**
  * @brief  qspi check busy
  * @param  none
  * @retval none
  */
void qspi_busy_check(void)
{
  w25q128_cmd_rdsr_config(&w25q128_cmd_config);
  qspi_cmd_operation_kick(QSPI1, &w25q128_cmd_config);

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}

/**
  * @brief  qspi write enable
  * @param  none
  * @retval none
  */
void qspi_write_enable(void)
{
  w25q128_cmd_wren_config(&w25q128_cmd_config);
  qspi_cmd_operation_kick(QSPI1, &w25q128_cmd_config);

  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}


/**
  * @brief  w25q128 cmd rsten config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @retval none
  */
void  w25q128_cmd_rsten_config(qspi_cmd_type *qspi_cmd_struct)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x66;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = 0;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
  qspi_cmd_struct->data_counter = 0;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = TRUE;
}

/**
  * @brief  w25q128 cmd rst config
  * @param  qspi_cmd_struct: the pointer for qspi_cmd_type parameter
  * @retval none
  */
void w25q128_cmd_rst_config(qspi_cmd_type *qspi_cmd_struct)
{
  qspi_cmd_struct->pe_mode_enable = FALSE;
  qspi_cmd_struct->pe_mode_operate_code = 0;
  qspi_cmd_struct->instruction_code = 0x99;
  qspi_cmd_struct->instruction_length = QSPI_CMD_INSLEN_1_BYTE;
  qspi_cmd_struct->address_code = 0;
  qspi_cmd_struct->address_length = QSPI_CMD_ADRLEN_0_BYTE;
  qspi_cmd_struct->data_counter = 0;
  qspi_cmd_struct->second_dummy_cycle_num = 0;
  qspi_cmd_struct->operation_mode = QSPI_OPERATE_MODE_111;
  qspi_cmd_struct->read_status_config = QSPI_RSTSC_HW_AUTO;
  qspi_cmd_struct->read_status_enable = FALSE;
  qspi_cmd_struct->write_data_enable = TRUE;
}

void w25q128_qspi_cmd_send(qspi_cmd_type* cmd)
{
  qspi_cmd_operation_kick(QSPI1, cmd);
  /* wait command completed */
  while(qspi_flag_get(QSPI1, QSPI_CMDSTS_FLAG) == RESET);
  qspi_flag_clear(QSPI1, QSPI_CMDSTS_FLAG);
}


/**
  * @brief  w25q128 xip init config
  * @param  qspi_xip_struct: the pointer for qspi_xip_type parameter
  * @retval none
  */
void w25q128_xip_init_config(qspi_xip_type *qspi_xip_struct)
{
  qspi_xip_struct->read_instruction_code = 0x6B; 
  qspi_xip_struct->read_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
  qspi_xip_struct->read_operation_mode = QSPI_OPERATE_MODE_114;
  qspi_xip_struct->read_second_dummy_cycle_num = 8;
  qspi_xip_struct->write_instruction_code = 0x32;
  qspi_xip_struct->write_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
  qspi_xip_struct->write_operation_mode = QSPI_OPERATE_MODE_114;
  qspi_xip_struct->write_second_dummy_cycle_num = 0;
  qspi_xip_struct->write_select_mode = QSPI_XIPW_SEL_MODED;
  qspi_xip_struct->write_time_counter = 0x7F;
  qspi_xip_struct->write_data_counter = 0x1F;
  qspi_xip_struct->read_select_mode = QSPI_XIPR_SEL_MODED;
  qspi_xip_struct->read_time_counter = 0x7F;
  qspi_xip_struct->read_data_counter = 0x1F;
}


void w25q128_qspi_xip_init(void)
{
  /* switch to command-port mode */
  qspi_xip_enable(QSPI1, FALSE);

  /* issue reset command */
  w25q128_cmd_rsten_config(&w25q128_cmd_config);
  w25q128_qspi_cmd_send(&w25q128_cmd_config);
  w25q128_cmd_rst_config(&w25q128_cmd_config);
  w25q128_qspi_cmd_send(&w25q128_cmd_config);
  
  /* initial xip */
  w25q128_xip_init_config(&w25q128_xip_init);
  qspi_xip_init(QSPI1, &w25q128_xip_init);
  qspi_xip_cache_bypass_set(QSPI1, FALSE);
  qspi_xip_enable(QSPI1, TRUE);
}

void ext_flash_init(void)
{
	qspi_flash_init();
	w25q128_qspi_xip_init();
}
