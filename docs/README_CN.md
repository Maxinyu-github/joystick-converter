# 手柄转换器 (Joystick Converter)

基于RP2350-PiZero开发板的手柄转换器，支持将手柄输入转换为游戏主机或PC兼容的输入，并支持按键映射和复杂鼠标宏功能。

## 功能特性

- **USB Host功能**：通过Type-C接口连接各种USB手柄
- **USB Device功能**：作为USB设备输出转换后的输入
- **灵活的按键映射**：
  - 手柄按键映射到其他手柄按键
  - 手柄按键映射到键盘按键
  - 手柄按键映射到鼠标按键
  - 手柄按键触发复杂宏
- **鼠标宏系统**：支持录制和执行复杂的鼠标和键盘操作序列
- **多种输出模式**：
  - 游戏手柄模式
  - 键盘模式
  - 鼠标模式
  - 键盘+鼠标组合模式
- **配置软件**：通过PC端图形界面软件进行配置
- **配置持久化**：配置保存在RP2350的Flash存储器中

## 硬件要求

- **开发板**：RP2350-PiZero（或兼容的RP2350开发板）
- **接口**：Type-C USB接口（用于连接手柄和PC）
- **可选**：状态指示LED（GPIO 25）

## 目录结构

```
joystick-converter/
├── firmware/               # RP2350固件代码
│   ├── main.c             # 主程序
│   ├── usb_host.c/h       # USB Host功能（手柄输入）
│   ├── usb_device.c/h     # USB Device功能（输出）
│   ├── config.c/h         # 配置管理
│   ├── remapping.c/h      # 按键重映射引擎
│   └── macro.c/h          # 宏系统
├── config_software/       # PC配置软件
│   ├── config_tool.py     # PyQt6 GUI应用
│   └── requirements.txt   # Python依赖
├── docs/                  # 文档
└── CMakeLists.txt        # CMake构建配置
```

## 快速开始

### 1. 构建固件

#### 前置要求

- CMake 3.13+
- ARM GCC工具链
- Pico SDK

#### 构建步骤

```bash
# 设置Pico SDK路径
export PICO_SDK_PATH=/path/to/pico-sdk

# 创建构建目录
mkdir build
cd build

# 配置CMake
cmake ..

# 编译
make
```

编译完成后，将在 `build/firmware/` 目录下生成 `joystick_converter.uf2` 文件。

### 2. 烧录固件

1. 按住RP2350开发板上的BOOTSEL按钮
2. 将开发板通过USB连接到PC
3. 开发板将显示为USB存储设备
4. 将 `joystick_converter.uf2` 复制到该存储设备
5. 设备将自动重启并运行固件
6. 固件启动后，电脑会自动识别到一个新的USB串口设备（Windows显示为COM端口，Linux显示为/dev/ttyACM0）

### 3. 使用配置软件

#### 安装依赖

```bash
cd config_software
pip install -r requirements.txt
```

#### 运行配置工具

```bash
python config_tool.py
```

#### 配置步骤

1. **连接设备**：
   - 在"Connection"区域选择串口
   - 点击"Connect"连接到设备

2. **设置输出类型**：
   - 在"Output Type"区域选择输出设备类型（手柄/键盘/鼠标/组合）

3. **配置按键映射**：
   - 点击"Add Mapping"添加按键映射
   - 选择源按键和目标动作
   - 可以将手柄按键映射为键盘按键、鼠标按键或宏

4. **创建宏**：
   - 点击"Add Macro"创建新宏
   - 输入宏ID和操作序列
   - 宏操作支持：按键按下/释放、鼠标移动、鼠标按键、延时等

5. **保存配置**：
   - 点击"Save to Device"将配置保存到设备
   - 配置将存储在Flash中，断电后不会丢失

## 使用示例

### 示例1：基本按键重映射

将手柄A键映射为键盘空格键：

1. 在配置软件中点击"Add Mapping"
2. 源按键：选择"Button A"
3. 映射类型：选择"Keyboard Key"
4. 目标按键：输入空格键代码（0x2C）
5. 保存配置

### 示例2：创建鼠标宏

创建一个点击宏（移动鼠标然后点击）：

```
宏ID: 1
操作序列:
mouse_move 100 50
delay 100
mouse_button_press 1
delay 50
mouse_button_release
```

