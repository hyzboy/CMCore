#include<hgl/type/UnicodeBlocks.h>

namespace hgl
{
    namespace
    {
        struct UnicodeBlockRange
        {
            UnicodeBlock type;
            uint32 begin;
            uint32 end;
            char name[64];
        };//struct UnicodeBlock

        #define UNICODE_BLOCK_DEFINE(begin,end,intro)   {UnicodeBlock::intro,0x##begin,0x##end,#intro}

        constexpr UnicodeBlockRange UnicodeBlockList[(size_t)UnicodeBlock::RANGE_SIZE]=
        {
            UNICODE_BLOCK_DEFINE(0000,007F,basic_latin),
            UNICODE_BLOCK_DEFINE(0080,00FF,latin_1_supplement),
            UNICODE_BLOCK_DEFINE(0100,017F,latin_extended_a),
            UNICODE_BLOCK_DEFINE(0180,024F,latin_extended_b),
            UNICODE_BLOCK_DEFINE(0250,02AF,ipa_extensions),
            UNICODE_BLOCK_DEFINE(02B0,02FF,spacing_modifier_letters),
            UNICODE_BLOCK_DEFINE(0300,036F,combining_diacritical_marks),
            UNICODE_BLOCK_DEFINE(0370,03FF,greek_coptic),
            UNICODE_BLOCK_DEFINE(0400,04FF,cyrillic),
            UNICODE_BLOCK_DEFINE(0500,052F,cyrillic_supplement),
            UNICODE_BLOCK_DEFINE(0530,058F,armenian),
            UNICODE_BLOCK_DEFINE(0590,05FF,hebrew),
            UNICODE_BLOCK_DEFINE(0600,06FF,arabic),
            UNICODE_BLOCK_DEFINE(0700,074F,syrian),
            UNICODE_BLOCK_DEFINE(0750,077F,arabic_supplement),
            UNICODE_BLOCK_DEFINE(0780,07BF,thaana),
            UNICODE_BLOCK_DEFINE(07C0,07FF,nko),
            UNICODE_BLOCK_DEFINE(0800,083F,samaritan),
            UNICODE_BLOCK_DEFINE(0840,085F,mandaic),
            UNICODE_BLOCK_DEFINE(0860,086F,syriac_supplement),
            UNICODE_BLOCK_DEFINE(08A0,08FF,arabic_extended_a),
            UNICODE_BLOCK_DEFINE(0900,097F,devanagari),
            UNICODE_BLOCK_DEFINE(0980,09FF,bengali),
            UNICODE_BLOCK_DEFINE(0A00,0A7F,gurmukhi),
            UNICODE_BLOCK_DEFINE(0A80,0AFF,gujarati),
            UNICODE_BLOCK_DEFINE(0B00,0B7F,oriya),
            UNICODE_BLOCK_DEFINE(0B80,0BFF,tamil),
            UNICODE_BLOCK_DEFINE(0C00,0C7F,telugu),
            UNICODE_BLOCK_DEFINE(0C80,0CFF,kannada),
            UNICODE_BLOCK_DEFINE(0D00,0D7F,malayalam),
            UNICODE_BLOCK_DEFINE(0D80,0DFF,sinhala),
            UNICODE_BLOCK_DEFINE(0E00,0E7F,thai),
            UNICODE_BLOCK_DEFINE(0E80,0EFF,lao),
            UNICODE_BLOCK_DEFINE(0F00,0FFF,tibetan),
            UNICODE_BLOCK_DEFINE(1000,109F,myanmar),
            UNICODE_BLOCK_DEFINE(10A0,10FF,georgian),
            UNICODE_BLOCK_DEFINE(1100,11FF,hangul_jamo),
            UNICODE_BLOCK_DEFINE(1200,137F,ethiopic),
            UNICODE_BLOCK_DEFINE(1380,139F,ethiopic_supplement),
            UNICODE_BLOCK_DEFINE(13A0,13FF,cherokee),
            UNICODE_BLOCK_DEFINE(1400,167F,unified_canadian_aboriginal_syllabics),
            UNICODE_BLOCK_DEFINE(1680,169F,ogham),
            UNICODE_BLOCK_DEFINE(16A0,16FF,runic),
            UNICODE_BLOCK_DEFINE(1700,171F,tagalog),
            UNICODE_BLOCK_DEFINE(1720,173F,hanunoo),
            UNICODE_BLOCK_DEFINE(1740,175F,buhid),
            UNICODE_BLOCK_DEFINE(1760,177F,tagbanwa),
            UNICODE_BLOCK_DEFINE(1780,17FF,khmer),
            UNICODE_BLOCK_DEFINE(1800,18AF,mongolian),
            UNICODE_BLOCK_DEFINE(18B0,18FF,unified_canadian_aboriginal_syllabics_extended),
            UNICODE_BLOCK_DEFINE(1900,194F,limbu),
            UNICODE_BLOCK_DEFINE(1950,197F,tai_le),
            UNICODE_BLOCK_DEFINE(1980,19DF,new_tai_lue),
            UNICODE_BLOCK_DEFINE(19E0,19FF,khmer_symbols),
            UNICODE_BLOCK_DEFINE(1A00,1A1F,buginese),
            UNICODE_BLOCK_DEFINE(1A20,1AAF,tai_tham),
            UNICODE_BLOCK_DEFINE(1AB0,1AFF,combining_diacritical_marks_extended),
            UNICODE_BLOCK_DEFINE(1B00,1B7F,balinese),
            UNICODE_BLOCK_DEFINE(1B80,1BBF,sundanese),
            UNICODE_BLOCK_DEFINE(1BC0,1BFF,batak),
            UNICODE_BLOCK_DEFINE(1C00,1C4F,lepcha),
            UNICODE_BLOCK_DEFINE(1C50,1C7F,ol_chiki),
            UNICODE_BLOCK_DEFINE(1C80,1C8F,cyrillic_extended_c),
            UNICODE_BLOCK_DEFINE(1C90,1CBF,georgian_extended),
            UNICODE_BLOCK_DEFINE(1CC0,1CCF,sundanese_supplement),
            UNICODE_BLOCK_DEFINE(1CD0,1CFF,vedic_extensions),
            UNICODE_BLOCK_DEFINE(1D00,1D7F,phonetic_extensions),
            UNICODE_BLOCK_DEFINE(1D80,1DBF,phonetic_extensions_supplement),
            UNICODE_BLOCK_DEFINE(1DC0,1DFF,combining_diacritical_marks_supplement),
            UNICODE_BLOCK_DEFINE(1E00,1EFF,latin_extended_additional),
            UNICODE_BLOCK_DEFINE(1F00,1FFF,greek_extended),
            UNICODE_BLOCK_DEFINE(2000,206F,general_punctuation),
            UNICODE_BLOCK_DEFINE(2070,209F,superscripts_and_subscripts),
            UNICODE_BLOCK_DEFINE(20A0,20CF,currency_symbols),
            UNICODE_BLOCK_DEFINE(20D0,20FF,combining_diacritical_marks_for_symbols),
            UNICODE_BLOCK_DEFINE(2100,214F,letterlike_symbols),
            UNICODE_BLOCK_DEFINE(2150,218F,number_forms),
            UNICODE_BLOCK_DEFINE(2190,21FF,arrows),
            UNICODE_BLOCK_DEFINE(2200,22FF,mathematical_operators),
            UNICODE_BLOCK_DEFINE(2300,23FF,miscellaneous_technical),
            UNICODE_BLOCK_DEFINE(2400,243F,control_pictures),
            UNICODE_BLOCK_DEFINE(2440,245F,optical_character_recognition),
            UNICODE_BLOCK_DEFINE(2460,24FF,enclosed_alphanumerics),
            UNICODE_BLOCK_DEFINE(2500,257F,box_drawing),
            UNICODE_BLOCK_DEFINE(2580,259F,block_elements),
            UNICODE_BLOCK_DEFINE(25A0,25FF,geometric_shapes),
            UNICODE_BLOCK_DEFINE(2600,26FF,miscellaneous_symbols),
            UNICODE_BLOCK_DEFINE(2700,27BF,dingbats),
            UNICODE_BLOCK_DEFINE(27C0,27EF,miscellaneous_mathematical_symbols_a),
            UNICODE_BLOCK_DEFINE(27F0,27FF,supplemental_arrows_a),
            UNICODE_BLOCK_DEFINE(2800,28FF,braille_patterns),
            UNICODE_BLOCK_DEFINE(2900,297F,supplemental_arrows_b),
            UNICODE_BLOCK_DEFINE(2980,29FF,miscellaneous_mathematical_symbols_b),
            UNICODE_BLOCK_DEFINE(2A00,2AFF,supplemental_mathematical_operators),
            UNICODE_BLOCK_DEFINE(2B00,2BFF,miscellaneous_symbols_and_arrows),
            UNICODE_BLOCK_DEFINE(2C00,2C5F,glagolitic),
            UNICODE_BLOCK_DEFINE(2C60,2C7F,latin_extended_c),
            UNICODE_BLOCK_DEFINE(2C80,2CFF,coptic),
            UNICODE_BLOCK_DEFINE(2D00,2D2F,georgian_supplement),
            UNICODE_BLOCK_DEFINE(2D30,2D7F,tifinagh),
            UNICODE_BLOCK_DEFINE(2D80,2DDF,ethiopic_extended),
            UNICODE_BLOCK_DEFINE(2DE0,2DFF,cyrillic_extended),
            UNICODE_BLOCK_DEFINE(2E00,2E7F,supplemental_punctuation),
            UNICODE_BLOCK_DEFINE(2E80,2EFF,cjk_radicals_supplement),
            UNICODE_BLOCK_DEFINE(2F00,2FDF,kangxi_radicals),
            UNICODE_BLOCK_DEFINE(2FF0,2FFF,ideographic_description_characters),
            UNICODE_BLOCK_DEFINE(3000,303F,cjk_symbols_and_punctuation),
            UNICODE_BLOCK_DEFINE(3040,309F,hiragana),
            UNICODE_BLOCK_DEFINE(30A0,30FF,katakana),
            UNICODE_BLOCK_DEFINE(3100,312F,bopomofo),
            UNICODE_BLOCK_DEFINE(3130,318F,hangul_compatibility_jamo),
            UNICODE_BLOCK_DEFINE(3190,319F,kanbun),
            UNICODE_BLOCK_DEFINE(31A0,31BF,bopomofo_extended),
            UNICODE_BLOCK_DEFINE(31C0,31EF,cjk_strokes),
            UNICODE_BLOCK_DEFINE(31F0,31FF,katakana_phonetic_extensions),
            UNICODE_BLOCK_DEFINE(3200,32FF,enclosed_cjk_letters_and_months),
            UNICODE_BLOCK_DEFINE(3300,33FF,cjk_compatibility),
            UNICODE_BLOCK_DEFINE(3400,4DBF,cjk_unified_ideographs_extension_a),
            UNICODE_BLOCK_DEFINE(4DC0,4DFF,yijing_hexagram_symbols),
            UNICODE_BLOCK_DEFINE(4E00,9FFF,cjk_unified_ideographs),
            UNICODE_BLOCK_DEFINE(A000,A48F,yi_syllables),
            UNICODE_BLOCK_DEFINE(A490,A4CF,yi_radicals),
            UNICODE_BLOCK_DEFINE(A4D0,A4FF,lisu),
            UNICODE_BLOCK_DEFINE(A500,A63F,old_lisu_alphabet),
            UNICODE_BLOCK_DEFINE(A640,A69F,ciryllic_extended_B),
            UNICODE_BLOCK_DEFINE(A6A0,A6FF,bamum),
            UNICODE_BLOCK_DEFINE(A700,A71F,modifier_tone_letters),
            UNICODE_BLOCK_DEFINE(A720,A7FF,latin_extended_d),
            UNICODE_BLOCK_DEFINE(A800,A82F,syloty_nagri),
            UNICODE_BLOCK_DEFINE(A830,A83F,indian_numeric_character),
            UNICODE_BLOCK_DEFINE(A840,A87F,phags_pa),
            UNICODE_BLOCK_DEFINE(A880,A8DF,saurashtra),
            UNICODE_BLOCK_DEFINE(A8E0,A8FF,devanagari_extended_characters),
            UNICODE_BLOCK_DEFINE(A900,A92F,kayah_li),
            UNICODE_BLOCK_DEFINE(A930,A95F,rejang),
            UNICODE_BLOCK_DEFINE(A960,A97F,hangul),
            UNICODE_BLOCK_DEFINE(A980,A9DF,javanese_alphabet),
            UNICODE_BLOCK_DEFINE(A9E0,A9FF,myanmar_extended_b),
            UNICODE_BLOCK_DEFINE(AA00,AA5F,cham_alphabet),
            UNICODE_BLOCK_DEFINE(AA60,AA7F,burmese_alphabet_extention_a),
            UNICODE_BLOCK_DEFINE(AA80,AADF,tai_viet_alphabet),
            UNICODE_BLOCK_DEFINE(AAE0,AAFF,meitei_language_extensions),
            UNICODE_BLOCK_DEFINE(AB00,AB2F,ethiopic_extended_a),
            UNICODE_BLOCK_DEFINE(AB30,AB6F,latin_extended_e),
            UNICODE_BLOCK_DEFINE(AB70,ABBF,cherokee_supplement),
            UNICODE_BLOCK_DEFINE(ABC0,ABFF,meitei_language),
            UNICODE_BLOCK_DEFINE(AC00,D7AF,hangul_syllables),
            UNICODE_BLOCK_DEFINE(D7B0,D7FF,hangul_jamo_extended_b),
            UNICODE_BLOCK_DEFINE(D800,DB7F,high_surrogates),
            UNICODE_BLOCK_DEFINE(DB80,DBFF,high_private_use_surrogates),
            UNICODE_BLOCK_DEFINE(DC00,DFFF,low_surrogates),
            UNICODE_BLOCK_DEFINE(E000,F8FF,private_use_area),
            UNICODE_BLOCK_DEFINE(F900,FAFF,cjk_compatibility_ideographs),
            UNICODE_BLOCK_DEFINE(FB00,FB4F,alphabetic_presentation_forms),
            UNICODE_BLOCK_DEFINE(FB50,FDFF,arabic_presentation_forms_a),
            UNICODE_BLOCK_DEFINE(FE00,FE0F,variation_selectors),
            UNICODE_BLOCK_DEFINE(FE10,FE1F,vertical_forms),
            UNICODE_BLOCK_DEFINE(FE20,FE2F,combining_half_marks),
            UNICODE_BLOCK_DEFINE(FE30,FE4F,cjk_compatibility_forms),
            UNICODE_BLOCK_DEFINE(FE50,FE6F,small_form_variants),
            UNICODE_BLOCK_DEFINE(FE70,FEFF,arabic_presentation_forms_b),
            UNICODE_BLOCK_DEFINE(FF00,FFEF,halfwidth_and_fullwidth_forms),
            UNICODE_BLOCK_DEFINE(FFF0,FFFF,specials),
            UNICODE_BLOCK_DEFINE(10000,1007F,linear_b_syllabary),
            UNICODE_BLOCK_DEFINE(10080,100FF,linear_b_ideograms),
            UNICODE_BLOCK_DEFINE(10100,1013F,aegean_numbers),
            UNICODE_BLOCK_DEFINE(10140,1018F,ancient_greek_numbers),
            UNICODE_BLOCK_DEFINE(10190,101CF,ancient_symbols),
            UNICODE_BLOCK_DEFINE(101D0,101FF,phaistos_disc),
            UNICODE_BLOCK_DEFINE(10280,1029F,lycian),
            UNICODE_BLOCK_DEFINE(102A0,102DF,carian),
            UNICODE_BLOCK_DEFINE(102E0,102FF,coptic_epact_numbers),
            UNICODE_BLOCK_DEFINE(10300,1032F,old_italic),
            UNICODE_BLOCK_DEFINE(10330,1034F,gothic),
            UNICODE_BLOCK_DEFINE(10350,1037F,old_permic),
            UNICODE_BLOCK_DEFINE(10380,1039F,ugaritic),
            UNICODE_BLOCK_DEFINE(103A0,103DF,old_persian),
            UNICODE_BLOCK_DEFINE(10400,1044F,deseret),
            UNICODE_BLOCK_DEFINE(10450,1047F,shavian),
            UNICODE_BLOCK_DEFINE(10480,104AF,osmanya),
            UNICODE_BLOCK_DEFINE(104B0,104FF,osage),
            UNICODE_BLOCK_DEFINE(10500,1052F,elbasan),
            UNICODE_BLOCK_DEFINE(10530,1056F,caucasian_albanian),
            UNICODE_BLOCK_DEFINE(10600,1077F,linear_a),
            UNICODE_BLOCK_DEFINE(10800,1083F,cypriot_syllabary),
            UNICODE_BLOCK_DEFINE(10840,1085F,imperial_aramaic),
            UNICODE_BLOCK_DEFINE(10860,1087F,palmyrene),
            UNICODE_BLOCK_DEFINE(10880,108AF,nabataean),
            UNICODE_BLOCK_DEFINE(108E0,108FF,hatran),
            UNICODE_BLOCK_DEFINE(10900,1091F,phoenician),
            UNICODE_BLOCK_DEFINE(10920,1093F,lydian),
            UNICODE_BLOCK_DEFINE(10980,1099F,meroitic_hieroglyphs),
            UNICODE_BLOCK_DEFINE(109A0,109FF,meroitic_cursive),
            UNICODE_BLOCK_DEFINE(10A00,10A5F,kharoshthi),
            UNICODE_BLOCK_DEFINE(10A60,10A7F,old_south_arabian),
            UNICODE_BLOCK_DEFINE(10A80,10A9F,old_north_arabian),
            UNICODE_BLOCK_DEFINE(10AC0,10AFF,manichaean),
            UNICODE_BLOCK_DEFINE(10B00,10B3F,avestan),
            UNICODE_BLOCK_DEFINE(10B40,10B5F,inscriptional_parthian),
            UNICODE_BLOCK_DEFINE(10B60,10B7F,inscriptional_pahlavi),
            UNICODE_BLOCK_DEFINE(10B80,10BAF,psalter_pahlavi),
            UNICODE_BLOCK_DEFINE(10C00,10C4F,old_turkic),
            UNICODE_BLOCK_DEFINE(10C80,10CFF,old_hungarian),
            UNICODE_BLOCK_DEFINE(10D00,10D3F,hanifi_rohingya),
            UNICODE_BLOCK_DEFINE(10E60,10E7F,rumi_numeral_symbols),
            UNICODE_BLOCK_DEFINE(10E80,10EBF,yezidi),
            UNICODE_BLOCK_DEFINE(10F00,10F2F,old_sogdian),
            UNICODE_BLOCK_DEFINE(10F30,10F6F,sogdian),
            UNICODE_BLOCK_DEFINE(10FB0,10FDF,chorasmian),
            UNICODE_BLOCK_DEFINE(10FE0,10FFF,elymaic),
            UNICODE_BLOCK_DEFINE(11000,1107F,brahmi),
            UNICODE_BLOCK_DEFINE(11080,110CF,kaithi),
            UNICODE_BLOCK_DEFINE(110D0,110FF,sora_sompeng),
            UNICODE_BLOCK_DEFINE(11100,1114F,chakma),
            UNICODE_BLOCK_DEFINE(11150,1117F,mahajani),
            UNICODE_BLOCK_DEFINE(11180,111DF,sharada),
            UNICODE_BLOCK_DEFINE(111E0,111FF,sinhala_archaic_numbers),
            UNICODE_BLOCK_DEFINE(11200,1124F,khojki),
            UNICODE_BLOCK_DEFINE(11280,112AF,multani),
            UNICODE_BLOCK_DEFINE(112B0,112FF,khudawadi),
            UNICODE_BLOCK_DEFINE(11300,1137F,grantha),
            UNICODE_BLOCK_DEFINE(11400,1147F,newa),
            UNICODE_BLOCK_DEFINE(11480,114DF,tirhuta),
            UNICODE_BLOCK_DEFINE(11580,115FF,siddham),
            UNICODE_BLOCK_DEFINE(11600,1165F,modi),
            UNICODE_BLOCK_DEFINE(11660,1167F,mongolian_supplement),
            UNICODE_BLOCK_DEFINE(11680,116CF,takri),
            UNICODE_BLOCK_DEFINE(11700,1173F,ahom),
            UNICODE_BLOCK_DEFINE(11800,1184F,dogra),
            UNICODE_BLOCK_DEFINE(118A0,118FF,warang_citi),
            UNICODE_BLOCK_DEFINE(11900,1195F,dives_akuru),
            UNICODE_BLOCK_DEFINE(119A0,119FF,nandinagari),
            UNICODE_BLOCK_DEFINE(11A00,11A4F,zanabazar_square),
            UNICODE_BLOCK_DEFINE(11A50,11AAF,soyombo),
            UNICODE_BLOCK_DEFINE(11AC0,11AFF,pau_cin_hau),
            UNICODE_BLOCK_DEFINE(11C00,11C6F,bhaiksuki),
            UNICODE_BLOCK_DEFINE(11C70,11CBF,marchen),
            UNICODE_BLOCK_DEFINE(11D00,11D5F,masaram_gondi),
            UNICODE_BLOCK_DEFINE(11D60,11DAF,gunjala_gondi),
            UNICODE_BLOCK_DEFINE(11EE0,11EFF,makasar),
            UNICODE_BLOCK_DEFINE(11FB0,11FBF,lisu_supplement),
            UNICODE_BLOCK_DEFINE(11FC0,11FFF,tamil_supplement),
            UNICODE_BLOCK_DEFINE(12000,123FF,cuneiform),
            UNICODE_BLOCK_DEFINE(12400,1247F,cuneiform_numbers_and_punctuation),
            UNICODE_BLOCK_DEFINE(12480,1254F,early_dynastic_cuneiform),
            UNICODE_BLOCK_DEFINE(13000,1342F,egyptian_hieroglyphs),
            UNICODE_BLOCK_DEFINE(13430,1343F,egyptian_hieroglyph_format_controls),
            UNICODE_BLOCK_DEFINE(14400,1467F,anatolian_hieroglyphs),
            UNICODE_BLOCK_DEFINE(16800,16A3F,bamum_supplement),
            UNICODE_BLOCK_DEFINE(16A40,16A6F,mro),
            UNICODE_BLOCK_DEFINE(16AD0,16AFF,bassa_vah),
            UNICODE_BLOCK_DEFINE(16B00,16B8F,pahawh_hmong),
            UNICODE_BLOCK_DEFINE(16E40,16E9F,medefaidrin),
            UNICODE_BLOCK_DEFINE(16F00,16F9F,miao),
            UNICODE_BLOCK_DEFINE(16FE0,16FFF,ideographic_symbols_and_punctuation),
            UNICODE_BLOCK_DEFINE(17000,187FF,tangut),
            UNICODE_BLOCK_DEFINE(18800,18AFF,tangut_components),
            UNICODE_BLOCK_DEFINE(18B00,18CFF,khitan_small_script),
            UNICODE_BLOCK_DEFINE(18D00,18D8F,tangut_supplement),
            UNICODE_BLOCK_DEFINE(1B000,1B0FF,kana_supplement),
            UNICODE_BLOCK_DEFINE(1B100,1B12F,kana_extended_a),
            UNICODE_BLOCK_DEFINE(1B130,1B16F,small_kana_extension),
            UNICODE_BLOCK_DEFINE(1B170,1B2FF,nushu),
            UNICODE_BLOCK_DEFINE(1BC00,1BC9F,duployan),
            UNICODE_BLOCK_DEFINE(1BCA0,1BCAF,shorthand_format_controls),
            UNICODE_BLOCK_DEFINE(1D000,1D0FF,byzantine_musical_symbols),
            UNICODE_BLOCK_DEFINE(1D100,1D1FF,musical_symbols),
            UNICODE_BLOCK_DEFINE(1D200,1D24F,ancient_greek_musical_notation),
            UNICODE_BLOCK_DEFINE(1D2E0,1D2FF,mayan_numerals),
            UNICODE_BLOCK_DEFINE(1D300,1D35F,tai_xuan_jing_symbols),
            UNICODE_BLOCK_DEFINE(1D360,1D37F,counting_rod_numerals),
            UNICODE_BLOCK_DEFINE(1D400,1D7FF,mathematical_alphanumeric_symbols),
            UNICODE_BLOCK_DEFINE(1D800,1DAAF,sutton_sign_writing),
            UNICODE_BLOCK_DEFINE(1E000,1E02F,glagolitic_supplement),
            UNICODE_BLOCK_DEFINE(1E100,1E14F,nyiakeng_puachue_hmong),
            UNICODE_BLOCK_DEFINE(1E2C0,1E2FF,wancho),
            UNICODE_BLOCK_DEFINE(1E800,1E8DF,mende_kikakui),
            UNICODE_BLOCK_DEFINE(1E900,1E95F,adlam),
            UNICODE_BLOCK_DEFINE(1EC70,1ECBF,indic_siyaq_numbers),
            UNICODE_BLOCK_DEFINE(1ED00,1ED4F,ottoman_siyaq_numbers),
            UNICODE_BLOCK_DEFINE(1EE00,1EEFF,arabic_mathematical_alphabetic_symbols),
            UNICODE_BLOCK_DEFINE(1F000,1F02F,mahjong_tiles),
            UNICODE_BLOCK_DEFINE(1F030,1F09F,domino_tiles),
            UNICODE_BLOCK_DEFINE(1F0A0,1F0FF,playing_cards),
            UNICODE_BLOCK_DEFINE(1F100,1F1FF,enclosed_alphanumeric_supplement),
            UNICODE_BLOCK_DEFINE(1F200,1F2FF,enclosed_ideographic_supplement),
            UNICODE_BLOCK_DEFINE(1F300,1F5FF,miscellaneous_symbols_and_pictographs),
            UNICODE_BLOCK_DEFINE(1F600,1F64F,emoticons),
            UNICODE_BLOCK_DEFINE(1F650,1F67F,ornamental_dingbats),
            UNICODE_BLOCK_DEFINE(1F680,1F6FF,transport_and_map_symbols),
            UNICODE_BLOCK_DEFINE(1F700,1F77F,alchemical_symbols),
            UNICODE_BLOCK_DEFINE(1F780,1F7FF,geometric_shapes_extended),
            UNICODE_BLOCK_DEFINE(1F800,1F8FF,supplemental_arrows_c),
            UNICODE_BLOCK_DEFINE(1F900,1F9FF,supplemental_symbols_and_pictographs),
            UNICODE_BLOCK_DEFINE(1FA00,1FA6F,chess_symbols),
            UNICODE_BLOCK_DEFINE(1FA70,1FAFF,symbols_and_pictographs_extended_a),
            UNICODE_BLOCK_DEFINE(1FB00,1FBFF,symbols_for_legacy_computing),
            UNICODE_BLOCK_DEFINE(20000,2A6DF,cjk_unified_ideographs_extension_b),
            UNICODE_BLOCK_DEFINE(2A700,2B73F,cjk_unified_ideographs_extension_c),
            UNICODE_BLOCK_DEFINE(2B740,2B81F,cjk_unified_ideographs_extension_d),
            UNICODE_BLOCK_DEFINE(2B820,2CEAF,cjk_unified_ideographs_extension_e),
            UNICODE_BLOCK_DEFINE(2CEB0,2EBEF,cjk_unified_ideographs_extension_f),
            UNICODE_BLOCK_DEFINE(2F800,2FA1F,cjk_compatibility_ideographs_supplement),
            UNICODE_BLOCK_DEFINE(30000,3134F,cjk_unified_ideographs_extension_g),
            UNICODE_BLOCK_DEFINE(E0000,E007F,tags),
            UNICODE_BLOCK_DEFINE(E0100,E01EF,variation_selectors_supplement),
            UNICODE_BLOCK_DEFINE(F0000,FFFFF,supplementary_private_use_area_a),
            UNICODE_BLOCK_DEFINE(100000,10FFFF,supplementary_private_use_area_b),
        };
    }//namespace hgl

