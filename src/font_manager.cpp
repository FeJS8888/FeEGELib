#include "font_Manager.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <algorithm>
#include <fstream>
#include <filesystem>

// 构造函数
FontManager::FontManager() : m_hFontResource(nullptr), m_isLoaded(false) {}

// 析构函数
FontManager::~FontManager() {
    UnloadFont();
}

// 移动构造函数
FontManager::FontManager(FontManager&& other) noexcept 
    : m_hFontResource(other.m_hFontResource)
    , m_fontName(std::move(other.m_fontName))
    , m_ttfFileName(std::move(other.m_ttfFileName))
    , m_fontData(std::move(other.m_fontData))
    , m_isLoaded(other.m_isLoaded) {
    
    other.m_hFontResource = nullptr;
    other.m_isLoaded = false;
}

// 移动赋值操作符
FontManager& FontManager::operator=(FontManager&& other) noexcept {
    if (this != &other) {
        UnloadFont();
        
        m_hFontResource = other.m_hFontResource;
        m_fontName = std::move(other.m_fontName);
        m_ttfFileName = std::move(other.m_ttfFileName);
        m_fontData = std::move(other.m_fontData);
        m_isLoaded = other.m_isLoaded;
        
        other.m_hFontResource = nullptr;
        other.m_isLoaded = false;
    }
    return *this;
}

// 查找TTF文件
std::wstring FontManager::FindTTFFile(const std::string& fileName) const {
    std::wstring wFileName(fileName.begin(), fileName.end());
    
    std::vector<std::wstring> searchPaths = {
        wFileName,
        L"fonts/" + wFileName,
        L"./fonts/" + wFileName,
        L"../fonts/" + wFileName,
        L"resources/fonts/" + wFileName,
        L"assets/fonts/" + wFileName,
    };
    
    // 检查环境变量FONT_PATH
    wchar_t* envPath = nullptr;
    size_t envLen = 0;
    if (_wdupenv_s(&envPath, &envLen, L"FONT_PATH") == 0 && envPath) {
        std::filesystem::path envFontPath = envPath;
        searchPaths.insert(searchPaths.begin(), envFontPath / wFileName);
        free(envPath);
    }
    
    // 检查每个路径
    for (const auto& path : searchPaths) {
        try {
            if (std::filesystem::exists(path)) {
                std::wcout << L"Found TTF file: " << path << std::endl;
                return std::filesystem::absolute(path);
            }
        } catch (...) {
            continue;
        }
    }
    
    // 检查系统字体目录
    wchar_t windir[MAX_PATH];
    if (GetEnvironmentVariableW(L"WINDIR", windir, MAX_PATH) > 0) {
        std::wstring systemFont = windir;
        systemFont += L"\\Fonts\\" + wFileName;
        if (std::filesystem::exists(systemFont)) {
            std::wcout << L"Found system font: " << systemFont << std::endl;
            return systemFont;
        }
    }
    
    return L"";
}

// 从文件加载TTF
bool FontManager::LoadTTFFromFile(const std::wstring& filePath) {
    std::ifstream file(std::filesystem::path(filePath), std::ios::binary);
    if (!file) {
        std::wcout << L"Cannot open TTF file: " << filePath << std::endl;
        return false;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize == 0) {
        std::wcout << L"TTF file is empty: " << filePath << std::endl;
        return false;
    }
    
    m_fontData.resize(fileSize);
    file.read(reinterpret_cast<char*>(m_fontData.data()), fileSize);
    file.close();
    
    if (file.gcount() != static_cast<std::streamsize>(fileSize)) {
        std::wcout << L"Failed to read complete TTF file" << std::endl;
        m_fontData.clear();
        return false;
    }
    
    std::wcout << L"TTF loaded: " << fileSize << L" bytes from " << filePath << std::endl;
    return true;
}

// 在font_Manager.cpp中实现TTF字体族名称提取
#pragma pack(push, 1)
struct TTFHeader {
    uint32_t version;
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
};

struct TTFTableEntry {
    char tag[4];
    uint32_t checksum;
    uint32_t offset;
    uint32_t length;
};

struct NameHeader {
    uint16_t format;
    uint16_t count;
    uint16_t stringOffset;
};

