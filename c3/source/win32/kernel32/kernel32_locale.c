/*
 * Locale support
 *
 * Copyright 1995 Martin von Loewis
 * Copyright 1998 David Lee Lambert
 * Copyright 2000 Julio César Gázquez
 * Copyright 2002 Alexandre Julliard for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "../include/winbase.h"
#include "../include/ntstatus.h"
#include "../include/unicode.h"
#include "memory.h"

#include <assert.h>
#include <locale.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define LOCALE_LOCALEINFOFLAGSMASK (LOCALE_NOUSEROVERRIDE|LOCALE_USE_CP_ACP|\
                                    LOCALE_RETURN_NUMBER|LOCALE_RETURN_GENITIVE_NAMES)

/* current code pages */
static const union cptable *ansi_cptable;
static const union cptable *oem_cptable;
static const union cptable *mac_cptable;
static const union cptable *unix_cptable;  /* NULL if UTF8 */

static const WCHAR szLocaleKeyName[] = {
    'M','a','c','h','i','n','e','\\','S','y','s','t','e','m','\\',
    'C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\',
    'C','o','n','t','r','o','l','\\','N','l','s','\\','L','o','c','a','l','e',0
};

static const WCHAR szLangGroupsKeyName[] = {
    'M','a','c','h','i','n','e','\\','S','y','s','t','e','m','\\',
    'C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\',
    'C','o','n','t','r','o','l','\\','N','l','s','\\',
    'L','a','n','g','u','a','g','e',' ','G','r','o','u','p','s',0
};

/* Charset to codepage map, sorted by name. */
static const struct charset_entry
{
    const char *charset_name;
    UINT        codepage;
} charset_names[] =
{
    { "BIG5", 950 },
    { "CP1250", 1250 },
    { "CP1251", 1251 },
    { "CP1252", 1252 },
    { "CP1253", 1253 },
    { "CP1254", 1254 },
    { "CP1255", 1255 },
    { "CP1256", 1256 },
    { "CP1257", 1257 },
    { "CP1258", 1258 },
    { "CP932", 932 },
    { "CP936", 936 },
    { "CP949", 949 },
    { "CP950", 950 },
    { "EUCJP", 20932 },
    { "GB2312", 936 },
    { "IBM037", 37 },
    { "IBM1026", 1026 },
    { "IBM424", 424 },
    { "IBM437", 437 },
    { "IBM500", 500 },
    { "IBM850", 850 },
    { "IBM852", 852 },
    { "IBM855", 855 },
    { "IBM857", 857 },
    { "IBM860", 860 },
    { "IBM861", 861 },
    { "IBM862", 862 },
    { "IBM863", 863 },
    { "IBM864", 864 },
    { "IBM865", 865 },
    { "IBM866", 866 },
    { "IBM869", 869 },
    { "IBM874", 874 },
    { "IBM875", 875 },
    { "ISO88591", 28591 },
    { "ISO885910", 28600 },
    { "ISO885913", 28603 },
    { "ISO885914", 28604 },
    { "ISO885915", 28605 },
    { "ISO885916", 28606 },
    { "ISO88592", 28592 },
    { "ISO88593", 28593 },
    { "ISO88594", 28594 },
    { "ISO88595", 28595 },
    { "ISO88596", 28596 },
    { "ISO88597", 28597 },
    { "ISO88598", 28598 },
    { "ISO88599", 28599 },
    { "KOI8R", 20866 },
    { "KOI8U", 21866 },
    { "UTF8", CP_UTF8 }
};

WCHAR* getLocalData(LCID locale, LCTYPE lctype) {
    switch (lctype)
    {
    case LOCALE_FONTSIGNATURE: return L"\x00af\x8000\x38cb\x0000\x0000\x0000\x0000\x0000\x0001\x0000\x0000\x8000\x01ff\x003f\x8000\xffff";
    case LOCALE_ICALENDARTYPE: return L"1";
    case LOCALE_ICENTURY: return L"1";
    case LOCALE_ICOUNTRY: return L"1";
    case LOCALE_ICURRDIGITS: return L"2";
    case LOCALE_ICURRENCY: return L"0";
    case LOCALE_IDATE: return L"0";
    case LOCALE_IDAYLZERO: return L"0";
    case LOCALE_IDEFAULTANSICODEPAGE: return L"1252";
    case LOCALE_IDEFAULTCODEPAGE: return L"437";
    case LOCALE_IDEFAULTCOUNTRY: return L"1";
    case LOCALE_IDEFAULTEBCDICCODEPAGE: return L"037";
    case LOCALE_IDEFAULTLANGUAGE: return L"0409";
    case LOCALE_IDEFAULTMACCODEPAGE: return L"10000";
    case LOCALE_IDEFAULTUNIXCODEPAGE: return L"28591";
    case LOCALE_IDIGITS: return L"2";
    case LOCALE_IDIGITSUBSTITUTION: return L"1";
    case LOCALE_IFIRSTDAYOFWEEK: return L"6";
    case LOCALE_IFIRSTWEEKOFYEAR: return L"0";
    case LOCALE_IINTLCURRDIGITS: return L"2";
    case LOCALE_ILANGUAGE: return L"0409";
    case LOCALE_ILDATE: return L"0";
    case LOCALE_ILZERO: return L"1";
    case LOCALE_IMEASURE: return L"1";
    case LOCALE_IMONLZERO: return L"0";
    case LOCALE_INEGCURR: return L"0";
    case LOCALE_INEGNUMBER: return L"1";
    case LOCALE_INEGSEPBYSPACE: return L"0";
    case LOCALE_INEGSIGNPOSN: return L"0";
    case LOCALE_INEGSYMPRECEDES: return L"1";
    case LOCALE_INEUTRAL: return L"0";
    case LOCALE_IOPTIONALCALENDAR: return L"0";
    case LOCALE_IPAPERSIZE: return L"1";
    case LOCALE_IPOSSEPBYSPACE: return L"0";
    case LOCALE_IPOSSIGNPOSN: return L"3";
    case LOCALE_IPOSSYMPRECEDES: return L"1";
    case LOCALE_IREADINGLAYOUT: return L"0";
    case LOCALE_ITIME: return L"0";
    case LOCALE_ITIMEMARKPOSN: return L"0";
    case LOCALE_ITLZERO: return L"0";
    case LOCALE_S1159: return L"AM";
    case LOCALE_S2359: return L"PM";
    case LOCALE_SABBREVCTRYNAME: return L"USA";
    case LOCALE_SABBREVDAYNAME1: return L"Mon";
    case LOCALE_SABBREVDAYNAME2: return L"Tue";
    case LOCALE_SABBREVDAYNAME3: return L"Wed";
    case LOCALE_SABBREVDAYNAME4: return L"Thu";
    case LOCALE_SABBREVDAYNAME5: return L"Fri";
    case LOCALE_SABBREVDAYNAME6: return L"Sat";
    case LOCALE_SABBREVDAYNAME7: return L"Sun";
    case LOCALE_SABBREVLANGNAME: return L"ENU";
    case LOCALE_SABBREVMONTHNAME1: return L"Jan";
    case LOCALE_SABBREVMONTHNAME2: return L"Feb";
    case LOCALE_SABBREVMONTHNAME3: return L"Mar";
    case LOCALE_SABBREVMONTHNAME4: return L"Apr";
    case LOCALE_SABBREVMONTHNAME5: return L"May";
    case LOCALE_SABBREVMONTHNAME6: return L"Jun";
    case LOCALE_SABBREVMONTHNAME7: return L"Jul";
    case LOCALE_SABBREVMONTHNAME8: return L"Aug";
    case LOCALE_SABBREVMONTHNAME9: return L"Sep";
    case LOCALE_SABBREVMONTHNAME10: return L"Oct";
    case LOCALE_SABBREVMONTHNAME11: return L"Nov";
    case LOCALE_SABBREVMONTHNAME12: return L"Dec";
    case LOCALE_SABBREVMONTHNAME13: return L"";
    case LOCALE_SCOUNTRY: return L"United States";
    case LOCALE_SCURRENCY: return L"$";
    case LOCALE_SDATE: return L"/";
    case LOCALE_SDAYNAME1: return L"Monday";
    case LOCALE_SDAYNAME2: return L"Tuesday";
    case LOCALE_SDAYNAME3: return L"Wednesday";
    case LOCALE_SDAYNAME4: return L"Thursday";
    case LOCALE_SDAYNAME5: return L"Friday";
    case LOCALE_SDAYNAME6: return L"Saturday";
    case LOCALE_SDAYNAME7: return L"Sunday";
    case LOCALE_SDECIMAL: return L".";
    case LOCALE_SENGCOUNTRY: return L"United States";
    case LOCALE_SENGCURRNAME: return L"US Dollar";
    case LOCALE_SENGLANGUAGE: return L"English";
    case LOCALE_SGROUPING: return L"3;0";
    case LOCALE_SINTLSYMBOL: return L"USD";
    case LOCALE_SISO3166CTRYNAME: return L"US";
    case LOCALE_SISO639LANGNAME: return L"en";
    case LOCALE_SLANGUAGE: return L"English (United States)";
    case LOCALE_SLIST: return L",";
    case LOCALE_SLONGDATE: return L"dddd, MMMM dd, yyyy";
    case LOCALE_SMONDECIMALSEP: return L".";
    case LOCALE_SMONGROUPING: return L"3;0";
    case LOCALE_SMONTHNAME1: return L"January";
    case LOCALE_SMONTHNAME2: return L"February";
    case LOCALE_SMONTHNAME3: return L"March";
    case LOCALE_SMONTHNAME4: return L"April";
    case LOCALE_SMONTHNAME5: return L"May";
    case LOCALE_SMONTHNAME6: return L"June";
    case LOCALE_SMONTHNAME7: return L"July";
    case LOCALE_SMONTHNAME8: return L"August";
    case LOCALE_SMONTHNAME9: return L"September";
    case LOCALE_SMONTHNAME10: return L"October";
    case LOCALE_SMONTHNAME11: return L"November";
    case LOCALE_SMONTHNAME12: return L"December";
    case LOCALE_SMONTHNAME13: return L"";
    case LOCALE_SMONTHOUSANDSEP: return L",";
    case LOCALE_SNAME: return L"en-US";
    case LOCALE_SNATIVECTRYNAME: return L"United States";
    case LOCALE_SNATIVECURRNAME: return L"US Dollar";
    case LOCALE_SNATIVEDIGITS: return L"0123456789";
    case LOCALE_SNATIVEDISPLAYNAME: return L"English (United States)";
    case LOCALE_SNATIVELANGNAME: return L"English";
    case LOCALE_SNEGATIVESIGN: return L"-";
    case LOCALE_SPOSITIVESIGN: return L"";
    case LOCALE_SSCRIPTS: return L"Latn;";
    case LOCALE_SSHORTDATE: return L"M/d/yyyy";
    case LOCALE_SSORTNAME: return L"Default";
    case LOCALE_STHOUSAND: return L",";
    case LOCALE_STIME: return L":";
    case LOCALE_STIMEFORMAT: return L"h:mm:ss tt";
    case LOCALE_SYEARMONTH: return L"MMMM, yyyy";
    }
    return NULL;
}

struct locale_name
{
    WCHAR  win_name[128];   /* Windows name ("en-US") */
    WCHAR  lang[128];       /* language ("en") (note: buffer contains the other strings too) */
    WCHAR *country;         /* country ("US") */
    WCHAR *charset;         /* charset ("UTF-8") for Unix format only */
    WCHAR *script;          /* script ("Latn") for Windows format only */
    WCHAR *modifier;        /* modifier or sort order */
    LCID   lcid;            /* corresponding LCID */
    int    matches;         /* number of elements matching LCID (0..4) */
    UINT   codepage;        /* codepage corresponding to charset */
};

int supportedLocals[] = {1033};
#define NUMBER_OF_SUPPORTED_LOCALS 1

/* Copy Ascii string to Unicode without using codepages */
static __inline void strcpynAtoW( WCHAR *dst, const char *src, size_t n )
{
    while (n > 1 && *src)
    {
        *dst++ = (unsigned char)*src++;
        n--;
    }
    if (n) *dst = 0;
}

static __inline unsigned short get_table_entry( const unsigned short *table, WCHAR ch )
{
    return table[table[table[ch >> 8] + ((ch >> 4) & 0x0f)] + (ch & 0xf)];
}

/***********************************************************************
 *		get_lcid_codepage
 *
 * Retrieve the ANSI codepage for a given locale.
 */
static __inline UINT get_lcid_codepage( LCID lcid )
{
    UINT ret;
    if (!GetLocaleInfoW_k( lcid, LOCALE_IDEFAULTANSICODEPAGE|LOCALE_RETURN_NUMBER, (WCHAR *)&ret,
                         sizeof(ret)/sizeof(WCHAR) )) ret = 0;
    return ret;
}


/***********************************************************************
 *		get_codepage_table
 *
 * Find the table for a given codepage, handling CP_ACP etc. pseudo-codepages
 */
static const union cptable *get_codepage_table( unsigned int codepage )
{
    const union cptable *ret = NULL;

    assert( ansi_cptable );  /* init must have been done already */

    switch(codepage)
    {
    case CP_ACP:
        return ansi_cptable;
    case CP_OEMCP:
        return oem_cptable;
    case CP_MACCP:
        return mac_cptable;
    case CP_UTF7:
    case CP_UTF8:
        break;
    case CP_THREAD_ACP:
        if (readd(NtCurrentTeb_k()+TEB_LOCALE) == GetUserDefaultLCID_k()) return ansi_cptable;
        codepage = get_lcid_codepage( readd(NtCurrentTeb_k()+TEB_LOCALE) );
        if (!codepage) return ansi_cptable;
        /* fall through */
    default:
        if (codepage == ansi_cptable->info.codepage) return ansi_cptable;
        if (codepage == oem_cptable->info.codepage) return oem_cptable;
        if (codepage == mac_cptable->info.codepage) return mac_cptable;
        ret = wine_cp_get_table( codepage );
        break;
    }
    return ret;
}


/***********************************************************************
 *              charset_cmp (internal)
 */
static int charset_cmp( const void *name, const void *entry )
{
    const struct charset_entry *charset = entry;
    return strcasecmp( name, charset->charset_name );
}

/***********************************************************************
 *		find_charset
 */
static UINT find_charset( const WCHAR *name )
{
    const struct charset_entry *entry;
    char charset_name[16];
    size_t i, j;

    /* remove punctuation characters from charset name */
    for (i = j = 0; name[i] && j < sizeof(charset_name)-1; i++)
        if (isalnum((unsigned char)name[i])) charset_name[j++] = (char)name[i];
    charset_name[j] = 0;

    entry = bsearch( charset_name, charset_names,
                     sizeof(charset_names)/sizeof(charset_names[0]),
                     sizeof(charset_names[0]), charset_cmp );
    if (entry) return entry->codepage;
    return 0;
}


/***********************************************************************
 *           find_locale_id_callback
 */
