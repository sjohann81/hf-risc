#include <hf-risc.h>

volatile int32_t cmpcount=0, ccount=0;

void timer0b_handler(void)
{
	ccount++;
}

void timer1ctc_handler(void)
{
	cmpcount++;
}

char text[]="Testing...\n";
char buf[20];
int xyz=0xbadbeef;
int abc;

char *strcpy2(char *s, const char *t){
	char *tmp=s;

	while((int)(*s++=*t++)) ;
	return(tmp);
}

static void itoa2(long n, char *s, int base, long *digits){
	long i,j,sign;
	unsigned long n2;
	char number[20];

	for(i=0;i<15;++i){
		number[i]=' ';
	}
	number[15]=0;
	if(n>=0||base!=10){
		sign=1;
	}else{
		sign=-1;
	}
	n2=n*sign;
	for(j=14;j>=0;--j){
		i=n2%base;
		n2/=base;
		number[j]=i<10?'0'+i:'a'+i-10;
		if(n2==0&&15-j>=*digits) break;
	} 
	if(sign==-1){
		number[--j]='-';
	}
	if(*digits==0||*digits<15-j) {
		strcpy2(s,&number[j]);
		*digits=15-j;
	}else{
		strcpy2(s,&number[15-*digits]);
	}
}

void print(long num,long base,long digits){
	char *ptr,buffer[128];

	itoa2(num,buffer,base,&digits);
	ptr=buffer;
	while(*ptr){
		putchar(*ptr++);
		if(ptr[-1]=='\n') *--ptr='\r';
	}
}              

void print_hex(unsigned long num){
	long i;
	unsigned long j;

	for(i=28;i>=0;i-=4){
		j=((num>>i)&0xf);
		if(j<10) putchar('0'+j);
		else putchar('a'-10+j);
	}
}

void print_string(char *p){
	int i;

	for(i=0;p[i];++i){
		putchar(p[i]);
	}
}

int prime(){
	int i,j;

	for(i=3;i<1000;i+=2){
		for(j=3;j<i;j+=2){
			if(i%j==0){
				j=0;
				break;
			}
		}
		if(j){
			print(i,10,0);
			putchar(' ');
		}
	}
	putchar('\n');
	return 0;
}

int main(void)
{
	long i,j;
	char char_buf[16];
	short short_buf[16];
	long long_buf[16];

	TIMER1PRE = TIMERPRE_DIV4;
	TIMER1 = TIMERSET;
	TIMER1 = 0;
	TIMER1CTC = 18750;

	cmpcount=0; ccount=0;

	TIMERMASK |= (MASK_TIMER0B | MASK_TIMER1CTC);

	while(1){

		//test shift
		j=0x12345678;
		for(i=0;i<32;++i){
			print_hex(j>>i);
			putchar(' ');
		}
		putchar('\n');
		j=0x92345678;
		for(i=0;i<32;++i){
			print_hex(j>>i);
			putchar(' ');
		}
		putchar('\n');
		j=0x12345678;
		for(i=0;i<32;++i){
			print_hex(j<<i);
			putchar(' ');
		}
		putchar('\n');
		putchar('\n');
  
		//test multiply and divide
		j=7;
		for(i=0;i<=10;++i){
			print(j*i,10,0);
			putchar(' ');
		}
		putchar('\n');
		j=0x321;
		for(i=0;i<=5;++i){
			print_hex(j*(i+0x12345));
			putchar(' ');
		}
		putchar('\n');
		j=0x54321;
		for(i=0;i<=5;++i){
			print_hex(j*(i+0x123));
			putchar(' ');
		}
		putchar('\n');
		j=0x12345;
		for(i=1;i<10;++i){
			print_hex(j/i);
			putchar(' ');
		}
		putchar('\n');
		for(i=1;i<10;++i) {
			print_hex(j%i);
			putchar(' ');
		}
		putchar('\n');
		putchar('\n');

		//test addition and subtraction
		j=0x1234;
		for(i=0;i<10;++i){
			print_hex(j+i);
			putchar(' ');
		}
		putchar('\n');
		for(i=0;i<10;++i){
			print_hex(j-i);
			putchar(' ');
		}
		putchar('\n');
		putchar('\n');
  
		//test bit operations
		i=0x1234;
		j=0x4321;
		print_hex(i&j);
		putchar(' ');
		print_hex(i|j);
		putchar(' ');
		print_hex(i^j);
		putchar(' ');
		print_hex(~i);
		putchar(' ');
		print_hex(i+0x12);
		putchar(' ');
		print_hex(i-0x12);
		putchar('\n');
		putchar('\n');
  
		//test memory access
		for(i=0;i<10;++i){
			char_buf[i]=i;
			short_buf[i]=i;
			long_buf[i]=i;
		}
		for(i=0;i<10;++i){
			j=char_buf[i];
			print(j,10,0);
			putchar(' ');
			j=short_buf[i];
			print(j,10,0);
			putchar(' ');
			j=long_buf[i];
			print(j,10,0);
			putchar('\n');
		}
		putchar('\n');
   
		prime();

		printf("\ninterrupts -> compare: %d counter: %d\n\n", cmpcount, ccount);

	}
}

