#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>

const wchar_t ah = L'\x3042';
const wchar_t n	 = L'\x3093';

int main () 
{
	setlocale(LC_ALL,"en_US.UTF-8");

	wchar_t i;

	for (i = ah; i <= n; i++)
	{
		wprintf(L"%lc\n",i);
	}
	return(0);
}