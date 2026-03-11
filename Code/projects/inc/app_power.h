#ifndef __APP_POWER_H__
#define __APP_POWER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool app_power_is_dcin_connected(void);
void app_power_handle_dcin_irq(void);
void app_power_process_pending(void);
void app_sleep_prepare_proc(void);
void app_sleep_resume_proc(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_POWER_H__ */
