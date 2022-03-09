#ifndef PELCODPROTOCOL_HPP
#define PELCODPROTOCOL_HPP

#include <algorithm>
#include <any>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

/*!
* c++17
PELCO_D 协议
https://www.pelco.com/
https://www.commfront.com/pages/pelco-d-protocol-tutorial

PELCO_P 协议
https://www.commfront.com/pages/pelco-p-protocol-tutorial

VISCA 协议
https://www.epiphan.com/userguides/LUMiO12x/Content/UserGuides/PTZ/3-operation/rs232.htm

消息格式：
+-----------+---------+-----------+-----------+--------+--------+----------+
|  Byte 1   | Byte 2  |  Byte 3   |  Byte 4   | Byte 5 | Byte 6 |  Byte 7  |
+-----------+---------+-----------+-----------+--------+--------+----------+
| Sync Byte | Address | Command 1 | Command 2 | Data 1 | Data 2 | Checksum |
+-----------+---------+-----------+-----------+--------+--------+----------+

标准命令集：
.-----------.-----------.-----------.-----------.------------------.---------------.------------.-----------.------------.
|           |   Bit 7   |   Bit 6   |   Bit 5   |      Bit 4       |     Bit 3     |   Bit 2    |   Bit 1   |   Bit 0    |
:-----------+-----------+-----------+-----------+------------------+---------------+------------+-----------+------------:
| Command 1 | Sense     | Reserved  | Reserved  | Auto/Manual Scan | Camera On/Off | Iris Close | Iris Open | Focus Near |
:-----------+-----------+-----------+-----------+------------------+---------------+------------+-----------+------------:
| Command 2 | Focus Far | Zoom Wide | Zoom Tele | Down             | Up            | Left       | Right     | Always 0   |
'-----------'-----------'-----------'-----------'------------------'---------------'------------'-----------'------------'
*/

struct _StandardCMD {
    // Command 1
    uint32_t focus_near : 1;
    uint32_t iris_open  : 1;
    uint32_t iris_close : 1;
    uint32_t camera     : 1;
    uint32_t scan       : 1;
    uint32_t reserve_1  : 1;// reserve
    uint32_t reserve_2  : 1;// reserve
    uint32_t sense      : 1;

    // Command 2
    uint32_t always     : 1;
    uint32_t right      : 1;
    uint32_t left       : 1;
    uint32_t up         : 1;
    uint32_t down       : 1;
    uint32_t zoom_tele  : 1;
    uint32_t zoom_wide  : 1;
    uint32_t focus_far  : 1;
};

struct StandardCMD_ {
    uint8_t cmd_1;
    uint8_t cmd_2;
};

union StandardCMD {
    StandardCMD_ cmd;
    _StandardCMD _cmd = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

class PelcoDProtocolConfig {
public:
    virtual std::string ip() const = 0;
    virtual uint32_t port() const = 0;
    virtual std::string rtspMainUrl() const = 0;
    virtual std::string rtspSubUrl() const = 0;
    virtual std::string username() const = 0;
    virtual std::string password() const = 0;
};

class SimplePelcoDProtocol {
public:
    virtual ~SimplePelcoDProtocol() {
    }
    // 向左平移 ←
    virtual void panLeft(std::uint8_t speed) = 0;
    // 向右平移 →
    virtual void panRight(std::uint8_t speed) = 0;
    // 向上倾斜 ↑
    virtual void tiltUp(std::uint8_t speed) = 0;
    // 向下倾斜 ↓
    virtual void tiltDown(std::uint8_t speed) = 0;
    // 左上移动 ←↑
    virtual void moveLeftUp(std::uint8_t speed) = 0;
    // 右上移动 →↑
    virtual void moveRightUp(std::uint8_t speed) = 0;
    // 左下移动 ←↓
    virtual void moveLeftDown(std::uint8_t speed) = 0;
    // 右下移动 →↓
    virtual void moveRightDown(std::uint8_t speed) = 0;
    // 停止移动
    virtual void stopMotion(void) = 0;

