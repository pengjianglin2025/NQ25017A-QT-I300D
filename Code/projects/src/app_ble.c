/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file app_ble.c
 * @author Nations Firmware Team
 * @version v1.0.2
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */
#include <string.h>
#include "n32wb03x.h"
#include "gapm_task.h"               // GAP management task interface
 
#include "app_ble.h"
#include "app_usart.h"
#include "app_gpio.h"
#include "app_power.h"

#include "ns_sec.h"
#include "app_rdtss_16bit.h"
#include "app_dis.h"
#include "app_batt.h"
#if (BLE_APP_NS_IUS)
#include "app_ns_ius.h"
#endif // BLE_APP_NS_IUS enabled
#include "app_user_config.h"
#include "ringbuffer.h"
#include "led.h"
#include "bat.h"
#include "wf433.h"
#include "aromaevent.h"
/** @addtogroup 
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool app_ble_initialized = false;

/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
void app_ble_connected(void);
void app_ble_disconnected(void);

bool app_ble_is_initialized(void)
{
    return app_ble_initialized;
}

/**
 * @brief  User message handler
 * @param  
 * @return 
 * @note   
 */
void app_user_msg_handler(ke_msg_id_t const msgid, void const *p_param)
{
    
    switch (msgid)
    {
    	case APP_CUSTS_TEST_EVT:
            app_usart_tx_process();
    		break;
			
    	case APP_5MS_EVT:
				app_power_process_pending();
				if(power.status == POWER_OFF) {if(++power.offTime > 1000) power.offTime = 1000;}
					else { power.offTime = 0; }
					
			Key_Scan();
			Key_Task();
					airpump_gpio_out();
					Led_Task();
					if(++ReceiveIdleCount > 200) ReceiveIdleCount = 200;
					if(++SendIdleCount > 200) SendIdleCount = 200;
					ke_timer_set(APP_5MS_EVT, TASK_APP, 5);
    		break;
    	case APP_10MS_EVT:
					Lcd_Task();
					Buzzer_Task10ms();
			
					ke_timer_set(APP_10MS_EVT, TASK_APP, 10);
    		break;
    	case APP_20MS_EVT:
		
					ke_timer_set(APP_20MS_EVT, TASK_APP, 20);
    		break;
			
    	case APP_100MS_EVT:
					Display_Task();
					ring_buffer_read();
					app_data_parse_task();
					event_Task();
			oil_currentVolume_calculate_task();
					app_data_up_task();
			
					ke_timer_set(APP_100MS_EVT, TASK_APP, 100);
    		break;
			
    	case APP_500MS_EVT:
        RTC_DateShow();
        RTC_TimeShow();
					ke_timer_set(APP_500MS_EVT, TASK_APP, 500);
    		break;
			
    	case APP_1S_EVT:
				
				oil_currentVolume_calculate_task();
				oil_surplusDay_task();
				Iap_Data_Comparison();
			
				ke_timer_set(APP_1S_EVT, TASK_APP, 1000);
    		break;

    	default:
    		break;
    }
 

}

/**
 * @brief  BLE message handler
 * @param  
 * @return 
 * @note   
 */
void app_ble_msg_handler(struct ble_msg_t const *p_ble_msg)
{
    switch (p_ble_msg->msg_id)
    {
        case APP_BLE_OS_READY:
            NS_LOG_INFO("APP_BLE_OS_READY\r\n");
				
					ke_timer_set(APP_5MS_EVT, TASK_APP, 5);
					ke_timer_set(APP_10MS_EVT, TASK_APP, 10);
					ke_timer_set(APP_20MS_EVT, TASK_APP, 20);
				ke_timer_set(APP_100MS_EVT, TASK_APP, 100);
				ke_timer_set(APP_500MS_EVT, TASK_APP, 500);
					ke_timer_set(APP_1S_EVT, TASK_APP, 1000);
            break;
        case APP_BLE_GAP_CONNECTED:
            app_ble_connected();
            break;
        case APP_BLE_GAP_DISCONNECTED:
            app_ble_disconnected();
            break;

        default:
            break;
    }

}

/**
 * @brief  User message handler?
 * @param  
 * @return 
 * @note   
 */
