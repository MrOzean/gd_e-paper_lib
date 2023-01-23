#include "gd_epaper.h"

#ifdef GD_EPAPER_USE_SOFTWARE_SPI
/*!
 * @brief Stupid delay function
 */
static inline void delay_ns(size_t ns)
{
    for (; ns > 1; ns--)
        ;
}
#endif

/*!
 * @brief internal write function, uses hardware or implements software spi if enabled
 */
static void spi_write(gd_epaper_display_dev *display, uint8_t value, bool is_command)
{

#ifdef GD_EPAPER_USE_SOFTWARE_SPI
    // if software spi, emulate it
    display->gpio_write_fptr(display->cs_pin, GD_EPAPER_GPIO_LOW); // toggle CS pin
    delay_ns(2);
#ifdef GD_EPAPER_USE_3_WIRE_SPI
    // if defined GD_EPAPER_USE_3_WIRE_SPI, send first bit 0 if command , 1 if data
    display->gpio_write_fptr(display->clk_pin, GD_EPAPER_GPIO_LOW);
    delay_ns(2);
    if (is_command)
    {
        display->gpio_write_fptr(display->mosi_pin, GD_EPAPER_GPIO_LOW);
    }
    else
    {
        display->gpio_write_fptr(display->mosi_pin, GD_EPAPER_GPIO_HIGH);
    }
    delay_ns(3);
    display->gpio_write_fptr(display->clk_pin, GD_EPAPER_GPIO_HIGH);
    delay_us(2);
#endif
    // send value bits
    for (uint8_t i = 0; i < 8; i++)
    {
        display->gpio_write_fptr(display->clk_pin, GD_EPAPER_GPIO_LOW);
        delay_ns(2);
        if (value & 0x80)
        {
            display->gpio_write_fptr(display->mosi_pin, GD_EPAPER_GPIO_HIGH);
        }
        else
        {
            display->gpio_write_fptr(display->mosi_pin, GD_EPAPER_GPIO_LOW);
        }
        value = (value << 1);
        delay_ns(3);
        display->gpio_write_fptr(display->clk_pin, GD_EPAPER_GPIO_HIGH);
        delay_ns(2);
    }
    display->gpio_write_fptr(display->cs_pin, GD_EPAPER_GPIO_HIGH); // toggle CS pin
#endif
#ifdef GD_EPAPER_USE_HARDWARE_SPI
// on hardware spi, just use it
#ifdef GD_EPAPER_USE_4_WIRE_SPI
    uint8_t buff[] = {value};
    display->spi_write_fptr(buff, 8);
#else
    // if defined GD_EPAPER_USE_3_WIRE_SPI, send first bit 0 if command , 1 if data
    uint8_t cmd_data = (!is_command & 0x01);
    uint8_t buff[] = {cmd_data, value};
    display->spi_write_fptr(buff, 9);
#endif
#endif
}
/*!
 * @brief internal  command write function
 */
static void write_command(gd_epaper_display_dev *display, uint8_t value)
{
#ifdef GD_EPAPER_USE_4_WIRE_SPI
    // on 4_WIRE_SPI DC must be set 0 to indicates command write
    display->gpio_write_fptr(display->dc_pin, GD_EPAPER_GPIO_LOW); // EPD_W21_DC_0; // command write
#endif
    spi_write(display, value, true);
}
/*!
 * @brief internal data write  function
 */
static void write_data(gd_epaper_display_dev *display, uint8_t value)
{
#ifdef GD_EPAPER_USE_4_WIRE_SPI
    // on 4_WIRE_SPI DC must be set 1 to indicates data write
    display->gpio_write_fptr(display->dc_pin, GD_EPAPER_GPIO_HIGH); // EPD_W21_DC_1; // data write
#endif
    spi_write(display, value, false);
}

/*!
 * @brief Internal function to wait display refresh. Loop until ic set 0 on busy pin
 *
 * @param[in] display          : Display device pointer
 */