struct NameRecord {
    uint16_t platformID;
    uint16_t encodingID;
    uint16_t languageID;
    uint16_t nameID;
    uint16_t length;
    uint16_t offset;
};
#pragma pack(pop)

// 字节序转换辅助函数
uint16_t SwapBytes16(uint16_t value) {
    return (value << 8) | (value >> 8);
}

uint32_t SwapBytes32(uint32_t value) {
    return ((value << 24) & 0xFF000000) |
           ((value << 8)  & 0x00FF0000) |
           ((value >> 8)  & 0x0000FF00) |
           ((value >> 24) & 0x000000FF);
}

std::wstring FontManager::ExtractFontFamilyName(const std::vector<BYTE>& fontData) const {
    if (fontData.size() < sizeof(TTFHeader)) {
        return L"";
    }
    
    const BYTE* data = fontData.data();
    const TTFHeader* header = reinterpret_cast<const TTFHeader*>(data);
    
    uint16_t numTables = SwapBytes16(header->numTables);
    
    // 查找name表
    uint32_t nameTableOffset = 0;
    uint32_t nameTableLength = 0;
    
    const TTFTableEntry* tables = reinterpret_cast<const TTFTableEntry*>(data + sizeof(TTFHeader));
    
    for (int i = 0; i < numTables; i++) {
        if (memcmp(tables[i].tag, "name", 4) == 0) {
            nameTableOffset = SwapBytes32(tables[i].offset);
            nameTableLength = SwapBytes32(tables[i].length);
            break;
        }
    }
    
    if (nameTableOffset == 0 || nameTableOffset + nameTableLength > fontData.size()) {
        return L"";
    }
    
    // 解析name表
    const NameHeader* nameHeader = reinterpret_cast<const NameHeader*>(data + nameTableOffset);
    uint16_t count = SwapBytes16(nameHeader->count);
    uint16_t stringOffset = SwapBytes16(nameHeader->stringOffset);
    
    const NameRecord* records = reinterpret_cast<const NameRecord*>(data + nameTableOffset + sizeof(NameHeader));
    
    // 查找字体族名称 (nameID = 1)
    // 优先查找Windows平台的Unicode编码
    std::wstring fontName;
    
    for (int i = 0; i < count; i++) {
        uint16_t platformID = SwapBytes16(records[i].platformID);
        uint16_t encodingID = SwapBytes16(records[i].encodingID);
        uint16_t languageID = SwapBytes16(records[i].languageID);
        uint16_t nameID = SwapBytes16(records[i].nameID);
        uint16_t length = SwapBytes16(records[i].length);
        uint16_t offset = SwapBytes16(records[i].offset);
        
        if (nameID == 1) { // 字体族名称
            uint32_t stringPos = nameTableOffset + stringOffset + offset;
            
            if (stringPos + length <= fontData.size()) {
                if (platformID == 3) { // Microsoft平台
                    // UTF-16 BE 编码
                    std::wstring name;
                    const uint16_t* utf16Data = reinterpret_cast<const uint16_t*>(data + stringPos);
                    
                    for (int j = 0; j < length / 2; j++) {
                        wchar_t ch = SwapBytes16(utf16Data[j]);
                        if (ch == 0) break;
                        name += ch;
                    }
                    
                    if (!name.empty()) {
                        fontName = name;
                        break; // 优先使用Microsoft平台的名称
                    }
                } else if (platformID == 1 && fontName.empty()) { // Macintosh平台（备用）
                    // ASCII编码
                    std::string asciiName(reinterpret_cast<const char*>(data + stringPos), length);
                    fontName = std::wstring(asciiName.begin(), asciiName.end());
                }
            }
        }
    }
    
    return fontName;
}

