// Hardware based example implementation on ESP32-C3 chip
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <sys/random.h> // for action

// #include "gd_epaper_defs.h" <- in this file GD_EPAPER_USE_HARDWARE_SPI must be defined
#include "gd_epaper.h" // main header

#define GD_SCREEN_HOST SPI2_HOST // any unused SPI host

#define PIN_NUM_BUSY 2
#define PIN_NUM_DC 5
#define PIN_NUM_RST 4

#define PIN_NUM_MOSI 7
#define PIN_NUM_CLK 6
#define PIN_NUM_CS 8

// we need D/C and reset pins as ouput
#define GPIO_OUTPUT_PIN_SELECT ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST))
// and busy as input
#define GPIO_INPUT_PIN_SELECT ((1ULL << PIN_NUM_BUSY))

// display
uint8_t buff[GD_EPAPER_SCREEN_BUFFER_SIZE]; // screen buffer
gd_epaper_display_dev display_dev = {};     // display handler
spi_device_handle_t display_spi;            // esp-idf specific device spi handler

// digital gpio read function, must be defined using platform specific functions
gd_epaper_gpio_value gpio_read(uint8_t gpio)
{
    return (gd_epaper_gpio_value)gpio_get_level(gpio);
}
// digital gpio write function, must be defined using platform specific functions
void gpio_write(uint8_t gpio, gd_epaper_gpio_value value)
{
    gpio_set_level(gpio, (uint8_t)value);
}
//  milliseconds delay function, must be defined using platform specific functions
void delay_us(uint32_t period)
{
    vTaskDelay(period / portTICK_PERIOD_MS); // good option to use FreeRTOS delay
}
// digital SPI write function, must be defined using platform specific functions
int8_t spi_write(uint8_t *data, size_t len)
{
    spi_transaction_t t = {
        .tx_buffer = data,
        .length = len,
    };
    return spi_device_transmit(display_spi, &t);
}

// setup display
void init_display(void)
{
    // set all GPIO's
    display_dev.busy_pin = PIN_NUM_BUSY;
    display_dev.clk_pin = PIN_NUM_CLK;
    display_dev.cs_pin = PIN_NUM_CS;
    display_dev.dc_pin = PIN_NUM_DC;
    display_dev.mosi_pin = PIN_NUM_MOSI;
    display_dev.reset_pin = PIN_NUM_RST;
    // set screenbufer
    display_dev.screen_buffer = &buff;

    // set functions
    display_dev.gpio_read_fptr = gpio_read;
    display_dev.gpio_write_fptr = gpio_write;
    display_dev.delay_us_fptr = delay_us;
    display_dev.spi_write_fptr = spi_write;
}

/// utility functions
// draw single pixel function
void draw_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    if (x > GD_EPAPER_WIDTH || y > GD_EPAPER_HEIGHT)
    {
        // Don't write outside the buffer
        return;
    }

    // draw in the right color
    if ((uint8_t)color == GD_EPAPER_BLACK)
    {
        buff[(y * GD_EPAPER_WIDTH / 8 + x / 8)] |= 1 << (7 - x % 8);
    }
    else
    {
        buff[(y * GD_EPAPER_WIDTH / 8 + x / 8)] &= ~(1 << (7 - x % 8));
    }
}
// draw line
void draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color)
{
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;

    draw_pixel(x2, y2, color);
    while ((x1 != x2) || (y1 != y2))
    {
        draw_pixel(x1, y1, color);
        error2 = error * 2;
        if (error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        else
        {
            /*nothing to do*/
        }

        if (error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
        else
        {
            /*nothing to do*/
        }
    }
    return;
}
// convert one number range to another (not precise, using ints)
int16_t map_value(int16_t input, int16_t input_start, int16_t input_end, int16_t output_start, int16_t output_end)
{
    return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
}
// fill entire screenbuffer with single color
void fill_color(uint8_t color)
{
    for (uint16_t x = 0; x < GD_EPAPER_WIDTH; x += 1)
    {
        for (uint16_t y = 0; y < GD_EPAPER_HEIGHT; y += 1)
        {
            draw_pixel(x, y, color);
        }
    }
}
/// platform specific setup

// esp-idf specific hardware SPI initialization
static esp_err_t init_spi(void)
{
    esp_err_t ret;

    ESP_LOGI("TAG", "Initializing bus SPI%d...", GD_SCREEN_HOST + 1);
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000000, // Clock out at 1 MHz
        .mode = 0,                     // SPI mode 0
        .spics_io_num = PIN_NUM_CS,    // CS pin
        .queue_size = 1,               // We want to be able to queue 7 transactions at a time
    };
    // Initialize the SPI bus
    ret = spi_bus_initialize(GD_SCREEN_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    // Attach the e-paper to the SPI bus
    ret = spi_bus_add_device(GD_SCREEN_HOST, &devcfg, &display_spi);
    return ret;
}
// esp-idf specific GPIO initialization
void init_gpio(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SELECT;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SELECT;
    // disable pull-down mode
    // io_conf.pull_down_en = 0;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    // configure GPIO with the given settings
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

void app_main(void)
{

    ESP_ERROR_CHECK(init_spi());
    ESP_LOGI("EXAMPLE", "SPI initialized successfully");

    init_display();
    init_gpio();

    int lines_count = 5 * 4;
    uint8_t random[lines_count];
    int max_lines = 0;

    while (1)
    {
        // clean display, if too many lines
        if (max_lines > 30)
        {
            max_lines = 0;
            fill_color(GD_EPAPER_WHITE); // clean by filling white color
        }

        getrandom(random, lines_count, NULL); // get bunch of random values
        for (size_t i = 0; i < lines_count; i += 4)
        {
            // draw line with random coordinates
            draw_line(
                map_value(random[i], 0, 255, 0, GD_EPAPER_WIDTH),
                map_value(random[i + 1], 0, 255, 0, GD_EPAPER_HEIGHT),
                map_value(random[i + 2], 0, 255, 0, GD_EPAPER_WIDTH),
                map_value(random[i + 3], 0, 255, 0, GD_EPAPER_HEIGHT), GD_EPAPER_BLACK);
        }

        gd_epaper_update_screen(&display_dev);
        max_lines += 1;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}