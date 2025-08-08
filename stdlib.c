#include <stddef.h>
#include <stdint.h>
#include "stdlib.h"
int itoa(int num, char* buf, size_t bufmax){
	if (!buf||bufmax<1)
		return -1;
	if (num<0){
		num*=-1;
		buf[0]='-';
		buf++;
		bufmax--;
	}
	unsigned int digitcnt = 0;
	int tmp = num;
	for (;tmp;tmp/=10){
		digitcnt++;
	}
	for (unsigned int i = 0;i<digitcnt;i++){
		buf[digitcnt-i-1] = '0'+(num%10);
		num/=10;
	}
	return 0;
}
int atoi(char* buf){
	if (!buf)
		return -1;
	int res = 0;
	for (unsigned int i = 0;buf[i];i++){
		if (buf[i]<'0'||buf[i]>'9')
			continue;
		unsigned int digit = buf[i]-'0';
		if (((int64_t)res*10+digit)>2146483647||((int64_t)res*10+digit)<-2147483647)
			break;
		res = res*10+digit;
	}
	return res;
}
char toUpper(char ch){
	static const char upperTable[]={
		['1'] = '!',
		['2'] = '@',
		['3'] = '#',
		['4'] = '$',
		['5'] = '%',
		['6'] = '^',
		['7'] = '&',
		['8'] = '*',
		['9'] = '(',
		['0'] = ')',
		['`'] = '~',
		['-'] = '_',
		['='] = '+',
		['['] = '{',
		[']'] = '}',
		[';'] = ':',
		['\''] = '"',
		[','] = '<',
		['.'] = '>',
		['/'] = '?',
	};
	if (ch<'a'||ch>'z')
		return upperTable[ch];
	return (ch-'a'+'A');
}
char toLower(char ch){
	static const char lowerTable[]={
		['!'] = '1',
		['@'] = '2',
		['#'] = '3',
		['$'] = '4',
		['%'] = '5',
		['^'] = '6',
		['&'] = '7',
		['*'] = '8',
		['('] = '9',
		[')'] = '0',
		['~'] = '`',
		['_'] = '-',
		['+'] = '=',
		['{'] = '[',
		['}'] = ']',
		[':'] = ';',
		['"'] = '\'',
		['<'] = ',',
		['>'] = '.',
		['?'] = '/',
	};
	if (ch<'A'||ch>'Z')
		return lowerTable[ch];
	return (ch-'A'+'a');
}