// 修改LoadFontFromMemory方法，在成功加载后提取字体名称
bool FontManager::LoadFontFromMemory() {
    if (m_fontData.empty()) return false;
    
    DWORD dwFonts = 0;
    m_hFontResource = AddFontMemResourceEx(
        m_fontData.data(),
        static_cast<DWORD>(m_fontData.size()),
        0,
        &dwFonts
    );
    
    if (m_hFontResource && dwFonts > 0) {
        // 从TTF文件中提取真实的字体族名称
        std::wstring realFontName = ExtractFontFamilyName(m_fontData);
        if (!realFontName.empty()) {
            m_fontName = realFontName;
            std::wcout << L"Extracted font family name: " << m_fontName << std::endl;
        } else {
            std::wcout << L"Failed to extract font name, using filename" << std::endl;
        }
        
        std::wcout << L"Font loaded to memory (" << dwFonts << L" fonts)" << std::endl;
        return true;
    } else {
        DWORD error = GetLastError();
        std::wcout << L"Failed to load font to memory. Error: " << error << std::endl;
        return false;
    }
}

// 主要接口：加载字体
bool FontManager::LoadFont(const std::string& ttfFileName) {
    if (m_isLoaded) {
        UnloadFont();
    }
    
    // 查找TTF文件
    std::wstring filePath = FindTTFFile(ttfFileName);
    if (filePath.empty()) {
        std::cout << "TTF file not found: " << ttfFileName << std::endl;
        return false;
    }
    
    // 加载到内存
    if (!LoadTTFFromFile(filePath)) {
        return false;
    }
    
    // 加载到系统
    if (!LoadFontFromMemory()) {
        m_fontData.clear();
        return false;
    }
    
    // 设置字体名称（去掉.ttf扩展名）
    m_ttfFileName = std::wstring(ttfFileName.begin(), ttfFileName.end());
    m_isLoaded = true;
    return true;
}

// 基础字体创建
LOGFONTW FontManager::CreateLogFont(int height, int weight, bool italic, bool underline) const {
    LOGFONTW logFont = {};
    std::cout<<"IS "<<height<<"\n";
    wcscpy_s(logFont.lfFaceName, LF_FACESIZE, m_fontName.c_str());    
    logFont.lfHeight = height;
    logFont.lfWidth = 0;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = weight;
    logFont.lfItalic = italic ? TRUE : FALSE;
    logFont.lfUnderline = underline ? TRUE : FALSE;
    logFont.lfStrikeOut = FALSE;
    logFont.lfCharSet = DEFAULT_CHARSET;
    logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont.lfQuality = CLEARTYPE_QUALITY;
    logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    
    return logFont;
}

// 预设尺寸样式实现
LOGFONTW FontManager::CreateTinyFont() const { return CreateLogFont(8); }
LOGFONTW FontManager::CreateSmallFont() const { return CreateLogFont(10); }
LOGFONTW FontManager::CreateNormalFont() const { return CreateLogFont(12); }
LOGFONTW FontManager::CreateMediumFont() const { return CreateLogFont(14); }
LOGFONTW FontManager::CreateLargeFont() const { return CreateLogFont(16); }
LOGFONTW FontManager::CreateXLargeFont() const { return CreateLogFont(18); }
LOGFONTW FontManager::CreateXXLargeFont() const { return CreateLogFont(20); }
LOGFONTW FontManager::CreateHugeFont() const { return CreateLogFont(24); }
LOGFONTW FontManager::CreateGiantFont() const { return CreateLogFont(32); }

// 字重样式实现
LOGFONTW FontManager::CreateThinFont() const { return CreateLogFont(16, FW_THIN); }
LOGFONTW FontManager::CreateLightFont() const { return CreateLogFont(16, FW_LIGHT); }
LOGFONTW FontManager::CreateSemiBoldFont() const { return CreateLogFont(16, FW_SEMIBOLD); }
LOGFONTW FontManager::CreateBoldFont() const { return CreateLogFont(16, FW_BOLD); }
LOGFONTW FontManager::CreateExtraBoldFont() const { return CreateLogFont(16, FW_EXTRABOLD); }
LOGFONTW FontManager::CreateBlackFont() const { return CreateLogFont(16, FW_BLACK); }

// 样式变体实现
LOGFONTW FontManager::CreateItalicFont() const { return CreateLogFont(16, FW_NORMAL, true); }
LOGFONTW FontManager::CreateBoldItalicFont() const { return CreateLogFont(16, FW_BOLD, true); }
LOGFONTW FontManager::CreateUnderlineFont() const { return CreateLogFont(16, FW_NORMAL, false, true); }

