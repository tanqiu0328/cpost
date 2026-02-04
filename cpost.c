/**
 * @file cpost.c
 * @author Aki
 * @brief c post - 延迟任务调度器实现
 * @version 1.3.0
 * @date 2026-02-04
 */
#include "cpost.h"

cpost_handler_t g_cpost_handlers[CPOST_MAX_HANDLER_SIZE] = {0};

/**
 * @brief 计算时间差值
 *
 * @param current 当前时间
 * @param start 开始时间
 *
 * @return CPOST_TICK_TYPE 时间差值
 */
static inline CPOST_TICK_TYPE cpost_tick_diff(CPOST_TICK_TYPE current, CPOST_TICK_TYPE start)
{
    return current - start;
}

/**
 * @brief cpost 添加handler
 *
 * @param param 参数
 *
 * @return int 错误码
 */
int cpost_add_handler(cpost_param_t *param)
{
    if (param == NULL || param->handler == NULL)
    {
        return CPOST_ERROR_PARAM;
    }

    int first_free_idx = -1;
    int result = CPOST_OK;

    CPOST_ENTER_CRITICAL();

    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == param->handler &&
            (!param->attrs.param_diff || g_cpost_handlers[i].param == param->param))
        {
            if (param->attrs.flag == CPOST_FLAG_CANCEL_CURRENT)
            {
                result = CPOST_ERROR_EXISTS;
                CPOST_EXIT_CRITICAL();
                return result;
            }
            else if (param->attrs.flag == CPOST_FLAG_CLEAR_FRONT)
            {
                g_cpost_handlers[i].handler = NULL;
            }
        }

        if (first_free_idx == -1 && g_cpost_handlers[i].handler == NULL)
        {
            first_free_idx = (int)i;
        }
    }

    if (first_free_idx != -1)
    {
        g_cpost_handlers[first_free_idx].start_time = CPOST_GET_TICK();
        g_cpost_handlers[first_free_idx].delay = param->delay;
        g_cpost_handlers[first_free_idx].handler = (void (*)(void *))(param->handler);
        g_cpost_handlers[first_free_idx].param = param->param;
        result = CPOST_OK;
    }
    else
    {
        result = CPOST_ERROR_FULL;
    }

    CPOST_EXIT_CRITICAL();
    return result;
}

/**
 * @brief 移除handler
 *
 * @param handler handler
 * @param param 参数，传 `NULL` 表示不比较参数
 *
 */
void cpost_remove(void *handler, void *param)
{
    if (handler == NULL)
    {
        return;
    }

    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == handler &&
            (param == NULL || param == g_cpost_handlers[i].param))
        {
            g_cpost_handlers[i].handler = NULL;
        }
    }
    CPOST_EXIT_CRITICAL();
}

/**
 * @brief 移除所有handler
 */
void cpost_remove_all(void)
{
    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        g_cpost_handlers[i].handler = NULL;
    }
    CPOST_EXIT_CRITICAL();
}

/**
 * @brief 判断handler是否已经在列表中
 *
 * @param handler handler
 * @param param 参数，传 `NULL` 表示不比较参数
 *
 * @return int 1存在 0不存在
 */
int cpost_is_in_list(void *handler, void *param)
{
    int found = 0;

    if (handler == NULL)
    {
        return 0;
    }

    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == handler &&
            (param == NULL || param == g_cpost_handlers[i].param))
        {
            found = 1;
            break;
        }
    }
    CPOST_EXIT_CRITICAL();

    return found;
}

/**
 * @brief cpost 处理
 */
void cpost_process(void)
{
    CPOST_TICK_TYPE current_tick = CPOST_GET_TICK();

    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        void (*handler)(void *) = NULL;
        void *param = NULL;

        CPOST_ENTER_CRITICAL();

        if (g_cpost_handlers[i].handler != NULL)
        {
            if (g_cpost_handlers[i].delay == 0 ||
                cpost_tick_diff(current_tick, g_cpost_handlers[i].start_time) >= g_cpost_handlers[i].delay)
            {
                handler = g_cpost_handlers[i].handler;
                param = g_cpost_handlers[i].param;

                g_cpost_handlers[i].handler = NULL;
            }
        }

        CPOST_EXIT_CRITICAL();

        if (handler != NULL)
        {
            handler(param);
        }
    }
}

/**
 * @brief 获取统计信息
 *
 * @param stats 统计信息结构体指针
 *
 * @return int CPOST_OK成功 CPOST_ERROR_PARAM参数错误
 */
int cpost_get_stats(cpost_stats_t *stats)
{
    if (stats == NULL)
    {
        return CPOST_ERROR_PARAM;
    }

    size_t active = 0;

    CPOST_ENTER_CRITICAL();

    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler != NULL)
        {
            active++;
        }
    }

    stats->active_count = active;
    stats->total_slots = CPOST_MAX_HANDLER_SIZE;
    stats->free_slots = CPOST_MAX_HANDLER_SIZE - active;

    CPOST_EXIT_CRITICAL();

    return CPOST_OK;
}

/**
 * @brief 获取当前活动任务数量
 *
 * @return size_t 活动任务数量
 */
size_t cpost_get_active_count(void)
{
    size_t active = 0;

    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler != NULL)
        {
            active++;
        }
    }
    CPOST_EXIT_CRITICAL();

    return active;
}