static BOOL CALLBACK find_locale_id_callback( HMODULE hModule, LPCWSTR type,
                                              LPCWSTR name, WORD LangID, LPARAM lParam )
{
    struct locale_name *data = (struct locale_name *)lParam;
    WCHAR buffer[128];
    int matches = 0;
    LCID lcid = MAKELCID( LangID, SORT_DEFAULT );  /* FIXME: handle sort order */

    if (PRIMARYLANGID(LangID) == LANG_NEUTRAL) return TRUE; /* continue search */

    /* first check exact name */
    if (data->win_name[0] &&
        GetLocaleInfoW_k( lcid, LOCALE_SNAME | LOCALE_NOUSEROVERRIDE,
                        buffer, sizeof(buffer)/sizeof(WCHAR) ))
    {
        if (!strcmpW( data->win_name, buffer ))
        {
            matches = 4;  /* everything matches */
            goto done;
        }
    }

    if (!GetLocaleInfoW_k( lcid, LOCALE_SISO639LANGNAME | LOCALE_NOUSEROVERRIDE,
                         buffer, sizeof(buffer)/sizeof(WCHAR) ))
        return TRUE;
    if (strcmpW( buffer, data->lang )) return TRUE;
    matches++;  /* language name matched */

    if (data->country)
    {
        if (GetLocaleInfoW_k( lcid, LOCALE_SISO3166CTRYNAME|LOCALE_NOUSEROVERRIDE,
                            buffer, sizeof(buffer)/sizeof(WCHAR) ))
        {
            if (strcmpW( buffer, data->country )) goto done;
            matches++;  /* country name matched */
        }
    }
    else  /* match default language */
    {
        if (SUBLANGID(LangID) == SUBLANG_DEFAULT) matches++;
    }

    if (data->codepage)
    {
        UINT unix_cp;
        if (GetLocaleInfoW_k( lcid, LOCALE_IDEFAULTUNIXCODEPAGE | LOCALE_RETURN_NUMBER,
                            (LPWSTR)&unix_cp, sizeof(unix_cp)/sizeof(WCHAR) ))
        {
            if (unix_cp == data->codepage) matches++;
        }
    }

    /* FIXME: check sort order */

done:
    if (matches > data->matches)
    {
        data->lcid = lcid;
        data->matches = matches;
    }
    return (data->matches < 4);  /* no need to continue for perfect match */
}


/***********************************************************************
 *		parse_locale_name
 *
 * Parse a locale name into a struct locale_name, handling both Windows and Unix formats.
 * Unix format is: lang[_country][.charset][@modifier]
 * Windows format is: lang[-script][-country][_modifier]
 */
static void parse_locale_name( const WCHAR *str, struct locale_name *name )
{
    static const WCHAR sepW[] = {'-','_','.','@',0};
    static const WCHAR winsepW[] = {'-','_',0};
    static const WCHAR posixW[] = {'P','O','S','I','X',0};
    static const WCHAR cW[] = {'C',0};
    static const WCHAR latinW[] = {'l','a','t','i','n',0};
    static const WCHAR latnW[] = {'-','L','a','t','n',0};
    WCHAR *p;

    TRACE("%s\n", debugstr_w(str));

    name->country = name->charset = name->script = name->modifier = NULL;
    name->lcid = MAKELCID( MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT), SORT_DEFAULT );
    name->matches = 0;
    name->codepage = 0;
    name->win_name[0] = 0;
    lstrcpynW( name->lang, str, sizeof(name->lang)/sizeof(WCHAR) );

    if (!(p = strpbrkW( name->lang, sepW )))
    {
        if (!strcmpW( name->lang, posixW ) || !strcmpW( name->lang, cW ))
        {
            name->matches = 4;  /* perfect match for default English lcid */
            return;
        }
        strcpyW( name->win_name, name->lang );
    }
    else if (*p == '-')  /* Windows format */
    {
        strcpyW( name->win_name, name->lang );
        *p++ = 0;
        name->country = p;
        if (!(p = strpbrkW( p, winsepW ))) goto done;
        if (*p == '-')
        {
            *p++ = 0;
            name->script = name->country;
            name->country = p;
            if (!(p = strpbrkW( p, winsepW ))) goto done;
        }
        *p++ = 0;
        name->modifier = p;
    }
    else  /* Unix format */
    {
        if (*p == '_')
        {
            *p++ = 0;
            name->country = p;
            p = strpbrkW( p, sepW + 2 );
        }
        if (p && *p == '.')
        {
            *p++ = 0;
            name->charset = p;
            p = strchrW( p, '@' );
        }
        if (p)
        {
            *p++ = 0;
            name->modifier = p;
        }

        if (name->charset)
            name->codepage = find_charset( name->charset );

        /* rebuild a Windows name if possible */

        if (name->charset) goto done;  /* can't specify charset in Windows format */
        if (name->modifier && strcmpW( name->modifier, latinW ))
            goto done;  /* only Latn script supported for now */
        strcpyW( name->win_name, name->lang );
        if (name->modifier) strcatW( name->win_name, latnW );
        if (name->country)
        {
            p = name->win_name + strlenW(name->win_name);
            *p++ = '-';
            strcpyW( p, name->country );
        }
    }
done:
    {
        int i;

        for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
            find_locale_id_callback(0, (LPCWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE, supportedLocals[i], (LPARAM)name);
        }
    }
    // EnumResourceLanguagesW( kernel32_handle, (LPCWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE, find_locale_id_callback, (LPARAM)name );
}

/***********************************************************************
 *           is_genitive_name_supported
 *
 * Determine could LCTYPE basically support genitive name form or not.
 */
static BOOL is_genitive_name_supported( LCTYPE lctype )
{
    switch(lctype & 0xffff)
    {
    case LOCALE_SMONTHNAME1:
    case LOCALE_SMONTHNAME2:
    case LOCALE_SMONTHNAME3:
    case LOCALE_SMONTHNAME4:
    case LOCALE_SMONTHNAME5:
    case LOCALE_SMONTHNAME6:
    case LOCALE_SMONTHNAME7:
    case LOCALE_SMONTHNAME8:
    case LOCALE_SMONTHNAME9:
    case LOCALE_SMONTHNAME10:
    case LOCALE_SMONTHNAME11:
    case LOCALE_SMONTHNAME12:
    case LOCALE_SMONTHNAME13:
         return TRUE;
    default:
         return FALSE;
    }
}


/***********************************************************************
 *		GetUserDefaultLangID (KERNEL32.@)
 *
 * Get the default language Id for the current user.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LANGID of the default language for the current user.
 */
LANGID WINAPI GetUserDefaultLangID_k(void)
{
    return LANGIDFROMLCID(GetUserDefaultLCID_k());
}


/***********************************************************************
 *		GetSystemDefaultLangID (KERNEL32.@)
 *
 * Get the default language Id for the system.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LANGID of the default language for the system.
 */
LANGID WINAPI GetSystemDefaultLangID_k(void)
{
    return LANGIDFROMLCID(GetSystemDefaultLCID_k());
}


/***********************************************************************
 *		GetUserDefaultLCID (KERNEL32.@)
 *
 * Get the default locale Id for the current user.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LCID of the default locale for the current user.
 */
LCID WINAPI GetUserDefaultLCID_k(void)
{
    LCID lcid;
    NtQueryDefaultLocale_k( TRUE, &lcid );
    return lcid;
}


/***********************************************************************
 *		GetSystemDefaultLCID (KERNEL32.@)
 *
 * Get the default locale Id for the system.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LCID of the default locale for the system.
 */
LCID WINAPI GetSystemDefaultLCID_k(void)
{
    LCID lcid;
    NtQueryDefaultLocale_k( FALSE, &lcid );
    return lcid;
}

/***********************************************************************
 *		GetSystemDefaultLocaleName (KERNEL32.@)
 */
INT WINAPI GetSystemDefaultLocaleName_k(LPWSTR localename, INT len)
{
    LCID lcid = GetSystemDefaultLCID_k();
    return LCIDToLocaleName_k(lcid, localename, len, 0);
}

/***********************************************************************
 *		GetUserDefaultUILanguage (KERNEL32.@)
 *
 * Get the default user interface language Id for the current user.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LANGID of the default UI language for the current user.
 */
LANGID WINAPI GetUserDefaultUILanguage_k(void)
{
    LANGID lang;
    NtQueryDefaultUILanguage_k( &lang );
    return lang;
}


/***********************************************************************
 *		GetSystemDefaultUILanguage (KERNEL32.@)
 *
 * Get the default user interface language Id for the system.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The current LANGID of the default UI language for the system. This is
 *  typically the same language used during the installation process.
 */
LANGID WINAPI GetSystemDefaultUILanguage(void)
{
    LANGID lang;
    NtQueryInstallUILanguage_k( &lang );
    return lang;
}


/***********************************************************************
 *           LocaleNameToLCID  (KERNEL32.@)
 */
LCID WINAPI LocaleNameToLCID_k( LPCWSTR name, DWORD flags )
{
    struct locale_name locale_name;

    if (flags) FIXME( "unsupported flags %x\n", flags );

    if (name == LOCALE_NAME_USER_DEFAULT)
        return GetUserDefaultLCID_k();

    /* string parsing */
    parse_locale_name( name, &locale_name );

    TRACE( "found lcid %x for %s, matches %d\n",
           locale_name.lcid, debugstr_w(name), locale_name.matches );

    if (!locale_name.matches)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (locale_name.matches == 1)
        WARN( "locale %s not recognized, defaulting to %s\n",
              debugstr_w(name), debugstr_w(locale_name.lang) );

    return locale_name.lcid;
}


/***********************************************************************
 *           LCIDToLocaleName  (KERNEL32.@)
 */
INT WINAPI LCIDToLocaleName_k( LCID lcid, LPWSTR name, INT count, DWORD flags )
{
    if (flags) FIXME( "unsupported flags %x\n", flags );

    return GetLocaleInfoW_k( lcid, LOCALE_SNAME | LOCALE_NOUSEROVERRIDE, name, count );
}

/******************************************************************************
 *		GetLocaleInfoA (KERNEL32.@)
 *
 * Get information about an aspect of a locale.
 *
 * PARAMS
 *  lcid   [I] LCID of the locale
 *  lctype [I] LCTYPE_ flags from "winnls.h"
 *  buffer [O] Destination for the information
 *  len    [I] Length of buffer in characters
 *
 * RETURNS
 *  Success: The size of the data requested. If buffer is non-NULL, it is filled
 *           with the information.
 *  Failure: 0. Use GetLastError() to determine the cause.
 *
 * NOTES
 *  - LOCALE_NEUTRAL is equal to LOCALE_SYSTEM_DEFAULT
 *  - The string returned is NUL terminated, except for LOCALE_FONTSIGNATURE,
 *    which is a bit string.
 */
INT WINAPI GetLocaleInfoA_k( LCID lcid, LCTYPE lctype, LPSTR buffer, INT len )
{
    WCHAR *bufferW;
    INT lenW, ret;

    TRACE( "(lcid=0x%x,lctype=0x%x,%p,%d)\n", lcid, lctype, buffer, len );

    if (len < 0 || (len && !buffer))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }
    if (lctype & LOCALE_RETURN_GENITIVE_NAMES )
    {
        SetLastError_k( ERROR_INVALID_FLAGS );
        return 0;
    }

    if (!len) buffer = NULL;

    if (!(lenW = GetLocaleInfoW_k( lcid, lctype, NULL, 0 ))) return 0;

    if (!(bufferW = HeapAlloc_k( GetProcessHeap_k(), 0, lenW * sizeof(WCHAR) )))
    {
        SetLastError_k( ERROR_NOT_ENOUGH_MEMORY );
        return 0;
    }
    if ((ret = GetLocaleInfoW_k( lcid, lctype, bufferW, lenW )))
    {
        if ((lctype & LOCALE_RETURN_NUMBER) ||
            ((lctype & ~LOCALE_LOCALEINFOFLAGSMASK) == LOCALE_FONTSIGNATURE))
        {
            /* it's not an ASCII string, just bytes */
            ret *= sizeof(WCHAR);
            if (buffer)
            {
                if (ret <= len) memcpy( buffer, bufferW, ret );
                else
                {
                    SetLastError_k( ERROR_INSUFFICIENT_BUFFER );
                    ret = 0;
                }
            }
        }
        else
        {
            UINT codepage = CP_ACP;
            if (!(lctype & LOCALE_USE_CP_ACP)) codepage = get_lcid_codepage( lcid );
            ret = WideCharToMultiByte_k( codepage, 0, bufferW, ret, buffer, len, NULL, NULL );
        }
    }
    HeapFree_k( GetProcessHeap_k(), 0, bufferW );
    return ret;
}

static int get_value_base_by_lctype( LCTYPE lctype )
{
    return lctype == LOCALE_ILANGUAGE || lctype == LOCALE_IDEFAULTLANGUAGE ? 16 : 10;
}

/******************************************************************************
 *		GetLocaleInfoW (KERNEL32.@)
 *
 * See GetLocaleInfoA.
 */

INT WINAPI GetLocaleInfoW_k( LCID lcid, LCTYPE lctype, LPWSTR buffer, INT len )
{
    LANGID lang_id;
    INT ret;
    UINT lcflags;
    const WCHAR *p;

    if (len < 0 || (len && !buffer))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }
    if (lctype & LOCALE_RETURN_GENITIVE_NAMES &&
       !is_genitive_name_supported( lctype ))
    {
        SetLastError_k( ERROR_INVALID_FLAGS );
        return 0;
    }

    if (!len) buffer = NULL;

    lcid = ConvertDefaultLocale_k( lcid );

    lcflags = lctype & LOCALE_LOCALEINFOFLAGSMASK;
    lctype &= 0xffff;

    TRACE( "(lcid=0x%x,lctype=0x%x,%p,%d)\n", lcid, lctype, buffer, len );

    // now load it from kernel resources 

    lang_id = LANGIDFROMLCID( lcid );

    // replace SUBLANG_NEUTRAL by SUBLANG_DEFAULT 
    if (SUBLANGID(lang_id) == SUBLANG_NEUTRAL)
        lang_id = MAKELANGID(PRIMARYLANGID(lang_id), SUBLANG_DEFAULT);

    p = getLocalData(lang_id, lctype);
    if (!p)
        return 0;

    if (lcflags & LOCALE_RETURN_NUMBER) 
        ret = sizeof(UINT)/sizeof(WCHAR);    
    else
        ret = (lctype == LOCALE_FONTSIGNATURE) ? strlenW(p) : strlenW(p) + 1;

    if (!buffer) return ret;

    if (ret > len)
    {
        SetLastError_k( ERROR_INSUFFICIENT_BUFFER );
        return 0;
    }

    if (lcflags & LOCALE_RETURN_NUMBER)
    {
        UINT number;
        WCHAR *end;
        number = strtolW( p, &end, get_value_base_by_lctype( lctype ) );
        if (!*end)
            memcpy( buffer, &number, sizeof(number) );
        else  // invalid number 
        {
            SetLastError_k( ERROR_INVALID_FLAGS );
            ret = 0;
        }

        TRACE( "(lcid=0x%x,lctype=0x%x,%p,%d) returning number %d\n",
               lcid, lctype, buffer, len, number );
    }
    else
    {
        memcpy( buffer, p, ret * sizeof(WCHAR) );

        TRACE( "(lcid=0x%x,lctype=0x%x,%p,%d) returning %d %s\n",
               lcid, lctype, buffer, len, ret, debugstr_w(buffer) );
    }
    return ret;
}


/******************************************************************************
 *           GetLocaleInfoEx (KERNEL32.@)
 */
INT WINAPI GetLocaleInfoEx(LPCWSTR locale, LCTYPE info, LPWSTR buffer, INT len)
{
    LCID lcid = LocaleNameToLCID_k(locale, 0);

    TRACE("%s, lcid=0x%x, 0x%x\n", debugstr_w(locale), lcid, info);

    if (!lcid) return 0;

    /* special handling for neutral locale names */
    if (info == LOCALE_SNAME && strlenW(locale) == 2)
    {
        if (len && len < 3)
        {
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }

        if (len) strcpyW(buffer, locale);
        return 3;
    }

    return GetLocaleInfoW_k(lcid, info, buffer, len);
}

/******************************************************************************
 *		SetLocaleInfoA	[KERNEL32.@]
 *
 * Set information about an aspect of a locale.
 *
 * PARAMS
 *  lcid   [I] LCID of the locale
 *  lctype [I] LCTYPE_ flags from "winnls.h"
 *  data   [I] Information to set
 *
 * RETURNS
 *  Success: TRUE. The information given will be returned by GetLocaleInfoA()
 *           whenever it is called without LOCALE_NOUSEROVERRIDE.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 *
 * NOTES
 *  - Values are only be set for the current user locale; the system locale
 *  settings cannot be changed.
 *  - Any settings changed by this call are lost when the locale is changed by
 *  the control panel (in Wine, this happens every time you change LANG).
 *  - The native implementation of this function does not check that lcid matches
 *  the current user locale, and simply sets the new values. Wine warns you in
 *  this case, but behaves the same.
 */