// 字体句柄创建
HFONT FontManager::CreateFontFromLogFont(const LOGFONTW& logFont) const {
    return CreateFontIndirectW(&logFont);
}

HFONT FontManager::CreateFontHandle(int height, int weight, bool italic, bool underline) const {
    LOGFONTW logFont = CreateLogFont(height, weight, italic, underline);
    return CreateFontIndirectW(&logFont);
}

// 状态查询
bool FontManager::IsLoaded() const { 
    return m_isLoaded; 
}

const std::wstring& FontManager::GetFontName() const { 
    return m_fontName; 
}

const std::string FontManager::GetTTFFileName() const { 
    return std::string(m_ttfFileName.begin(), m_ttfFileName.end()); 
}

// 工具方法
void FontManager::PrintFontInfo(const LOGFONTW& logFont) const {
    std::wcout << L"=== LOGFONTW Information ===" << std::endl;
    std::wcout << L"Font Name: " << logFont.lfFaceName << std::endl;
    std::wcout << L"Height: " << logFont.lfHeight << std::endl;
    std::wcout << L"Weight: " << logFont.lfWeight;
    
    // 字重说明
    if (logFont.lfWeight <= FW_THIN) std::wcout << L" (Thin)";
    else if (logFont.lfWeight <= FW_LIGHT) std::wcout << L" (Light)";
    else if (logFont.lfWeight <= FW_NORMAL) std::wcout << L" (Normal)";
    else if (logFont.lfWeight <= FW_SEMIBOLD) std::wcout << L" (Semibold)";
    else if (logFont.lfWeight <= FW_BOLD) std::wcout << L" (Bold)";
    else if (logFont.lfWeight <= FW_EXTRABOLD) std::wcout << L" (Extra Bold)";
    else std::wcout << L" (Black)";
    
    std::wcout << std::endl;
    std::wcout << L"Italic: " << (logFont.lfItalic ? L"Yes" : L"No") << std::endl;
    std::wcout << L"Underline: " << (logFont.lfUnderline ? L"Yes" : L"No") << std::endl;
    std::wcout << L"=========================" << std::endl;
}

bool FontManager::TestFont() const {
    if (!m_isLoaded) {
        std::wcout << L"Font not loaded" << std::endl;
        return false;
    }
    
    HFONT hTestFont = CreateFontHandle(20);
    if (hTestFont) {
        std::wcout << L"Font handle created successfully" << std::endl;
        DeleteObject(hTestFont);
        return true;
    } else {
        std::wcout << L"Failed to create font handle" << std::endl;
        return false;
    }
}

bool FontManager::GetFontMetrics(HDC hdc, TEXTMETRICW& tm) const {
    if (!m_isLoaded) return false;
    
    HFONT hFont = CreateFontHandle(20);
    if (!hFont) return false;
    
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    BOOL result = GetTextMetricsW(hdc, &tm);
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    
    return result == TRUE;
}

void FontManager::PrintFontMetrics(HDC hdc) const {
    TEXTMETRICW tm;
    if (GetFontMetrics(hdc, tm)) {
        std::wcout << L"=== Font Metrics ===" << std::endl;
        std::wcout << L"Height: " << tm.tmHeight << std::endl;
        std::wcout << L"Ascent: " << tm.tmAscent << std::endl;
        std::wcout << L"Descent: " << tm.tmDescent << std::endl;
        std::wcout << L"Ave Char Width: " << tm.tmAveCharWidth << std::endl;
        std::wcout << L"Max Char Width: " << tm.tmMaxCharWidth << std::endl;
        std::wcout << L"Weight: " << tm.tmWeight << std::endl;
        std::wcout << L"===================" << std::endl;
    } else {
        std::wcout << L"Failed to get font metrics" << std::endl;
    }
}

// 私有方法：卸载字体
void FontManager::UnloadFont() {
    if (m_hFontResource) {
        RemoveFontMemResourceEx(m_hFontResource);
        m_hFontResource = nullptr;
    }
    m_isLoaded = false;
    m_fontName.clear();
    m_ttfFileName.clear();
    m_fontData.clear();
}