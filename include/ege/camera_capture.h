#pragma once
#ifndef EGE_CAMERA_CAPTURE_H
#define EGE_CAMERA_CAPTURE_H

// 鍒ゆ柇涓€涓嬫槸鍚︽敮鎸?C++17, 涓嶆敮鎸佺殑缁欎竴涓鍛?

#if !(defined(EGE_ENABLE_CAMERA_CAPTURE) && EGE_ENABLE_CAMERA_CAPTURE) && __cplusplus < 201703L

#pragma message( \
    "Warning: 浣犳鍦ㄤ娇鐢?EGE 鐨勭浉鏈烘ā鍧? 浣嗘槸浣犵殑缂栬瘧鍣ㄨ缃负浜嗚緝浣庣殑 C++ 鏍囧噯. 濡傛灉鎻愮ず鎵句笉鍒扮浉鍏崇鍙? 璇疯€冭檻鏇挎崲涓烘敮鎸?C++17 鐨勭紪璇戝櫒. 瀹氫箟瀹?EGE_ENABLE_CAMERA_CAPTURE=1 鏉ュ拷鐣ヨ繖涓鍛?")

// 闃叉涓枃涔辩爜鐪嬩笉瑙? 鑻辨枃鐗堜篃鎵撳嵃涓€涓?
#pragma message( \
    "Warning: You are using the EGE camera module, but your compiler is set to a lower C++ standard. If you encounter missing symbols, please consider switching to a compiler that supports C++17. Define the macro EGE_ENABLE_CAMERA_CAPTURE=1 to ignore this warning.")

#endif

#include "ege.h"

/// 寮哄埗鐩告満杈撳嚭鏍煎紡涓?BGRA, 閫傞厤 ege::Image.
/// FrameOrientation 闄愬埗涓?TopToBottom, 閫傞厤 ege::Image.
/// Allocator 閲嶆柊瀹炵幇涓?ege::Image 鍐呴儴.
/// 鏈ご鏂囦欢灏介噺鍩轰簬鏃х増鏈殑 C++ 鏍囧噯鏉ョ紪鍐? 浠ヤ究浣跨敤鑰呭湪鏃х増鏈殑缂栬瘧鍣ㄤ笂涔熻兘浣跨敤锛?
/// 杩欐牱鍙缂栬瘧鍣ㄦ湰韬繍琛屾椂鏀寔锛屽苟涓?link 鍒?ege 灏卞彲浠ヤ簡.

namespace ccap
{
class Provider;
struct VideoFrame;
} // namespace ccap

namespace ege
{
class CameraCapture;
struct FrameContainer;

/// @brief 妫€鏌ユ槸鍚﹀惎鐢ㄤ簡鐩告満鎹曡幏妯″潡. 杩欓€氬父鏄紪璇戞椂纭畾鐨勩€?
/// 鐩墠鏀寔鐨勭増鏈负: 1. MSVC2022+, 2. 鏀寔 C++17 鐨?MinGW 绛?
/// @return 濡傛灉鍚敤浜嗙浉鏈烘崟鑾锋ā鍧? 杩斿洖 true; 鍚﹀垯杩斿洖 false.
bool hasCameraCaptureModule();

/// 涓€涓抚鏁版嵁鐨勫鍣? 鍋氫竴浜涙暟鎹洖鏀剁浉鍏崇殑宸ヤ綔, 闃叉鍙嶅鍒嗛厤鍐呭瓨.
/// 涓轰簡濂戝悎 ege::IMAGE 鐨勮璁? 杩欓噷鐨勫儚绱犳牸寮忔案杩滄槸 BGRA.
class CameraFrame
{
protected:
    /// @brief 璇蜂娇鐢?release 鏂规硶鏉ュ洖鏀? 涓嶈鐩存帴 delete.
    virtual ~CameraFrame() = 0;

public:
    CameraFrame();

    /// @brief 璇蜂娇鐢?release 鏂规硶鏉ュ洖鏀? 涓嶈鐩存帴 delete.
    virtual void release() = 0;