然后将手柄按键映射到这个宏。

### 示例3：摇杆控制鼠标

在"Output Type"中选择"Mouse"或"Keyboard+Mouse"模式，右摇杆将自动映射为鼠标移动。

## LED状态指示

设备的LED（GPIO 25）会根据状态闪烁：

- **快速闪烁（100ms）**：初始化中或错误状态
- **中速闪烁（500ms）**：等待手柄连接
- **慢速闪烁（2000ms）**：正常工作中
- **快速闪烁（200ms）**：配置模式

## 技术细节

### USB双角色

本项目使用RP2350的USB功能实现双角色：
- **USB Host**：连接并读取手柄输入
- **USB Device**：作为HID设备输出到PC/游戏机

### 配置存储

配置数据存储在Flash的最后一个扇区中，包括：
- 输出设备类型
- 按键映射表（最多32个映射）
- 宏定义（最多16个宏，每个最多128步）

### 按键映射引擎

支持以下映射类型：
1. **直通模式**：无映射时，直接传递手柄输入
2. **按键到按键**：手柄按键映射到其他手柄按键
3. **按键到键盘**：手柄按键映射到键盘按键
4. **按键到鼠标**：手柄按键映射到鼠标按键
5. **按键到宏**：手柄按键触发宏执行

### 宏系统

宏系统支持以下操作：
- `key_press <keycode>` - 按下键盘按键
- `key_release` - 释放键盘按键
- `mouse_move <x> <y>` - 移动鼠标
- `mouse_button_press <button>` - 按下鼠标按键
- `mouse_button_release` - 释放鼠标按键
- `delay <ms>` - 延时（毫秒）

## Debug模式 - 调试手柄输入

配置软件提供了Debug模式来实时显示手柄输入，帮助识别特殊按键和排查问题。

### 使用Debug模式

1. 在配置软件中连接设备
2. 切换到"Debug Mode"标签页
3. 连接手柄到转换器设备
4. 点击"Start Debug Mode"开始调试
5. 测试手柄各个按键和摇杆
6. 观察显示的实时数据

### Debug模式显示内容

- **按键状态**：所有标准按键的实时状态（A、B、X、Y、LB、RB、Back、Start、LS、RS）
- **摇杆位置**：左右摇杆的X/Y坐标值（-32768到32767）
- **扳机值**：左右扳机的压力值（0-255）
- **方向键**：D-Pad的方向指示
- **原始数据**：十六进制按键位图，用于识别未映射的特殊按键

### 识别特殊按键

如果你的手柄有特殊按键（如背键、摇杆按键等），在Debug模式的"Raw Button Data"区域：
- 按下特殊按键时，十六进制值会发生变化
- 记录该值来确定按键对应的位
- 可以据此在固件中添加对该按键的支持

详细说明请参考 [Debug模式文档](DEBUG_MODE.md)（英文）。

## 故障排除

### 问题：设备无法识别（Windows没有检测到串口）
- 检查USB连接线是否为数据线（不是纯充电线）
- 确认固件已正确烧录（LED应该闪烁）
- 打开Windows设备管理器，查看"端口(COM和LPT)"下是否有新设备
- 固件启动后应该自动创建USB CDC串口设备
- 尝试不同的USB端口

### 问题：设备无法识别
- 检查USB连接
- 确认固件已正确烧录
- 查看串口输出（波特率115200）

### 问题：手柄无法连接
- 确认手柄支持USB HID协议
- 检查USB Host配置
- 使用Debug模式查看手柄是否有数据输出

### 问题：配置无法保存
- 确认Flash写入权限
- 检查配置数据大小是否超限

### 问题：特殊按键不工作
- 使用Debug模式查看按键是否被识别
- 检查原始按键数据中是否有对应的位变化
- 可能需要更新固件以支持该按键

## 开发计划

- [x] TinyUSB Host集成
- [ ] PIO-USB设备输出集成
- [ ] 支持更多手柄类型
- [ ] 配置文件导入/导出
- [ ] 宏录制功能
- [ ] 蓝牙支持
- [ ] Web配置界面

## 许可证

本项目采用MIT许可证。

## 贡献

欢迎提交Issue和Pull Request！

## 联系方式

如有问题或建议，请在GitHub上创建Issue。
