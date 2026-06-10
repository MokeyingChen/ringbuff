# RingBuff — 嵌入式环形缓冲区库

一个轻量级、可移植的嵌入式环形缓冲区（Ring Buffer）C 语言库。**仅依赖 `stdint.h` 和 `string.h`**，适用于任何 MCU 平台（STM32、GD32、ESP32、AVR 等）。

## 特性

- **零依赖** — 只需标准 C 库的 `stdint.h` 和 `string.h`
- **可配置槽数量** — 每个缓冲区实例可独立设定槽（slot）数量，RAM 用量精准可控
- **DMA 友好** — 槽化设计天然适配 UART DMA 空闲中断场景
- **无溢出的写入** — 空间不足时返回 0，不会覆盖旧数据
- **高效读取** — 两段 `memcpy` 替代逐字节拷贝
- **事件回调** — 自动提取并分派数据，无需手动轮询

## 文件

```
Middle/
├── middle_ringbuff.h    ← 库头文件（API 接口）
└── middle_ringbuff.c    ← 库实现
```

> 将这两个文件复制到你的工程即可使用。

## 快速开始

### 1. 定义缓冲区和槽数组

```c
#include "middle_ringbuff.h"

#define RX_BUF_SIZE  1024
#define RX_SLOT_NUM  10

uint8_t          rx_buffer[RX_BUF_SIZE];      // 环形数据缓冲
RingBuff_Slot    rx_slots[RX_SLOT_NUM];       // 槽数组
RingBuff_CB      rx_rb;                        // 环形缓冲控制块
```

### 2. 初始化

```c
RingBuff_Init(&rx_rb, rx_buffer, RX_BUF_SIZE, rx_slots, RX_SLOT_NUM);
```

### 3. 写入数据（通常在 ISR 中）

```c
// UART DMA 空闲中断回调
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    RingBuff_Write(&rx_rb, dma_buf, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_buf, DMA_BUF_SIZE);
}
```

### 4. 读取 + 处理数据（主循环或任务中）

```c
// 方式一：事件回调（推荐）
void OnDataReceived(uint8_t *data, uint16_t length)
{
    // 处理收到的数据...
    printf("收到 %d 字节\n", length);
}

void main_loop(void)
{
    uint8_t temp_buf[256];
    RingBuff_Process(&rx_rb, temp_buf, sizeof(temp_buf), OnDataReceived);
    // 每隔一定周期调用一次
}
```

```c
// 方式二：直接读取
void main_loop(void)
{
    uint8_t temp_buf[256];
    uint16_t len = RingBuff_Read(&rx_rb, temp_buf, sizeof(temp_buf));
    if (len > 0) {
        // 手动处理 temp_buf 中的 len 字节数据
    }
}
```

## API 参考

### 数据结构

| 类型 | 说明 |
|---|---|
| `RingBuff_CB` | 环形缓冲区控制块。包含缓冲区指针、读写指针、槽信息 |
| `RingBuff_Slot` | 槽结构。记录每段数据的起始、结束和长度 |
| `RingBuff_Event_t` | 回调函数类型：`void (*)(uint8_t *data, uint16_t length)` |

### 函数

| 函数 | 说明 |
|---|---|
| `RingBuff_Init(rb, buffer, bufSize, slotArray, slotNum)` | 初始化环形缓冲区 |
| `RingBuff_Write(rb, data, length)` | 写入数据。返回实际写入字节数，空间不足返回 0 |
| `RingBuff_Read(rb, dst, maxLen)` | 读取数据到目标缓冲。返回实际读取字节数 |
| `RingBuff_Process(rb, buf, size, callback)` | 自动读取并调用回调处理。有数据时触发 |
| `RingBuff_GetLength(rb)` | 获取缓冲内待读取数据长度 |
| `RingBuff_GetRemain(rb)` | 获取剩余可写空间 |

### `RingBuff_Init` 参数

```c
void RingBuff_Init(
    RingBuff_CB  *rb,         // 控制块指针
    uint8_t      *buffer,     // 环形数据缓冲区
    uint16_t      bufSize,    // 数据缓冲区大小（字节）
    RingBuff_Slot *slotArray, // 槽数组指针
    uint16_t      slotNum     // 槽数量
);
```

### 返回值

| 函数 | 返回值 |
|---|---|
| `RingBuff_Write` | 实际写入的字节数；空间不足返回 **0** |
| `RingBuff_Read` | 实际读取的字节数；无数据返回 **0** |

---

## 示例工程

本仓库同时包含一个完整的 STM32F103ZET6 示例工程，演示如何使用本库配合 **A7680C 4G 模块**通过 MQTT 协议连接**华为云 IoTDA** 平台。

### 硬件

| 组件 | 型号 |
|---|---|
| MCU | STM32F103ZET6 |
| 4G 模块 | A7680C（Ai-Thinker） |
| 云端 | 华为云 IoTDA |

### 软件结构

```
Core/           STM32 HAL 初始化、main.c
Hardware/       硬件驱动（A7680C 4G 模块）
Middle/         中间件
  ├── middle_ringbuff.c/h   环形缓冲区库
  └── middle_mqtt.c/h       MQTT 协议封装
Drivers/        STM32 HAL 库
MDK-ARM/        Keil MDK 工程文件
```

### 构建

1. 安装 **Keil MDK-ARM 5** 及 STM32F1xx 设备包
2. 打开 `MDK-ARM/stm32f103zet_example.uvprojx`
3. 编译并下载

### 接线（4G 模块）

| STM32 | A7680C |
|---|---|
| USART2 TX (PA2) | RXD |
| USART2 RX (PA3) | TXD |
| PD7 | PWRKEY |

---

## 移植到其他平台

只需修改 `middle_ringbuff.h` 的包含：

```c
#include <stdint.h>   // 提供 uint8_t, uint16_t
#include <string.h>   // 提供 memset, memcpy
```

如果你的平台没有这两个标准头文件，替换为对应类型定义和内存操作函数即可。

---

## License

MIT License. 详见 [LICENSE](LICENSE) 文件。
