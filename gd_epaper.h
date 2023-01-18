#ifndef _GD_EPAPER_H_
#define _GD_EPAPER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "./gd_epaper_defs.h"

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
void gd_epaper_update_screen(gd_epaper_display_dev *display);

void gd_epaper_clean_screen (gd_epaper_display_dev *display);
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
void gd_epaper_draw_pixel(gd_epaper_display_dev *display, uint16_t x, uint16_t y, gd_epaper_color color);

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
void gd_epaper_move_cursor(gd_epaper_display_dev *display, uint16_t x, uint16_t y);

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
void gd_epaper_write_string(gd_epaper_display_dev *display, char *str, gd_epaper_color color);

#endif