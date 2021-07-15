#include "ad9959.h"

const struct AD9950_IO ad9959_io = {
        .CS             =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 7 ),
        .SCLK           =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 9 ),
        .UPDATE         =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 5 ),
        .PS0            =   (volatile unsigned long  *)BIT_BAND(GPIOD_ODR_Addr, 6 ),
        .PS1            =   (volatile unsigned long  *)BIT_BAND(GPIOG_ODR_Addr, 13),
        .PS2            =   (volatile unsigned long  *)BIT_BAND(GPIOG_ODR_Addr, 15),
        .PS3            =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 4 ),
        .SDIO0          =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 6 ),
        .SDIO1          =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 8 ),
        .SDIO2          =   (volatile unsigned long  *)BIT_BAND(GPIOE_ODR_Addr, 0 ),
        .SDIO3          =   (volatile unsigned long  *)BIT_BAND(GPIOE_ODR_Addr, 1 ),
        .PDC            =   (volatile unsigned long  *)BIT_BAND(GPIOG_ODR_Addr, 14),
        .Reset          =   (volatile unsigned long  *)BIT_BAND(GPIOB_ODR_Addr, 3 )
};

uint8_t FR2_DATA[2] = {0x20, 0x00};//default Value = 0x0000
uint8_t CFR_DATA[3] = {0x00, 0x03, 0x02};//default Value = 0x000302

uint8_t CPOW0_DATA[2] = {0x00, 0x00};//default Value = 0x0000   @ = POW/2^14*360

uint8_t LSRR_DATA[2] = {0x00, 0x00};//default Value = 0x----

uint8_t RDW_DATA[4] = {0x00, 0x00, 0x00, 0x00};//default Value = 0x--------

uint8_t FDW_DATA[4] = {0x00, 0x00, 0x00, 0x00};//default Value = 0x--------

/**
* @brief AD9959略微延时
* */
static void ad9959_delay(uint32_t length) {
    length = length * 12;
    while (length--);
}

/**
* @brief AD9959初始化
* */
void ad9959_init(void) {
    uint8_t FR1_DATA[3] = {0xD0, 0x00,
                           0x00};//20倍频 Charge pump control = 75uA FR1<23> -- VCO gain control =0时 system clock below 160 MHz;

    ad9959_io_init();
    ad9959_reset();

    ad9959_write_data(AD9959_REG_FR1, 3, FR1_DATA, 1);//写功能寄存器1
    ad9959_write_data(AD9959_REG_FR2, 2, FR2_DATA, 1);

    //写入初始频率
    ad9959_write_frequency(AD9959_CHANNEL_0, 1000);
    ad9959_write_frequency(AD9959_CHANNEL_1, 1000);
    ad9959_write_frequency(AD9959_CHANNEL_2, 1000);
    ad9959_write_frequency(AD9959_CHANNEL_3, 1000);

    ad9959_write_phase(AD9959_CHANNEL_0, 0);
    ad9959_write_phase(AD9959_CHANNEL_1, 0);
    ad9959_write_phase(AD9959_CHANNEL_2, 0);
    ad9959_write_phase(AD9959_CHANNEL_3, 0);

    ad9959_write_amplitude(AD9959_CHANNEL_0, 0xFF);
    ad9959_write_amplitude(AD9959_CHANNEL_1, 0xFF);
    ad9959_write_amplitude(AD9959_CHANNEL_2, 0xFF);
    ad9959_write_amplitude(AD9959_CHANNEL_3, 0xFF);
}

/**
* @brief AD9959复位
* */
void ad9959_reset(void) {
    *ad9959_io.Reset = 0;
    ad9959_delay(1);
    *ad9959_io.Reset = 1;
    ad9959_delay(30);
    *ad9959_io.Reset = 0;
}

/**
* @brief AD9959IO口初始化
* */
void ad9959_io_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /*Configure GPIO pin Output Level */

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    *ad9959_io.PDC = 0;
    *ad9959_io.CS = 1;
    *ad9959_io.SCLK = 0;
    *ad9959_io.UPDATE = 0;
    *ad9959_io.PS0 = 0;
    *ad9959_io.PS1 = 0;
    *ad9959_io.PS2 = 0;
    *ad9959_io.PS3 = 0;
    *ad9959_io.SDIO0 = 0;
    *ad9959_io.SDIO1 = 0;
    *ad9959_io.SDIO2 = 0;
    *ad9959_io.SDIO3 = 0;
}

/**
 * @brief AD9959更新IO寄存器
 * */
void ad9959_io_update(void) {
    *ad9959_io.UPDATE = 0;
    ad9959_delay(2);
    *ad9959_io.UPDATE = 1;
    ad9959_delay(4);
    *ad9959_io.UPDATE = 0;
}