BOOL WINAPI SetLocaleInfoA_k(LCID lcid, LCTYPE lctype, LPCSTR data)
{
    UINT codepage = CP_ACP;
    WCHAR *strW;
    DWORD len;
    BOOL ret;

    if (!(lctype & LOCALE_USE_CP_ACP)) codepage = get_lcid_codepage( lcid );

    if (!data)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    len = MultiByteToWideChar_k( codepage, 0, data, -1, NULL, 0 );
    if (!(strW = HeapAlloc_k( GetProcessHeap_k(), 0, len * sizeof(WCHAR) )))
    {
        SetLastError_k( ERROR_NOT_ENOUGH_MEMORY );
        return FALSE;
    }
    MultiByteToWideChar_k( codepage, 0, data, -1, strW, len );
    ret = SetLocaleInfoW_k( lcid, lctype, strW );
    HeapFree_k( GetProcessHeap_k(), 0, strW );
    return ret;
}


/******************************************************************************
 *		SetLocaleInfoW	(KERNEL32.@)
 *
 * See SetLocaleInfoA.
 */
BOOL WINAPI SetLocaleInfoW_k( LCID lcid, LCTYPE lctype, LPCWSTR data )
{
    return TRUE;
}

/******************************************************************************
 *              GetACP   (KERNEL32.@)
 *
 * Get the current Ansi code page Id for the system.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *    The current Ansi code page identifier for the system.
 */
UINT WINAPI GetACP_k(void)
{
    assert( ansi_cptable );
    return ansi_cptable->info.codepage;
}


/******************************************************************************
 *              SetCPGlobal   (KERNEL32.@)
 *
 * Set the current Ansi code page Id for the system.
 *
 * PARAMS
 *    acp [I] code page ID to be the new ACP.
 *
 * RETURNS
 *    The previous ACP.
 */
UINT WINAPI SetCPGlobal_k( UINT acp )
{
    UINT ret = GetACP_k();
    const union cptable *new_cptable = wine_cp_get_table( acp );

    if (new_cptable) ansi_cptable = new_cptable;
    return ret;
}


/***********************************************************************
 *              GetOEMCP   (KERNEL32.@)
 *
 * Get the current OEM code page Id for the system.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *    The current OEM code page identifier for the system.
 */
UINT WINAPI GetOEMCP_k(void)
{
    assert( oem_cptable );
    return oem_cptable->info.codepage;
}


/***********************************************************************
 *           IsValidCodePage   (KERNEL32.@)
 *
 * Determine if a given code page identifier is valid.
 *
 * PARAMS
 *  codepage [I] Code page Id to verify.
 *
 * RETURNS
 *  TRUE, If codepage is valid and available on the system,
 *  FALSE otherwise.
 */
BOOL WINAPI IsValidCodePage_k( UINT codepage )
{
    switch(codepage) {
    case CP_UTF7:
    case CP_UTF8:
        return TRUE;
    default:
        return wine_cp_get_table( codepage ) != NULL;
    }
}


/***********************************************************************
 *           IsDBCSLeadByteEx   (KERNEL32.@)
 *
 * Determine if a character is a lead byte in a given code page.
 *
 * PARAMS
 *  codepage [I] Code page for the test.
 *  testchar [I] Character to test
 *
 * RETURNS
 *  TRUE, if testchar is a lead byte in codepage,
 *  FALSE otherwise.
 */
BOOL WINAPI IsDBCSLeadByteEx_k( UINT codepage, BYTE testchar )
{
    const union cptable *table = get_codepage_table( codepage );
    return table && wine_is_dbcs_leadbyte( table, testchar );
}


/***********************************************************************
 *           IsDBCSLeadByte   (KERNEL32.@)
 *           IsDBCSLeadByte   (KERNEL.207)
 *
 * Determine if a character is a lead byte.
 *
 * PARAMS
 *  testchar [I] Character to test
 *
 * RETURNS
 *  TRUE, if testchar is a lead byte in the ANSI code page,
 *  FALSE otherwise.
 */
BOOL WINAPI IsDBCSLeadByte_k( BYTE testchar )
{
    if (!ansi_cptable) return FALSE;
    return wine_is_dbcs_leadbyte( ansi_cptable, testchar );
}


/***********************************************************************
 *           GetCPInfo   (KERNEL32.@)
 *
 * Get information about a code page.
 *
 * PARAMS
 *  codepage [I] Code page number
 *  cpinfo   [O] Destination for code page information
 *
 * RETURNS
 *  Success: TRUE. cpinfo is updated with the information about codepage.
 *  Failure: FALSE, if codepage is invalid or cpinfo is NULL.
 */
BOOL WINAPI GetCPInfo_k( UINT codepage, LPCPINFO cpinfo )
{
    const union cptable *table;

    if (!cpinfo)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (!(table = get_codepage_table( codepage )))
    {
        switch(codepage)
        {
            case CP_UTF7:
            case CP_UTF8:
                cpinfo->DefaultChar[0] = 0x3f;
                cpinfo->DefaultChar[1] = 0;
                cpinfo->LeadByte[0] = cpinfo->LeadByte[1] = 0;
                cpinfo->MaxCharSize = (codepage == CP_UTF7) ? 5 : 4;
                return TRUE;
        }

        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    if (table->info.def_char & 0xff00)
    {
        cpinfo->DefaultChar[0] = (table->info.def_char & 0xff00) >> 8;
        cpinfo->DefaultChar[1] = table->info.def_char & 0x00ff;
    }
    else
    {
        cpinfo->DefaultChar[0] = table->info.def_char & 0xff;
        cpinfo->DefaultChar[1] = 0;
    }
    if ((cpinfo->MaxCharSize = table->info.char_size) == 2)
        memcpy( cpinfo->LeadByte, table->dbcs.lead_bytes, sizeof(cpinfo->LeadByte) );
    else
        cpinfo->LeadByte[0] = cpinfo->LeadByte[1] = 0;

    return TRUE;
}

/***********************************************************************
 *           GetCPInfoExA   (KERNEL32.@)
 *
 * Get extended information about a code page.
 *
 * PARAMS
 *  codepage [I] Code page number
 *  dwFlags  [I] Reserved, must to 0.
 *  cpinfo   [O] Destination for code page information
 *
 * RETURNS
 *  Success: TRUE. cpinfo is updated with the information about codepage.
 *  Failure: FALSE, if codepage is invalid or cpinfo is NULL.
 */
BOOL WINAPI GetCPInfoExA_k( UINT codepage, DWORD dwFlags, LPCPINFOEXA cpinfo )
{
    CPINFOEXW cpinfoW;

    if (!GetCPInfoExW_k( codepage, dwFlags, &cpinfoW ))
      return FALSE;

    /* the layout is the same except for CodePageName */
    memcpy(cpinfo, &cpinfoW, sizeof(CPINFOEXA));
    WideCharToMultiByte_k(CP_ACP, 0, cpinfoW.CodePageName, -1, cpinfo->CodePageName, sizeof(cpinfo->CodePageName), NULL, NULL);
    return TRUE;
}

/***********************************************************************
 *           GetCPInfoExW   (KERNEL32.@)
 *
 * Unicode version of GetCPInfoExA.
 */
BOOL WINAPI GetCPInfoExW_k( UINT codepage, DWORD dwFlags, LPCPINFOEXW cpinfo )
{
    if (!GetCPInfo_k( codepage, (LPCPINFO)cpinfo ))
      return FALSE;

    switch(codepage)
    {
        case CP_UTF7:
        {
            static const WCHAR utf7[] = {'U','n','i','c','o','d','e',' ','(','U','T','F','-','7',')',0};

            cpinfo->CodePage = CP_UTF7;
            cpinfo->UnicodeDefaultChar = 0x3f;
            strcpyW(cpinfo->CodePageName, utf7);
            break;
        }

        case CP_UTF8:
        {
            static const WCHAR utf8[] = {'U','n','i','c','o','d','e',' ','(','U','T','F','-','8',')',0};

            cpinfo->CodePage = CP_UTF8;
            cpinfo->UnicodeDefaultChar = 0x3f;
            strcpyW(cpinfo->CodePageName, utf8);
            break;
        }

        default:
        {
            const union cptable *table = get_codepage_table( codepage );

            cpinfo->CodePage = table->info.codepage;
            cpinfo->UnicodeDefaultChar = table->info.def_unicode_char;
            MultiByteToWideChar_k( CP_ACP, 0, table->info.name, -1, cpinfo->CodePageName,
                                 sizeof(cpinfo->CodePageName)/sizeof(WCHAR));
            break;
        }
    }
    return TRUE;
}

/***********************************************************************
 *              EnumSystemCodePagesA   (KERNEL32.@)
 *
 * Call a user defined function for every code page installed on the system.
 *
 * PARAMS
 *   lpfnCodePageEnum [I] User CODEPAGE_ENUMPROC to call with each found code page
 *   flags            [I] Reserved, set to 0.
 *
 * RETURNS
 *  TRUE, If all code pages have been enumerated, or
 *  FALSE if lpfnCodePageEnum returned FALSE to stop the enumeration.
 */
BOOL WINAPI EnumSystemCodePagesA_k( CODEPAGE_ENUMPROCA lpfnCodePageEnum, DWORD flags )
{
    const union cptable *table;
    char buffer[10];
    int index = 0;

    for (;;)
    {
        if (!(table = wine_cp_enum_table( index++ ))) break;
        sprintf( buffer, "%d", table->info.codepage );
        if (!lpfnCodePageEnum( buffer )) break;
    }
    return TRUE;
}


/***********************************************************************
 *              EnumSystemCodePagesW   (KERNEL32.@)
 *
 * See EnumSystemCodePagesA.
 */
BOOL WINAPI EnumSystemCodePagesW_k( CODEPAGE_ENUMPROCW lpfnCodePageEnum, DWORD flags )
{
    const union cptable *table;
    WCHAR buffer[10], *p;
    int page, index = 0;

    for (;;)
    {
        if (!(table = wine_cp_enum_table( index++ ))) break;
        p = buffer + sizeof(buffer)/sizeof(WCHAR);
        *--p = 0;
        page = table->info.codepage;
        do
        {
            *--p = '0' + (page % 10);
            page /= 10;
        } while( page );
        if (!lpfnCodePageEnum( p )) break;
    }
    return TRUE;
}


/***********************************************************************
 *              MultiByteToWideChar   (KERNEL32.@)
 *
 * Convert a multibyte character string into a Unicode string.
 *
 * PARAMS
 *   page   [I] Codepage character set to convert from
 *   flags  [I] Character mapping flags
 *   src    [I] Source string buffer
 *   srclen [I] Length of src (in bytes), or -1 if src is NUL terminated
 *   dst    [O] Destination buffer
 *   dstlen [I] Length of dst (in WCHARs), or 0 to compute the required length
 *
 * RETURNS
 *   Success: If dstlen > 0, the number of characters written to dst.
 *            If dstlen == 0, the number of characters needed to perform the
 *            conversion. In both cases the count includes the terminating NUL.
 *   Failure: 0. Use GetLastError() to determine the cause. Possible errors are
 *            ERROR_INSUFFICIENT_BUFFER, if not enough space is available in dst
 *            and dstlen != 0; ERROR_INVALID_PARAMETER,  if an invalid parameter
 *            is passed, and ERROR_NO_UNICODE_TRANSLATION if no translation is
 *            possible for src.
 */
INT WINAPI MultiByteToWideChar_k( UINT page, DWORD flags, LPCSTR src, INT srclen,
                                LPWSTR dst, INT dstlen )
{
    const union cptable *table;
    int ret;

    if (!src || !srclen || (!dst && dstlen))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if (srclen < 0) srclen = strlen(src) + 1;

    switch(page)
    {
    case CP_SYMBOL:
        if (flags)
        {
            SetLastError_k( ERROR_INVALID_FLAGS );
            return 0;
        }
        ret = wine_cpsymbol_mbstowcs( src, srclen, dst, dstlen );
        break;
    case CP_UTF7:
        if (flags)
        {
            SetLastError_k( ERROR_INVALID_FLAGS );
            return 0;
        }
        FIXME("UTF-7 not supported\n");
        SetLastError_k( ERROR_CALL_NOT_IMPLEMENTED );
        return 0;
    case CP_UNIXCP:
        if (unix_cptable)
        {
            ret = wine_cp_mbstowcs( unix_cptable, flags, src, srclen, dst, dstlen );
            break;
        }
#ifdef __APPLE__
        flags |= MB_COMPOSITE;  /* work around broken Mac OS X filesystem that enforces decomposed Unicode */
#endif
        /* fall through */
    case CP_UTF8:
        ret = wine_utf8_mbstowcs( flags, src, srclen, dst, dstlen );
        break;
    default:
        if (!(table = get_codepage_table( page )))
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = wine_cp_mbstowcs( table, flags, src, srclen, dst, dstlen );
        break;
    }

    if (ret < 0)
    {
        switch(ret)
        {
        case -1: SetLastError_k( ERROR_INSUFFICIENT_BUFFER ); break;
        case -2: SetLastError_k( ERROR_NO_UNICODE_TRANSLATION ); break;
        }
        ret = 0;
    }
    // TRACE("cp %d %s -> %s, ret = %d\n", page, debugstr_an(src, srclen), debugstr_wn(dst, ret), ret);
    return ret;
}


/***********************************************************************
 *              WideCharToMultiByte   (KERNEL32.@)
 *
 * Convert a Unicode character string into a multibyte string.
 *
 * PARAMS
 *   page    [I] Code page character set to convert to
 *   flags   [I] Mapping Flags (MB_ constants from "winnls.h").
 *   src     [I] Source string buffer
 *   srclen  [I] Length of src (in WCHARs), or -1 if src is NUL terminated
 *   dst     [O] Destination buffer
 *   dstlen  [I] Length of dst (in bytes), or 0 to compute the required length
 *   defchar [I] Default character to use for conversion if no exact
 *		    conversion can be made
 *   used    [O] Set if default character was used in the conversion
 *
 * RETURNS
 *   Success: If dstlen > 0, the number of characters written to dst.
 *            If dstlen == 0, number of characters needed to perform the
 *            conversion. In both cases the count includes the terminating NUL.
 *   Failure: 0. Use GetLastError() to determine the cause. Possible errors are
 *            ERROR_INSUFFICIENT_BUFFER, if not enough space is available in dst
 *            and dstlen != 0, and ERROR_INVALID_PARAMETER, if an invalid
 *            parameter was given.
 */
INT WINAPI WideCharToMultiByte_k( UINT page, DWORD flags, LPCWSTR src, INT srclen,
                                LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used )
{
    const union cptable *table;
    int ret, used_tmp;

    if (!src || !srclen || (!dst && dstlen))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if (srclen < 0) srclen = strlenW(src) + 1;

    switch(page)
    {
    case CP_SYMBOL:
        /* when using CP_SYMBOL, ERROR_INVALID_FLAGS takes precedence */
        if (flags)
        {
            SetLastError_k( ERROR_INVALID_FLAGS );
            return 0;
        }
        if (defchar || used)
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = wine_cpsymbol_wcstombs( src, srclen, dst, dstlen );
        break;
    case CP_UTF7:
        /* when using CP_UTF7, ERROR_INVALID_PARAMETER takes precedence */
        if (defchar || used)
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
        if (flags)
        {
            SetLastError_k( ERROR_INVALID_FLAGS );
            return 0;
        }
        FIXME("UTF-7 not supported\n");
        SetLastError_k( ERROR_CALL_NOT_IMPLEMENTED );
        return 0;
    case CP_UNIXCP:
        if (unix_cptable)
        {
            ret = wine_cp_wcstombs( unix_cptable, flags, src, srclen, dst, dstlen,
                                    defchar, used ? &used_tmp : NULL );
            break;
        }
        /* fall through */
    case CP_UTF8:
        if (defchar || used)
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = wine_utf8_wcstombs( flags, src, srclen, dst, dstlen );
        break;
    default:
        if (!(table = get_codepage_table( page )))
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
        ret = wine_cp_wcstombs( table, flags, src, srclen, dst, dstlen,
                                defchar, used ? &used_tmp : NULL );
        if (used) *used = used_tmp;
        break;
    }

    if (ret < 0)
    {
        switch(ret)
        {
        case -1: SetLastError_k( ERROR_INSUFFICIENT_BUFFER ); break;
        case -2: SetLastError_k( ERROR_NO_UNICODE_TRANSLATION ); break;
        }
        ret = 0;
    }
    // TRACE("cp %d %s -> %s, ret = %d\n", page, debugstr_wn(src, srclen), debugstr_an(dst, ret), ret);
    return ret;
}


/***********************************************************************
 *           GetThreadLocale    (KERNEL32.@)
 *
 * Get the current threads locale.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  The LCID currently associated with the calling thread.
 */
LCID WINAPI GetThreadLocale_k(void)
{
    LCID ret = readd(NtCurrentTeb_k()+TEB_LOCALE);
    if (!ret) {
        ret = GetUserDefaultLCID_k();
        writed(NtCurrentTeb_k()+TEB_LOCALE, ret);
    }
    return ret;
}

/**********************************************************************
 *           SetThreadLocale    (KERNEL32.@)
 *
 * Set the current threads locale.
 *
 * PARAMS
 *  lcid [I] LCID of the locale to set
 *
 * RETURNS
 *  Success: TRUE. The threads locale is set to lcid.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI SetThreadLocale_k( LCID lcid )
{
    TRACE("(0x%04X)\n", lcid);

    lcid = ConvertDefaultLocale_k(lcid);

    if (lcid != GetThreadLocale_k())
    {
        if (!IsValidLocale_k(lcid, LCID_SUPPORTED))
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        writed(NtCurrentTeb_k()+TEB_LOCALE, lcid);
    }
    return TRUE;
}

/**********************************************************************
 *           SetThreadUILanguage    (KERNEL32.@)
 *
 * Set the current threads UI language.
 *
 * PARAMS
 *  langid [I] LANGID of the language to set, or 0 to use
 *             the available language which is best supported
 *             for console applications
 *
 * RETURNS
 *  Success: The return value is the same as the input value.
 *  Failure: The return value differs from the input value.
 *           Use GetLastError() to determine the cause.
 */
LANGID WINAPI SetThreadUILanguage_k( LANGID langid )
{
    TRACE("(0x%04x) stub - returning success\n", langid);
    return langid;
}

/******************************************************************************
 *		ConvertDefaultLocale (KERNEL32.@)
 *
 * Convert a default locale identifier into a real identifier.
 *
 * PARAMS
 *  lcid [I] LCID identifier of the locale to convert
 *
 * RETURNS
 *  lcid unchanged, if not a default locale or its sublanguage is
 *   not SUBLANG_NEUTRAL.
 *  GetSystemDefaultLCID(), if lcid == LOCALE_SYSTEM_DEFAULT.
 *  GetUserDefaultLCID(), if lcid == LOCALE_USER_DEFAULT or LOCALE_NEUTRAL.
 *  Otherwise, lcid with sublanguage changed to SUBLANG_DEFAULT.
 */
LCID WINAPI ConvertDefaultLocale_k( LCID lcid )
{
    LANGID langid;

    switch (lcid)
    {
    case LOCALE_SYSTEM_DEFAULT:
        lcid = GetSystemDefaultLCID_k();
        break;
    case LOCALE_USER_DEFAULT:
    case LOCALE_NEUTRAL:
        lcid = GetUserDefaultLCID_k();
        break;
    default:
        /* Replace SUBLANG_NEUTRAL with SUBLANG_DEFAULT */
        langid = LANGIDFROMLCID(lcid);
        if (SUBLANGID(langid) == SUBLANG_NEUTRAL)
        {
          langid = MAKELANGID(PRIMARYLANGID(langid), SUBLANG_DEFAULT);
          lcid = MAKELCID(langid, SORTIDFROMLCID(lcid));
        }
    }
    return lcid;
}


/******************************************************************************
 *           IsValidLocale   (KERNEL32.@)
 *
 * Determine if a locale is valid.
 *
 * PARAMS
 *  lcid  [I] LCID of the locale to check
 *  flags [I] LCID_SUPPORTED = Valid, LCID_INSTALLED = Valid and installed on the system
 *
 * RETURNS
 *  TRUE,  if lcid is valid,
 *  FALSE, otherwise.
 *
 * NOTES
 *  Wine does not currently make the distinction between supported and installed. All
 *  languages supported are installed by default.
 */

BOOL WINAPI IsValidLocale_k( LCID lcid, DWORD flags )
{
    int i;

    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        if (LANGIDFROMLCID(lcid)==supportedLocals[i])
            return TRUE;
    }
    return FALSE;
}