    bool IsInUnicodeBlock(const UnicodeBlock &type,const uint32 ch)
    {
        if(type<UnicodeBlock::basic_latin
            ||type>UnicodeBlock::END_RANGE)
            return(false);

        if(ch<UnicodeBlockList[(size_t)type].begin)return(false);
        if(ch<UnicodeBlockList[(size_t)type].end)return(false);

        return(true);
    }

    #define CHECK_UNICODE_BLOCKS(type)  if(IsInUnicodeBlock(UnicodeBlock::type,ch))return(true);

    /**
     * 判断当前字符是否是拉丁字符
     */
    bool isLatin(const u32char ch)
    {
        if(ch>0xFFFF)return(false);
        
        CHECK_UNICODE_BLOCKS(basic_latin)
        CHECK_UNICODE_BLOCKS(latin_1_supplement)
        CHECK_UNICODE_BLOCKS(latin_extended_a)
        CHECK_UNICODE_BLOCKS(latin_extended_b)
        CHECK_UNICODE_BLOCKS(latin_extended_additional)
        CHECK_UNICODE_BLOCKS(latin_extended_c)
        CHECK_UNICODE_BLOCKS(latin_extended_d)
        CHECK_UNICODE_BLOCKS(latin_extended_e)

        return(false);
    }

