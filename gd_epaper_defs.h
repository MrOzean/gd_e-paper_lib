#ifndef _GD_EPAPER_DEFS_H_
#define _GD_EPAPER_DEFS_H_

#include <stdint.h>

#ifndef GD_EPAPER_USE_HARDWARE_SPI // use hardware SPI  instead software implementation
#define GD_EPAPER_USE_SOFTWARE_SPI
#endif

#ifndef GD_EPAPER_USE_3_WIRE_SPI  // 
#define GD_EPAPER_USE_4_WIRE_SPI
#endif

#define GDEY075T7 // 7.5 inch e-ink screen 3s/frame electronic paper display, GDEY075T7
                  // This is a 7.5 inch e-ink screen with 800x480 resolution, UC8179, SPI interface
                  // and the electronic paper display supports 4 grayscale.

#ifdef GDEY075T7

#define GD_EPAPER_WIDTH 800
#define GD_EPAPER_HEIGHT 480

#define GD_EPAPER_SCREEN_BUFFER_SIZE (GD_EPAPER_WIDTH * GD_EPAPER_HEIGHT / 8)

#define GD_EPAPER_HRES_BYTE_HIGH 0x03 // 800
#define GD_EPAPER_HRES_BYTE_LOW 0x20
#define GD_EPAPER_VRES_BYTE_HIGH 0x01 // 480
#define GD_EPAPER_VRES_BYTE_LOW 0xE0

#define GD_EPAPER_POWER_SETTINGS_1 0x01 // POWER SETTING
#define GD_EPAPER_POWER_SETTINGS_2 0x07
#define GD_EPAPER_VGH_VGL 0x07 // VGH=20V,VGL=-20V
#define GD_EPAPER_VDH 0x3f     // VDH=15V
#define GD_EPAPER_VDL 0x3f     // VDL=-15V

#define GD_EPAPER_POWER_ON 0x04 // Power on

#define GD_EPAPER_PANNEL_SETTING_1 0X00 // PANNEL SETTING
#define GD_EPAPER_PANNEL_SETTING_2 0x1F // KW-3f   KWR-2F BWROTP 0f BWOTP 1f
#define GD_EPAPER_PANNEL_SETTING_3 0x61 // tres

#define GD_EPAPER_PANNEL_SETTING_4 0x15
#define GD_EPAPER_PANNEL_SETTING_5 0x00

#define GD_EPAPER_VCOM_1 0X50 // VCOM AND DATA INTERVAL SETTING
#define GD_EPAPER_VCOM_2 0x10
#define GD_EPAPER_VCOM_3 0x07

#define GD_EPAPER_TCON_1 0X60 // TCON SETTING
#define GD_EPAPER_TCON_2 0x22

#define GD_EPAPER_DISPLAY_REFRESH 0x12

#define GD_EPAPER_DISPLAY_WAIT 0x71

typedef enum
{
    GD_EPAPER_WHITE = 0x00,
    GD_EPAPER_BLACK = 0xFF,
} gd_epaper_color;

#endif

typedef enum
{
    GD_EPAPER_GPIO_LOW = 0x0,
    GD_EPAPER_GPIO_HIGH = 0x1
} gd_epaper_gpio_value;

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific write functions of the user
 *
 * @param[in] data          : Pointer to data buffer in which data to be written
 *                            is stored.
 * @param[in] len           : Number of bytes of data to be written.
 * @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                            for interface related call backs
 *
 * @retval 0                -> Success.
 * @retval Non zero value   -> Fail.
 *
 */
typedef int8_t (*gd_epaper_spi_write_fptr_t)(uint8_t *data, size_t len);

/*!
 * @brief GPIO write function pointer which should be mapped to
 * the platform specific write functions of the user
 *
 * @param[in] gpio          : Any digital preconfigured GPIO
 * @param[in] value         : GPIO value
 * @param[in, out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                            for interface related call backs
 *
 */
typedef void (*gd_epaper_write_gpio_fptr_t)(uint8_t gpio, gd_epaper_gpio_value value);

/*!
 * @brief GPIO read function pointer which should be mapped to
 * the platform specific write functions of the user
 *
 * @param[in] gpio          : Any digital preconfigured GPIO
 *
 * @retval GPIO value (GD_EPAPER_GPIO_LOW or GD_EPAPER_GPIO_HIGH)
 *
 */
typedef gd_epaper_gpio_value (*gd_epaper_read_gpio_fptr_t)(uint8_t gpio);

/*!
 * @brief Delay function pointer which should be mapped to
 * delay function of the user
 *
 * @param[in] period              : Delay in microseconds.
 * @param[in, out] intf_ptr       : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs
 *
 */
typedef void (*gd_epaper_delay_ms_fptr_t)(uint32_t period);

/*!
 * @brief e-paper display device
 */
typedef struct
{
    /* hardware interface function pointer used to enable the device address for SPI, can be undefined if software SPI enabled */
    void *spi_bus_ptr;
    /* user defined hardware  SPI write funtion pointer, can be undefined if software SPI enabled */
    gd_epaper_spi_write_fptr_t spi_write_fptr;
    /* user defined hardware  GPIO read, required */
    gd_epaper_read_gpio_fptr_t gpio_read_fptr;
    /* user defined hardware GPIO write, required */
    gd_epaper_write_gpio_fptr_t gpio_write_fptr;
    /* user defined microseconds delay function, required */
    gd_epaper_delay_ms_fptr_t delay_ms_fptr;
    /* BUSY pin */
    int busy_pin;
    /* RESET pin */
    int reset_pin;
    /* D/C pin */
    int dc_pin;
    /* RESET pin, can be undefined if hardware SPI enabled */
    int mosi_pin;
    /* RESET pin, can be undefined if hardware SPI enabled */
    int clk_pin;
    /* CS pin, can be undefined if hardware SPI enabled*/
    int cs_pin;

    /* screen buffer ptr */
    uint8_t* screen_buffer;

    // SemaphoreHandle_t ready_sem; ///< Semaphore for ready signal
} gd_epaper_display_dev;

#endif