    /**
     * @brief 鑾峰彇 CameraFrame 瀵瑰簲鐨?ege::IMAGE. 杩欓噷鐨?PIMAGE 鏄竴涓寚閽?
     *      浣嗘槸瀹冪殑鍐呭瓨鏄垎閰嶅湪 CameraFrame 鍐呴儴鐨?
     *      杩欓噷鐨勮繑鍥炲€?PIMAGE 鐨勭敓鍛藉懆鏈熷綊 CameraFrame 绠＄悊.
     *
     * @return 涓€涓寚鍚?ege::IMAGE 鐨勬寚閽? 鎴栬€?nullptr (濡傛灉娌℃湁鏁版嵁).
     * @note 杩欎釜鏂规硶杩炵画璋冪敤澶氭浼氳繑鍥炲悓涓€涓寚閽? 娌℃湁棰濆鐨勫紑閿€.
     *
     */
    virtual PIMAGE getImage() = 0;

    /**
     * @brief 浣滅敤璺?getImage 绫讳技锛?浣嗘槸寮哄埗鎷疯礉涓€浠芥暟鎹? 浣跨敤鑰呴渶瑕佽嚜宸卞彇閲婃斁杩欎釜 IMAGE.
     *  濡傛灉甯屾湜鍦ㄧ浉鏈哄叧闂箣鍚庣户缁娇鐢ㄨ繖涓暟鎹? 璇蜂娇鐢ㄨ繖涓嚱鏁?
     *  鍙互鐢ㄥ湪绫讳技浜庢媿鐓т箣鍚庣户缁繘琛屾洿澶氬鐞嗙殑鍦烘櫙.
     *
     * @return 涓€涓寚鍚?ege::IMAGE 鐨勬寚閽? 鎴栬€?nullptr (濡傛灉娌℃湁鏁版嵁).
     * @note 杩欎釜鏂规硶鍐呴儴鏃犵紦瀛? 璇锋敞鎰忛噴鏀惧唴瀛?
     */
    virtual PIMAGE copyImage() = 0;

    /// 鑾峰彇鍘熷鏁版嵁
    virtual unsigned char* getData() = 0;

    /// 姣忎竴琛岀殑瀛楄妭鏁? 涓嶄竴瀹氱瓑浜?width * 4, 鍙兘鏄榻愮殑.
    virtual int getLineSizeInBytes() = 0;

    /// 鑾峰彇鍘熷鏁版嵁鍥惧儚鐨勫
    virtual int getWidth() const = 0;

    /// 鑾峰彇鍘熷鏁版嵁鍥惧儚鐨勯珮
    virtual int getHeight() const = 0;

protected:
    CameraCapture*    m_camera;
    ccap::VideoFrame* m_frame;
};

class CameraCapture
{
public:
    CameraCapture();
    ~CameraCapture();

    struct DeviceInfo
    {
        char name[128]; ///< 璁惧鍚嶇О
        // ... 鍚庨潰涔熻浼氭坊鍔犳洿澶氫俊鎭? 姣斿鐩告満鏀寔鐨勫垎杈ㄧ巼, 鍍忕礌鏍煎紡绛?
    };

    struct DeviceList
    {
        DeviceList() = default;

        DeviceList(DeviceInfo* _info, int _count) : info(_info), count(_count) {}

        DeviceList(const DeviceList&) = delete;

        DeviceList(DeviceList&& d) : info(d.info), count(d.count)
        {
            const_cast<DeviceInfo*&>(d.info) = nullptr; // 閬垮厤鏋愭瀯鏃堕噸澶嶉噴鏀?
            const_cast<int&>(d.count)        = 0;
        }

        DeviceList& operator=(const DeviceList&) = delete;
        ~DeviceList();
        const DeviceInfo* info  = nullptr; ///< 璁惧淇℃伅(鏁扮粍)
        const int         count = 0;       ///< 璁惧鏁伴噺
    };

