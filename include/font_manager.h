/**
 * @file font_manager.h
 * @brief 字体管理器类定义
 * @author FeJS8888
 * @version 2.10.0.0
 * @date 2025-10-26
 * 
 * 本文件定义了FontManager类，用于加载和管理TTF字体文件
 * 提供了多种预设字体尺寸和字重样式的创建方法
 */

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <windows.h>
#include <string>
#include <vector>

/**
 * @class FontManager
 * @brief 字体管理器类，用于加载和管理TTF字体
 * @details 支持从文件加载TTF字体，并提供多种字体样式的创建方法
 *          包括不同尺寸、字重和样式(斜体、下划线等)的字体
 */
class FontManager {
private:
    HANDLE m_hFontResource;        ///< 字体资源句柄
    std::wstring m_fontName;       ///< 字体名称
    std::wstring m_ttfFileName;    ///< TTF文件名
    std::vector<BYTE> m_fontData;  ///< 字体数据
    bool m_isLoaded;               ///< 字体是否已加载
    
    /**
     * @brief 查找TTF字体文件
     * @param fileName 文件名
     * @return 文件完整路径
     */
    std::wstring FindTTFFile(const std::string& fileName) const;
    
    /**
     * @brief 从文件加载TTF字体
     * @param filePath 文件路径
     * @return 是否成功
     */
    bool LoadTTFFromFile(const std::wstring& filePath);
    
    /**
     * @brief 提取字体家族名称
     * @param fontData 字体数据
     * @return 字体家族名称
     */
    std::wstring ExtractFontFamilyName(const std::vector<BYTE>& fontData) const;
    
    /**
     * @brief 从内存加载字体
     * @return 是否成功
     */
    bool LoadFontFromMemory();
    
    /**
     * @brief 卸载字体
     */
    void UnloadFont();
    
public:
    /**
     * @brief 默认构造函数
     */
    FontManager();
    
    /**
     * @brief 析构函数
     */
    ~FontManager();
    
    // 禁用拷贝，允许移动
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    /**
     * @brief 移动构造函数
     * @param other 源对象
     */
    FontManager(FontManager&& other) noexcept;
    
    /**
     * @brief 移动赋值运算符
     * @param other 源对象
     * @return 当前对象引用
     */
    FontManager& operator=(FontManager&& other) noexcept;
    
    /**
     * @brief 加载指定的TTF字体文件
     * @param ttfFileName TTF文件名
     * @return 是否加载成功
     */
    bool LoadFont(const std::string& ttfFileName);
    
    /**
     * @brief 创建逻辑字体结构
     * @param height 字体高度(默认16)
     * @param weight 字重(默认FW_NORMAL)
     * @param italic 是否斜体(默认false)
     * @param underline 是否下划线(默认false)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateLogFont(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    // 预设尺寸字体创建方法
    /**
     * @brief 创建极小字体(8px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateTinyFont() const;
    
    /**
     * @brief 创建小字体(10px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateSmallFont() const;
    
    /**
     * @brief 创建普通字体(12px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateNormalFont() const;
    
    /**
     * @brief 创建中等字体(14px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateMediumFont() const;
    
    /**
     * @brief 创建大字体(16px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateLargeFont() const;
    
    /**
     * @brief 创建超大字体(18px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateXLargeFont() const;
    
    /**
     * @brief 创建特大字体(20px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateXXLargeFont() const;
    
    /**
     * @brief 创建巨大字体(24px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateHugeFont() const;
    
    /**
     * @brief 创建超级字体(32px)
     * @return LOGFONTW结构
     */
    LOGFONTW CreateGiantFont() const;
    
    // 字重变体（使用默认尺寸16px）
    /**
     * @brief 创建细体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateThinFont() const;
    
    /**
     * @brief 创建轻体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateLightFont() const;
    
    /**
     * @brief 创建半粗体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateSemiBoldFont() const;
    
    /**
     * @brief 创建粗体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateBoldFont() const;
    
    /**
     * @brief 创建特粗体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateExtraBoldFont() const;
    
    /**
     * @brief 创建黑体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateBlackFont() const;
    
    // 样式变体
    /**
     * @brief 创建斜体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateItalicFont() const;
    
    /**
     * @brief 创建粗斜体字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateBoldItalicFont() const;
    
    /**
     * @brief 创建下划线字体
     * @return LOGFONTW结构
     */
    LOGFONTW CreateUnderlineFont() const;
    
    /**
     * @brief 从LOGFONT结构创建字体句柄
     * @param logFont LOGFONT结构
     * @return 字体句柄
     */
    HFONT CreateFontFromLogFont(const LOGFONTW& logFont) const;
    
    /**
     * @brief 创建字体句柄
     * @param height 字体高度(默认16)
     * @param weight 字重(默认FW_NORMAL)
     * @param italic 是否斜体(默认false)
     * @param underline 是否下划线(默认false)
     * @return 字体句柄
     */
    HFONT CreateFontHandle(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    /**
     * @brief 打印字体信息
     * @param logFont LOGFONT结构
     */
    void PrintFontInfo(const LOGFONTW& logFont) const;
    
    /**
     * @brief 测试字体
     * @return 是否测试成功
     */
    bool TestFont() const;
    
    /**
     * @brief 获取字体度量信息
     * @param hdc 设备上下文句柄
     * @param tm 输出的文本度量结构
     * @return 是否成功
     */
    bool GetFontMetrics(HDC hdc, TEXTMETRICW& tm) const;
    
    /**
     * @brief 打印字体度量信息
     * @param hdc 设备上下文句柄
     */
    void PrintFontMetrics(HDC hdc) const;
    
    /**
     * @brief 检查字体是否已加载
     * @return 是否已加载
     */
    bool IsLoaded() const;
    
    /**
     * @brief 获取字体名称
     * @return 字体名称
     */
    const std::wstring& GetFontName() const;
    
    /**
     * @brief 获取TTF文件名
     * @return TTF文件名
     */
    const std::string GetTTFFileName() const;
};

#endif // FONT_MANAGER_H