/******************************************************************************
 *           IsValidLocaleName   (KERNEL32.@)
 */
BOOL WINAPI IsValidLocaleName_k( LPCWSTR locale )
{
    struct locale_name locale_name;

    /* string parsing */
    parse_locale_name( locale, &locale_name );

    TRACE( "found lcid %x for %s, matches %d\n",
           locale_name.lcid, debugstr_w(locale), locale_name.matches );

    return locale_name.matches > 0;
}

static BOOL CALLBACK enum_lang_proc_a( HMODULE hModule, LPCSTR type,
                                       LPCSTR name, WORD LangID, LONG_PTR lParam )
{
    LOCALE_ENUMPROCA lpfnLocaleEnum = (LOCALE_ENUMPROCA)lParam;
    char buf[20];

    sprintf(buf, "%08x", (UINT)LangID);
    return lpfnLocaleEnum( buf );
}

static BOOL CALLBACK enum_lang_proc_w( HMODULE hModule, LPCWSTR type,
                                       LPCWSTR name, WORD LangID, LONG_PTR lParam )
{
    static const WCHAR formatW[] = {'%','0','8','x',0};
    LOCALE_ENUMPROCW lpfnLocaleEnum = (LOCALE_ENUMPROCW)lParam;
    WCHAR buf[20];
    sprintfW( buf, formatW, (UINT)LangID );
    return lpfnLocaleEnum( buf );
}

/******************************************************************************
 *           EnumSystemLocalesA  (KERNEL32.@)
 *
 * Call a users function for each locale available on the system.
 *
 * PARAMS
 *  lpfnLocaleEnum [I] Callback function to call for each locale
 *  dwFlags        [I] LOCALE_SUPPORTED=All supported, LOCALE_INSTALLED=Installed only
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */

BOOL WINAPI EnumSystemLocalesA_k( LOCALE_ENUMPROCA lpfnLocaleEnum, DWORD dwFlags )
{
    int i;

    TRACE("(%p,%08x)\n", lpfnLocaleEnum, dwFlags);
    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        enum_lang_proc_a(0, (LPSTR)RT_STRING, (LPCSTR)LOCALE_ILANGUAGE, supportedLocals[i], (LONG_PTR)lpfnLocaleEnum);
    }
    return TRUE;
}


/******************************************************************************
 *           EnumSystemLocalesW  (KERNEL32.@)
 *
 * See EnumSystemLocalesA.
 */

BOOL WINAPI EnumSystemLocalesW_k( LOCALE_ENUMPROCW lpfnLocaleEnum, DWORD dwFlags )
{
    int i;

    TRACE("(%p,%08x)\n", lpfnLocaleEnum, dwFlags);
    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        enum_lang_proc_w(0, (LPWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE, supportedLocals[i], (LONG_PTR)lpfnLocaleEnum);
    }
    return TRUE;
}

struct enum_locale_ex_data
{
    LOCALE_ENUMPROCEX proc;
    DWORD             flags;
    LPARAM            lparam;
};

static BOOL CALLBACK enum_locale_ex_proc( HMODULE module, LPCWSTR type,
                                          LPCWSTR name, WORD lang, LONG_PTR lparam )
{
    struct enum_locale_ex_data *data = (struct enum_locale_ex_data *)lparam;
    WCHAR buffer[256];
    DWORD neutral;
    unsigned int flags;

    GetLocaleInfoW_k( MAKELCID( lang, SORT_DEFAULT ), LOCALE_SNAME | LOCALE_NOUSEROVERRIDE,
                    buffer, sizeof(buffer) / sizeof(WCHAR) );
    if (!GetLocaleInfoW_k( MAKELCID( lang, SORT_DEFAULT ),
                         LOCALE_INEUTRAL | LOCALE_NOUSEROVERRIDE | LOCALE_RETURN_NUMBER,
                         (LPWSTR)&neutral, sizeof(neutral) / sizeof(WCHAR) ))
        neutral = 0;
    flags = LOCALE_WINDOWS;
    flags |= neutral ? LOCALE_NEUTRALDATA : LOCALE_SPECIFICDATA;
    if (data->flags && ~(data->flags & flags)) return TRUE;
    return data->proc( buffer, flags, data->lparam );
}

/******************************************************************************
 *           EnumSystemLocalesEx  (KERNEL32.@)
 */
BOOL WINAPI EnumSystemLocalesEx_k( LOCALE_ENUMPROCEX proc, DWORD flags, LPARAM lparam, LPVOID reserved )
{
    struct enum_locale_ex_data data;
    int i;

    if (reserved)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    data.proc   = proc;
    data.flags  = flags;
    data.lparam = lparam;

    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        enum_locale_ex_proc(0, (LPWSTR)RT_STRING, (LPCWSTR)MAKEINTRESOURCE((LOCALE_SNAME >> 4) + 1), supportedLocals[i], (LONG_PTR)&data);
    }

    //EnumResourceLanguagesW( kernel32_handle, (LPCWSTR)RT_STRING, (LPCWSTR)MAKEINTRESOURCE((LOCALE_SNAME >> 4) + 1), enum_locale_ex_proc, (LONG_PTR)&data );
    return TRUE;
}

/***********************************************************************
 *           VerLanguageNameA  (KERNEL32.@)
 *
 * Get the name of a language.
 *
 * PARAMS
 *  wLang  [I] LANGID of the language
 *  szLang [O] Destination for the language name
 *
 * RETURNS
 *  Success: The size of the language name. If szLang is non-NULL, it is filled
 *           with the name.
 *  Failure: 0. Use GetLastError() to determine the cause.
 *
 */
DWORD WINAPI VerLanguageNameA_k( DWORD wLang, LPSTR szLang, DWORD nSize )
{
    return GetLocaleInfoA_k( MAKELCID(wLang, SORT_DEFAULT), LOCALE_SENGLANGUAGE, szLang, nSize );
}


/***********************************************************************
 *           VerLanguageNameW  (KERNEL32.@)
 *
 * See VerLanguageNameA.
 */
DWORD WINAPI VerLanguageNameW_k( DWORD wLang, LPWSTR szLang, DWORD nSize )
{
    return GetLocaleInfoW_k( MAKELCID(wLang, SORT_DEFAULT), LOCALE_SENGLANGUAGE, szLang, nSize );
}


/******************************************************************************
 *           GetStringTypeW    (KERNEL32.@)
 *
 * See GetStringTypeA.
 */
BOOL WINAPI GetStringTypeW_k( DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
    static const unsigned char type2_map[16] =
    {
        C2_NOTAPPLICABLE,      /* unassigned */
        C2_LEFTTORIGHT,        /* L */
        C2_RIGHTTOLEFT,        /* R */
        C2_EUROPENUMBER,       /* EN */
        C2_EUROPESEPARATOR,    /* ES */
        C2_EUROPETERMINATOR,   /* ET */
        C2_ARABICNUMBER,       /* AN */
        C2_COMMONSEPARATOR,    /* CS */
        C2_BLOCKSEPARATOR,     /* B */
        C2_SEGMENTSEPARATOR,   /* S */
        C2_WHITESPACE,         /* WS */
        C2_OTHERNEUTRAL,       /* ON */
        C2_RIGHTTOLEFT,        /* AL */
        C2_NOTAPPLICABLE,      /* NSM */
        C2_NOTAPPLICABLE,      /* BN */
        C2_OTHERNEUTRAL        /* LRE, LRO, RLE, RLO, PDF */
    };

    if (count == -1) count = strlenW(src) + 1;
    switch(type)
    {
    case CT_CTYPE1:
        while (count--) *chartype++ = get_char_typeW( *src++ ) & 0xfff;
        break;
    case CT_CTYPE2:
        while (count--) *chartype++ = type2_map[get_char_typeW( *src++ ) >> 12];
        break;
    case CT_CTYPE3:
    {
        WARN("CT_CTYPE3: semi-stub.\n");
        while (count--)
        {
            int c = *src;
            WORD type1, type3 = 0; /* C3_NOTAPPLICABLE */

            type1 = get_char_typeW( *src++ ) & 0xfff;
            /* try to construct type3 from type1 */
            if(type1 & C1_SPACE) type3 |= C3_SYMBOL;
            if(type1 & C1_ALPHA) type3 |= C3_ALPHA;
            if ((c>=0x30A0)&&(c<=0x30FF)) type3 |= C3_KATAKANA;
            if ((c>=0x3040)&&(c<=0x309F)) type3 |= C3_HIRAGANA;
            if ((c>=0x4E00)&&(c<=0x9FAF)) type3 |= C3_IDEOGRAPH;
            if ((c>=0x0600)&&(c<=0x06FF)) type3 |= C3_KASHIDA;
            if ((c>=0x3000)&&(c<=0x303F)) type3 |= C3_SYMBOL;

            if ((c>=0xFF00)&&(c<=0xFF60)) type3 |= C3_FULLWIDTH;
            if ((c>=0xFF00)&&(c<=0xFF20)) type3 |= C3_SYMBOL;
            if ((c>=0xFF3B)&&(c<=0xFF40)) type3 |= C3_SYMBOL;
            if ((c>=0xFF5B)&&(c<=0xFF60)) type3 |= C3_SYMBOL;
            if ((c>=0xFF21)&&(c<=0xFF3A)) type3 |= C3_ALPHA;
            if ((c>=0xFF41)&&(c<=0xFF5A)) type3 |= C3_ALPHA;
            if ((c>=0xFFE0)&&(c<=0xFFE6)) type3 |= C3_FULLWIDTH;
            if ((c>=0xFFE0)&&(c<=0xFFE6)) type3 |= C3_SYMBOL;

            if ((c>=0xFF61)&&(c<=0xFFDC)) type3 |= C3_HALFWIDTH;
            if ((c>=0xFF61)&&(c<=0xFF64)) type3 |= C3_SYMBOL;
            if ((c>=0xFF65)&&(c<=0xFF9F)) type3 |= C3_KATAKANA;
            if ((c>=0xFF65)&&(c<=0xFF9F)) type3 |= C3_ALPHA;
            if ((c>=0xFFE8)&&(c<=0xFFEE)) type3 |= C3_HALFWIDTH;
            if ((c>=0xFFE8)&&(c<=0xFFEE)) type3 |= C3_SYMBOL;
            *chartype++ = type3;
        }
        break;
    }
    default:
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    return TRUE;
}


/******************************************************************************
 *           GetStringTypeExW    (KERNEL32.@)
 *
 * See GetStringTypeExA.
 */
BOOL WINAPI GetStringTypeExW_k( LCID locale, DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
    /* locale is ignored for Unicode */
    return GetStringTypeW_k( type, src, count, chartype );
}


/******************************************************************************
 *           GetStringTypeA    (KERNEL32.@)
 *
 * Get characteristics of the characters making up a string.
 *
 * PARAMS
 *  locale   [I] Locale Id for the string
 *  type     [I] CT_CTYPE1 = classification, CT_CTYPE2 = directionality, CT_CTYPE3 = typographic info
 *  src      [I] String to analyse
 *  count    [I] Length of src in chars, or -1 if src is NUL terminated
 *  chartype [O] Destination for the calculated characteristics
 *
 * RETURNS
 *  Success: TRUE. chartype is filled with the requested characteristics of each char
 *           in src.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI GetStringTypeA_k( LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype )
{
    UINT cp;
    INT countW;
    LPWSTR srcW;
    BOOL ret = FALSE;

    if(count == -1) count = strlen(src) + 1;

    if (!(cp = get_lcid_codepage( locale )))
    {
        FIXME("For locale %04x using current ANSI code page\n", locale);
        cp = GetACP_k();
    }

    countW = MultiByteToWideChar_k(cp, 0, src, count, NULL, 0);
    if((srcW = HeapAlloc_k(GetProcessHeap_k(), 0, countW * sizeof(WCHAR))))
    {
        MultiByteToWideChar_k(cp, 0, src, count, srcW, countW);
    /*
     * NOTE: the target buffer has 1 word for each CHARACTER in the source
     * string, with multibyte characters there maybe be more bytes in count
     * than character space in the buffer!
     */
        ret = GetStringTypeW_k(type, srcW, countW, chartype);
        HeapFree_k(GetProcessHeap_k(), 0, srcW);
    }
    return ret;
}

