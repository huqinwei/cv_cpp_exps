
#include <string.h>
#include <ctype.h>
/*****************************************************************************
**  ��������: Trim
**  ��������: ����һ��C-String�ı���(������ֵ����)��ȥ����β����Blank���ַ�
**  ��ǰ�汾:
**  ��    ��: JohnnyHu
**  ��    �ģ�
**  �������: char *src   --->  ����'\0'��C-String
**  �������: char *src       --->  ��ʽ����(ȥ����β����Blank���ַ�)
**  ���ؽ������
**  ʹ������������ĸֵ������C-String����
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
**  ��������: Trim2 (�����⺯��)
**  ��������: ����һ��C-String�ı���(������ֵ����)��ȥ����β����Blank���ַ�
**  ��ǰ�汾:
**  ��    ��: JohnnyHu
**  ��    �ģ�
**  �������: char *src   --->  ����'\0'��C-String
**  �������: char *src       --->  ��ʽ����(ȥ����β����Blank���ַ�)
**  ���ؽ����src
**  ʹ������������ĸֵ������C-String����
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