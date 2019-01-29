/* MDH WCET BENCHMARK SUITE */
/*
 * Changes: CS 2006/05/19: Changed loop bound from constant to variable.
 */

#include <hf-risc.h>

int fac (int n)
{
  if (n == 0)
     return 1;
  else
     return (n * fac (n-1));
}

int _main (void)
{
  int i;
  int s = 0;
  volatile int n;

  n = 5;
  for (i = 0;  i <= n; i++)
      s += fac (i);

  return (s);
}


void main(void){
	volatile unsigned int cycles;

	printf("\nFAC benchmark");
	cycles = TIMER0;
	_main();
	cycles = TIMER0 - cycles;
	printf("\nWCET: %d cycles\n", cycles);
	
}

