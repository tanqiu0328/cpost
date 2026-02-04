/**
 * @file cpost.h
 * @author Aki
 * @brief c post
 * @version 1.1.0
 * @date 2026-02-04
 */
#ifndef CPOST_H
#define CPOST_H

#include "cpost_cfg.h"

#define CPOST_FLAG_CLEAR_FRONT 0    // post列表中，存在相同handler, 清除之前的post
#define CPOST_FLAG_CANCEL_CURRENT 1 // post列表中，存在相同handler, 取消当前post
#define CPOST_FLAG_ADD_NEW 2        // 添加新post，不论post列表中是否存在相同handler

#define CPOST_PARAM_DIFF_DISABLE 0 // 判断是否相同的handler时，忽略参数
#define CPOST_PARAM_DIFF_ENABLE 1  // 判断是否相同的handler时，包括参数的比较

#ifndef CPOST_MAX_HANDLER_SIZE
/**
 * @brief 最大handler数量
 */
#define CPOST_MAX_HANDLER_SIZE 16
#endif

#ifndef CPOST_GET_TICK
/**
 * @brief 获取系统tick函数
 */
#define CPOST_GET_TICK() 0
#endif

#ifndef CPOST_MAX_TICK
/**
 * @brief tick最大值
 */
#define CPOST_MAX_TICK 0xFFFFFFFF
#endif

typedef struct
{
    size_t start_time;
    size_t delay;
    void (*handler)(void *);
    void *param;
} cpost_handler_t;

typedef struct
{
    void *handler;
    void *param;
    size_t delay;
    struct
    {
        unsigned char flag : 2;
        unsigned char param_diff : 1;
    } attrs;
} cpost_param_t;

/**
 * @brief cpost
 *
 * @param ... 参数列表
 * @note 参数按照`cpost_param_t`结构体进行传递
 *       按顺序传递时，直接按照`cpost(handler, param，delay)`传参
 *       不按顺序时，需要指定成员名，比如`cpost(handler, .delay=5000)`
 *       属性参数必须指定成员名，比如`cpost(handler, .attrs.flag=CPOST_FLAG_CANCEL_CURRENT)`
 *
 * @return signed char 0 成功 -1 失败
 */
#define cpost(...) \
    cpost_add_handler(&((cpost_param_t){__VA_ARGS__}))

#define cpost_delay(_handler, _delay) \
    cpost(_handler, .delay = _delay)

#define cpost_ex(_handler, _param) \
    cpost(_handler, .param = _param)

#define cpost_delay_ex(_handler, _param, _delay) \
    cpost(_handler, .param = _param, .delay = _delay)

signed char cpost_add_handler(cpost_param_t *param);

void cpost_process(void);

void cpost_remove(void *handler, void *param);

void cpost_remove_all(void);

signed char cpost_is_in_list(void *handler, void *param);

#endif /* CPOST_H */
