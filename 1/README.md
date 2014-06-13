# 实验一的代码说明 #
本代码经过ANSI和UNICODE两种字符集模式编译，都通过了。

## VC/VS版本的字符集问题 ##

自从VC6.0开始，就已经支持UNICODE字符集，但是VC6.0**默认**的是“多字节字符集”（MBCS），使用UNICODE字符集需要自行在项目中定义UNICODE宏和_UNICODE宏；然而VS.NET2002以及更高版本**默认**的却是UNICODE字符集，即定义了UNICODE宏和_UNICODE宏，除非你在项目属性中人工指定使用“多字节字符集”（MBCS），那样就不会定义UNICODE宏和_UNICODE宏。

VS2013的MFC更是完全废除了MBCS，使用MBCS会给编译错误信息：MBCS is depreciated。

## 什么是ANSI字符集和UNICODE字符集？ ##

> 特别强调：这里说的确实是ANSI，不是ASCII。

### ANSI代码页（ANSI code page） ###
最早的时候，计算机字符编码只有0-127的编码，叫做ASCII字符集，然而其它国家的文字就无法被表示。于是，西欧国家就创建了更大的字符集，名叫latin-1字符集。后来，日本为日文编写了一个字符编码，名叫Shift-JIS；中国大陆也为简体字日文编写了一个字符编码，名叫GB2312-80；中国台湾也为繁体字日文编写了一个字符编码，名叫BIG-5。后来中国大陆又兼GB2312-80的汉字大少，只有简体字，所以又把繁体字拉进来，出现了GBK字符集，后来又出现了GB18030-2000字符集，等等。

这么多字符集混乱不堪，那么计算机怎么知道你要用哪套字符集呢？于是，提出了一套“治标不治本”的方案，这就是“ANSI代码页”。每种字符集规定一个数字作为唯一标识符，叫做“代码页”，更换这个唯一标识符就切换字符集。例如ASCII的代码页是437，latin-1的代码页是1252，GB2312-80的代码页是20936，GBK的代码页是936，BIG-5的代码页是950，GB18030的代码页是54936等等。

起初，Windows用的就是“ANSI代码页”切换方案，其字符串的储存方式就是用char数组来储存，叫做“多字节字符集”（MBCS），之所以这么叫是因为有些字符占不止一个字符，例如一个GBK汉字占两个字节。

微软还在C标准以外，私自定义了以下几个“C风格窄字符串”（以'\0'结尾的窄字符数组）类型：

    #define CONST const
    typedef char CHAR;
    typedef CHAR *NPSTR, *LPSTR, *PSTR;
    typedef CONST CHAR *LPCSTR, *PCSTR;

### UNICODE字符集 ###

上面说过了，这几个方法只是“治标不治本”的方案。依靠切换ANSI代码页根本靠不住，例如同屏显示简繁体字就成了个技术难题（所以才有了“同文堂”等软件）。如果同时显示中文和日文，那难度更大。更严重的是，你的软件在你的电脑上用简体中文Windows上运行得很好（用936代码页），别人的电脑上可能是繁体中文（代码页950）或英文（代码页1252）看到的只能是一堆乱码！！！

那怎么办？为什么要多种字符集“各自为政”，每种文字体系一种字符编码集？难道不能全球统一都用同一种字符编码集吗？只要这种字符编码集包含了全球一切文字就行了啊！是的，就是这样，于是Unicode出现了。“Unicode联盟”（Unicode Consortium）与“国际化标准组织”（ISO）共同创建了一套包含全球一切文字的字符编码集，前者叫做“Unicode字符集”，后者叫做“ISO/IEC 10646”国际标准。由于这两个标准是相互兼容的，我们可以认为是同一套标准。正是这套标准，才从根本上解决了乱码问题。

Unicode（ISO/IEC 10646）的字符编码有三种“表示方式”，分别是UTF-8、UTF-16、UTF-32。后两者又有大小端的区别（读者可自行搜索“大端模式”和“小端模式”，这里不细讲）。

【ISO国际标准】根据ISO的C标准和C++标准，字符类型有两种：一种叫做“窄字符”（narrow character），用char表示，每个单位占一个字节；另一种叫做“宽字符”（wide character），用wchar_t表示，每个单位占的字节数必须多于char，但没有规定具体占几个字节。窄字符和宽字符的表示方式分别是：char s[] = "narrow character"; wchar_t ws[] = L"wide character";

【微软标准】在微软的编译器中，宽字符集被叫做“UNICODE字符集”，而且特指“UTF-16小端模式”（UTF-16 little endian），每个单位占两个字节，类型用wchar_t表示。

微软还在C标准以外，私自定义了以下几个“C风格宽字符串”（以L'\0'结尾的宽字符数组）类型：

    #define CONST const
    typedef wchar_t WCHAR;
    typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
    typedef CONST WCHAR *LPCWSTR, *PCWSTR;

### TCHAR类型 ###
为了便于在ANSI字符集和UNICODE字符集之间的切换，微软私自定义了一个字符类型——TCHAR。在<winnt.h>中定义如下：

    typedef wchar_t WCHAR;
    #ifdef  UNICODE
    typedef WCHAR TCHAR, *PTCHAR;
    typedef LPWSTR PTSTR, LPTSTR;
    typedef LPCWSTR PCTSTR, LPCTSTR;
    #else
    typedef char TCHAR, *PTCHAR;
    typedef LPSTR PTSTR, LPTSTR;
    typedef LPCSTR PCTSTR, LPCTSTR, PCUTSTR, LPCUTSTR;
    #endif

此外，微软还在<winnt.h>定义了TEXT宏：

    #ifdef  UNICODE
    #define __TEXT(quote) L##quote
    #else
    #define __TEXT(quote) quote
    #endif
    #define TEXT(quote) __TEXT(quote)

微软还在<tchar.h>头文件中定义了_T宏和_TEXT宏：

    #ifdef _UNICODE
    #define __T(x)      L ## x
    typedef wchar_t     TCHAR;
    #else
    #define __T(x)      x
    typedef char            TCHAR;
    #endif
    #define _T(x)       __T(x)
    #define _TEXT(x)    __T(x)

ISO的C和C++标准也有对应的窄字符集函数printf、strcat、strcmp和宽字符集函数wprintf、strcat、strcmp等等。微软也私自在<tchar.h>中宏定义了这些函数的TCHAR风格类型：

    #ifdef _UNICODE
    #define _tprintf        wprintf
    #define _tcscmp         wcscmp
    #define _tcscat         wcscat
    #else
    #define _tprintf        printf
    #define _tcscmp         strcmp
    #define _tcscat         strcat
    #endif

## 跨平台时要注意的几点 ##
【ISO国际标准】ISO的C标准和C++标准只规定了窄字符char为一个字节，宽字符wchar_t的字节数大于char。没有规定wchar_t具体占几个字节。没有TCHAR这个东西，不存在<tchar.h>头文件。

【微软标准】在微软的编译器中，char每个单位占一个字节，表示ANSI字符集；wchar_t占两个字节，表示UTF-16小端模式。可以用TCHAR类型（在<tchar.h>头文件中），并用是否定义UNICODE宏和_UNICODE宏来切换。

【Linux通常情况】在大多数Linux下的编译器中，char每个单位占一个字节，通常表示UTF-8字符集；wchar_t占四个字节，表示UTF-32字符集，大小端与CPU相关。没有TCHAR这种东西。