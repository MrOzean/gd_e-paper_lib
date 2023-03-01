/*!
 * Main header for GooDisplay e-paper screens based on UC8179 ic driver
 */

#ifndef _GD_EPAPER_H_
#define _GD_EPAPER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "./gd_epaper_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /*!
     * @brief Function to wakeup and init display
     *
     * @param[in] display          : Display device pointer
     */
    void send_init(gd_epaper_display_dev *display);
    /*!
     * @brief Function to send display refresh command
     *
     * @param[in] display          : Display device pointer
     */
    void send_refresh(gd_epaper_display_dev *display);
    /*!
     * @brief Function to send deep sleep command
     *
     * @param[in] display          : Display device pointer
     */
    void send_sleep(gd_epaper_display_dev *display);
    /*!
     * @brief Function to send buffer to display
     *
     * @param[in] display          : Display device pointer
     */
    void send_buffer(gd_epaper_display_dev *display);
    /*!
     * @brief Full refresh display function. Init display, send and draw screen buffer, and send display to deep sleep
     *
     * @param[in] display          : Display device pointer
     *
     */
    void gd_epaper_update_screen(gd_epaper_display_dev *display);

#ifdef __cplusplus
}
#endif
#endif