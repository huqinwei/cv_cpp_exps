
#include <string.h>
#include <ctype.h>
/*****************************************************************************
**  函数名称: Trim
**  功能描述: 给定一个C-String的变量(非字面值常量)，去除首尾两端Blank的字符
**  当前版本:
**  作    者: JohnnyHu
**  修    改：
**  输入参数: char *src   --->  带有'\0'的C-String
**  输出参数: char *src       --->  格式化后(去除首尾两端Blank的字符)
**  返回结果：无
**  使用条件：非字母值常量的C-String变量
****************************************************************************/
void Trim(char* src)
{
    char* begin = src;
    char* end = src;

    while (*end++) {
        ;  // Statement is null  
    }

    if (begin == end)  return;

    while (*begin == ' ' || *begin == '\t')
        ++begin;
    while ((*end) == '\0' || *end == ' ' || *end == '\t')
        --end;

    if (begin > end) {
        *src = '\0';   return;
    }
    //printf("begin=%1.1s; end=%1.1s\n", begin, end);
    while (begin != end) {
        *src++ = *begin++;
    }

    *src++ = *end;
    *src = '\0';

    return;
}
/*****************************************************************************
**  函数名称: Trim2 (依赖库函数)
**  功能描述: 给定一个C-String的变量(非字面值常量)，去除首尾两端Blank的字符
**  当前版本:
**  作    者: JohnnyHu
**  修    改：
**  输入参数: char *src   --->  带有'\0'的C-String
**  输出参数: char *src       --->  格式化后(去除首尾两端Blank的字符)
**  返回结果：src
**  使用条件：非字母值常量的C-String变量
****************************************************************************/
char* Trim2(char* src)
{
    char* ori_src = src;

    char* begin = src;
    char* end = src + strlen(src);

    if (begin == end)  return ori_src;

    while (isblank(*begin))
        ++begin;

    while (isblank(*end) || !(*end))
        --end;

    if (begin > end) {
        *src = '\0';   return  ori_src;
    }


    //printf("begin=%1.1s; end=%1.1s\n", begin, end);
    while (begin != end) {
        *src++ = *begin++;
    }

    *src++ = *end;
    *src = '\0';

    return ori_src;
}