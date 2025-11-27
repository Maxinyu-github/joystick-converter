# 日志上传功能使用指南

## 概述

日志上传功能允许您从配置软件中获取设备运行日志，便于调试和问题排查。设备运行过程中的关键事件和错误信息都会被记录到日志缓冲区，可以通过配置软件实时获取和分析。

## 功能特性

日志系统提供：
- **日志级别**：支持4个级别的日志（DEBUG、INFO、WARN、ERROR）
- **环形缓冲区**：使用4KB环形缓冲区存储日志，自动覆盖旧日志
- **时间戳**：每条日志都带有毫秒级时间戳
- **日志导出**：支持将日志导出到本地文件
- **溢出检测**：检测日志缓冲区是否发生溢出

## 如何使用日志功能

### 1. 连接设备

1. 打开配置工具：
   ```bash
   cd config_software
   python config_tool.py
   ```

2. 从下拉列表中选择设备的串口
3. 点击"Connect"（连接）

### 2. 访问日志功能

1. 点击"Device Logs"（设备日志）标签页
2. 可以看到以下控制按钮：
   - **Refresh Logs**：刷新/获取日志
   - **Clear Logs**：清空设备日志
   - **Export Logs**：导出日志到文件
   - **Get Status**：获取日志状态

### 3. 获取日志

1. 点击"Refresh Logs"按钮
2. 日志将显示在下方的文本区域
3. 日志格式：`[时间戳][级别] 消息内容`

### 4. 设置日志级别

使用"Log Level"下拉框选择要捕获的最低日志级别：
- **DEBUG**（0）：显示所有日志，包括调试信息
- **INFO**（1）：显示信息、警告和错误（默认）
- **WARN**（2）：只显示警告和错误
- **ERROR**（3）：只显示错误

### 5. 导出日志

1. 点击"Export Logs"按钮
2. 选择保存位置和文件名
3. 日志将保存为文本文件，包含导出时间戳

### 6. 查看日志状态

点击"Get Status"按钮可以查看：
- **Entries**：当前日志条目数
- **Overflow**：是否发生缓冲区溢出

## 日志级别说明

| 级别 | 值 | 说明 |
|------|-----|------|
| DEBUG | 0 | 详细调试信息，用于开发调试 |
| INFO | 1 | 一般信息，如初始化成功、设备连接等 |
| WARN | 2 | 警告信息，如配置加载失败使用默认值 |
| ERROR | 3 | 错误信息，如初始化失败 |

## 日志内容示例

```
[1234][INFO] Joystick Converter starting...
[1235][INFO] Configuration loaded successfully
[1240][INFO] USB host initialized
[1245][INFO] USB device initialized
[1250][INFO] Remapping engine initialized
[1255][INFO] Macro system initialized
[1260][INFO] Initialization complete. Waiting for gamepad...
[5000][INFO] Gamepad connected
[10000][INFO] Gamepad disconnected
```

## 通信协议

### 串口命令

日志功能使用简单的文本协议（波特率115200）：

**命令（PC → 设备）：**
| 命令 | 说明 |
|------|------|
| `LOG_GET\n` | 获取所有日志 |
| `LOG_CLEAR\n` | 清空日志缓冲区 |
| `LOG_COUNT\n` | 获取日志条目数 |
| `LOG_LEVEL X\n` | 设置日志级别（X为0-3） |
| `LOG_STATUS\n` | 获取日志状态 |

**响应（设备 → PC）：**
| 响应 | 说明 |
|------|------|
| `LOG_START\n...\nLOG_END\n` | 日志内容包裹在标记中 |
| `LOG_EMPTY\n` | 无日志 |
| `LOG_CLEARED\n` | 日志已清空 |
| `LOG_COUNT:X\n` | X为日志条目数 |
| `LOG_LEVEL_SET:X\n` | X为设置的级别 |
| `LOG_STATUS:level=X,count=Y,overflow=Z\n` | 日志状态 |

## 固件开发指南

### 记录日志

在固件代码中使用以下宏记录日志：

```c
#include "logging.h"

// 记录不同级别的日志
LOG_DEBUG("详细调试信息: value=%d", value);
LOG_INFO("系统初始化完成");
LOG_WARN("配置加载失败，使用默认值");
LOG_ERROR("USB初始化失败");
```

### 日志系统初始化

在`main()`函数中初始化日志系统：

```c
// 初始化日志系统
logging_init();
LOG_INFO("Joystick Converter starting...");
```

## 故障排除

### 无法获取日志

- 确保设备已连接（状态栏显示"Connected"）
- 检查串口是否正确选择
- 尝试点击"Get Status"确认通信正常

### 日志显示不完整

- 日志缓冲区大小有限（4KB），旧日志可能被覆盖
- 检查"Overflow"状态，如果为"Yes"表示有日志丢失
- 考虑降低日志级别减少日志量

### 日志时间戳异常

- 时间戳是从设备启动开始计算的毫秒数
- 设备重启后时间戳会重置

## 技术规格

| 参数 | 值 |
|------|-----|
| 缓冲区大小 | 4096 字节 |
| 单条日志最大长度 | 128 字节 |
| 支持的日志级别 | 4 个 |
| 串口波特率 | 115200 |

## 相关资料

- [API文档](API.md) - 编程接口详情（英文）
- [Debug模式指南](DEBUG_MODE_CN.md) - 手柄调试功能
- [故障排除](TROUBLESHOOTING.md) - 常见问题（英文）