    // 焦点前调/调近焦点/聚焦近
    virtual void focusNear(void) = 0;
    // 焦点后调/调远焦点/聚焦远
    virtual void focusFar(void) = 0;

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void zoomIn(void) = 0;
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void zoomOut(void) = 0;

    // 光圈扩大
    virtual void irisOpen(void) = 0;
    // 光圈缩小
    virtual void irisClose(void) = 0;

    // 扩展指令
    // 设置预置点
    virtual void setPreset(std::uint8_t presetID) = 0;
    // 清除预置点
    virtual void clearPreset(std::uint8_t presetID) = 0;
    // 调用预置点
    virtual void callPreset(std::uint8_t presetID) = 0;

    // 高级扩展指令
    // 设置云台绝对值
    virtual void setPanPosition(std::uint8_t msb, std::uint8_t lsb) = 0;
    virtual void setTiltPosition(std::uint8_t msb, std::uint8_t lsb) = 0;
    virtual void setZoomPosition(std::uint8_t msb, std::uint8_t lsb) = 0;
    virtual void setMagnification(std::uint8_t msb, std::uint8_t lsb) = 0;

    virtual void setPanPosition(std::uint16_t pos) = 0;
    virtual void setTiltPosition(std::uint16_t pos) = 0;
    virtual void setZoomPosition(std::uint16_t pos) = 0;
    virtual void setMagnification(std::uint16_t pos) = 0;

    // 便捷的浮点操作函数
    // 0.0 ~ 360.0
    virtual void setPanPosition(std::float_t pos) = 0;
    // -90.0 ~ 90.0
    virtual void setTiltPosition(std::float_t pos) = 0;
    // 0.0 ~ 1.0
    virtual void setZoomPosition(std::float_t pos) = 0;
    virtual void setMagnification(std::float_t pos) = 0;

    // 查询指令
    virtual void queryPanPosition(void) = 0;
    virtual void queryTiltPosition(void) = 0;
    virtual void queryZoomPosition(void) = 0;
    virtual void queryMagnification(void) = 0;

    // 响应查询指令
    virtual void queryPanPositionResponse(std::uint8_t pos) = 0;
    virtual void queryTiltPositionResponse(std::uint8_t pos) = 0;
    virtual void queryZoomPositionResponse(std::uint8_t pos) = 0;
    virtual void queryMagnificationResponse(std::uint8_t pos) = 0;

    // 用于各种奇葩的厂家扩展
    virtual void setAnyValue(std::uint8_t type, const std::any& value) = 0;
    virtual std::any getAnyValue(std::uint8_t type) = 0;

    // 发送原始字节
    virtual void sendRawCmd(const std::vector<std::uint8_t>& data) = 0;

    // 红外摄像头
    virtual void infraredZoomIn() = 0;
    virtual void infraredZoomOut() = 0;

    // 状态管理
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual void notify(const std::any& value) = 0;

    // 延迟函数
    virtual void delay(const std::uint32_t& ms) = 0;

    // 更新配置
    virtual void updateConfig(const std::any& config) = 0;

    // 额外函数
    // 组合函数 功能指令+停止指令 单次调用 默认 100 毫秒后调用停止指令
    /*!
     * singleCall([&](){});
    */
    virtual void singleCall(std::function<void()> func, std::uint32_t delay_ms = 100) = 0;

