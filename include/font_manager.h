#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <windows.h>
#include <string>
#include <vector>

class FontManager {
private:
    HANDLE m_hFontResource;
    std::wstring m_fontName;
    std::wstring m_ttfFileName;
    std::vector<BYTE> m_fontData;
    bool m_isLoaded;
    
    // 私有辅助方法
    std::wstring FindTTFFile(const std::string& fileName) const;
    bool LoadTTFFromFile(const std::wstring& filePath);
    std::wstring ExtractFontFamilyName(const std::vector<BYTE>& fontData) const;
    bool LoadFontFromMemory();
    void UnloadFont();
    
public:
    FontManager();
    ~FontManager();
    
    // 禁用拷贝，允许移动
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&& other) noexcept;
    FontManager& operator=(FontManager&& other) noexcept;
    
    // 主要接口：加载指定的TTF文件
    bool LoadFont(const std::string& ttfFileName);
    
    // 基础字体创建方法
    LOGFONTW CreateLogFont(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    // 预设尺寸
    LOGFONTW CreateTinyFont() const;      // 8px
    LOGFONTW CreateSmallFont() const;     // 10px
    LOGFONTW CreateNormalFont() const;    // 12px
    LOGFONTW CreateMediumFont() const;    // 14px
    LOGFONTW CreateLargeFont() const;     // 16px
    LOGFONTW CreateXLargeFont() const;    // 18px
    LOGFONTW CreateXXLargeFont() const;   // 20px
    LOGFONTW CreateHugeFont() const;      // 24px
    LOGFONTW CreateGiantFont() const;     // 32px
    
    // 字重变体（使用默认尺寸16px）
    LOGFONTW CreateThinFont() const;
    LOGFONTW CreateLightFont() const;
    LOGFONTW CreateSemiBoldFont() const;
    LOGFONTW CreateBoldFont() const;
    LOGFONTW CreateExtraBoldFont() const;
    LOGFONTW CreateBlackFont() const;
    
    // 样式变体
    LOGFONTW CreateItalicFont() const;
    LOGFONTW CreateBoldItalicFont() const;
    LOGFONTW CreateUnderlineFont() const;
    
    // 字体句柄创建
    HFONT CreateFontFromLogFont(const LOGFONTW& logFont) const;
    HFONT CreateFontHandle(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    // 工具方法
    void PrintFontInfo(const LOGFONTW& logFont) const;
    bool TestFont() const;
    bool GetFontMetrics(HDC hdc, TEXTMETRICW& tm) const;
    void PrintFontMetrics(HDC hdc) const;
    
    // 状态查询
    bool IsLoaded() const;
    const std::wstring& GetFontName() const;
    const std::string GetTTFFileName() const;
};

#endif // FONT_MANAGER_H