/**
 * @brief 通过SPI向AD9959写数据
 * @param register_address 寄存器地址
 * @param number_of_registers 所含字节数
 * @param register_data 数据起始地址
 * @param update 是否更新IO寄存器
 * */
void ad9959_write_data(AD9959_REG_ADDR register_address, uint8_t number_of_registers, const uint8_t *register_data,
                       bool update) {
    uint8_t ControlValue = 0;
    uint8_t ValueToWrite = 0;
    uint8_t RegisterIndex = 0;
    uint8_t i = 0;

    assert_param(IS_AD9959_REG_ADDR(register_address));

    ControlValue = register_address;
//写入地址
    *ad9959_io.SCLK = 0;
    *ad9959_io.CS = 0;
    for (i = 0; i < 8; i++) {
        *ad9959_io.SCLK = 0;
        if (0x80 == (ControlValue & 0x80))
            *ad9959_io.SDIO0 = 1;
        else
            *ad9959_io.SDIO0 = 0;
        *ad9959_io.SCLK = 1;
        ControlValue <<= 1;
    }
    *ad9959_io.SCLK = 0;
//写入数据
    for (RegisterIndex = 0; RegisterIndex < number_of_registers; RegisterIndex++) {
        ValueToWrite = register_data[RegisterIndex];
        for (i = 0; i < 8; i++) {
            *ad9959_io.SCLK = 0;
            if (0x80 == (ValueToWrite & 0x80))
                *ad9959_io.SDIO0 = 1;
            else
                *ad9959_io.SDIO0 = 0;
            *ad9959_io.SCLK = 1;
            ValueToWrite <<= 1;
        }
        *ad9959_io.SCLK = 0;
    }
    if (update) ad9959_io_update();
    *ad9959_io.CS = 1;
}

/**
 * @brief 设置通道输出相位
 * @param channel 输出通道
 * @param phase 输出相位 14bit 输出相位范围：0~16383(对应角度：0°~360°)
 * */
void ad9959_write_phase(AD9959_CHANNEL channel, uint16_t phase) {
    uint8_t cs_data = channel;
    assert_param(IS_AD9959_CHANNEL(channel));
    CPOW0_DATA[1] = (uint8_t) phase;
    CPOW0_DATA[0] = (uint8_t) (phase >> 8);
    ad9959_write_data(AD9959_REG_CSR, 1, &cs_data, 1);
    ad9959_write_data(AD9959_REG_CPOW0, 2, CPOW0_DATA, 1);

}

/**
 * @brief 设置通道输出频率
 * @param channel 输出通道
 * @param amplitude 输出频率 (频率范围 1 ~ 200000000Hz)
 * */
void ad9959_write_frequency(AD9959_CHANNEL channel, uint32_t Freq) {
    uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00};    //中间变量
    uint32_t frequency;
    uint8_t cs_data = channel;

    assert_param(IS_AD9959_CHANNEL(channel));

    frequency = (uint32_t) Freq * 8.589934592;       //将输入频率因子分为四个字节  8.589934592=(2^32)/500000000 其中500M=25M*20(倍频数可编程)
    CFTW0_DATA[3] = (uint8_t) frequency;
    CFTW0_DATA[2] = (uint8_t) (frequency >> 8);
    CFTW0_DATA[1] = (uint8_t) (frequency >> 16);
    CFTW0_DATA[0] = (uint8_t) (frequency >> 24);

    ad9959_write_data(AD9959_REG_CSR, 1, &cs_data, 1);
    ad9959_write_data(AD9959_REG_CFTW0, 4, CFTW0_DATA, 1);


}


/**
 * @brief 设置通道输出幅度
 * @param channel 输出通道
 * @param amplitude 输出幅度 10bit 输出相位范围：0~1023(对应幅度：0 ~ 530mV)
 * */
void ad9959_write_amplitude(AD9959_CHANNEL channel, uint16_t amplitude) {
    uint8_t ACR_DATA[3] = {0x00, 0x00, 0x00};//default Value = 0x--0000 Rest = 18.91/Iout
    uint8_t cs_data = channel;

    assert_param(IS_AD9959_CHANNEL(channel));

    amplitude = amplitude | 0x1000;
    ACR_DATA[2] = (uint8_t) amplitude;
    ACR_DATA[1] = (uint8_t) (amplitude >> 8);

    ad9959_write_data(AD9959_REG_CSR, 1, &cs_data, 1);
    ad9959_write_data(AD9959_REG_ACR, 3, ACR_DATA, 1);

}
