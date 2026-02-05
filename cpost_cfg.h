/**
 * @file cpost_cfg.h
 * @author Aki
 * @brief cpost 配置文件
 * @version 1.3.1
 * @date 2026-02-05
 */
#ifndef CPOST_CFG_H
#define CPOST_CFG_H

#include <stddef.h>
#include <stdint.h>

#include "main.h"

/**
 * @brief 获取系统tick
 */
#define CPOST_GET_TICK() HAL_GetTick()

/**
 * @brief tick 类型定义
 */
#define CPOST_TICK_TYPE uint32_t

/**
 * @brief tick 最大值
 */
#define CPOST_MAX_TICK 0xFFFFFFFF

/**
 * @brief 进入临界区
 */
#define CPOST_ENTER_CRITICAL() __disable_irq()

/**
 * @brief 退出临界区
 */
#define CPOST_EXIT_CRITICAL() __enable_irq()

#endif /* CPOST_CFG_H */