/******************************************************************************
 *           GetStringTypeExA    (KERNEL32.@)
 *
 * Get characteristics of the characters making up a string.
 *
 * PARAMS
 *  locale   [I] Locale Id for the string
 *  type     [I] CT_CTYPE1 = classification, CT_CTYPE2 = directionality, CT_CTYPE3 = typographic info
 *  src      [I] String to analyse
 *  count    [I] Length of src in chars, or -1 if src is NUL terminated
 *  chartype [O] Destination for the calculated characteristics
 *
 * RETURNS
 *  Success: TRUE. chartype is filled with the requested characteristics of each char
 *           in src.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI GetStringTypeExA_k( LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype )
{
    return GetStringTypeA_k(locale, type, src, count, chartype);
}

/*************************************************************************
 *           LCMapStringEx   (KERNEL32.@)
 *
 * Map characters in a locale sensitive string.
 *
 * PARAMS
 *  name     [I] Locale name for the conversion.
 *  flags    [I] Flags controlling the mapping (LCMAP_ constants from "winnls.h")
 *  src      [I] String to map
 *  srclen   [I] Length of src in chars, or -1 if src is NUL terminated
 *  dst      [O] Destination for mapped string
 *  dstlen   [I] Length of dst in characters
 *  version  [I] reserved, must be NULL
 *  reserved [I] reserved, must be NULL
 *  lparam   [I] reserved, must be 0
 *
 * RETURNS
 *  Success: The length of the mapped string in dst, including the NUL terminator.
 *  Failure: 0. Use GetLastError() to determine the cause.
 */
INT WINAPI LCMapStringEx_k(LPCWSTR name, DWORD flags, LPCWSTR src, INT srclen, LPWSTR dst, INT dstlen,
                         LPNLSVERSIONINFO version, LPVOID reserved, LPARAM lparam)
{
    LPWSTR dst_ptr;

    if (version) FIXME("unsupported version structure %p\n", version);
    if (reserved) FIXME("unsupported reserved pointer %p\n", reserved);
    if (lparam) FIXME("unsupported lparam %lx\n", lparam);

    if (!src || !srclen || dstlen < 0)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    /* mutually exclusive flags */
    if ((flags & (LCMAP_LOWERCASE | LCMAP_UPPERCASE)) == (LCMAP_LOWERCASE | LCMAP_UPPERCASE) ||
        (flags & (LCMAP_HIRAGANA | LCMAP_KATAKANA)) == (LCMAP_HIRAGANA | LCMAP_KATAKANA) ||
        (flags & (LCMAP_HALFWIDTH | LCMAP_FULLWIDTH)) == (LCMAP_HALFWIDTH | LCMAP_FULLWIDTH) ||
        (flags & (LCMAP_TRADITIONAL_CHINESE | LCMAP_SIMPLIFIED_CHINESE)) == (LCMAP_TRADITIONAL_CHINESE | LCMAP_SIMPLIFIED_CHINESE))
    {
        SetLastError_k(ERROR_INVALID_FLAGS);
        return 0;
    }

    if (!dstlen) dst = NULL;

    if (flags & LCMAP_SORTKEY)
    {
        INT ret;
        if (src == dst)
        {
            SetLastError_k(ERROR_INVALID_FLAGS);
            return 0;
        }

        if (srclen < 0) srclen = strlenW(src);

        //TRACE("(%s,0x%08x,%s,%d,%p,%d)\n", debugstr_w(name), flags, debugstr_wn(src, srclen), srclen, dst, dstlen);

        ret = wine_get_sortkey(flags, src, srclen, (char *)dst, dstlen);
        if (ret == 0)
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        else
            ret++;
        return ret;
    }

    /* SORT_STRINGSORT must be used exclusively with LCMAP_SORTKEY */
    if (flags & SORT_STRINGSORT)
    {
        SetLastError_k(ERROR_INVALID_FLAGS);
        return 0;
    }

    if (srclen < 0) srclen = strlenW(src) + 1;

    // TRACE("(%s,0x%08x,%s,%d,%p,%d)\n", debugstr_w(name), flags, debugstr_wn(src, srclen), srclen, dst, dstlen);

    if (!dst) /* return required string length */
    {
        INT len;

        for (len = 0; srclen; src++, srclen--)
        {
            WCHAR wch = *src;
            /* tests show that win2k just ignores NORM_IGNORENONSPACE,
             * and skips white space and punctuation characters for
             * NORM_IGNORESYMBOLS.
             */
            if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
                continue;
            len++;
        }
        return len;
    }

    if (flags & LCMAP_UPPERCASE)
    {
        for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
        {
            WCHAR wch = *src;
            if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
                continue;
            *dst_ptr++ = toupperW(wch);
            dstlen--;
        }
    }
    else if (flags & LCMAP_LOWERCASE)
    {
        for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
        {
            WCHAR wch = *src;
            if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
                continue;
            *dst_ptr++ = tolowerW(wch);
            dstlen--;
        }
    }
    else
    {
        if (src == dst)
        {
            SetLastError_k(ERROR_INVALID_FLAGS);
            return 0;
        }
        for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
        {
            WCHAR wch = *src;
            if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
                continue;
            *dst_ptr++ = wch;
            dstlen--;
        }
    }

    if (srclen)
    {
        SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }

    return dst_ptr - dst;
}

/*************************************************************************
 *           LCMapStringW    (KERNEL32.@)
 *
 * See LCMapStringA.
 */
INT WINAPI LCMapStringW_k(LCID lcid, DWORD flags, LPCWSTR src, INT srclen,
                        LPWSTR dst, INT dstlen)
{
    // TRACE("(0x%04x,0x%08x,%s,%d,%p,%d)\n", lcid, flags, debugstr_wn(src, srclen), srclen, dst, dstlen);

    return LCMapStringEx_k(NULL, flags, src, srclen, dst, dstlen, NULL, NULL, 0);
}

/*************************************************************************
 *           LCMapStringA    (KERNEL32.@)
 *
 * Map characters in a locale sensitive string.
 *
 * PARAMS
 *  lcid   [I] LCID for the conversion.
 *  flags  [I] Flags controlling the mapping (LCMAP_ constants from "winnls.h").
 *  src    [I] String to map
 *  srclen [I] Length of src in chars, or -1 if src is NUL terminated
 *  dst    [O] Destination for mapped string
 *  dstlen [I] Length of dst in characters
 *
 * RETURNS
 *  Success: The length of the mapped string in dst, including the NUL terminator.
 *  Failure: 0. Use GetLastError() to determine the cause.
 */
INT WINAPI LCMapStringA_k(LCID lcid, DWORD flags, LPCSTR src, INT srclen,
                        LPSTR dst, INT dstlen)
{
    WCHAR *bufW = (WCHAR*)(NtCurrentTeb_k()+TEB_STATIC_UNICODE_BUFFER);
    LPWSTR srcW, dstW;
    INT ret = 0, srclenW, dstlenW;
    UINT locale_cp = CP_ACP;

    if (!src || !srclen || dstlen < 0)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!(flags & LOCALE_USE_CP_ACP)) locale_cp = get_lcid_codepage( lcid );

    srclenW = MultiByteToWideChar_k(locale_cp, 0, src, srclen, bufW, 260);
    if (srclenW)
        srcW = bufW;
    else
    {
        srclenW = MultiByteToWideChar_k(locale_cp, 0, src, srclen, NULL, 0);
        srcW = HeapAlloc_k(GetProcessHeap_k(), 0, srclenW * sizeof(WCHAR));
        if (!srcW)
        {
            SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }
        MultiByteToWideChar_k(locale_cp, 0, src, srclen, srcW, srclenW);
    }

    if (flags & LCMAP_SORTKEY)
    {
        if (src == dst)
        {
            SetLastError_k(ERROR_INVALID_FLAGS);
            goto map_string_exit;
        }
        ret = wine_get_sortkey(flags, srcW, srclenW, dst, dstlen);
        if (ret == 0)
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        else
            ret++;
        goto map_string_exit;
    }

    if (flags & SORT_STRINGSORT)
    {
        SetLastError_k(ERROR_INVALID_FLAGS);
        goto map_string_exit;
    }

    dstlenW = LCMapStringEx_k(NULL, flags, srcW, srclenW, NULL, 0, NULL, NULL, 0);
    if (!dstlenW)
        goto map_string_exit;

    dstW = HeapAlloc_k(GetProcessHeap_k(), 0, dstlenW * sizeof(WCHAR));
    if (!dstW)
    {
        SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
        goto map_string_exit;
    }

    LCMapStringEx_k(NULL, flags, srcW, srclenW, dstW, dstlenW, NULL, NULL, 0);
    ret = WideCharToMultiByte_k(locale_cp, 0, dstW, dstlenW, dst, dstlen, NULL, NULL);
    HeapFree_k(GetProcessHeap_k(), 0, dstW);

map_string_exit:
    if (srcW != bufW) HeapFree_k(GetProcessHeap_k(), 0, srcW);
    return ret;
}

/*************************************************************************
 *           FoldStringA    (KERNEL32.@)
 *
 * Map characters in a string.
 *
 * PARAMS
 *  dwFlags [I] Flags controlling chars to map (MAP_ constants from "winnls.h")
 *  src     [I] String to map
 *  srclen  [I] Length of src, or -1 if src is NUL terminated
 *  dst     [O] Destination for mapped string
 *  dstlen  [I] Length of dst, or 0 to find the required length for the mapped string
 *
 * RETURNS
 *  Success: The length of the string written to dst, including the terminating NUL. If
 *           dstlen is 0, the value returned is the same, but nothing is written to dst,
 *           and dst may be NULL.
 *  Failure: 0. Use GetLastError() to determine the cause.
 */
INT WINAPI FoldStringA_k(DWORD dwFlags, LPCSTR src, INT srclen,
                       LPSTR dst, INT dstlen)
{
    INT ret = 0, srclenW = 0;
    WCHAR *srcW = NULL, *dstW = NULL;

    if (!src || !srclen || dstlen < 0 || (dstlen && !dst) || src == dst)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    srclenW = MultiByteToWideChar_k(CP_ACP, dwFlags & MAP_COMPOSITE ? MB_COMPOSITE : 0,
                                  src, srclen, NULL, 0);
    srcW = HeapAlloc_k(GetProcessHeap_k(), 0, srclenW * sizeof(WCHAR));

    if (!srcW)
    {
        SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
        goto FoldStringA_exit;
    }

    MultiByteToWideChar_k(CP_ACP, dwFlags & MAP_COMPOSITE ? MB_COMPOSITE : 0,
                        src, srclen, srcW, srclenW);

    dwFlags = (dwFlags & ~MAP_PRECOMPOSED) | MAP_FOLDCZONE;

    ret = FoldStringW_k(dwFlags, srcW, srclenW, NULL, 0);
    if (ret && dstlen)
    {
        dstW = HeapAlloc_k(GetProcessHeap_k(), 0, ret * sizeof(WCHAR));

        if (!dstW)
        {
            SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
            goto FoldStringA_exit;
        }

        ret = FoldStringW_k(dwFlags, srcW, srclenW, dstW, ret);
        if (!WideCharToMultiByte_k(CP_ACP, 0, dstW, ret, dst, dstlen, NULL, NULL))
        {
            ret = 0;
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        }
    }

    HeapFree_k(GetProcessHeap_k(), 0, dstW);

FoldStringA_exit:
    HeapFree_k(GetProcessHeap_k(), 0, srcW);
    return ret;
}

/*************************************************************************
 *           FoldStringW    (KERNEL32.@)
 *
 * See FoldStringA.
 */
INT WINAPI FoldStringW_k(DWORD dwFlags, LPCWSTR src, INT srclen,
                       LPWSTR dst, INT dstlen)
{
    int ret;

    switch (dwFlags & (MAP_COMPOSITE|MAP_PRECOMPOSED|MAP_EXPAND_LIGATURES))
    {
    case 0:
        if (dwFlags)
          break;
        /* Fall through for dwFlags == 0 */
    case MAP_PRECOMPOSED|MAP_COMPOSITE:
    case MAP_PRECOMPOSED|MAP_EXPAND_LIGATURES:
    case MAP_COMPOSITE|MAP_EXPAND_LIGATURES:
        SetLastError_k(ERROR_INVALID_FLAGS);
        return 0;
    }

    if (!src || !srclen || dstlen < 0 || (dstlen && !dst) || src == dst)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    ret = wine_fold_string(dwFlags, src, srclen, dst, dstlen);
    if (!ret)
        SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
    return ret;
}

/******************************************************************************
 *           CompareStringW    (KERNEL32.@)
 *
 * See CompareStringA.
 */
INT WINAPI CompareStringW_k(LCID lcid, DWORD flags,
                          LPCWSTR str1, INT len1, LPCWSTR str2, INT len2)
{
    return CompareStringEx_k(NULL, flags, str1, len1, str2, len2, NULL, NULL, 0);
}

/******************************************************************************
 *           CompareStringEx    (KERNEL32.@)
 */
INT WINAPI CompareStringEx_k(LPCWSTR locale, DWORD flags, LPCWSTR str1, INT len1,
                           LPCWSTR str2, INT len2, LPNLSVERSIONINFO version, LPVOID reserved, LPARAM lParam)
{
    INT ret;

    if (version) FIXME("unexpected version parameter\n");
    if (reserved) FIXME("unexpected reserved value\n");
    if (lParam) FIXME("unexpected lParam\n");

    if (!str1 || !str2)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if( flags & ~(NORM_IGNORECASE|NORM_IGNORENONSPACE|NORM_IGNORESYMBOLS|
        SORT_STRINGSORT|NORM_IGNOREKANATYPE|NORM_IGNOREWIDTH|LOCALE_USE_CP_ACP|0x10000000) )
    {
        SetLastError_k(ERROR_INVALID_FLAGS);
        return 0;
    }

    /* this style is related to diacritics in Arabic, Japanese, and Hebrew */
    if (flags & 0x10000000)
        WARN("Ignoring unknown flags 0x10000000\n");

    if (len1 < 0) len1 = strlenW(str1);
    if (len2 < 0) len2 = strlenW(str2);

    ret = wine_compare_string(flags, str1, len1, str2, len2);

    if (ret) /* need to translate result */
        return (ret < 0) ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
    return CSTR_EQUAL;
}

