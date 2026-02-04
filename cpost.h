/**
 * @file cpost.h
 * @author Aki
 * @brief c post - 延迟任务调度器
 * @version 1.3.0
 * @date 2026-02-04
 */
#ifndef CPOST_H
#define CPOST_H

#include "cpost_cfg.h"

#define CPOST_OK 0            // 操作成功
#define CPOST_ERROR -1        // 一般错误
#define CPOST_ERROR_FULL -2   // 任务列表已满
#define CPOST_ERROR_PARAM -3  // 参数错误
#define CPOST_ERROR_EXISTS -4 // 任务已存在

#define CPOST_FLAG_CLEAR_FRONT 0    // post列表中，存在相同handler, 清除之前的post
#define CPOST_FLAG_CANCEL_CURRENT 1 // post列表中，存在相同handler, 取消当前post
#define CPOST_FLAG_ADD_NEW 2        // 添加新post，不论post列表中是否存在相同handler

#define CPOST_PARAM_DIFF_DISABLE 0 // 判断是否相同的handler时，忽略参数
#define CPOST_PARAM_DIFF_ENABLE 1  // 判断是否相同的handler时，包括参数的比较

#ifndef CPOST_ENTER_CRITICAL
/**
 * @brief 进入临界区
 */
#define CPOST_ENTER_CRITICAL()
#endif

#ifndef CPOST_EXIT_CRITICAL
/**
 * @brief 退出临界区
 */
#define CPOST_EXIT_CRITICAL()
#endif

#ifndef CPOST_MAX_HANDLER_SIZE
/**
 * @brief 最大handler数量
 */
#define CPOST_MAX_HANDLER_SIZE 16
#endif

#ifndef CPOST_GET_TICK
/**
 * @brief 获取系统tick函数
 * @return 当前系统tick值
 */
#define CPOST_GET_TICK() 0
#endif

#ifndef CPOST_MAX_TICK
/**
 * @brief tick最大值
 */
#define CPOST_MAX_TICK 0xFFFFFFFF
#endif

#ifndef CPOST_TICK_TYPE
/**
 * @brief tick类型定义
 */
#define CPOST_TICK_TYPE uint32_t
#endif

/**
 * @brief handler 结构体
 */
typedef struct
{
    CPOST_TICK_TYPE start_time; // 任务开始时间
    CPOST_TICK_TYPE delay;      // 延迟时间
    void (*handler)(void *);    // 回调函数
    void *param;                // 回调参数
} cpost_handler_t;

/**
 * @brief cpost 参数结构体
 */
typedef struct
{
    void *handler;         // 回调函数指针
    void *param;           // 回调参数
    CPOST_TICK_TYPE delay; // 延迟时间
    struct
    {
        unsigned char flag : 2;       // 冲突处理标志
        unsigned char param_diff : 1; // 是否比较参数
        unsigned char reserved : 5;   // 保留位
    } attrs;
} cpost_param_t;

/**
 * @brief 统计信息结构体
 */
typedef struct
{
    size_t active_count; // 当前活动任务数
    size_t total_slots;  // 总槽位数
    size_t free_slots;   // 空闲槽位数
} cpost_stats_t;

/**
 * @brief cpost 主接口宏
 *
 * @param ... 参数列表
 *
 * @return int 错误码 (CPOST_OK, CPOST_ERROR_FULL, CPOST_ERROR_PARAM 等)
 */
#define cpost(...) \
    cpost_add_handler(&((cpost_param_t){__VA_ARGS__}))

/**
 * @brief 延迟执行宏（无参数）
 * @param _handler 回调函数
 * @param _delay 延迟时间
 */
#define cpost_delay(_handler, _delay) \
    cpost((_handler), .delay = (_delay))

/**
 * @brief 立即执行宏
 * @param _handler 回调函数
 * @param _param 参数
 */
#define cpost_ex(_handler, _param) \
    cpost((_handler), .param = (_param))

/**
 * @brief 延迟执行宏（带参数）
 * @param _handler 回调函数
 * @param _param 参数
 * @param _delay 延迟时间
 */
#define cpost_delay_ex(_handler, _param, _delay) \
    cpost((_handler), .param = (_param), .delay = (_delay))

/**
 * @brief 添加handler到任务列表
 *
 * @param param 参数结构体指针
 *
 * @return int 错误码
 *         - CPOST_OK: 添加成功
 *         - CPOST_ERROR_PARAM: 参数错误
 *         - CPOST_ERROR_FULL: 任务列表已满
 *         - CPOST_ERROR_EXISTS: 任务已存在
 */
int cpost_add_handler(cpost_param_t *param);

/**
 * @brief 处理任务列表
 */
void cpost_process(void);

/**
 * @brief 移除指定的handler
 *
 * @param handler handler函数指针
 * @param param 参数，传 NULL 表示不比较参数
 */
void cpost_remove(void *handler, void *param);

/**
 * @brief 移除所有handler
 */
void cpost_remove_all(void);

/**
 * @brief 判断handler是否已经在列表中
 *
 * @param handler handler函数指针
 * @param param 参数，传 NULL 表示不比较参数
 *
 * @return int 1存在 0不存在
 */
int cpost_is_in_list(void *handler, void *param);

/**
 * @brief 获取统计信息
 *
 * @param stats 统计信息结构体指针
 *
 * @return int CPOST_OK成功 CPOST_ERROR_PARAM参数错误
 */
int cpost_get_stats(cpost_stats_t *stats);

/**
 * @brief 获取当前活动任务数量
 *
 * @return size_t 活动任务数量
 */
size_t cpost_get_active_count(void);

#endif /* CPOST_H */