static void wait_display(gd_epaper_display_dev *display)
{
    uint8_t busy;
    do
    {
        write_command(display, GD_EPAPER_DISPLAY_WAIT);
        busy = (uint8_t)(display->gpio_read_fptr(display->busy_pin));
        busy = !(busy & 0x01);
        display->delay_us_fptr(100);
    } while (busy);
    display->delay_us_fptr(200); // minimum 100 us
}
#ifdef GDEY075T7
/*!
 * @brief Internal function to wakeup and init display
 *
 * @param[in] display          : Display device pointer
 */
static void send_init(gd_epaper_display_dev *display)
{

    display->gpio_write_fptr(display->reset_pin, GD_EPAPER_GPIO_LOW);  //  IC reset
    display->delay_us_fptr(15);                                        //!!! At least 10ms
    display->gpio_write_fptr(display->reset_pin, GD_EPAPER_GPIO_HIGH); //  IC reset
    display->delay_us_fptr(15);                                        //!!! At least 10ms

    write_command(display, GD_EPAPER_POWER_SETTINGS_1); // POWER SETTING
    write_data(display, GD_EPAPER_POWER_SETTINGS_2);
    write_data(display, GD_EPAPER_VGH_VGL); // VGH=20V,VGL=-20V
    write_data(display, GD_EPAPER_VDH);     // VDH=15V
    write_data(display, GD_EPAPER_VDL);     // VDL=-15V

    write_command(display, GD_EPAPER_POWER_ON); // Power on
    wait_display(display);                      // waiting for the electronic paper IC to release the idle signal

    write_command(display, GD_EPAPER_PANNEL_SETTING_1); // PANNEL SETTING
    write_data(display, GD_EPAPER_PANNEL_SETTING_2);    // KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    write_command(display, GD_EPAPER_PANNEL_SETTING_3); // tres
    write_data(display, GD_EPAPER_HRES_BYTE_HIGH);      // source 800
    write_data(display, GD_EPAPER_HRES_BYTE_LOW);
    write_data(display, GD_EPAPER_VRES_BYTE_HIGH); // gate 480
    write_data(display, GD_EPAPER_VRES_BYTE_LOW);

    write_command(display, GD_EPAPER_PANNEL_SETTING_4);
    write_data(display, GD_EPAPER_PANNEL_SETTING_5);

    write_command(display, GD_EPAPER_VCOM_1); // VCOM AND DATA INTERVAL SETTING
    write_data(display, GD_EPAPER_VCOM_2);
    write_data(display, GD_EPAPER_VCOM_3);

    write_command(display, GD_EPAPER_TCON_1); // TCON SETTING
    write_data(display, GD_EPAPER_TCON_2);
}
/*!
 * @brief Internal function to send display refresh command
 *
 * @param[in] display          : Display device pointer
 */
static void send_refresh(gd_epaper_display_dev *display)
{
    write_command(display, GD_EPAPER_DISPLAY_REFRESH); // send refresh
    display->delay_us_fptr(20);                        //!!! The delay here is necessary, 20uS at least!!!
    wait_display(display);                             //  wait until drawing
}
/*!
 * @brief Internal function to send deep sleep command
 *
 * @param[in] display          : Display device pointer
 */
static void send_sleep(gd_epaper_display_dev *display)
{
    write_command(display, 0x50);
    write_data(display, 0xF7);

    write_command(display, 0x02); // power off
    wait_display(display);        // wait until execute
    write_command(display, 0x07); // deep sleep
    write_data(display, 0xA5);
}
#endif

void gd_epaper_update_screen(gd_epaper_display_dev *display)
{
    send_init(display);

    size_t i;
    write_command(display, 0x10); // Transfer old data
    for (i = 0; i < GD_EPAPER_SCREEN_BUFFER_SIZE; i++)
    {
        write_data(display, (uint8_t)0x00); // zero send required here
    }

    write_command(display, 0x13); // Transfer new data
    for (i = 0; i < GD_EPAPER_SCREEN_BUFFER_SIZE; i++)
    {
        write_data(display, display->screen_buffer[i]);
    }

    send_refresh(display);
    send_sleep(display);
}