/******************************************************************************
 *           CompareStringA    (KERNEL32.@)
 *
 * Compare two locale sensitive strings.
 *
 * PARAMS
 *  lcid  [I] LCID for the comparison
 *  flags [I] Flags for the comparison (NORM_ constants from "winnls.h").
 *  str1  [I] First string to compare
 *  len1  [I] Length of str1, or -1 if str1 is NUL terminated
 *  str2  [I] Second string to compare
 *  len2  [I] Length of str2, or -1 if str2 is NUL terminated
 *
 * RETURNS
 *  Success: CSTR_LESS_THAN, CSTR_EQUAL or CSTR_GREATER_THAN depending on whether
 *           str1 is less than, equal to or greater than str2 respectively.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
INT WINAPI CompareStringA_k(LCID lcid, DWORD flags,
                          LPCSTR str1, INT len1, LPCSTR str2, INT len2)
{
    WCHAR *buf1W = (WCHAR*)(NtCurrentTeb_k()+TEB_STATIC_UNICODE_BUFFER);
    WCHAR *buf2W = buf1W + 130;
    LPWSTR str1W, str2W;
    INT len1W = 0, len2W = 0, ret;
    UINT locale_cp = CP_ACP;

    if (!str1 || !str2)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (len1 < 0) len1 = strlen(str1);
    if (len2 < 0) len2 = strlen(str2);

    if (!(flags & LOCALE_USE_CP_ACP)) locale_cp = get_lcid_codepage( lcid );

    if (len1)
    {
        if (len1 <= 130) len1W = MultiByteToWideChar_k(locale_cp, 0, str1, len1, buf1W, 130);
        if (len1W)
            str1W = buf1W;
        else
        {
            len1W = MultiByteToWideChar_k(locale_cp, 0, str1, len1, NULL, 0);
            str1W = HeapAlloc_k(GetProcessHeap_k(), 0, len1W * sizeof(WCHAR));
            if (!str1W)
            {
                SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
                return 0;
            }
            MultiByteToWideChar_k(locale_cp, 0, str1, len1, str1W, len1W);
        }
    }
    else
    {
        len1W = 0;
        str1W = buf1W;
    }

    if (len2)
    {
        if (len2 <= 130) len2W = MultiByteToWideChar_k(locale_cp, 0, str2, len2, buf2W, 130);
        if (len2W)
            str2W = buf2W;
        else
        {
            len2W = MultiByteToWideChar_k(locale_cp, 0, str2, len2, NULL, 0);
            str2W = HeapAlloc_k(GetProcessHeap_k(), 0, len2W * sizeof(WCHAR));
            if (!str2W)
            {
                if (str1W != buf1W) HeapFree_k(GetProcessHeap_k(), 0, str1W);
                SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
                return 0;
            }
            MultiByteToWideChar_k(locale_cp, 0, str2, len2, str2W, len2W);
        }
    }
    else
    {
        len2W = 0;
        str2W = buf2W;
    }

    ret = CompareStringEx_k(NULL, flags, str1W, len1W, str2W, len2W, NULL, NULL, 0);

    if (str1W != buf1W) HeapFree_k(GetProcessHeap_k(), 0, str1W);
    if (str2W != buf2W) HeapFree_k(GetProcessHeap_k(), 0, str2W);
    return ret;
}

/******************************************************************************
 *           CompareStringOrdinal    (KERNEL32.@)
 */
INT WINAPI CompareStringOrdinal_k(const WCHAR *str1, INT len1, const WCHAR *str2, INT len2, BOOL ignore_case)
{
    int ret, len;

    if (!str1 || !str2)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (len1 < 0) len1 = strlenW(str1);
    if (len2 < 0) len2 = strlenW(str2);

    len = min(len1, len2);
    if (ignore_case)
    {
        ret = memicmpW(str1, str2, len);
    }
    else
    {
        ret = 0;
        for (; len > 0; len--)
            if ((ret = (*str1++ - *str2++))) break;
    }
    if (!ret) ret = len1 - len2;

    if (ret < 0) return CSTR_LESS_THAN;
    if (ret > 0) return CSTR_GREATER_THAN;
    return CSTR_EQUAL;
}

/*************************************************************************
 *           lstrcmp     (KERNEL32.@)
 *           lstrcmpA    (KERNEL32.@)
 *
 * Compare two strings using the current thread locale.
 *
 * PARAMS
 *  str1  [I] First string to compare
 *  str2  [I] Second string to compare
 *
 * RETURNS
 *  Success: A number less than, equal to or greater than 0 depending on whether
 *           str1 is less than, equal to or greater than str2 respectively.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
int WINAPI lstrcmpA_k(LPCSTR str1, LPCSTR str2)
{
    int ret;
    
    if ((str1 == NULL) && (str2 == NULL)) return 0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return 1;

    ret = CompareStringA_k(GetThreadLocale_k(), LOCALE_USE_CP_ACP, str1, -1, str2, -1);
    if (ret) ret -= 2;
    
    return ret;
}

/*************************************************************************
 *           lstrcmpi     (KERNEL32.@)
 *           lstrcmpiA    (KERNEL32.@)
 *
 * Compare two strings using the current thread locale, ignoring case.
 *
 * PARAMS
 *  str1  [I] First string to compare
 *  str2  [I] Second string to compare
 *
 * RETURNS
 *  Success: A number less than, equal to or greater than 0 depending on whether
 *           str2 is less than, equal to or greater than str1 respectively.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
int WINAPI lstrcmpiA_k(LPCSTR str1, LPCSTR str2)
{
    int ret;
    
    if ((str1 == NULL) && (str2 == NULL)) return 0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return 1;

    ret = CompareStringA_k(GetThreadLocale_k(), NORM_IGNORECASE|LOCALE_USE_CP_ACP, str1, -1, str2, -1);
    if (ret) ret -= 2;
    
    return ret;
}

/*************************************************************************
 *           lstrcmpW    (KERNEL32.@)
 *
 * See lstrcmpA.
 */
int WINAPI lstrcmpW_k(LPCWSTR str1, LPCWSTR str2)
{
    int ret;

    if ((str1 == NULL) && (str2 == NULL)) return 0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return 1;

    ret = CompareStringW_k(GetThreadLocale_k(), 0, str1, -1, str2, -1);
    if (ret) ret -= 2;
    
    return ret;
}

/*************************************************************************
 *           lstrcmpiW    (KERNEL32.@)
 *
 * See lstrcmpiA.
 */
int WINAPI lstrcmpiW_k(LPCWSTR str1, LPCWSTR str2)
{
    int ret;
    
    if ((str1 == NULL) && (str2 == NULL)) return 0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return 1;

    ret = CompareStringW_k(GetThreadLocale_k(), NORM_IGNORECASE, str1, -1, str2, -1);
    if (ret) ret -= 2;
    
    return ret;
}

/******************************************************************************
 *		LOCALE_Init
 */
void LOCALE_Init(void)
{
    setlocale( LC_ALL, "" );

    ansi_cptable = wine_cp_get_table( 1252 );
    oem_cptable  = wine_cp_get_table( 437 );
    unix_cptable  = wine_cp_get_table( 28591 );
    setlocale(LC_NUMERIC, "C");  /* FIXME: oleaut32 depends on this */
}
/*
static HANDLE NLS_RegOpenKey(HANDLE hRootKey, LPCWSTR szKeyName)
{
    UNICODE_STRING keyName;
    OBJECT_ATTRIBUTES attr;
    HANDLE hkey;

    RtlInitUnicodeString( &keyName, szKeyName );
    InitializeObjectAttributes(&attr, &keyName, 0, hRootKey, NULL);

    if (NtOpenKey( &hkey, KEY_READ, &attr ) != STATUS_SUCCESS)
        hkey = 0;

    return hkey;
}
*/

/*
static BOOL NLS_RegEnumSubKey(HANDLE hKey, UINT ulIndex, LPWSTR szKeyName,
                              ULONG keyNameSize)
{
    BYTE buffer[80];
    KEY_BASIC_INFORMATION *info = (KEY_BASIC_INFORMATION *)buffer;
    DWORD dwLen;

    if (NtEnumerateKey( hKey, ulIndex, KeyBasicInformation, buffer,
                        sizeof(buffer), &dwLen) != STATUS_SUCCESS ||
        info->NameLength > keyNameSize)
    {
        return FALSE;
    }

    TRACE("info->Name %s info->NameLength %d\n", debugstr_w(info->Name), info->NameLength);

    memcpy( szKeyName, info->Name, info->NameLength);
    szKeyName[info->NameLength / sizeof(WCHAR)] = '\0';

    TRACE("returning %s\n", debugstr_w(szKeyName));
    return TRUE;
}
*/
/*
static BOOL NLS_RegEnumValue(HANDLE hKey, UINT ulIndex,
                             LPWSTR szValueName, ULONG valueNameSize,
                             LPWSTR szValueData, ULONG valueDataSize)
{
    BYTE buffer[80];
    KEY_VALUE_FULL_INFORMATION *info = (KEY_VALUE_FULL_INFORMATION *)buffer;
    DWORD dwLen;

    if (NtEnumerateValueKey( hKey, ulIndex, KeyValueFullInformation,
        buffer, sizeof(buffer), &dwLen ) != STATUS_SUCCESS ||
        info->NameLength > valueNameSize ||
        info->DataLength > valueDataSize)
    {
        return FALSE;
    }

    TRACE("info->Name %s info->DataLength %d\n", debugstr_w(info->Name), info->DataLength);

    memcpy( szValueName, info->Name, info->NameLength);
    szValueName[info->NameLength / sizeof(WCHAR)] = '\0';
    memcpy( szValueData, buffer + info->DataOffset, info->DataLength );
    szValueData[info->DataLength / sizeof(WCHAR)] = '\0';

    TRACE("returning %s %s\n", debugstr_w(szValueName), debugstr_w(szValueData));
    return TRUE;
}
*/
/*
static BOOL NLS_RegGetDword(HANDLE hKey, LPCWSTR szValueName, DWORD *lpVal)
{
    BYTE buffer[128];
    const KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *)buffer;
    DWORD dwSize = sizeof(buffer);
    UNICODE_STRING valueName;

    RtlInitUnicodeString( &valueName, szValueName );

    TRACE("%p, %s\n", hKey, debugstr_w(szValueName));
    if (NtQueryValueKey( hKey, &valueName, KeyValuePartialInformation,
                         buffer, dwSize, &dwSize ) == STATUS_SUCCESS &&
        info->DataLength == sizeof(DWORD))
    {
        memcpy(lpVal, info->Data, sizeof(DWORD));
        return TRUE;
    }

    return FALSE;
}
*/
/*
static BOOL NLS_GetLanguageGroupName(LGRPID lgrpid, LPWSTR szName, ULONG nameSize)
{
    LANGID  langId;
    LPCWSTR szResourceName = MAKEINTRESOURCEW(((lgrpid + 0x2000) >> 4) + 1);
    HRSRC   hResource;
    BOOL    bRet = FALSE;

    // FIXME: Is it correct to use the system default langid? 
    langId = GetSystemDefaultLangID();

    if (SUBLANGID(langId) == SUBLANG_NEUTRAL)
        langId = MAKELANGID( PRIMARYLANGID(langId), SUBLANG_DEFAULT );

    hResource = FindResourceExW( kernel32_handle, (LPWSTR)RT_STRING, szResourceName, langId );

    if (hResource)
    {
        HGLOBAL hResDir = LoadResource( kernel32_handle, hResource );

        if (hResDir)
        {
            ULONG   iResourceIndex = lgrpid & 0xf;
            LPCWSTR lpResEntry = LockResource( hResDir );
            ULONG   i;

            for (i = 0; i < iResourceIndex; i++)
                lpResEntry += *lpResEntry + 1;

            if (*lpResEntry < nameSize)
            {
                memcpy( szName, lpResEntry + 1, *lpResEntry * sizeof(WCHAR) );
                szName[*lpResEntry] = '\0';
                bRet = TRUE;
            }

        }
        FreeResource( hResource );
    }
    return bRet;
}
*/
/* Registry keys for NLS related information */

static const WCHAR szCountryListName[] = {
    'M','a','c','h','i','n','e','\\','S','o','f','t','w','a','r','e','\\',
    'M','i','c','r','o','s','o','f','t','\\','W','i','n','d','o','w','s','\\',
    'C','u','r','r','e','n','t','V','e','r','s','i','o','n','\\',
    'T','e','l','e','p','h','o','n','y','\\',
    'C','o','u','n','t','r','y',' ','L','i','s','t','\0'
};


/* Callback function ptrs for EnumSystemLanguageGroupsA/W */
typedef struct
{
  LANGUAGEGROUP_ENUMPROCA procA;
  LANGUAGEGROUP_ENUMPROCW procW;
  DWORD    dwFlags;
  LONG_PTR lParam;
} ENUMLANGUAGEGROUP_CALLBACKS;

/* Internal implementation of EnumSystemLanguageGroupsA/W */
/*
static BOOL NLS_EnumSystemLanguageGroups(ENUMLANGUAGEGROUP_CALLBACKS *lpProcs)
{
    WCHAR szNumber[10], szValue[4];
    HANDLE hKey;
    BOOL bContinue = TRUE;
    ULONG ulIndex = 0;

    if (!lpProcs)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    switch (lpProcs->dwFlags)
    {
    case 0:
        // Default to LGRPID_INSTALLED 
        lpProcs->dwFlags = LGRPID_INSTALLED;
        // Fall through... 
    case LGRPID_INSTALLED:
    case LGRPID_SUPPORTED:
        break;
    default:
        SetLastError(ERROR_INVALID_FLAGS);
        return FALSE;
    }

    hKey = NLS_RegOpenKey( 0, szLangGroupsKeyName );

    if (!hKey)
        FIXME("NLS registry key not found. Please apply the default registry file 'wine.inf'\n");

    while (bContinue)
    {
        if (NLS_RegEnumValue( hKey, ulIndex, szNumber, sizeof(szNumber),
                              szValue, sizeof(szValue) ))
        {
            BOOL bInstalled = szValue[0] == '1';
            LGRPID lgrpid = strtoulW( szNumber, NULL, 16 );

            TRACE("grpid %s (%sinstalled)\n", debugstr_w(szNumber),
                   bInstalled ? "" : "not ");

            if (lpProcs->dwFlags == LGRPID_SUPPORTED || bInstalled)
            {
                WCHAR szGrpName[48];

                if (!NLS_GetLanguageGroupName( lgrpid, szGrpName, sizeof(szGrpName) / sizeof(WCHAR) ))
                    szGrpName[0] = '\0';

                if (lpProcs->procW)
                    bContinue = lpProcs->procW( lgrpid, szNumber, szGrpName, lpProcs->dwFlags,
                                                lpProcs->lParam );
                else
                {
                    char szNumberA[sizeof(szNumber)/sizeof(WCHAR)];
                    char szGrpNameA[48];

                    // FIXME: MSDN doesn't say which code page the W->A translation uses,
                    //        or whether the language names are ever localised. Assume CP_ACP.
                    //

                    WideCharToMultiByte(CP_ACP, 0, szNumber, -1, szNumberA, sizeof(szNumberA), 0, 0);
                    WideCharToMultiByte(CP_ACP, 0, szGrpName, -1, szGrpNameA, sizeof(szGrpNameA), 0, 0);

                    bContinue = lpProcs->procA( lgrpid, szNumberA, szGrpNameA, lpProcs->dwFlags,
                                                lpProcs->lParam );
                }
            }

            ulIndex++;
        }
        else
            bContinue = FALSE;

        if (!bContinue)
            break;
    }

    if (hKey)
        NtClose( hKey );

    return TRUE;
}
*/
/******************************************************************************
 *           EnumSystemLanguageGroupsA    (KERNEL32.@)
 *
 * Call a users function for each language group available on the system.
 *
 * PARAMS
 *  pLangGrpEnumProc [I] Callback function to call for each language group
 *  dwFlags          [I] LGRPID_SUPPORTED=All Supported, LGRPID_INSTALLED=Installed only
 *  lParam           [I] User parameter to pass to pLangGrpEnumProc
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI EnumSystemLanguageGroupsA(LANGUAGEGROUP_ENUMPROCA pLangGrpEnumProc,
                                      DWORD dwFlags, LONG_PTR lParam)
{
    ENUMLANGUAGEGROUP_CALLBACKS procs;

    TRACE("(%p,0x%08X,0x%08lX)\n", pLangGrpEnumProc, dwFlags, lParam);

    procs.procA = pLangGrpEnumProc;
    procs.procW = NULL;
    procs.dwFlags = dwFlags;
    procs.lParam = lParam;

    // return NLS_EnumSystemLanguageGroups( pLangGrpEnumProc ? &procs : NULL);
    return FALSE;
}

/******************************************************************************
 *           EnumSystemLanguageGroupsW    (KERNEL32.@)
 *
 * See EnumSystemLanguageGroupsA.
 */
BOOL WINAPI EnumSystemLanguageGroupsW(LANGUAGEGROUP_ENUMPROCW pLangGrpEnumProc,
                                      DWORD dwFlags, LONG_PTR lParam)
{
    ENUMLANGUAGEGROUP_CALLBACKS procs;

    TRACE("(%p,0x%08X,0x%08lX)\n", pLangGrpEnumProc, dwFlags, lParam);

    procs.procA = NULL;
    procs.procW = pLangGrpEnumProc;
    procs.dwFlags = dwFlags;
    procs.lParam = lParam;

    // return NLS_EnumSystemLanguageGroups( pLangGrpEnumProc ? &procs : NULL);
    return FALSE;
}

