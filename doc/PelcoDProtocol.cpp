//
// Created by cheungxiongwei on 2021/10/01.
//

#include "PelcoDProtocol.h"
#include <stdexcept>

/*!
PELCO_D 协议
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

struct _StandardCMD
{
    //Command 1
    uint32_t focus_near : 1;
    uint32_t iris_open : 1;
    uint32_t iris_close : 1;
    uint32_t camera : 1;
    uint32_t scan : 1;
    uint32_t reserve_1 : 1;//reserve
    uint32_t reserve_2 : 1;//reserve
    uint32_t sense : 1;

    //Command 2
    uint32_t always : 1;
    uint32_t right : 1;
    uint32_t left : 1;
    uint32_t up : 1;
    uint32_t down : 1;
    uint32_t zoom_tele : 1;
    uint32_t zoom_wide : 1;
    uint32_t focus_far : 1;
};

struct StandardCMD_
{
    uint8_t cmd_1;
    uint8_t cmd_2;
};

union StandardCMD
{
    StandardCMD_ cmd;
    _StandardCMD _cmd = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
};

PelcoDProtocol::PelcoDProtocol(uint8_t deviceAddress):mDeviceAddress(deviceAddress)  {

}

PelcoDProtocol::PelcoDProtocol(std::function<void(std::vector<uint8_t> &)> callback, uint8_t deviceAddress)
        : mHandle(callback), mDeviceAddress(deviceAddress) {

}

PelcoDProtocol::~PelcoDProtocol() noexcept {

}

void PelcoDProtocol::panLeft(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.left = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::panRight(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.right = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::tiltUp(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.up = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::tiltDown(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.down = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::moveLeftUp(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.left = 1;
    standard_cmd._cmd.up = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::moveRightUp(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.right = 1;
    standard_cmd._cmd.up = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::MoveLeftDown(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.left = 1;
    standard_cmd._cmd.down = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::moveRightDown(uint8_t speed) {
    normalSpeed(speed);
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.right = 1;
    standard_cmd._cmd.down = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(speed);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::stopMotion(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::focusNear(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.focus_near = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::focusFar(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.focus_far = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::zoomTele(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.zoom_tele = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::zoomWide(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.zoom_wide = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::irisClose(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.iris_close = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::irisOpen(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.iris_open = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::cameraOn(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.camera = 1;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::cameraOff(void) {
    auto cmd = magic();
    StandardCMD standard_cmd;
    standard_cmd._cmd.camera = 0;
    auto & std_cmd = standard_cmd.cmd;
    cmd.push_back(std_cmd.cmd_1);
    cmd.push_back(std_cmd.cmd_2);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setPreset(uint8_t presetID) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x03);
    cmd.push_back(0x00);
    cmd.push_back(presetID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::clearPreset(uint8_t presetID) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x05);
    cmd.push_back(0x00);
    cmd.push_back(presetID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::goToPreset(uint8_t presetID) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x07);
    cmd.push_back(0x00);
    cmd.push_back(presetID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::flip180(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x07);
    cmd.push_back(0x00);
    cmd.push_back(0x21);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::goToZeroPan(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x07);
    cmd.push_back(0x00);
    cmd.push_back(0x22);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setAuxiliary(uint8_t auxiliaryID) {
    normal(auxiliaryID, 0x01, 0x08);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x09);
    cmd.push_back(0x00);
    cmd.push_back(auxiliaryID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::clearAuxiliary(uint8_t auxiliaryID) {
    normal(auxiliaryID, 0x01, 0x08);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x0b);
    cmd.push_back(0x00);
    cmd.push_back(auxiliaryID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::remoteReset(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x0f);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setZoneStart(uint8_t zoneID) {
    normal(zoneID, 0x01, 0x08);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x11);
    cmd.push_back(0x00);
    cmd.push_back(zoneID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setZoneEnd(uint8_t zoneID) {
    normal(zoneID, 0x01, 0x08);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x13);
    cmd.push_back(0x00);
    cmd.push_back(zoneID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::writeCharacterToScreen(uint8_t column, uint8_t asciiValue) {
    normal(column, 0x00, 0x27);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x15);
    cmd.push_back(column);
    cmd.push_back(asciiValue);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::clearScreen(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x17);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::alarmAcknowledge(uint8_t alarmLevel) {
    normal(alarmLevel, 0x01, 0x08);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x19);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::zoneScanOn(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x1B);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::zoneScanOff(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x1D);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setPatternStart(uint8_t patternID) {
    normal(patternID,1,4);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x1F);
    cmd.push_back(0x00);
    cmd.push_back(patternID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setPatternStop(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x21);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::runPattern(uint8_t patternID) {
    normal(patternID,1,4);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x23);
    cmd.push_back(0x00);
    cmd.push_back(patternID);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setZoomSpeed(uint8_t speed) {
    normal(speed, 0x00, 0x03);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x25);
    cmd.push_back(0x00);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setFocusSpeed(uint8_t speed) {
    normal(speed, 0x00, 0x03);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x27);
    cmd.push_back(0x00);
    cmd.push_back(speed);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::resetCameraToDefaults(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x29);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoFocus(uint8_t mode) {
    normal(mode, 0x00, 0x02);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2B);
    cmd.push_back(0x00);
    cmd.push_back(mode);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoFocus(PelcoDProtocol::Mode mode) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2B);
    cmd.push_back(0x00);
    cmd.push_back(static_cast<uint8_t>(mode));
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoIris(uint8_t mode) {
    normal(mode, 0x00, 0x02);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2D);
    cmd.push_back(0x00);
    cmd.push_back(mode);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoIris(PelcoDProtocol::Mode mode) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2D);
    cmd.push_back(0x00);
    cmd.push_back(static_cast<uint8_t>(mode));
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoAGC(uint8_t mode) {
    normal(mode, 0x00, 0x02);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2F);
    cmd.push_back(0x00);
    cmd.push_back(mode);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoAGC(PelcoDProtocol::Mode mode) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x2F);
    cmd.push_back(0x00);
    cmd.push_back(static_cast<uint8_t>(mode));
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::backlightCompensation(uint8_t mode) {
    normal(mode, 0x01, 0x02);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x31);
    cmd.push_back(0x00);
    cmd.push_back(mode);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::backlightCompensation(PelcoDProtocol::Mode mode) {
    if(mode == Mode::Auto){
        std::invalid_argument("parameter error");
    }
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x31);
    cmd.push_back(0x00);
    cmd.push_back(static_cast<uint8_t>(mode));
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoWhiteBalance(uint8_t mode) {
    normal(mode, 0x01, 0x02);
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x33);
    cmd.push_back(0x00);
    cmd.push_back(mode);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::autoWhiteBalance(PelcoDProtocol::Mode mode) {
    if(mode == Mode::Auto){
        std::invalid_argument("parameter error");
    }
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x33);
    cmd.push_back(0x00);
    cmd.push_back(static_cast<uint8_t>(mode));
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::enableDevicePhaseDelayMode(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x35);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setZeroPosition(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x49);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setPanPosition(uint8_t msb, uint8_t lsb) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x4B);
    cmd.push_back(msb);
    cmd.push_back(lsb);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setPanPosition(uint16_t angle) {
    angle = angle > 35999 ? 35999 : angle;
    setPanPosition((angle >> 8) & 0xff,angle & 0xff);
}

void PelcoDProtocol::setPanPosition(float_t angle)
{
    setPanPosition(static_cast<uint16_t>(angle * 100.f));
}

void PelcoDProtocol::setTiltPosition(uint8_t msb, uint8_t lsb) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x4D);
    cmd.push_back(msb);
    cmd.push_back(lsb);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setTiltPosition(uint16_t angle) {
    angle = angle > 35999 ? 35999 : angle;
    setTiltPosition((angle >> 8) & 0xff,angle & 0xff);
}

/*!
0: 水平0度
4500:水平向下45度
33000:水平向上30度 36000-3000
*/
void PelcoDProtocol::setTiltPosition(float_t angle)
{
    angle *= -100.f;
    if(angle > 0.f){
        setTiltPosition(static_cast<uint16_t>(angle));
    }else if(angle < 0.f){
        angle += 36000;
        setTiltPosition(static_cast<uint16_t>(angle));
    }else{
        setTiltPosition(static_cast<uint16_t>(0));
    }
}