void app_ble_adv_msg_handler(enum app_adv_mode adv_mode)
{
    switch (adv_mode)
    {
        case APP_ADV_MODE_DIRECTED:
            
            break;
        case APP_ADV_MODE_FAST:
            
            break;
        case APP_ADV_MODE_SLOW:
            
            break;
        default:
            break;
    }

}
    

/**
 * @brief  BLE GAP parameter initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_gap_params_init(void)
{
    struct ns_gap_params_t dev_info = {0};
    uint8_t *p_mac = SystemGetMacAddr();
    // Read the MAC address from TRIM.
    if(p_mac != NULL)
    {
        // Copy the MAC address into the device address.
        memcpy(dev_info.mac_addr.addr, p_mac , BD_ADDR_LEN); 
    }
    else{
        memcpy(dev_info.mac_addr.addr, "\x01\x02\x03\x04\x05\x06" , BD_ADDR_LEN);
    }
    

    /* Configure a static random address. */
    dev_info.mac_addr_type = GAPM_STATIC_ADDR;
    dev_info.appearance = 0;
    dev_info.dev_role = GAP_ROLE_PERIPHERAL;
    
    dev_info.dev_name_len = sizeof(CUSTOM_DEVICE_NAME)-1;
    memcpy(dev_info.dev_name, CUSTOM_DEVICE_NAME, dev_info.dev_name_len); 
		sprintf(&dev_info.dev_name[8], "%X%X%X%X", (*(dev_info.mac_addr.addr+5))/16,(*(dev_info.mac_addr.addr+5))%16,\
		(*(dev_info.mac_addr.addr+4))/16,(*(dev_info.mac_addr.addr+4))%16);// Append the last two MAC bytes as hex.
   
   
    dev_info.dev_conn_param.intv_min = MSECS_TO_UNIT(MIN_CONN_INTERVAL,MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.intv_max = MSECS_TO_UNIT(MAX_CONN_INTERVAL,MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.latency  = SLAVE_LATENCY;
    dev_info.dev_conn_param.time_out = MSECS_TO_UNIT(CONN_SUP_TIMEOUT,MSECS_UNIT_10_MS);
    dev_info.conn_param_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    
    ns_ble_gap_init(&dev_info);
    
}


/**
 * @brief  BLE advertising initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_adv_init(void)
{
    struct ns_adv_params_t user_adv = {0};
    
    // Load custom advertising and scan response payloads.
    user_adv.adv_data_len = CUSTOM_USER_ADVERTISE_DATA_LEN;
    memcpy(user_adv.adv_data,CUSTOM_USER_ADVERTISE_DATA,CUSTOM_USER_ADVERTISE_DATA_LEN);
    user_adv.scan_rsp_data_len = CUSTOM_USER_ADV_SCNRSP_DATA_LEN;
    memcpy(user_adv.scan_rsp_data,CUSTOM_USER_ADV_SCNRSP_DATA,CUSTOM_USER_ADV_SCNRSP_DATA_LEN);
    
    user_adv.attach_appearance  = false;
    user_adv.attach_name        = true;
    user_adv.ex_adv_enable      = false;
    user_adv.adv_phy            = PHY_1MBPS_VALUE;
    
    // Disable directed advertising.
    user_adv.directed_adv.enable = false;

    user_adv.fast_adv.enable    = true;
    user_adv.fast_adv.duration  = CUSTOM_ADV_FAST_DURATION;
    user_adv.fast_adv.adv_intv  = CUSTOM_ADV_FAST_INTERVAL;
    
    user_adv.slow_adv.enable    = true;  
    user_adv.slow_adv.duration  = CUSTOM_ADV_SLOW_DURATION;
    user_adv.slow_adv.adv_intv  = CUSTOM_ADV_SLOW_INTERVAL;
    
    user_adv.ble_adv_msg_handler = app_ble_adv_msg_handler;
    
    #if (EX_ADV_TEST_ENABLE)
    user_adv.ex_adv_enable = true;
    const static uint8_t ex_adv[] = {"\x0D\x09RDTSS-EX_ADV\x29\xff""1234567890123456789012345678901234567890"};
    user_adv.ex_adv_p_data = (uint8_t*)ex_adv;
    user_adv.ex_adv_data_len = 14+42;
    #if (CODED_ADV_TEST_ENABLE)
    user_adv.adv_phy            = PHY_CODED_VALUE;
    #endif
    #endif
    ns_ble_adv_init(&user_adv);
    

}

void app_ble_sec_init(void)
{
    struct ns_sec_init_t sec_init = {0};
    
    sec_init.rand_pin_enable = false;
    sec_init.pin_code = 123456;
    
    sec_init.pairing_feat.auth      = ( SEC_PARAM_BOND | (SEC_PARAM_MITM<<2) | (SEC_PARAM_LESC<<3) | (SEC_PARAM_KEYPRESS<<4) );
    sec_init.pairing_feat.iocap     = SEC_PARAM_IO_CAPABILITIES;
    sec_init.pairing_feat.key_size  = SEC_PARAM_KEY_SIZE;
    sec_init.pairing_feat.oob       = SEC_PARAM_OOB;
    sec_init.pairing_feat.ikey_dist = SEC_PARAM_IKEY;
    sec_init.pairing_feat.rkey_dist = SEC_PARAM_RKEY;
    sec_init.pairing_feat.sec_req   = SEC_PARAM_SEC_MODE_LEVEL;
    
    sec_init.bond_enable            = BOND_STORE_ENABLE;
    sec_init.bond_db_addr           = BOND_DATA_BASE_ADDR;
    sec_init.bond_max_peer          = MAX_BOND_PEER;
    sec_init.bond_sync_delay        = 2000;
    
    sec_init.ns_sec_msg_handler     = NULL;
    
    ns_sec_init(&sec_init);
}

void app_ble_prf_init(void)
{
    #if (BLE_APP_DIS)
    // Register the Device Information Service.
    ns_ble_add_prf_func_register(app_dis_add_dis);    
    #endif // BLE_APP_DIS enabled
    #if (BLE_APP_BATT)
    // Register the Battery Service.
    ns_ble_add_prf_func_register(app_batt_add_bas);
    #endif // BLE_APP_BATT enabled
    #if (BLE_APP_NS_IUS)
    ns_ble_add_prf_func_register(app_ns_ius_add_ns_ius);
    #endif // BLE_APP_NS_IUS enabled    
    // Register the custom RDTSS service.
    ns_ble_add_prf_func_register(app_rdtss_16bit_add_rdtss_16bit);
    

    
}


/**
 * @brief  BLE stack initialization
 * @param  
 * @return 
 * @note   
 */
void app_ble_init(void)
{
    if (app_ble_initialized)
    {
        return;
    }
    struct ns_stack_cfg_t app_handler = {0};
    app_handler.ble_msg_handler  = app_ble_msg_handler;
    app_handler.user_msg_handler = app_user_msg_handler;
    app_handler.lsc_cfg          = BLE_LSC_LSE_32768HZ;
    // Initialize the BLE stack.
    ns_ble_stack_init(&app_handler);
    
    app_ble_gap_params_init();
    app_ble_sec_init();
    app_ble_adv_init();
    app_ble_prf_init();
    app_ble_initialized = true;

    /* 只在外部电源存在时启动广播，纽扣电池模式下保留协议栈调度但不主动广播。 */
    if (app_power_is_dcin_connected())
    {
        ns_ble_adv_start();
    }
}

/**
 * @brief  BLE connected callback
 * @param  
 * @return 
 * @note   
 */
void app_ble_connected(void)
{
    // Mark BLE as connected for UI display.
lcd.bleStatus = 1;  
    
    #if (BLE_APP_BATT)
    // Enable the battery profile after connection.
    app_batt_enable_prf(app_env.conidx);
    #endif // BLE_APP_BATT enabled    
    #if (0)
    ns_ble_phy_set(GAP_PHY_125KBPS);
    #endif
}

/**
 * @brief  BLE disconnected callback
 * @param  
 * @return 
 * @note   
 */
void app_ble_disconnected(void)
{
    /* Restart advertising only when the device is still in the active state. */
    if(power.status == POWER_ON)
    {
        ns_ble_adv_start();
    }
  
    // Mark BLE as disconnected for UI display.
	lcd.bleStatus = 0;
}





/**
 * @}
 */