/******************************************************************************
 *           IsValidLanguageGroup    (KERNEL32.@)
 *
 * Determine if a language group is supported and/or installed.
 *
 * PARAMS
 *  lgrpid  [I] Language Group Id (LGRPID_ values from "winnls.h")
 *  dwFlags [I] LGRPID_SUPPORTED=Supported, LGRPID_INSTALLED=Installed
 *
 * RETURNS
 *  TRUE, if lgrpid is supported and/or installed, according to dwFlags.
 *  FALSE otherwise.
 */
BOOL WINAPI IsValidLanguageGroup(LGRPID lgrpid, DWORD dwFlags)
{
    /*
    static const WCHAR szFormat[] = { '%','x','\0' };
    WCHAR szValueName[16], szValue[2];
    BOOL bSupported = FALSE, bInstalled = FALSE;
    HANDLE hKey;


    switch (dwFlags)
    {
    case LGRPID_INSTALLED:
    case LGRPID_SUPPORTED:

        hKey = NLS_RegOpenKey( 0, szLangGroupsKeyName );

        sprintfW( szValueName, szFormat, lgrpid );

        if (NLS_RegGetDword( hKey, szValueName, (LPDWORD)szValue ))
        {
            bSupported = TRUE;

            if (szValue[0] == '1')
                bInstalled = TRUE;
        }

        if (hKey)
            NtClose( hKey );

        break;
    }

    if ((dwFlags == LGRPID_SUPPORTED && bSupported) ||
        (dwFlags == LGRPID_INSTALLED && bInstalled))
        return TRUE;
    */
    return FALSE;
}

/* Callback function ptrs for EnumLanguageGrouplocalesA/W */
typedef struct
{
  LANGGROUPLOCALE_ENUMPROCA procA;
  LANGGROUPLOCALE_ENUMPROCW procW;
  DWORD    dwFlags;
  LGRPID   lgrpid;
  LONG_PTR lParam;
} ENUMLANGUAGEGROUPLOCALE_CALLBACKS;

/* Internal implementation of EnumLanguageGrouplocalesA/W */
/*
static BOOL NLS_EnumLanguageGroupLocales(ENUMLANGUAGEGROUPLOCALE_CALLBACKS *lpProcs)
{
    static const WCHAR szAlternateSortsKeyName[] = {
      'A','l','t','e','r','n','a','t','e',' ','S','o','r','t','s','\0'
    };
    WCHAR szNumber[10], szValue[4];
    HANDLE hKey;
    BOOL bContinue = TRUE, bAlternate = FALSE;
    LGRPID lgrpid;
    ULONG ulIndex = 1;  // Ignore default entry of 1st key 

    if (!lpProcs || !lpProcs->lgrpid || lpProcs->lgrpid > LGRPID_ARMENIAN)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if (lpProcs->dwFlags)
    {
        SetLastError(ERROR_INVALID_FLAGS);
        return FALSE;
    }

    hKey = NLS_RegOpenKey( 0, szLocaleKeyName );

    if (!hKey)
        WARN("NLS registry key not found. Please apply the default registry file 'wine.inf'\n");

    while (bContinue)
    {
        if (NLS_RegEnumValue( hKey, ulIndex, szNumber, sizeof(szNumber),
                              szValue, sizeof(szValue) ))
        {
            lgrpid = strtoulW( szValue, NULL, 16 );

            TRACE("lcid %s, grpid %d (%smatched)\n", debugstr_w(szNumber),
                   lgrpid, lgrpid == lpProcs->lgrpid ? "" : "not ");

            if (lgrpid == lpProcs->lgrpid)
            {
                LCID lcid;

                lcid = strtoulW( szNumber, NULL, 16 );

                // FIXME: native returns extra text for a few (17/150) locales, e.g:
                // '00000437          ;Georgian'
                // At present we only pass the LCID string.
                

                if (lpProcs->procW)
                    bContinue = lpProcs->procW( lgrpid, lcid, szNumber, lpProcs->lParam );
                else
                {
                    char szNumberA[sizeof(szNumber)/sizeof(WCHAR)];

                    WideCharToMultiByte(CP_ACP, 0, szNumber, -1, szNumberA, sizeof(szNumberA), 0, 0);

                    bContinue = lpProcs->procA( lgrpid, lcid, szNumberA, lpProcs->lParam );
                }
            }

            ulIndex++;
        }
        else
        {
            // Finished enumerating this key 
            if (!bAlternate)
            {
                // Enumerate alternate sorts also 
                hKey = NLS_RegOpenKey( hKey, szAlternateSortsKeyName );
                bAlternate = TRUE;
                ulIndex = 0;
            }
            else
                bContinue = FALSE; // Finished both keys 
        }

        if (!bContinue)
            break;
    }

    if (hKey)
        NtClose( hKey );

    return TRUE;
}
*/
/******************************************************************************
 *           EnumLanguageGroupLocalesA    (KERNEL32.@)
 *
 * Call a users function for every locale in a language group available on the system.
 *
 * PARAMS
 *  pLangGrpLcEnumProc [I] Callback function to call for each locale
 *  lgrpid             [I] Language group (LGRPID_ values from "winnls.h")
 *  dwFlags            [I] Reserved, set to 0
 *  lParam             [I] User parameter to pass to pLangGrpLcEnumProc
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI EnumLanguageGroupLocalesA(LANGGROUPLOCALE_ENUMPROCA pLangGrpLcEnumProc,
                                      LGRPID lgrpid, DWORD dwFlags, LONG_PTR lParam)
{
    ENUMLANGUAGEGROUPLOCALE_CALLBACKS callbacks;

    TRACE("(%p,0x%08X,0x%08X,0x%08lX)\n", pLangGrpLcEnumProc, lgrpid, dwFlags, lParam);

    callbacks.procA   = pLangGrpLcEnumProc;
    callbacks.procW   = NULL;
    callbacks.dwFlags = dwFlags;
    callbacks.lgrpid  = lgrpid;
    callbacks.lParam  = lParam;

    //return NLS_EnumLanguageGroupLocales( pLangGrpLcEnumProc ? &callbacks : NULL );
    return TRUE;
}

/******************************************************************************
 *           EnumLanguageGroupLocalesW    (KERNEL32.@)
 *
 * See EnumLanguageGroupLocalesA.
 */
BOOL WINAPI EnumLanguageGroupLocalesW(LANGGROUPLOCALE_ENUMPROCW pLangGrpLcEnumProc,
                                      LGRPID lgrpid, DWORD dwFlags, LONG_PTR lParam)
{
    ENUMLANGUAGEGROUPLOCALE_CALLBACKS callbacks;

    TRACE("(%p,0x%08X,0x%08X,0x%08lX)\n", pLangGrpLcEnumProc, lgrpid, dwFlags, lParam);

    callbacks.procA   = NULL;
    callbacks.procW   = pLangGrpLcEnumProc;
    callbacks.dwFlags = dwFlags;
    callbacks.lgrpid  = lgrpid;
    callbacks.lParam  = lParam;

    // return NLS_EnumLanguageGroupLocales( pLangGrpLcEnumProc ? &callbacks : NULL );
    return TRUE;
}

/******************************************************************************
 *           EnumSystemGeoID    (KERNEL32.@)
 *
 * Call a users function for every location available on the system.
 *
 * PARAMS
 *  geoclass     [I] Type of information desired (SYSGEOTYPE enum from "winnls.h")
 *  reserved     [I] Reserved, set to 0
 *  pGeoEnumProc [I] Callback function to call for each location
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: FALSE. Use GetLastError() to determine the cause.
 */
BOOL WINAPI EnumSystemGeoID(GEOCLASS geoclass, GEOID reserved, GEO_ENUMPROC pGeoEnumProc)
{
    /*
    static const WCHAR szCountryCodeValueName[] = {
      'C','o','u','n','t','r','y','C','o','d','e','\0'
    };
    WCHAR szNumber[10];
    HANDLE hKey;
    ULONG ulIndex = 0;

    TRACE("(0x%08X,0x%08X,%p)\n", geoclass, reserved, pGeoEnumProc);

    if (geoclass != GEOCLASS_NATION || reserved || !pGeoEnumProc)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    hKey = NLS_RegOpenKey( 0, szCountryListName );

    while (NLS_RegEnumSubKey( hKey, ulIndex, szNumber, sizeof(szNumber) ))
    {
        BOOL bContinue = TRUE;
        DWORD dwGeoId;
        HANDLE hSubKey = NLS_RegOpenKey( hKey, szNumber );

        if (hSubKey)
        {
            if (NLS_RegGetDword( hSubKey, szCountryCodeValueName, &dwGeoId ))
            {
                TRACE("Got geoid %d\n", dwGeoId);

                if (!pGeoEnumProc( dwGeoId ))
                    bContinue = FALSE;
            }

            NtClose( hSubKey );
        }

        if (!bContinue)
            break;

        ulIndex++;
    }

    if (hKey)
        NtClose( hKey );
    */
    return TRUE;
}

/******************************************************************************
 *           InvalidateNLSCache           (KERNEL32.@)
 *
 * Invalidate the cache of NLS values.
 *
 * PARAMS
 *  None.
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: FALSE.
 */
BOOL WINAPI InvalidateNLSCache_k(void)
{
  FIXME("() stub\n");
  return FALSE;
}

/******************************************************************************
 *           GetUserGeoID (KERNEL32.@)
 */

GEOID WINAPI GetUserGeoID( GEOCLASS GeoClass )
{
    return -1;// GEOID_NOT_AVAILABLE;
}

/******************************************************************************
 *           SetUserGeoID (KERNEL32.@)
 */

BOOL WINAPI SetUserGeoID( GEOID GeoID )
{
    return TRUE;
}

typedef struct
{
    union
    {
        UILANGUAGE_ENUMPROCA procA;
        UILANGUAGE_ENUMPROCW procW;
    } u;
    DWORD flags;
    LONG_PTR param;
} ENUM_UILANG_CALLBACK;

static BOOL CALLBACK enum_uilang_proc_a( HMODULE hModule, LPCSTR type,
                                         LPCSTR name, WORD LangID, LONG_PTR lParam )
{
    ENUM_UILANG_CALLBACK *enum_uilang = (ENUM_UILANG_CALLBACK *)lParam;
    char buf[20];

    sprintf(buf, "%08x", (UINT)LangID);
    return enum_uilang->u.procA( buf, enum_uilang->param );
}

static BOOL CALLBACK enum_uilang_proc_w( HMODULE hModule, LPCWSTR type,
                                         LPCWSTR name, WORD LangID, LONG_PTR lParam )
{
    static const WCHAR formatW[] = {'%','0','8','x',0};
    ENUM_UILANG_CALLBACK *enum_uilang = (ENUM_UILANG_CALLBACK *)lParam;
    WCHAR buf[20];

    sprintfW( buf, formatW, (UINT)LangID );
    return enum_uilang->u.procW( buf, enum_uilang->param );
}

/******************************************************************************
 *           EnumUILanguagesA (KERNEL32.@)
 */

