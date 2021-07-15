#ifndef AD9959_AD9959_H
#define AD9959_AD9959_H

#include "main.h"
#include "stdbool.h"

typedef enum {
    AD9959_CHANNEL_0 = 0x10U,
    AD9959_CHANNEL_1 = 0x20U,
    AD9959_CHANNEL_2 = 0x40U,
    AD9959_CHANNEL_3 = 0x80U,
} AD9959_CHANNEL;

typedef enum {
    AD9959_REG_CSR       =   0x00U,  //CSR 通道选择寄存器
    AD9959_REG_FR1       =   0x01U,  //FR1 功能寄存器1
    AD9959_REG_FR2       =   0x02U,  //FR2 功能寄存器2
    AD9959_REG_CFR       =   0x03U,  //CFR 通道功能寄存器
    AD9959_REG_CFTW0     =   0x04U,  //CTW0 通道频率转换字寄存器
    AD9959_REG_CPOW0     =   0x05U,  //CPW0 通道相位转换字寄存器
    AD9959_REG_ACR       =   0x06U,  //ACR 幅度控制寄存器
    AD9959_REG_LSRR      =   0x07U,  //LSR 通道线性扫描寄存器
    AD9959_REG_RDW       =   0x08U,  //RDW 通道线性向上扫描寄存器
    AD9959_REG_FDW       =   0x09U,  //FDW 通道线性向下扫描寄存器
} AD9959_REG_ADDR;

struct AD9950_IO{
    volatile unsigned long *CS;
    volatile unsigned long *SCLK;
    volatile unsigned long *UPDATE;
    volatile unsigned long *PS0;
    volatile unsigned long *PS1;
    volatile unsigned long *PS2;
    volatile unsigned long *PS3;
    volatile unsigned long *SDIO0;
    volatile unsigned long *SDIO1;
    volatile unsigned long *SDIO2;
    volatile unsigned long *SDIO3;
    volatile unsigned long *PDC;
    volatile unsigned long *Reset;
};

extern const struct AD9950_IO ad9959_io;

#define IS_AD9959_REG_ADDR(REG_ADDR)    (((REG_ADDR) == AD9959_REG_CSR  ) || \
                                         ((REG_ADDR) == AD9959_REG_FR1  ) || \
                                         ((REG_ADDR) == AD9959_REG_FR2  ) || \
                                         ((REG_ADDR) == AD9959_REG_CFR  ) || \
                                         ((REG_ADDR) == AD9959_REG_CFTW0) || \
                                         ((REG_ADDR) == AD9959_REG_CPOW0) || \
                                         ((REG_ADDR) == AD9959_REG_ACR  ) || \
                                         ((REG_ADDR) == AD9959_REG_LSRR ) || \
                                         ((REG_ADDR) == AD9959_REG_RDW  ) || \
                                         ((REG_ADDR) == AD9959_REG_FDW  ))

#define IS_AD9959_CHANNEL(CHANNEL)      (((CHANNEL) == AD9959_CHANNEL_0 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_1 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_2 ) || \
                                         ((CHANNEL) == AD9959_CHANNEL_3 ))

void ad9959_init(void);
void ad9959_reset(void);
void ad9959_io_init(void);
void ad9959_io_update(void);
void ad9959_write_data(AD9959_REG_ADDR register_address, uint8_t number_of_registers, const uint8_t *register_data, bool update);
void ad9959_write_phase(AD9959_CHANNEL channel, uint16_t phase);
void ad9959_write_frequency(AD9959_CHANNEL channel, uint32_t frequency);
void ad9959_write_amplitude(AD9959_CHANNEL channel, uint16_t amplitude);

#endif //AD9959_AD9959_H