/*!
X5/X184
X184 最大放大倍数
X5 当前放大倍数
Position = (5 / 184) * 65535 = approximately 1781
*/
void PelcoDProtocol::setZoomPosition(uint8_t msb, uint8_t lsb) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x4F);
    cmd.push_back(msb);
    cmd.push_back(lsb);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setZoomPosition(uint16_t factor) {
    setZoomPosition((factor >> 8) & 0xff,factor & 0xff);
}

void PelcoDProtocol::queryPanPosition(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x51);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::queryTiltPosition(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x53);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::queryZoomPosition(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x55);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

//500 = 5x 1000 = 10x 放大倍数
void PelcoDProtocol::setMagnification(uint8_t msb, uint8_t lsb) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x5F);
    cmd.push_back(msb);
    cmd.push_back(lsb);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::setMagnification(uint16_t factor) {
    setMagnification((factor >> 8) & 0xff,factor & 0xff);
}

void PelcoDProtocol::queryMagnification(void) {
    auto cmd = magic();
    cmd.push_back(0x00);
    cmd.push_back(0x61);
    cmd.push_back(0x00);
    cmd.push_back(0x00);
    cmd.push_back(checkSum(cmd));
    sendCMD(cmd);
}

void PelcoDProtocol::sendRawCMD(std::vector<uint8_t> data) {
    sendCMD(data);
}

bool PelcoDProtocol::isValid(uint8_t *raw, uint8_t len)
{
    if(len != 7)
        return false;
    uint8_t checksum = 0;
    for(auto i = 1;i < 6;++i){
        checksum += raw[i];
        checksum &= 0xff;
    }
    return checksum == (uint8_t)raw[6] ? true : false;
}

void PelcoDProtocol::sendCMD(std::vector<uint8_t> &data) {
    if(mHandle){
        mHandle(data);
    }
}

std::vector<uint8_t> PelcoDProtocol::magic() {
    return std::vector<uint8_t>{0xff, mDeviceAddress};
}

uint8_t PelcoDProtocol::checkSum(std::vector<uint8_t> cmd) {
    auto sum = 0;
    for (auto i = 1; i < cmd.size(); ++i) {
        sum += cmd.at(i);
    }
    return sum % 0x100;
}

void PelcoDProtocol::normalSpeed(uint8_t &speed) {
    if (speed > 0x3f) {
        speed = 0x3f;
    }
}

void PelcoDProtocol::normal(uint8_t &value, const uint8_t min, const uint8_t max) {
    value = value > max ? max : (value < min ? min : value);
}