    /**
     * 判断当前字符是否是CJK字符
     */
    bool isCJK(const u16char ch)
    {
        CHECK_UNICODE_BLOCKS(cjk_radicals_supplement)
        CHECK_UNICODE_BLOCKS(cjk_symbols_and_punctuation)
        CHECK_UNICODE_BLOCKS(cjk_strokes)
        CHECK_UNICODE_BLOCKS(enclosed_cjk_letters_and_months)
        CHECK_UNICODE_BLOCKS(cjk_compatibility)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_a)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs)
        CHECK_UNICODE_BLOCKS(cjk_compatibility_ideographs)
        CHECK_UNICODE_BLOCKS(cjk_compatibility_forms)

        return(false);
    }

    /**
     * 判断当前字符是否是CJK字符
     */
    bool isCJK(const u32char ch)
    {
        if(ch<0x10000)
            return isCJK((const u16char)ch);

        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_b)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_c)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_d)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_e)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_f)
        CHECK_UNICODE_BLOCKS(cjk_compatibility_ideographs_supplement)
        CHECK_UNICODE_BLOCKS(cjk_unified_ideographs_extension_g)

        return(false);
    }

    /**
     * 判断当前字符是否是表情符号
     */
    bool isEmoji(const u32char ch)
    {
        CHECK_UNICODE_BLOCKS(emoticons)

        return(false);
    }

    #undef CHECK_UNICODE_BLOCKS
}//namespace hgl