BOOL WINAPI EnumUILanguagesA_k(UILANGUAGE_ENUMPROCA pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam)
{
    ENUM_UILANG_CALLBACK enum_uilang;
    int i;

    TRACE("%p, %x, %lx\n", pUILangEnumProc, dwFlags, lParam);

    if(!pUILangEnumProc) {
	SetLastError_k(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
    if(dwFlags) {
	SetLastError_k(ERROR_INVALID_FLAGS);
	return FALSE;
    }

    enum_uilang.u.procA = pUILangEnumProc;
    enum_uilang.flags = dwFlags;
    enum_uilang.param = lParam;

    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        enum_uilang_proc_a(0, (LPSTR)RT_STRING, (LPCSTR)LOCALE_ILANGUAGE, supportedLocals[i], (LONG_PTR)&enum_uilang);
    }
    // EnumResourceLanguagesA( kernel32_handle, (LPCSTR)RT_STRING, (LPCSTR)LOCALE_ILANGUAGE, enum_uilang_proc_a, (LONG_PTR)&enum_uilang);
    return TRUE;
}

/******************************************************************************
 *           EnumUILanguagesW (KERNEL32.@)
 */

BOOL WINAPI EnumUILanguagesW_k(UILANGUAGE_ENUMPROCW pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam)
{
    ENUM_UILANG_CALLBACK enum_uilang;
    int i;

    TRACE("%p, %x, %lx\n", pUILangEnumProc, dwFlags, lParam);


    if(!pUILangEnumProc) {
	SetLastError_k(ERROR_INVALID_PARAMETER);
	return FALSE;
    }
    if(dwFlags) {
	SetLastError_k(ERROR_INVALID_FLAGS);
	return FALSE;
    }

    enum_uilang.u.procW = pUILangEnumProc;
    enum_uilang.flags = dwFlags;
    enum_uilang.param = lParam;

    for (i=0;i<NUMBER_OF_SUPPORTED_LOCALS;i++) {
        enum_uilang_proc_w(0, (LPWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE, supportedLocals[i], (LONG_PTR)&enum_uilang);
    }
    // EnumResourceLanguagesW( kernel32_handle, (LPCWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE, enum_uilang_proc_w, (LONG_PTR)&enum_uilang);
    return TRUE;
}

INT WINAPI GetGeoInfoW_k(GEOID GeoId, GEOTYPE GeoType, LPWSTR lpGeoData, 
                int cchData, LANGID language)
{
    FIXME("%d %d %p %d %d\n", GeoId, GeoType, lpGeoData, cchData, language);
    return 0;
}

INT WINAPI GetGeoInfoA_k(GEOID GeoId, GEOTYPE GeoType, LPSTR lpGeoData, 
                int cchData, LANGID language)
{
    FIXME("%d %d %p %d %d\n", GeoId, GeoType, lpGeoData, cchData, language);
    return 0;
}

INT WINAPI GetUserDefaultLocaleName_k(LPWSTR localename, int buffersize)
{
    LCID userlcid;

    TRACE("%p, %d\n", localename,  buffersize);
    
    userlcid = GetUserDefaultLCID_k();
    return LCIDToLocaleName_k(userlcid, localename, buffersize, 0);
}

/******************************************************************************
 *           NormalizeString (KERNEL32.@)
 */
INT WINAPI NormalizeString_k(NORM_FORM NormForm, LPCWSTR lpSrcString, INT cwSrcLength,
                           LPWSTR lpDstString, INT cwDstLength)
{
    FIXME("%x %p %d %p %d\n", NormForm, lpSrcString, cwSrcLength, lpDstString, cwDstLength);
    SetLastError_k(ERROR_CALL_NOT_IMPLEMENTED);
    return 0;
}

/******************************************************************************
 *           IsNormalizedString (KERNEL32.@)
 */
BOOL WINAPI IsNormalizedString_k(NORM_FORM NormForm, LPCWSTR lpString, INT cwLength)
{
    FIXME("%x %p %d\n", NormForm, lpString, cwLength);
    SetLastError_k(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

enum {
    BASE = 36,
    TMIN = 1,
    TMAX = 26,
    SKEW = 38,
    DAMP = 700,
    INIT_BIAS = 72,
    INIT_N = 128
};

static __inline INT adapt(INT delta, INT numpoints, BOOL firsttime)
{
    INT k;

    delta /= (firsttime ? DAMP : 2);
    delta += delta/numpoints;

    for(k=0; delta>((BASE-TMIN)*TMAX)/2; k+=BASE)
        delta /= BASE-TMIN;
    return k+((BASE-TMIN+1)*delta)/(delta+SKEW);
}

/******************************************************************************
 *           IdnToAscii (KERNEL32.@)
 * Implementation of Punycode based on RFC 3492.
 */
INT WINAPI IdnToAscii_k(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar,
                      LPWSTR lpASCIICharStr, INT cchASCIIChar)
{
    static const WCHAR prefixW[] = {'x','n','-','-'};

    WCHAR *norm_str;
    INT i, label_start, label_end, norm_len, out_label, out = 0;

    TRACE("%x %p %d %p %d\n", dwFlags, lpUnicodeCharStr, cchUnicodeChar,
        lpASCIICharStr, cchASCIIChar);

    norm_len = IdnToNameprepUnicode_k(dwFlags, lpUnicodeCharStr, cchUnicodeChar, NULL, 0);
    if(!norm_len)
        return 0;
    norm_str = HeapAlloc_k(GetProcessHeap_k(), 0, norm_len*sizeof(WCHAR));
    if(!norm_str) {
        SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    norm_len = IdnToNameprepUnicode_k(dwFlags, lpUnicodeCharStr,
            cchUnicodeChar, norm_str, norm_len);
    if(!norm_len) {
        HeapFree_k(GetProcessHeap_k(), 0, norm_str);
        return 0;
    }

    for(label_start=0; label_start<norm_len;) {
        INT n = INIT_N, bias = INIT_BIAS;
        INT delta = 0, b = 0, h;

        out_label = out;
        for(i=label_start; i<norm_len && norm_str[i]!='.' &&
                norm_str[i]!=0x3002 && norm_str[i]!='\0'; i++)
            if(norm_str[i] < 0x80)
                b++;
        label_end = i;

        if(b == label_end-label_start) {
            if(label_end < norm_len)
                b++;
            if(!lpASCIICharStr) {
                out += b;
            }else if(out+b <= cchASCIIChar) {
                memcpy(lpASCIICharStr+out, norm_str+label_start, b*sizeof(WCHAR));
                out += b;
            }else {
                HeapFree_k(GetProcessHeap_k(), 0, norm_str);
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            label_start = label_end+1;
            continue;
        }

        if(!lpASCIICharStr) {
            out += 5+b; /* strlen(xn--...-) */
        }else if(out+5+b <= cchASCIIChar) {
            memcpy(lpASCIICharStr+out, prefixW, sizeof(prefixW));
            out += 4;
            for(i=label_start; i<label_end; i++)
                if(norm_str[i] < 0x80)
                    lpASCIICharStr[out++] = norm_str[i];
            lpASCIICharStr[out++] = '-';
        }else {
            HeapFree_k(GetProcessHeap_k(), 0, norm_str);
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        if(!b)
            out--;

        for(h=b; h<label_end-label_start;) {
            INT m = 0xffff, q, k;

            for(i=label_start; i<label_end; i++) {
                if(norm_str[i]>=n && m>norm_str[i])
                    m = norm_str[i];
            }
            delta += (m-n)*(h+1);
            n = m;

            for(i=label_start; i<label_end; i++) {
                if(norm_str[i] < n) {
                    delta++;
                }else if(norm_str[i] == n) {
                    for(q=delta, k=BASE; ; k+=BASE) {
                        INT t = k<=bias ? TMIN : k>=bias+TMAX ? TMAX : k-bias;
                        INT disp = q<t ? q : t+(q-t)%(BASE-t);
                        if(!lpASCIICharStr) {
                            out++;
                        }else if(out+1 <= cchASCIIChar) {
                            lpASCIICharStr[out++] = disp<='z'-'a' ?
                                'a'+disp : '0'+disp-'z'+'a'-1;
                        }else {
                            HeapFree_k(GetProcessHeap_k(), 0, norm_str);
                            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                            return 0;
                        }
                        if(q < t)
                            break;
                        q = (q-t)/(BASE-t);
                    }
                    bias = adapt(delta, h+1, h==b);
                    delta = 0;
                    h++;
                }
            }
            delta++;
            n++;
        }

        if(out-out_label > 63) {
            HeapFree_k(GetProcessHeap_k(), 0, norm_str);
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end < norm_len) {
            if(!lpASCIICharStr) {
                out++;
            }else if(out+1 <= cchASCIIChar) {
                lpASCIICharStr[out++] = norm_str[label_end] ? '.' : 0;
            }else {
                HeapFree_k(GetProcessHeap_k(), 0, norm_str);
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
        }
        label_start = label_end+1;
    }

    HeapFree_k(GetProcessHeap_k(), 0, norm_str);
    return out;
}

/******************************************************************************
 *           IdnToNameprepUnicode (KERNEL32.@)
 */
INT WINAPI IdnToNameprepUnicode_k(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar,
                                LPWSTR lpNameprepCharStr, INT cchNameprepChar)
{
    enum {
        UNASSIGNED = 0x1,
        PROHIBITED = 0x2,
        BIDI_RAL   = 0x4,
        BIDI_L     = 0x8
    };

    extern const unsigned short nameprep_char_type[];
    extern const WCHAR nameprep_mapping[];
    const WCHAR *ptr;
    WORD flags;
    WCHAR buf[64], *map_str, norm_str[64], ch;
    DWORD i, map_len, norm_len, mask, label_start, label_end, out = 0;
    BOOL have_bidi_ral, prohibit_bidi_ral, ascii_only;

    TRACE("%x %p %d %p %d\n", dwFlags, lpUnicodeCharStr, cchUnicodeChar,
        lpNameprepCharStr, cchNameprepChar);

    if(dwFlags & ~(IDN_ALLOW_UNASSIGNED|IDN_USE_STD3_ASCII_RULES)) {
        SetLastError_k(ERROR_INVALID_FLAGS);
        return 0;
    }

    if(!lpUnicodeCharStr || cchUnicodeChar<-1) {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(cchUnicodeChar == -1)
        cchUnicodeChar = strlenW(lpUnicodeCharStr)+1;
    if(!cchUnicodeChar || (cchUnicodeChar==1 && lpUnicodeCharStr[0]==0)) {
        SetLastError_k(ERROR_INVALID_NAME);
        return 0;
    }

    for(label_start=0; (INT)label_start<cchUnicodeChar;) {
        ascii_only = TRUE;
        for(i=label_start; (INT)i<cchUnicodeChar; i++) {
            ch = lpUnicodeCharStr[i];

            if(i!=cchUnicodeChar-1 && !ch) {
                SetLastError_k(ERROR_INVALID_NAME);
                return 0;
            }
            /* check if ch is one of label separators defined in RFC3490 */
            if(!ch || ch=='.' || ch==0x3002 || ch==0xff0e || ch==0xff61)
                break;

            if(ch > 0x7f) {
                ascii_only = FALSE;
                continue;
            }

            if((dwFlags&IDN_USE_STD3_ASCII_RULES) == 0)
                continue;
            if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')
                    || (ch>='0' && ch<='9') || ch=='-')
                continue;

            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }
        label_end = i;
        /* last label may be empty */
        if(label_start==label_end && ch) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if((dwFlags&IDN_USE_STD3_ASCII_RULES) && (lpUnicodeCharStr[label_start]=='-' ||
                    lpUnicodeCharStr[label_end-1]=='-')) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if(ascii_only) {
            /* maximal label length is 63 characters */
            if(label_end-label_start > 63) {
                SetLastError_k(ERROR_INVALID_NAME);
                return 0;
            }
            if((INT)label_end < cchUnicodeChar)
                label_end++;

            if(!lpNameprepCharStr) {
                out += label_end-label_start;
            }else if(out+label_end-label_start <= (DWORD)cchNameprepChar) {
                memcpy(lpNameprepCharStr+out, lpUnicodeCharStr+label_start,
                        (label_end-label_start)*sizeof(WCHAR));
                if(lpUnicodeCharStr[label_end-1] > 0x7f)
                    lpNameprepCharStr[out+label_end-label_start-1] = '.';
                out += label_end-label_start;
            }else {
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }

            label_start = label_end;
            continue;
        }

        map_len = 0;
        for(i=label_start; i<label_end; i++) {
            ch = lpUnicodeCharStr[i];
            ptr = nameprep_mapping + nameprep_mapping[ch>>8];
            ptr = nameprep_mapping + ptr[(ch>>4)&0x0f] + 3*(ch&0x0f);

            if(!ptr[0]) map_len++;
            else if(!ptr[1]) map_len++;
            else if(!ptr[2]) map_len += 2;
            else if(ptr[0]!=0xffff || ptr[1]!=0xffff || ptr[2]!=0xffff) map_len += 3;
        }
        if(map_len*sizeof(WCHAR) > sizeof(buf)) {
            map_str = HeapAlloc_k(GetProcessHeap_k(), 0, map_len*sizeof(WCHAR));
            if(!map_str) {
                SetLastError_k(ERROR_NOT_ENOUGH_MEMORY);
                return 0;
            }
        }else {
            map_str = buf;
        }
        map_len = 0;
        for(i=label_start; i<label_end; i++) {
            ch = lpUnicodeCharStr[i];
            ptr = nameprep_mapping + nameprep_mapping[ch>>8];
            ptr = nameprep_mapping + ptr[(ch>>4)&0x0f] + 3*(ch&0x0f);

            if(!ptr[0]) {
                map_str[map_len++] = ch;
            }else if(!ptr[1]) {
                map_str[map_len++] = ptr[0];
            }else if(!ptr[2]) {
                map_str[map_len++] = ptr[0];
                map_str[map_len++] = ptr[1];
            }else if(ptr[0]!=0xffff || ptr[1]!=0xffff || ptr[2]!=0xffff) {
                map_str[map_len++] = ptr[0];
                map_str[map_len++] = ptr[1];
                map_str[map_len++] = ptr[2];
            }
        }

        norm_len = FoldStringW_k(MAP_FOLDCZONE, map_str, map_len,
                norm_str, sizeof(norm_str)/sizeof(WCHAR)-1);
        if(map_str != buf)
            HeapFree_k(GetProcessHeap_k(), 0, map_str);
        if(!norm_len) {
            if(GetLastError_k() == ERROR_INSUFFICIENT_BUFFER)
                SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if((INT)label_end < cchUnicodeChar) {
            norm_str[norm_len++] = lpUnicodeCharStr[label_end] ? '.' : 0;
            label_end++;
        }

        if(!lpNameprepCharStr) {
            out += norm_len;
        }else if(out+norm_len <= (DWORD)cchNameprepChar) {
            memcpy(lpNameprepCharStr+out, norm_str, norm_len*sizeof(WCHAR));
            out += norm_len;
        }else {
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }

        have_bidi_ral = prohibit_bidi_ral = FALSE;
        mask = PROHIBITED;
        if((dwFlags&IDN_ALLOW_UNASSIGNED) == 0)
            mask |= UNASSIGNED;
        for(i=0; i<norm_len; i++) {
            ch = norm_str[i];
            flags = get_table_entry( nameprep_char_type, ch );

            if(flags & mask) {
                SetLastError_k((flags & PROHIBITED) ? ERROR_INVALID_NAME
                        : ERROR_NO_UNICODE_TRANSLATION);
                return 0;
            }

            if(flags & BIDI_RAL)
                have_bidi_ral = TRUE;
            if(flags & BIDI_L)
                prohibit_bidi_ral = TRUE;
        }

        if(have_bidi_ral) {
            ch = norm_str[0];
            flags = get_table_entry( nameprep_char_type, ch );
            if((flags & BIDI_RAL) == 0)
                prohibit_bidi_ral = TRUE;

            ch = norm_str[norm_len-1];
            flags = get_table_entry( nameprep_char_type, ch );
            if((flags & BIDI_RAL) == 0)
                prohibit_bidi_ral = TRUE;
        }

        if(have_bidi_ral && prohibit_bidi_ral) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        label_start = label_end;
    }

    return out;
}

/******************************************************************************
 *           IdnToUnicode (KERNEL32.@)
 */
INT WINAPI IdnToUnicode(DWORD dwFlags, LPCWSTR lpASCIICharStr, INT cchASCIIChar,
                        LPWSTR lpUnicodeCharStr, INT cchUnicodeChar)
{
    extern const unsigned short nameprep_char_type[];

    INT i, label_start, label_end, out_label, out = 0;
    WCHAR ch;

    TRACE("%x %p %d %p %d\n", dwFlags, lpASCIICharStr, cchASCIIChar,
        lpUnicodeCharStr, cchUnicodeChar);

    for(label_start=0; label_start<cchASCIIChar;) {
        INT n = INIT_N, pos = 0, old_pos, w, k, bias = INIT_BIAS, delim=0, digit, t;

        out_label = out;
        for(i=label_start; i<cchASCIIChar; i++) {
            ch = lpASCIICharStr[i];

            if(ch>0x7f || (i!=cchASCIIChar-1 && !ch)) {
                SetLastError_k(ERROR_INVALID_NAME);
                return 0;
            }

            if(!ch || ch=='.')
                break;
            if(ch == '-')
                delim = i;

            if((dwFlags&IDN_USE_STD3_ASCII_RULES) == 0)
                continue;
            if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')
                    || (ch>='0' && ch<='9') || ch=='-')
                continue;

            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }
        label_end = i;
        /* last label may be empty */
        if(label_start==label_end && ch) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if((dwFlags&IDN_USE_STD3_ASCII_RULES) && (lpASCIICharStr[label_start]=='-' ||
                    lpASCIICharStr[label_end-1]=='-')) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }
        if(label_end-label_start > 63) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end-label_start<4 ||
                tolowerW(lpASCIICharStr[label_start])!='x' ||
                tolowerW(lpASCIICharStr[label_start+1])!='n' ||
                lpASCIICharStr[label_start+2]!='-' || lpASCIICharStr[label_start+3]!='-') {
            if(label_end < cchASCIIChar)
                label_end++;

            if(!lpUnicodeCharStr) {
                out += label_end-label_start;
            }else if(out+label_end-label_start <= cchUnicodeChar) {
                memcpy(lpUnicodeCharStr+out, lpASCIICharStr+label_start,
                        (label_end-label_start)*sizeof(WCHAR));
                out += label_end-label_start;
            }else {
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }

            label_start = label_end;
            continue;
        }

        if(delim == label_start+3)
            delim++;
        if(!lpUnicodeCharStr) {
            out += delim-label_start-4;
        }else if(out+delim-label_start-4 <= cchUnicodeChar) {
            memcpy(lpUnicodeCharStr+out, lpASCIICharStr+label_start+4,
                    (delim-label_start-4)*sizeof(WCHAR));
            out += delim-label_start-4;
        }else {
            SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        if(out != out_label)
            delim++;

        for(i=delim; i<label_end;) {
            old_pos = pos;
            w = 1;
            for(k=BASE; ; k+=BASE) {
                ch = i<label_end ? tolowerW(lpASCIICharStr[i++]) : 0;
                if((ch<'a' || ch>'z') && (ch<'0' || ch>'9')) {
                    SetLastError_k(ERROR_INVALID_NAME);
                    return 0;
                }
                digit = ch<='9' ? ch-'0'+'z'-'a'+1 : ch-'a';
                pos += digit*w;
                t = k<=bias ? TMIN : k>=bias+TMAX ? TMAX : k-bias;
                if(digit < t)
                    break;
                w *= BASE-t;
            }
            bias = adapt(pos-old_pos, out-out_label+1, old_pos==0);
            n += pos/(out-out_label+1);
            pos %= out-out_label+1;

            if((dwFlags&IDN_ALLOW_UNASSIGNED)==0 &&
                    get_table_entry(nameprep_char_type, n)==1/*UNASSIGNED*/) {
                SetLastError_k(ERROR_INVALID_NAME);
                return 0;
            }
            if(!lpUnicodeCharStr) {
                out++;
            }else if(out+1 <= cchASCIIChar) {
                memmove(lpUnicodeCharStr+out_label+pos+1,
                        lpUnicodeCharStr+out_label+pos,
                        (out-out_label-pos)*sizeof(WCHAR));
                lpUnicodeCharStr[out_label+pos] = n;
                out++;
            }else {
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            pos++;
        }

        if(out-out_label > 63) {
            SetLastError_k(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end < cchASCIIChar) {
            if(!lpUnicodeCharStr) {
                out++;
            }else if(out+1 <= cchUnicodeChar) {
                lpUnicodeCharStr[out++] = lpASCIICharStr[label_end];
            }else {
                SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
        }
        label_start = label_end+1;
    }

    return out;
}