    /**
     * @brief 鏌ユ壘鎵€鏈夊彲鐢ㄧ殑鐩告満璁惧鍚嶇О.
     * @note 杩欎釜鍑芥暟浼氭壂鎻忔墍鏈夊彲鐢ㄧ殑鐩告満璁惧, 骞惰繑鍥炰竴涓?DeviceList 瀵硅薄.
     *       杩欎釜 DeviceList 瀵硅薄鍖呭惈浜嗘墍鏈夎澶囩殑鍚嶇О鍜屾暟閲?
     *       杩欓噷鍘熸湰搴旇杩斿洖 `std::vector<std::string>`,
     *       浣嗘槸 EGE 鍐呴儴鍦?MSVC 涓嬩娇鐢?`/MT` 缂栬瘧鏉ラ伩鍏嶅紩鍏ュ姩鎬佸簱渚濊禆, 鎵€浠ラ伩鍏嶆绫绘帴鍙? 涓嬪悓.
     * @return DeviceList 瀵硅薄, 鍖呭惈鎵€鏈夊彲鐢ㄧ殑鐩告満璁惧鍚嶇О. 娉ㄦ剰, 鐩墠鍙湁鍚嶇О.
     */
    DeviceList findDeviceNames();

    /**
     * @brief 璁剧疆鏈熸湜鐨勭浉鏈哄垎杈ㄧ巼. 鐩告満涓嶄竴瀹氭敮鎸佽繖涓墿鐞嗗垎杈ㄧ巼, 鍙槸浼氬敖鍙兘鎵句竴涓窡杩欎釜鍒嗚鲸鐜囨帴杩戠殑.
     * @note 涓嶈缃殑璇濅細闅忎究缁欎竴涓€?濡傛灉瑕佽幏鍙栫湡瀹炵殑鍒嗚鲸鐜? 璇峰湪鑾峰緱甯ф暟鎹箣鍚庯紝鐩存帴浠庡抚鏁版嵁涓幏鍙?
     * @attention 璇峰湪 `open` 涔嬪墠璁剧疆杩欎釜鍙傛暟. `open` 涔嬪悗璁剧疆杩欎釜鍙傛暟鏃犳晥.
     */
    void setFrameSize(int width, int height);

    /**
     * @brief 璁剧疆鏈熸湜鐨勭浉鏈哄抚鐜? 鐩告満涓嶄竴瀹氭敮鎸佽繖涓墿鐞嗗抚鐜? 鍙槸浼氬敖鍙兘鎵句竴涓窡杩欎釜甯х巼鎺ヨ繎鐨?
     * @note 涓嶈缃殑璇濅細闅忎究缁欎竴涓? 涓€鑸槸 30.
     * @attention 璇峰湪 `open` 涔嬪墠璁剧疆杩欎釜鍙傛暟. `open` 涔嬪悗璁剧疆杩欎釜鍙傛暟鏃犳晥.
     */
    void setFrameRate(double fps);

    /**
     * @brief 閫氳繃鐩告満璁惧鍚嶇О鎵撳紑鐩告満璁惧
     *
     * @param deviceName 鐩告満璁惧鍚嶇О, 鍙互閫氳繃 `findDeviceNames` 鑾峰彇.
     *      浼犲叆绌哄瓧绗︿覆琛ㄧず鑷姩閫夋嫨.
     * @param autoStart open 鎴愬姛鍚庢槸鍚﹁嚜鍔ㄨ皟鐢?start() 鏂规硶寮€濮嬫崟鑾峰浘鍍?
     * @note 娉ㄦ剰, 鑷姩閫夋嫨鐨勮澶囦笉涓€瀹氭槸 `findDeviceNames` 涓殑绗竴涓澶?
     * @return 濡傛灉璁惧鎵撳紑鎴愬姛, 杩斿洖 true, 鍚﹀垯杩斿洖 false.
     */
    bool open(const char* deviceName, bool autoStart = true);

