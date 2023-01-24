# GooDisplay display driver

## Brief  

Full featured C11 screen buffer based driver for GooDisplay e-paper display with  4-wire hardware/software SPI
Current supported:

- 7.5" 800x480 (GDEY075T7); [official site](https://www.good-display.com/product/396.html), [Aliexpress shop](https://aliexpress.com/item/1005002958176971.html)

## Usage

1. Copy to you project libraries
2. Set `GD_EPAPER_USE_HARDWARE_SPI` or `GD_EPAPER_USE_SOFTWARE_SPI` in `gd_epaper_defs.h`
3. Implement platform specific functions
4. Initialize device (`display_dev`)
5. Write someone data to screen buffer
6. Enjoy

In case of troubles see examples
