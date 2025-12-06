#include<hgl/platform/Platform.h>
#include<hgl/text/FontInfo.h>
#include<hgl/type/StrChar.h>

namespace hgl
{
    namespace
    {
        BOOL CALLBACK EnumFontFamCallBack(CONST LOGFONTW *lplf, CONST TEXTMETRICW *lpntm, DWORD FontType, LPARAM lpUser)
        { 
            FontInfoList *fi_list=(FontInfoList *)lpUser;
 
            FontInfo fi;

            hgl::strcpy(fi.name,HGL_FONT_NAME_MAX_LENGTH,lplf->lfFaceName);
 
            if (FontType & RASTER_FONTTYPE  ) fi.type=FontType::Raster;else
            if (FontType & TRUETYPE_FONTTYPE) fi.type=FontType::TrueType;else
                                              fi.type=FontType::Vector;
 
            fi.charset             =lpntm->tmCharSet;
            fi.height              =lpntm->tmHeight;
            fi.ascent              =lpntm->tmAscent;
            fi.descent             =lpntm->tmDescent;           
            fi.internal_leading    =lpntm->tmInternalLeading;
            fi.external_leading    =lpntm->tmExternalLeading;
            fi.ave_char_width      =lpntm->tmAveCharWidth;
            fi.max_char_width      =lpntm->tmMaxCharWidth;
            fi.weight              =lpntm->tmWeight;
            fi.overhang            =lpntm->tmOverhang;
            fi.digitized_aspect_x  =lpntm->tmDigitizedAspectX;
            fi.digitized_aspect_y  =lpntm->tmDigitizedAspectY;
            fi.first_char          =lpntm->tmFirstChar;
            fi.last_char           =lpntm->tmLastChar;
            fi.default_char        =lpntm->tmDefaultChar;
            fi.break_char          =lpntm->tmBreakChar;
            fi.italic              =lpntm->tmItalic;
            fi.underlined          =lpntm->tmUnderlined;
            fi.struck_out          =lpntm->tmStruckOut;
            fi.pitch_and_family    =lpntm->tmPitchAndFamily;

            fi_list->Add(fi);
            return(TRUE);
        }
    }

    void EnumOSFonts(FontInfoList *fi_list)
    {
        if(!fi_list)return;

        LOGFONTW lf;

        mem_zero(lf);

        lf.lfCharSet=DEFAULT_CHARSET;

        HDC dc=GetDC(NULL);

        int result=EnumFontFamiliesExW(dc,&lf,EnumFontFamCallBack,(LPARAM)fi_list,0);

        ReleaseDC(NULL,dc);
    }
}//namespace hgl