    /**
     * @brief 閫氳繃鐩告満璁惧绱㈠紩鎵撳紑鐩告満璁惧
     *
     * @param deviceIndex 鐩告満璁惧绱㈠紩, 鍙互閫氳繃 `findDeviceNames` 鑾峰彇.
     *      浼犲叆 -1 琛ㄧず鑷姩閫夋嫨, 浼犲叆鑷劧鏁拌〃绀?`findDeviceNames` 鎵惧埌鐨勫搴旇澶?
     *      濡傛灉浼犲叆鐨勬暟瓒呰繃浜嗚澶囨暟閲? 浼氶€夋嫨鏈€鍚庝竴涓澶?
     * @param autoStart open 鎴愬姛鍚庢槸鍚﹁嚜鍔ㄨ皟鐢?start() 鏂规硶寮€濮嬫崟鑾峰浘鍍?
     * @note 娉ㄦ剰, 鑷姩閫夋嫨鐨勮澶囦笉涓€瀹氭槸 `findDeviceNames` 涓殑绗竴涓澶?
     * 濡傛灉浼犲叆鐨勬暟瓒呰繃浜嗚澶囨暟閲? 浼氶€夋嫨鏈€鍚庝竴涓澶?
     * @return 濡傛灉璁惧鎵撳紑鎴愬姛, 杩斿洖 true, 鍚﹀垯杩斿洖 false.
     */
    bool open(int deviceIndex, bool autoStart = true);

    /**
     * @brief 鍏抽棴鐩告満璁惧, 閲婃斁璧勬簮.
     * @note 璋冪敤杩欎釜鏂规硶鍚? CameraCapture 瀵硅薄涓嶅簲璇ュ啀琚娇鐢?
     *       杩欎釜鏂规硶浼氬湪 CameraCapture 瀵硅薄琚攢姣佹椂鑷姩璋冪敤.
     */
    void close();

    /**
     * @brief 妫€鏌ョ浉鏈鸿澶囨槸鍚﹀凡缁忔墦寮€.
     * @return 濡傛灉鐩告満璁惧宸茬粡鎵撳紑, 杩斿洖 true, 鍚﹀垯杩斿洖 false.
     */
    bool isOpened() const;

    /**
     * @brief 鍚姩鐩告満, 寮€濮嬫崟鑾峰浘鍍?
     * @return 濡傛灉鍚姩鎴愬姛, 杩斿洖 true, 鍚﹀垯杩斿洖 false.
     */
    bool start();

    /**
     * @brief 妫€鏌ョ浉鏈烘槸鍚︽鍦ㄦ崟鑾峰浘鍍?
     * @return 濡傛灉鐩告満姝ｅ湪鎹曡幏鍥惧儚, 杩斿洖 true, 鍚﹀垯杩斿洖 false.
     */
    bool isStarted() const;

    /**
     * @brief 鍋滄鐩告満鎹曡幏鍥惧儚.
     * @note 鍙互鍦ㄨ皟鐢?start() 鍚庤皟鐢ㄦ鏂规硶鏉ュ仠姝㈡崟鑾峰浘鍍?
     */
    void stop();

    /**
     * @brief 鑾峰彇褰撳墠甯ф暟鎹?
     * @param timeout 瓒呮椂绛夊緟鏃堕棿, 鍗曚綅姣. 0 琛ㄧず涓嶇瓑寰? 鐩存帴杩斿洖.
     *
     * @return 褰撳墠甯ф暟鎹殑鎸囬拡. 濡傛灉绛夊緟瓒呮椂鎴栬€呮病鏈夋暟鎹? 杩斿洖 nullptr.
     *      娉ㄦ剰, 浣跨敤鑰呭湪涓嶄娇鐢ㄧ殑鏃跺€欓渶瑕佽皟鐢?CameraFrame::release() 鏉ラ噴鏀?
     *      鍚﹀垯浼氶€犳垚鍐呭瓨娉勬紡.
     */
    CameraFrame* grabFrame(unsigned int timeoutInMs = 0);

    ////////////////////////////////////////////////

    // 鍐呴儴瀹炵幇, 浣跨敤鑰呮棤闇€鍏冲績.
    FrameContainer* getFrameContainer() const;

private:
    ccap::Provider* m_provider;
    FrameContainer* m_frameContainer;
};

/// @brief  logLevel 0: 涓嶈緭鍑烘棩蹇? 1: 杈撳嚭璀﹀憡鏃ュ織, 2: 杈撳嚭甯歌淇℃伅, 3: 杈撳嚭璋冭瘯淇℃伅, 瓒呰繃 3 绛夊悓浜?3.
void enableCameraModuleLog(unsigned int logLevel);

} // namespace ege

// TODO: 鐪嬬湅鍚庨潰瑕佷笉瑕佸鍔犱竴濂?C 璇█鐨勬帴鍙?

#endif