    // 便利的单次调用函数
    // 向左平移 ←
    virtual void singlePanLeft(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 向右平移 →
    virtual void singlePanRight(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 向上倾斜 ↑
    virtual void singleTiltUp(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 向下倾斜 ↓
    virtual void singleTiltDown(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 左上移动 ←↑
    virtual void singleMoveLeftUp(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 右上移动 →↑
    virtual void singleMoveRightUp(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 左下移动 ←↓
    virtual void singleMoveLeftDown(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;
    // 右下移动 →↓
    virtual void singleMoveRightDown(std::uint8_t speed, std::uint32_t delay_ms = 100) = 0;

    // 焦点前调/调近焦点/聚焦近
    virtual void singleFocusNear(std::uint32_t delay_ms = 100) = 0;
    // 焦点后调/调远焦点/聚焦远
    virtual void singleFocusFar(std::uint32_t delay_ms = 100) = 0;

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void singleZoomIn(std::uint32_t delay_ms = 100) = 0;
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void singleZoomOut(std::uint32_t delay_ms = 100) = 0;

    // 光圈扩大
    virtual void singleIrisOpen(std::uint32_t delay_ms = 100) = 0;
    // 光圈缩小
    virtual void singleIrisClose(std::uint32_t delay_ms = 100) = 0;

    virtual PelcoDProtocolConfig* config() = 0;

    // [isValid,pan,tilt,zoom] [isValid, 0.0~360.0, -90.0~90.0, 0.0 ~ 1.0]
    virtual std::tuple<bool, float, float, float> ptz() = 0;
};

class SimplePelcoDProtocolImpl : public SimplePelcoDProtocol {
public:
    virtual ~SimplePelcoDProtocolImpl() {
    }
    // 向左平移 ←
    virtual void panLeft(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.left = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 向右平移 →
    virtual void panRight(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.right = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 向上倾斜 ↑
    virtual void tiltUp(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.up = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 向下倾斜 ↓
    virtual void tiltDown(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.down = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 左上移动 ←↑
    virtual void moveLeftUp(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.left = 1;
        standard_cmd._cmd.up = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 右上移动 →↑
    virtual void moveRightUp(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.right = 1;
        standard_cmd._cmd.up = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 左下移动 ←↓
    virtual void moveLeftDown(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.left = 1;
        standard_cmd._cmd.down = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 右下移动 →↓
    virtual void moveRightDown(std::uint8_t speed) override {
        speed = std::clamp<std::uint8_t>(speed, 0, 0x3f);
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.right = 1;
        standard_cmd._cmd.down = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(speed);
        cmd.push_back(speed);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 停止移动
    virtual void stopMotion(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // 焦点前调/调近焦点/聚焦近
    virtual void focusNear(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.focus_near = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 焦点后调/调远焦点/聚焦远
    virtual void focusFar(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.focus_far = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void zoomIn(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.zoom_tele = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void zoomOut(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.zoom_wide = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // 光圈扩大
    virtual void irisOpen(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.iris_open = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 光圈缩小
    virtual void irisClose(void) override {
        auto cmd = magic();
        StandardCMD standard_cmd;
        standard_cmd._cmd.iris_close = 1;
        auto& std_cmd = standard_cmd.cmd;
        cmd.push_back(std_cmd.cmd_1);
        cmd.push_back(std_cmd.cmd_2);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // 扩展指令
    // 设置预置点
    virtual void setPreset(std::uint8_t presetID) override {
        // 0x0~0x20
        presetID = std::clamp<std::uint8_t>(presetID, 0, 0xff);
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x03);
        cmd.push_back(0x00);
        cmd.push_back(presetID);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 清除预置点
    virtual void clearPreset(std::uint8_t presetID) override {
        presetID = std::clamp<std::uint8_t>(presetID, 0, 0xff);
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x05);
        cmd.push_back(0x00);
        cmd.push_back(presetID);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    // 调用预置点
    virtual void callPreset(std::uint8_t presetID) override {
        presetID = std::clamp<std::uint8_t>(presetID, 0, 0xff);
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x07);
        cmd.push_back(0x00);
        cmd.push_back(presetID);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // 高级扩展指令
    // 设置云台绝对值
    virtual void setPanPosition(std::uint8_t msb, std::uint8_t lsb) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x4B);
        cmd.push_back(msb);
        cmd.push_back(lsb);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void setTiltPosition(std::uint8_t msb, std::uint8_t lsb) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x4D);
        cmd.push_back(msb);
        cmd.push_back(lsb);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void setZoomPosition(std::uint8_t msb, std::uint8_t lsb) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x4F);
        cmd.push_back(msb);
        cmd.push_back(lsb);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void setMagnification(std::uint8_t msb, std::uint8_t lsb) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x5F);
        cmd.push_back(msb);
        cmd.push_back(lsb);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    /*!
    * 1) the value to use to set the pan position to 45 degrees is 4500.
    */
    virtual void setPanPosition(std::uint16_t pos) override {
        pos = std::clamp<std::uint16_t>(pos, 0, 35999);
        setPanPosition((pos >> 8) & 0xff, pos & 0xff);
    }
    /*!
    * 1) the value used to set the tilt position to 45 degrees below the horizon, is 4500.
    * 2) the value used to set the tilt position 30 degrees above the horizon, is 33000.
    * 3) Zero degrees indicates that the device is pointed horizontally (at the horizon).
    * 4) Ninety degrees indicates that the device is pointed straight down.
    */
    virtual void setTiltPosition(std::uint16_t pos) override {
        pos = std::clamp<std::uint16_t>(pos, 0, 35999);
        setTiltPosition((pos >> 8) & 0xff, pos & 0xff);
    }
    /*!
    * 镜头物理倍率归一化至 0~65535 区间
    * current_magnification = (position / 65535) * zoom_limit
    * position:镜头缩放当前位置
    * zoom_limit:镜头物理倍率极限(最大)位置
    */
    virtual void setZoomPosition(std::uint16_t pos) override {
        setZoomPosition((pos >> 8) & 0xff, pos & 0xff);
    }
    /*!
    * 百分之一放大倍率
    * 500 = 5X
    * 1000 = 10x
    */
    virtual void setMagnification(std::uint16_t pos) override {
        setMagnification((pos >> 8) & 0xff, pos & 0xff);
    }

    // 便捷的浮点操作函数
    // 0.0 ~ 360.0
    virtual void setPanPosition(std::float_t pos) override {
        pos = std::clamp<std::float_t>(pos, 0.f, 360.f);
        std::uint16_t value = static_cast<std::uint16_t>(100 * pos);
        setPanPosition(value);
    }
    // -90.0 ~ 90.0
    virtual void setTiltPosition(std::float_t pos) override {
        pos = std::clamp<std::float_t>(pos, -90.f, 90.f);
        if (std::isless(pos, 0.f)) {// <
            setTiltPosition(std::uint16_t(pos * -100.f));
            return;
        } else if (std::isgreater(pos, 0.f)) {// >
            setTiltPosition(std::uint16_t(36000 - pos * 100));
        } else {
            setTiltPosition(std::uint16_t(0));
        }
    }
    // 0.0 ~ 1.0
    virtual void setZoomPosition(std::float_t pos) override {
        pos = std::clamp<std::float_t>(pos, 0.f, 1.f);
        setZoomPosition(std::uint16_t(65535 * pos));
    }
    virtual void setMagnification(std::float_t pos) override {
        throw std::logic_error("use of undefined function");
    }

    // 查询指令
    virtual void queryPanPosition(void) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x51);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void queryTiltPosition(void) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x53);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void queryZoomPosition(void) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x55);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }
    virtual void queryMagnification(void) override {
        auto cmd = magic();
        cmd.push_back(0x00);
        cmd.push_back(0x61);
        cmd.push_back(0x00);
        cmd.push_back(0x00);
        cmd.push_back(checkSum(cmd));
        sendData(cmd);
    }

    // 响应查询指令
    // 需要重写 virtual void receiveData(const std::vector<std::uint8_t>& data) 函数并在里面调用下面的响应函数.
    virtual void queryPanPositionResponse(std::uint8_t pos) override {
        throw std::logic_error("use of undefined function");
    }
    virtual void queryTiltPositionResponse(std::uint8_t pos) override {
        throw std::logic_error("use of undefined function");
    }
    virtual void queryZoomPositionResponse(std::uint8_t pos) override {
        throw std::logic_error("use of undefined function");
    }
    virtual void queryMagnificationResponse(std::uint8_t pos) override {
        throw std::logic_error("use of undefined function");
    }

    // 用于各种奇葩的厂家扩展
    // 0:设置设备ID
    virtual void setAnyValue(std::uint8_t type, const std::any& value) override {
        try {
            switch (type) {
            case 0: {
                mDeviceAddress = std::any_cast<std::uint8_t>(value);
                break;
            }
            default: break;
            }
        } catch (const std::bad_any_cast& e) {
            std::cout << e.what() << std::endl;
        }
    }
    virtual std::any getAnyValue(std::uint8_t type) override {
        switch (type) {
        case 0: {
            return std::make_any<std::uint8_t>(mDeviceAddress);
        }
        default: break;
        }
        return std::any();
    }

    // 发送原始字节
    virtual void sendRawCmd(const std::vector<std::uint8_t>& data) override {
        this->sendData(data);
    }

    // 红外摄像头
    virtual void infraredZoomIn() override {
        auto old = getAnyValue(0);
        setAnyValue(0, std::make_any<std::uint8_t>(0x02));
        this->zoomIn();
        setAnyValue(0, old);
    };
    virtual void infraredZoomOut() override {
        auto old = getAnyValue(0);
        setAnyValue(0, std::make_any<std::uint8_t>(0x02));
        this->zoomOut();
        setAnyValue(0, old);
    };

    virtual void connect() override {
        throw std::logic_error("use of undefined function | virtual void connect()");
    };
    virtual void disconnect() override {
        throw std::logic_error("use of undefined function | virtual void disconnect()");
    };
    virtual bool isConnected() override {
        throw std::logic_error("use of undefined function | virtual bool isConnected()");
    };
    virtual void notify(const std::any& value) override {
        throw std::logic_error("use of undefined function | virtual void notify(const std::any& value)");
    };

    virtual void delay(const std::uint32_t& ms) override {
        throw std::logic_error("use of undefined function | virtual void delay(const std::int32_t& ms)");
    }

    virtual void updateConfig(const std::any& config) override {
        try {
            mConfig = std::any_cast<std::shared_ptr<PelcoDProtocolConfig>>(config);
        } catch (const std::bad_any_cast& e) {
            std::cout << e.what() << std::endl;
        }
    }

    // 单次调用函数
    virtual void singleCall(std::function<void()> func, std::uint32_t delay_ms = 100) override {
        func();
        delay(delay_ms);
        stopMotion();
    }

    // 便利的单次调用函数
    // 向左平移 ←
    virtual void singlePanLeft(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        panLeft(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 向右平移 →
    virtual void singlePanRight(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        panRight(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 向上倾斜 ↑
    virtual void singleTiltUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        tiltUp(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 向下倾斜 ↓
    virtual void singleTiltDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        tiltDown(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 左上移动 ←↑
    virtual void singleMoveLeftUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        moveLeftUp(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 右上移动 →↑
    virtual void singleMoveRightUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        moveRightUp(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 左下移动 ←↓
    virtual void singleMoveLeftDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        moveLeftDown(speed);
        delay(delay_ms);
        stopMotion();
    };
    // 右下移动 →↓
    virtual void singleMoveRightDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        moveRightDown(speed);
        delay(delay_ms);
        stopMotion();
    };

    // 焦点前调/调近焦点/聚焦近
    virtual void singleFocusNear(std::uint32_t delay_ms = 100) override {
        focusNear();
        delay(delay_ms);
        stopMotion();
    };
    // 焦点后调/调远焦点/聚焦远
    virtual void singleFocusFar(std::uint32_t delay_ms = 100) override {
        focusFar();
        delay(delay_ms);
        stopMotion();
    };

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void singleZoomIn(std::uint32_t delay_ms = 100) override {
        zoomIn();
        delay(delay_ms);
        stopMotion();
    };
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void singleZoomOut(std::uint32_t delay_ms = 100) override {
        zoomOut();
        delay(delay_ms);
        stopMotion();
    };

    // 光圈扩大
    virtual void singleIrisOpen(std::uint32_t delay_ms = 100) override {
        irisOpen();
        delay(delay_ms);
        stopMotion();
    };
    // 光圈缩小
    virtual void singleIrisClose(std::uint32_t delay_ms = 100) override {
        irisClose();
        delay(delay_ms);
        stopMotion();
    };

    virtual PelcoDProtocolConfig* config() override {
        return mConfig.operator->();
    }

    virtual std::tuple<bool, float, float, float> ptz() override {
        return std::make_tuple<bool, float, float, float>(false, 0.f, 0.f, 0.f);
    }

protected:
    // 重载该函数处理发送数据逻辑
    virtual void sendData(const std::vector<std::uint8_t>& data) {
        throw std::logic_error("use of undefined function | virtual void sendData(const std::vector<std::uint8_t>& data)");
    }
    // 重载该函数处理接收数据逻辑
    virtual void receiveData(const std::vector<std::uint8_t>& data) {
        throw std::logic_error("use of undefined function | virtual void receiveData(const std::vector<std::uint8_t>& data)");
    }
    // 魔术头
    virtual std::vector<uint8_t> magic() {
        return std::vector<uint8_t>{0xff, mDeviceAddress};
    }
    // 校验函数
    virtual uint8_t checkSum(const std::vector<uint8_t>& data) {
        auto sum = 0;
        for (auto i = 1u; i < data.size(); ++i) {
            sum += data.at(i);
        }
        return sum % 0x100;
    }

protected:
    /*!
    * 设备地址 0x01~0xff
    * 当然也可以使用 virtual std::uint8_t deviceAddress() {return 0x01;} 函数替代该成员变量,需要修改设备ID时继承该类重写该函数即可.
    */
    std::uint8_t mDeviceAddress = 0x01;
    std::shared_ptr<PelcoDProtocolConfig> mConfig;
};

class SimplePelcoDDecorator : public SimplePelcoDProtocol {
public:
    SimplePelcoDDecorator(SimplePelcoDProtocol* component)
        : mComponent(component) {
    }

    // 向左平移 ←
    virtual void panLeft(std::uint8_t speed) override {
        this->mComponent->panLeft(speed);
    }
    // 向右平移 →
    virtual void panRight(std::uint8_t speed) override {
        this->mComponent->panRight(speed);
    }
    // 向上倾斜 ↑
    virtual void tiltUp(std::uint8_t speed) override {
        this->mComponent->tiltUp(speed);
    }
    // 向下倾斜 ↓
    virtual void tiltDown(std::uint8_t speed) override {
        this->mComponent->tiltDown(speed);
    }
    // 左上移动 ←↑
    virtual void moveLeftUp(std::uint8_t speed) override {
        this->mComponent->moveLeftUp(speed);
    }
    // 右上移动 →↑
    virtual void moveRightUp(std::uint8_t speed) override {
        this->mComponent->moveRightUp(speed);
    }
    // 左下移动 ←↓
    virtual void moveLeftDown(std::uint8_t speed) override {
        this->mComponent->moveLeftDown(speed);
    }
    // 右下移动 →↓
    virtual void moveRightDown(std::uint8_t speed) override {
        this->mComponent->moveRightDown(speed);
    }
    // 停止移动
    virtual void stopMotion(void) override {
        this->mComponent->stopMotion();
    }

    // 焦点前调/调近焦点/聚焦近
    virtual void focusNear(void) override {
        this->mComponent->focusNear();
    }
    // 焦点后调/调远焦点/聚焦远
    virtual void focusFar(void) override {
        this->mComponent->focusFar();
    }

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void zoomIn(void) override {
        this->mComponent->zoomIn();
    }
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void zoomOut(void) override {
        this->mComponent->zoomOut();
    }

    // 光圈扩大
    virtual void irisOpen(void) override {
        this->mComponent->irisOpen();
    }
    // 光圈缩小
    virtual void irisClose(void) override {
        this->mComponent->irisClose();
    }

    // 扩展指令
    // 设置预置点
    virtual void setPreset(std::uint8_t presetID) override {
        this->mComponent->setPreset(presetID);
    }
    // 清除预置点
    virtual void clearPreset(std::uint8_t presetID) override {
        this->mComponent->clearPreset(presetID);
    }
    // 调用预置点
    virtual void callPreset(std::uint8_t presetID) override {
        this->mComponent->callPreset(presetID);
    }

    // 高级扩展指令
    // 设置云台绝对值
    virtual void setPanPosition(std::uint8_t msb, std::uint8_t lsb) override {
        this->mComponent->setPanPosition(msb, lsb);
    }
    virtual void setTiltPosition(std::uint8_t msb, std::uint8_t lsb) override {
        this->mComponent->setTiltPosition(msb, lsb);
    }
    virtual void setZoomPosition(std::uint8_t msb, std::uint8_t lsb) override {
        this->mComponent->setZoomPosition(msb, lsb);
    }
    virtual void setMagnification(std::uint8_t msb, std::uint8_t lsb) override {
        this->mComponent->setMagnification(msb, lsb);
    }

    virtual void setPanPosition(std::uint16_t pos) override {
        this->mComponent->setPanPosition(pos);
    }
    virtual void setTiltPosition(std::uint16_t pos) override {
        this->mComponent->setTiltPosition(pos);
    }
    virtual void setZoomPosition(std::uint16_t pos) override {
        this->mComponent->setZoomPosition(pos);
    }
    virtual void setMagnification(std::uint16_t pos) override {
        this->mComponent->setMagnification(pos);
    }

    // 便捷的浮点操作函数
    // 0.0 ~ 360.0
    virtual void setPanPosition(std::float_t pos) override {
        this->mComponent->setPanPosition(pos);
    }
    // -90.0 ~ 90.0
    virtual void setTiltPosition(std::float_t pos) override {
        this->mComponent->setTiltPosition(pos);
    }
    // 0.0 ~ 1.0
    virtual void setZoomPosition(std::float_t pos) override {
        this->mComponent->setZoomPosition(pos);
    }
    virtual void setMagnification(std::float_t pos) override {
        this->mComponent->setMagnification(pos);
    }

    // 查询指令
    virtual void queryPanPosition(void) override {
        this->mComponent->queryPanPosition();
    }
    virtual void queryTiltPosition(void) override {
        this->mComponent->queryTiltPosition();
    }
    virtual void queryZoomPosition(void) override {
        this->mComponent->queryZoomPosition();
    }
    virtual void queryMagnification(void) override {
        this->mComponent->queryMagnification();
    }

    // 响应查询指令
    virtual void queryPanPositionResponse(std::uint8_t pos) override {
        this->mComponent->queryPanPositionResponse(pos);
    }
    virtual void queryTiltPositionResponse(std::uint8_t pos) override {
        this->mComponent->queryTiltPositionResponse(pos);
    }
    virtual void queryZoomPositionResponse(std::uint8_t pos) override {
        this->mComponent->queryZoomPositionResponse(pos);
    }
    virtual void queryMagnificationResponse(std::uint8_t pos) override {
        this->mComponent->queryMagnificationResponse(pos);
    }

    // 用于各种奇葩的厂家扩展
    virtual void setAnyValue(std::uint8_t type, const std::any& value) override {
        this->mComponent->setAnyValue(type, value);
    }
    virtual std::any getAnyValue(std::uint8_t type) override {
        return this->mComponent->getAnyValue(type);
    }

    // 发送原始字节
    virtual void sendRawCmd(const std::vector<std::uint8_t>& data) override {
        this->mComponent->sendRawCmd(data);
    }

    // 红外摄像头放大/缩小
    virtual void infraredZoomIn() override {
        this->mComponent->infraredZoomIn();
    };
    virtual void infraredZoomOut() override {
        this->mComponent->infraredZoomOut();
    };

    // 状态管理
    virtual void connect() override {
        return this->mComponent->connect();
    }
    virtual void disconnect() override {
        return this->mComponent->disconnect();
    }
    virtual bool isConnected() override {
        return this->mComponent->isConnected();
    }
    // 状态通知
    virtual void notify(const std::any& value) override {
        this->mComponent->notify(value);
    }

    // 延迟指定毫秒
    virtual void delay(const std::uint32_t& ms) override {
        this->mComponent->delay(ms);
    }

    // 更新配置
    virtual void updateConfig(const std::any& config) override {
        this->mComponent->updateConfig(config);
    }

    // 单次调用函数
    virtual void singleCall(std::function<void()> func, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleCall(func, delay_ms);
    }
    virtual void singlePanLeft(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singlePanLeft(speed, delay_ms);
    }
    // 向右平移 →
    virtual void singlePanRight(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singlePanRight(speed, delay_ms);
    }
    // 向上倾斜 ↑
    virtual void singleTiltUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleTiltUp(speed, delay_ms);
    }
    // 向下倾斜 ↓
    virtual void singleTiltDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleTiltDown(speed, delay_ms);
    }
    // 左上移动 ←↑
    virtual void singleMoveLeftUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleMoveLeftUp(speed, delay_ms);
    }
    // 右上移动 →↑
    virtual void singleMoveRightUp(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleMoveRightUp(speed, delay_ms);
    }
    // 左下移动 ←↓
    virtual void singleMoveLeftDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleMoveLeftDown(speed, delay_ms);
    }
    // 右下移动 →↓
    virtual void singleMoveRightDown(std::uint8_t speed, std::uint32_t delay_ms = 100) override {
        this->mComponent->singleMoveRightDown(speed, delay_ms);
    }

    // 焦点前调/调近焦点/聚焦近
    virtual void singleFocusNear(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleFocusNear(delay_ms);
    }
    // 焦点后调/调远焦点/聚焦远
    virtual void singleFocusFar(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleFocusFar(delay_ms);
    }

    // zoomIn/放大/焦距变大/倍率变大/zoomTele
    virtual void singleZoomIn(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleZoomIn(delay_ms);
    }
    // zoomOut/缩小/焦距变小/倍率变小/zoomWide
    virtual void singleZoomOut(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleZoomOut(delay_ms);
    }

    // 光圈扩大
    virtual void singleIrisOpen(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleIrisOpen(delay_ms);
    }
    // 光圈缩小
    virtual void singleIrisClose(std::uint32_t delay_ms = 100) override {
        this->mComponent->singleIrisClose(delay_ms);
    }

    virtual PelcoDProtocolConfig* config() override {
        return this->mComponent->config();
    }

    virtual std::tuple<bool, float, float, float> ptz() override {
        return this->mComponent->ptz();
    }

protected:
    SimplePelcoDProtocol* mComponent;
};

class SimplePelcoDDecoratorA final : public SimplePelcoDDecorator {
public:
    SimplePelcoDDecoratorA(SimplePelcoDProtocol* component)
        : SimplePelcoDDecorator(component) {
    }
    virtual void stopMotion(void) override {
        std::cout << "call void stopMotion(void) function" << std::endl;
        SimplePelcoDDecorator::stopMotion();
    }
};

class ExampleAttachObject : public PelcoDProtocolConfig {
public:
    virtual std::string ip() const override {
        return R"(192.168.1.68)";
    };
    virtual uint32_t port() const override {
        return 8000;
    };
    virtual std::string rtspMainUrl() const override {
        return R"(rtsp://admin:system123@192.168.1.68:554/Stream/Live/101?transportmode=unicast&profile=ONFProfileToken_101)";
    };
    virtual std::string rtspSubUrl() const override {
        return R"()";
    };
    virtual std::string username() const override {
        return R"(admin)";
    };
    virtual std::string password() const override {
        return R"(system123)";
    };
};

class PluginContext {
};

class PluginFramework {
public:
    // QPluginLoader
    void registerService(std::string name, void* instance) {
    }
    void* getService(std::string name) {
        return nullptr;
    }
};

#endif//PelcoDProtocol