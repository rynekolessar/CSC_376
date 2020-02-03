#include	<stdlib.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<wchar.h>
#include	<locale.h>

int isUpper (wchar_t wc)
{
	// YOUR CODE HERE
}

int isLower (wchar_t wc)
{
  	// YOUR CODE HERE
}

wchar_t toUpper (wchar_t wc)
{
  	// YOUR CODE HERE
}

wchar_t toLower (wchar_t wc)
{
  	// YOUR CODE HERE
}

int main ()
{  
	setlocale(LC_ALL,"");

	wchar_t	wc;

	for (wc = 0x20; wc <= 0xFE; wc++)
  	{
		if((wc >= 0x7F) && (wc <= 0x9F))
			continue;

		wprintf(L"U+%04X is %lc ",wc,wc);

		if(isUpper(wc))
			wprintf(L": uppercase, lower form %lc\n",toLower(wc));
		else
			if (isLower(wc))
	  			wprintf(L": lowercase, upper form %lc\n",toUpper(wc));
			else
	  			wprintf(L"\n");
  }
  return(EXIT_SUCCESS);
}