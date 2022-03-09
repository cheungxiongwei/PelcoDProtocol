//
// Created by cheungxiongwei on 2021/9/27.
//

#ifndef PELCODPROTOCOL_H
#define PELCODPROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

// ref https://gist.github.com/jn0/cc5c78f4a0f447a6fb2e45a5d9efa13d
class PelcoDProtocol {
    enum class Mode { Auto, On, Off };
    enum class Speed { Low, Medium, High ,Super };
public:
    PelcoDProtocol(uint8_t deviceAddress = 0x01);
    virtual ~PelcoDProtocol() noexcept;
    PelcoDProtocol(std::function<void(std::vector<uint8_t> &data)> callback,uint8_t deviceAddress = 0x01);
    // 标准命令
    // 向左平移 ←
    void panLeft(uint8_t speed);
    // 向右偏移 →
    void panRight(uint8_t speed);
    // 向上倾斜 ↑
    void tiltUp(uint8_t speed);
    // 向下倾斜 ↓
    void tiltDown(uint8_t speed);
    // 左上移动 ←↑
    void moveLeftUp(uint8_t speed);
    // 右上移动 →↑
    void moveRightUp(uint8_t speed);
    // 左下移动 ←↓
    void MoveLeftDown(uint8_t speed);
    // 右下移动 →↓
    void moveRightDown(uint8_t speed);
    // 停止移动
    void stopMotion(void);

    // 调近焦距/聚焦近
    void focusNear(void);
    // 调远焦距/聚焦远
    void focusFar(void);
    // 缩小镜头/变倍小
    void zoomTele(void);
    // 放大镜头/变倍大
    void zoomWide(void);
    // 光圈关
    void irisClose(void);
    // 光圈开
    void irisOpen(void);

    void cameraOn(void);
    void cameraOff(void);

    // 扩展命令
    //设置预制点
    void setPreset(uint8_t presetID);
    //清除预制点
    void clearPreset(uint8_t presetID);
    //移动到指定预制点
    void goToPreset(uint8_t presetID);
    // 翻转180
    void flip180(void);
    void goToZeroPan(void);
    void setAuxiliary(uint8_t auxiliaryID/*01-08*/);
    void clearAuxiliary(uint8_t auxiliaryID/*01-08*/);
    void remoteReset(void);
    void setZoneStart(uint8_t zoneID/*01-08*/);
    void setZoneEnd(uint8_t zoneID/*01-08*/);
    void writeCharacterToScreen(uint8_t column/*0x00-0x27*/, uint8_t asciiValue);
    void clearScreen(void);
    void alarmAcknowledge(uint8_t alarmLevel/*0x01-0x08*/);
    void zoneScanOn(void);
    void zoneScanOff(void);
    // 开启模式录制
    void setPatternStart(uint8_t patternID/*0x01-0x04*/);
    // 停止模式录制
    void setPatternStop(void);
    // 运行指定模式
    void runPattern(uint8_t patternID/*0x01-0x04*/);
    void setZoomSpeed(uint8_t speed/*0x00-0x03*/);
    void setFocusSpeed(uint8_t speed/*0x00-0x03*/);
    void resetCameraToDefaults(void);
    void autoFocus(uint8_t mode/*0x00-0x02*/);//auto/on/off
    void autoFocus(Mode mode/*0x00-0x02*/);
    void autoIris(uint8_t mode/*0x00-0x02*/);//auto/on/off
    void autoIris(Mode mode/*0x00-0x02*/);
    void autoAGC(uint8_t mode/*0x00-0x02*/);//auto/on/off
    void autoAGC(Mode mode/*0x00-0x02*/);
    // 开启/关闭背光补偿
    void backlightCompensation(uint8_t mode/*0x01-0x02*/);//on/off
    void backlightCompensation(Mode mode);
    void autoWhiteBalance(uint8_t mode/*0x01-0x02*/);
    void autoWhiteBalance(Mode mode);
    void enableDevicePhaseDelayMode(void);
    void setShutterSpeed(uint16_t any) = delete;
    void adjustLineLockPhaseDelay(uint16_t any) = delete;
    void adjustWhiteBalanceRB(uint16_t any) = delete;
    void adjustWhiteBalanceMG(uint16_t any) = delete;
    void adjustGain(uint16_t any) = delete;
    void adjustAutoIrisLevel(uint16_t any) = delete;
    void adjustAutoIrisPeakValue(uint16_t any) = delete;
    void query(uint16_t any) = delete;
    // 高级命令
    // 设置起始零位置
    void setZeroPosition(void);
    // 设置水平位置
    void setPanPosition(uint8_t msb, uint8_t lsb);
    void setPanPosition(uint16_t angle/*0-35999*/);
    void setPanPosition(float_t angle/*0.f-359.99.f*/);
    // 设置垂直位置
    // 0+x向下 36000-x向上
    void setTiltPosition(uint8_t msb, uint8_t lsb);
    void setTiltPosition(uint16_t angle/*0-35999*/);
    void setTiltPosition(float_t angle/*-180.f~180.f*/);
    // 设置缩放比例
    void setZoomPosition(uint8_t msb, uint8_t lsb);
    void setZoomPosition(uint16_t factor);
    // 查询水平角度
    void queryPanPosition(void);
    // 查询垂直角度
    void queryTiltPosition(void);
    // 查询缩放系数
    void queryZoomPosition(void);
    // 设置放大倍率
    void setMagnification(uint8_t msb,uint8_t lsb);
    void setMagnification(uint16_t factor);
    void queryMagnification(void);

    // 发送原始字节
    void sendRawCMD(std::vector<uint8_t> data);

    // PelcoD协议 7 字节校验
    bool isValid(uint8_t *raw,uint8_t len);
protected:
    // 继承类需要重载该函数实现自己的发送数据逻辑
    virtual void sendCMD(std::vector<uint8_t> &data);
    // 当使用的是非标协议时需要重载该数据实现非标魔术头
    virtual std::vector<uint8_t> magic();
    // 当使用的是非标协议时需要重载该数据实现非标效验
    virtual uint8_t checkSum(std::vector<uint8_t> cmd);
    void normalSpeed(uint8_t &speed);
    void normal(uint8_t &value, const uint8_t min, const uint8_t max);
private:
    uint8_t mDeviceAddress;//0x01-0xff
    // 回调函数
    std::function<void(std::vector<uint8_t> &data)> mHandle;
};

#endif //PELCODPROTOCOL_H
