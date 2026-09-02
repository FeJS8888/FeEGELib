[CmdletBinding()]
param(
    [int]$Width = 64,
    [int]$Height = 64,
    [int]$FontHeight = 30
)

$source = @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

public static class Win32MultilineEditProbe
{
    private const uint WS_CHILD = 0x40000000;
    private const uint WS_POPUP = 0x80000000;
    private const uint WS_BORDER = 0x00800000;
    private const uint WS_EX_CLIENTEDGE = 0x00000200;
    private const uint ES_MULTILINE = 0x0004;
    private const uint ES_AUTOVSCROLL = 0x0040;
    private const uint ES_AUTOHSCROLL = 0x0080;
    private const uint WM_SETFONT = 0x0030;
    private const uint WM_GETTEXT = 0x000D;
    private const uint WM_GETTEXTLENGTH = 0x000E;
    private const uint WM_CHAR = 0x0102;
    private const uint EM_SETSEL = 0x00B1;
    private const uint EM_LIMITTEXT = 0x00C5;
    private const uint EM_GETLIMITTEXT = 0x00D5;

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern IntPtr CreateWindowExW(uint exStyle, string className,
        string windowName, uint style, int x, int y, int width, int height,
        IntPtr parent, IntPtr menu, IntPtr instance, IntPtr parameter);

    [DllImport("user32.dll")]
    private static extern bool DestroyWindow(IntPtr window);

    [DllImport("user32.dll")]
    private static extern IntPtr SendMessageW(IntPtr window, uint message,
        IntPtr wParam, IntPtr lParam);

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern IntPtr SendMessageW(IntPtr window, uint message,
        IntPtr wParam, StringBuilder lParam);

    [DllImport("gdi32.dll", CharSet = CharSet.Unicode)]
    private static extern IntPtr CreateFontW(int height, int width, int escapement,
        int orientation, int weight, uint italic, uint underline, uint strikeOut,
        uint charSet, uint outputPrecision, uint clipPrecision, uint quality,
        uint pitchAndFamily, string faceName);

    [DllImport("gdi32.dll")]
    private static extern bool DeleteObject(IntPtr handle);

    public sealed class Result
    {
        public string Name;
        public int BeforeLength;
        public int AfterLength;
        public long Limit;
        public string Text;
        public bool Accepted;
    }

    public static Result Run(string name, string initialText, bool autoVScroll,
        int width, int height, int fontHeight)
    {
        IntPtr parent = CreateWindowExW(0, "STATIC", "", WS_POPUP,
            0, 0, 320, 240, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
        uint style = WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL;
        if (autoVScroll) style |= ES_AUTOVSCROLL;

        IntPtr edit = CreateWindowExW(WS_EX_CLIENTEDGE, "EDIT", initialText,
            style, 0, 0, width, height, parent, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
        IntPtr font = CreateFontW(fontHeight, 0, 0, 0, 0, 0, 0, 0,
            1, 0, 0, 0, 0, "SimSun");

        try
        {
            SendMessageW(edit, WM_SETFONT, font, IntPtr.Zero);
            SendMessageW(edit, EM_LIMITTEXT, new IntPtr(-1), IntPtr.Zero);
            int before = SendMessageW(edit, WM_GETTEXTLENGTH,
                IntPtr.Zero, IntPtr.Zero).ToInt32();
            SendMessageW(edit, EM_SETSEL, new IntPtr(1), new IntPtr(1));
            SendMessageW(edit, WM_CHAR, new IntPtr((int)'X'), IntPtr.Zero);
            int after = SendMessageW(edit, WM_GETTEXTLENGTH,
                IntPtr.Zero, IntPtr.Zero).ToInt32();
            var buffer = new StringBuilder(after + 1);
            SendMessageW(edit, WM_GETTEXT, new IntPtr(buffer.Capacity), buffer);

            return new Result {
                Name = name,
                BeforeLength = before,
                AfterLength = after,
                Limit = SendMessageW(edit, EM_GETLIMITTEXT,
                    IntPtr.Zero, IntPtr.Zero).ToInt64(),
                Text = buffer.ToString().Replace("\r", "<CR>").Replace("\n", "<LF>"),
                Accepted = after == before + 1
            };
        }
        finally
        {
            DestroyWindow(edit);
            DestroyWindow(parent);
            if (font != IntPtr.Zero) DeleteObject(font);
        }
    }
}
'@

Add-Type -TypeDefinition $source

$manyLines = [string]::Join("`r`n", 1..10)
$cases = @(
    [Win32MultilineEditProbe]::Run('one-line/no-auto-vscroll', 'abc', $false,
        $Width, $Height, $FontHeight),
    [Win32MultilineEditProbe]::Run('many-lines/no-auto-vscroll', $manyLines, $false,
        $Width, $Height, $FontHeight),
    [Win32MultilineEditProbe]::Run('many-lines/auto-vscroll', $manyLines, $true,
        $Width, $Height, $FontHeight)
)

$cases | Select-Object Name, BeforeLength, AfterLength, Limit, Accepted, Text |
    Format-Table -AutoSize

$passed = $cases[0].Accepted -and -not $cases[1].Accepted -and $cases[2].Accepted
if (-not $passed) {
    Write-Error 'Observed behavior differs from the expected Win32 EDIT matrix.'
    exit 1
}

Write-Output 'Probe passed: ES_AUTOVSCROLL restores editing after hard lines overflow the formatting rectangle.'
