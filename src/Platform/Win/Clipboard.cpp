#include<hgl/type/TypeCore.h>
#include<windows.h>
#include<shobjidl.h>

namespace hgl::os
{
    void CopyTextToClipboard(const wchar_t *str)
    {
        if(!str||!(*str))return;

        if (!OpenClipboard(nullptr))return;

        EmptyClipboard();

        HGLOBAL clipbuffer;
        wchar_t * buffer;

        clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(str)+1);
        buffer = (wchar_t *)GlobalLock(clipbuffer);

        wcscpy(buffer, str);

        GlobalUnlock(clipbuffer);
        SetClipboardData(CF_UNICODETEXT, clipbuffer);
        CloseClipboard();
    }

    const wchar_t *GetTextFromClipboard()
    {
        if (!OpenClipboard(nullptr))
            return 0;

        wchar_t * buffer = 0;

        HANDLE hData = GetClipboardData( CF_UNICODETEXT );
        buffer = (wchar_t *)GlobalLock( hData );
        GlobalUnlock( hData );
        CloseClipboard();
        return buffer;
    }
}//namespace hgl::os
