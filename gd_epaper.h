/*!
 * Main header for GooDisplay e-paper screens based on UC8179 ic driver 
 */

#ifndef _GD_EPAPER_H_
#define _GD_EPAPER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "./gd_epaper_defs.h"

/*!
 * @brief Full refresh display function (draw uploaded screen buffer), and send deep sleep command
 *
 * @param[in] display          : Display device pointer
 *                            
 */
void gd_epaper_update_screen(gd_epaper_display_dev *display);

#endif