# cpost

![version](https://img.shields.io/badge/version-1.3.1-brightgreen.svg)
![build](https://img.shields.io/badge/build-2026.02.05-brightgreen.svg)
![license](https://img.shields.io/badge/license-MIT-brightgreen.svg)

一个为嵌入式系统设计的非阻塞延迟任务调度器

## 移植说明

1. 添加文件到工程

    将cpost.c、cpost.h和cpost_cfg.h添加到你的工程中

2. 配置cpost_cfg.h

    根据你的硬件平台修改配置文件，默认已适配 STM32 HAL 库：
    ```c
    // 设置获取系统Tick的函数
    #define CPOST_GET_TICK() HAL_GetTick()
    
    // 设置临界区保护
    #define CPOST_ENTER_CRITICAL() __disable_irq()
    #define CPOST_EXIT_CRITICAL()  __enable_irq()
    ```

3. 代码调用

    ```c
    #include "cpost.h"
    
    void my_task(void *param)
    {
        printf("Task executed\n");
    }
    
    int main(void)
    {
        // 立即执行
        cpost(my_task);
    
        // 延迟执行
        cpost_delay(my_task, 1000);
        
        // 主循环
        while (1)
        {
            cpost_process();
            HAL_Delay(10);
        }
    }
    ```
    
    基本使用
    
    ```c
    // 延迟 1000ms 执行某个函数
    cpost_delay(led_off_handler, 1000);
    
    // 立即执行带参数的任务
    cpost_ex(log_handler, "Hello World");
    
    // 延迟 500ms，如果列表中已有该任务则清除之前的，只保留本次
    cpost(task_func, .delay = 500, .attrs.flag = CPOST_FLAG_CLEAR_FRONT);
    ```

## 常用接口宏

| 宏名称                                | 说明                         |
| :------------------------------------ | :--------------------------- |
| cpost_delay(handler, delay)           | 延迟指定时间后执行（无参数） |
| cpost_ex(handler, param)              | 立即加入队列执行（带参数）   |
| cpost_delay_ex(handler, param, delay) | 延迟指定时间后执行（带参数） |
| cpost(...)                            | 全参数配置方案               |
