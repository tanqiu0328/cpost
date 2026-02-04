/**
 * @file cpost.c
 * @author Aki
 * @brief c post
 * @version 1.1.0
 * @date 2026-02-04
 */
#include "cpost.h"

cpost_handler_t g_cpost_handlers[CPOST_MAX_HANDLER_SIZE] = {0};

/**
 * @brief cpost 添加handler
 *
 * @param param 参数
 *
 * @return signed char 0 添加成功 -1 添加失败
 */
signed char cpost_add_handler(cpost_param_t *param)
{
    if (cpost_is_in_list(param->handler, param->attrs.param_diff ? param->param : NULL) == 0)
    {
        switch (param->attrs.flag)
        {
        case CPOST_FLAG_CLEAR_FRONT:
            cpost_remove(param->handler, param->attrs.param_diff ? param->param : NULL);
            break;

        case CPOST_FLAG_CANCEL_CURRENT:
            return 0;

        case CPOST_FLAG_ADD_NEW:
            /* do nothing */
            break;

        default:
            break;
        }
    }

    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == NULL)
        {
            g_cpost_handlers[i].start_time = CPOST_GET_TICK();
            g_cpost_handlers[i].delay = param->delay;
            g_cpost_handlers[i].handler = (void (*)(void *))(param->handler);
            g_cpost_handlers[i].param = param->param;
            CPOST_EXIT_CRITICAL();
            return 0;
        }
    }
    CPOST_EXIT_CRITICAL();
    return -1;
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
    CPOST_ENTER_CRITICAL();
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == handler && (param == NULL || param == g_cpost_handlers[i].param))
        {
            g_cpost_handlers[i].handler = NULL;
        }
    }
    CPOST_EXIT_CRITICAL();
}

/**
 * @brief 移除所有handler
 *
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
 * @return signed char 0 存在 -1 不存在
 */
signed char cpost_is_in_list(void *handler, void *param)
{
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler == handler && (param == NULL || param == g_cpost_handlers[i].param))
        {
            return 0;
        }
    }
    return -1;
}

/**
 * @brief cpost 处理
 *
 */
void cpost_process(void)
{
    size_t tick;
    for (size_t i = 0; i < CPOST_MAX_HANDLER_SIZE; i++)
    {
        if (g_cpost_handlers[i].handler)
        {
            tick = CPOST_GET_TICK();
            if (g_cpost_handlers[i].delay == 0 ||
                (CPOST_MAX_TICK - g_cpost_handlers[i].start_time > g_cpost_handlers[i].delay
                     ? tick - g_cpost_handlers[i].start_time >= g_cpost_handlers[i].delay
                     : CPOST_MAX_TICK - g_cpost_handlers[i].start_time + tick >= g_cpost_handlers[i].delay))
            {
                void (*handler)(void *) = g_cpost_handlers[i].handler;
                void *param = g_cpost_handlers[i].param;

                g_cpost_handlers[i].handler = NULL;
                if (handler)
                {
                    handler(param);
                }
            }
        }
    }
}
