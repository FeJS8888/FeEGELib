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
    
    // ˽�и�������
    std::wstring FindTTFFile(const std::string& fileName) const;
    bool LoadTTFFromFile(const std::wstring& filePath);
    std::wstring ExtractFontFamilyName(const std::vector<BYTE>& fontData) const;
    bool LoadFontFromMemory();
    void UnloadFont();
    
public:
    FontManager();
    ~FontManager();
    
    // ���ÿ����������ƶ�
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&& other) noexcept;
    FontManager& operator=(FontManager&& other) noexcept;
    
    // ��Ҫ�ӿڣ�����ָ����TTF�ļ�
    bool LoadFont(const std::string& ttfFileName);
    
    // �������崴������
    LOGFONTW CreateLogFont(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    // Ԥ��ߴ�
    LOGFONTW CreateTinyFont() const;      // 8px
    LOGFONTW CreateSmallFont() const;     // 10px
    LOGFONTW CreateNormalFont() const;    // 12px
    LOGFONTW CreateMediumFont() const;    // 14px
    LOGFONTW CreateLargeFont() const;     // 16px
    LOGFONTW CreateXLargeFont() const;    // 18px
    LOGFONTW CreateXXLargeFont() const;   // 20px
    LOGFONTW CreateHugeFont() const;      // 24px
    LOGFONTW CreateGiantFont() const;     // 32px
    
    // ���ر��壨ʹ��Ĭ�ϳߴ�16px��
    LOGFONTW CreateThinFont() const;
    LOGFONTW CreateLightFont() const;
    LOGFONTW CreateSemiBoldFont() const;
    LOGFONTW CreateBoldFont() const;
    LOGFONTW CreateExtraBoldFont() const;
    LOGFONTW CreateBlackFont() const;
    
    // ��ʽ����
    LOGFONTW CreateItalicFont() const;
    LOGFONTW CreateBoldItalicFont() const;
    LOGFONTW CreateUnderlineFont() const;
    
    // ����������
    HFONT CreateFontFromLogFont(const LOGFONTW& logFont) const;
    HFONT CreateFontHandle(int height = 16, int weight = FW_NORMAL, bool italic = false, bool underline = false) const;
    
    // ���߷���
    void PrintFontInfo(const LOGFONTW& logFont) const;
    bool TestFont() const;
    bool GetFontMetrics(HDC hdc, TEXTMETRICW& tm) const;
    void PrintFontMetrics(HDC hdc) const;
    
    // ״̬��ѯ
    bool IsLoaded() const;
    const std::wstring& GetFontName() const;
    const std::string GetTTFFileName() const;
};

#endif // FONT_MANAGER_H