#include "common.h"
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
void interpolate(void);
int edge_to_rpm(int);
void fdiv_func(int, int, int *, int *);
void engine(void);
struct table
{
  uchar t_val;
  ushort t_base;
  ushort t_increment;
};
int debug_val;
int debug_base;
int debug_interval;
struct table vaf_fi_tab0[] = {
  {0, 2400, 20}
  ,
  {50, 3400, 115}
  ,
  {100, 9150, 120}
  ,{150, 15150, 100}
  ,
  {200, 20150, 100}
  ,
  {255, 25650, 0}
};
struct table vaf_fi_tab1000[] = {
  {0, 2500, 20}
  ,
  {50, 3500, 40}
  ,{75, 4500, 150}
  ,{100, 8250, 200}
  ,
  {200, 28250, 200}
  ,
  {255, 39250, 0}
};
struct table vaf_fi_tab1500[] = {
  {0, 2600, 20}
  ,
  {50, 3600, 40}
  ,
  {75, 4600, 200}
  ,{100, 9600, 150}
  ,{150, 17100, 100}
  ,
  {200, 22100, 200}
  ,
  {255, 33100, 0}
};
struct table vaf_fi_tab2000[] = {
  {0, 2500, 10}
  ,{50, 3000, 30}
  ,
  {100, 4500, 120}
  ,{125, 7500, 180}
  ,
  {150, 12000, 300}
  ,
  {175, 19500, 400}
  ,
  {200, 29500, 300}
  ,
  {255, 46000, 0}
};
struct table vaf_fi_tab4000[] = {
  {0, 2500, 10}
  ,{50, 3000, 30}
  ,
  {100, 4500, 180}
  ,
  {125, 9000, 250}
  ,
  {150, 15250, 300}
  ,{175, 22750, 300}
  ,
  {200, 30250, 300}
  ,
  {255, 46750, 0}
};
struct table vaf_fi_tab4500[] = {
  {0, 2500, 10}
  ,{50, 3000, 30}
  ,
  {100, 4500, 180}
  ,
  {125, 9000, 250}
  ,
  {150, 15250, 325}
  ,{175, 23375, 325}
  ,
  {200, 31500, 300}
  ,
  {255, 48000, 0}
};
struct table vaf_fi_tab5000[] = {
  {0, 2500, 10}
  ,{50, 3000, 30}
  ,
  {100, 4500, 180}
  ,
  {125, 9000, 200}
  ,
  {150, 14000, 300}
  ,{175, 21500, 300}
  ,
  {200, 29000, 300}
  ,
  {255, 45500, 0}
};
struct table vaf_fi_tab8000[] = {
  {0, 2500, 10}
  ,
  {50, 3000, 30}
  ,
  {100, 4500, 180}
  ,{150, 13500, 275}
  ,
  {175, 20375, 280}
  ,
  {200, 27375, 290}
  ,
  {225, 34625, 300}
  ,
  {255, 43625, 0}
};
struct rtable
{
  ushort r_rpm;
  struct table *r_tab;
};
struct rtable rpm_vfi_tab[] = {
  {0, vaf_fi_tab0}
  ,
  {1000, vaf_fi_tab1000}
  ,
  {1500, vaf_fi_tab1500}
  ,
  {2000, vaf_fi_tab2000}
  ,
  {4000, vaf_fi_tab4000}
  ,
  {4500, vaf_fi_tab4500}
  ,
  {5000, vaf_fi_tab5000}
  ,
  {8000, vaf_fi_tab8000}
};
struct table *tab1;
struct table *tab2;
ushort rpm1;
ushort rpm2;
ushort rpm;
uchar ival;
ushort inter_val;
void engine(void)
{
  struct rtable *rtp;
  int vaf, rpm;
  unsigned int hc_count;
  for (vaf = 0; vaf < 255; vaf += 10)
    {
      ival = vaf;
      hc_count = 60000;
      while (hc_count > 4000)
	{
	  rpm = edge_to_rpm(hc_count);
	  hc_count -= hc_count / 25;
	  rtp = rpm_vfi_tab;
	  while (rtp->r_rpm < rpm)
            rtp++;
	  if (rtp->r_rpm == rpm)
	    {
	      tab1 = tab2 = rtp->r_tab;
	      rpm1 = rpm2 = rpm;
	    } 
	  else
	    {
	      tab2 = rtp->r_tab;
	      rpm2 = rtp->r_rpm;
	      rtp--;
	      tab1 = rtp->r_tab;
	      rpm1 = rtp->r_rpm;
	    }
	  interpolate();
	}
      debug_val += rpm;
      debug_base += vaf;
      debug_interval += inter_val;
    }
  return;
}
struct table *tabptr;
ushort rpm_delta;
ushort inter1;
ushort inter2;
ushort inter_delta;
ushort offset;
ushort inter_temp;
uint big_temp;
void interpolate(void)
{
  tabptr = tab1;
  while (tabptr->t_val < ival)
    tabptr++;
  if (tabptr->t_val == ival)
    {
      inter1 = tabptr->t_base;
    } 
  else
    {
      tabptr--;
      inter1 = tabptr->t_base +
	((ival - tabptr->t_val) * tabptr->t_increment);
    }
  if (tab2 == tab1)
    {
      inter_val = inter1;
    } 
  else
    {
      tabptr = tab2;
      while (tabptr->t_val < ival)
	tabptr++;
      if (tabptr->t_val == ival)
	{
	  inter2 = tabptr->t_base;
	} 
      else
	{
	  tabptr--;
	  inter2 = tabptr->t_base +
	    ((ival - tabptr->t_val) * tabptr->t_increment);
	}
      rpm_delta = rpm2 - rpm1;
      if (inter2 > inter1)
	{
	  inter_delta = inter2 - inter1;
	  offset = rpm - rpm1;
	  inter_temp = inter1;
	} 
      else
	{
	  inter_delta = inter1 - inter2;
	  offset = rpm2 - rpm;
	  inter_temp = inter2;
	}
      big_temp = offset * inter_delta;
      inter_val = big_temp / rpm_delta;
      inter_val += inter_temp;
    }
  return;
}
int edge_to_rpm(count)
     int count;
{
  int rpm;
  int quot, rem;
  fdiv_func(1, count, &quot, &rem);
  rpm = quot * 458;
  fdiv_func(rem, count, &quot, &rem);
  rpm += quot / 143;
  return (rpm);
}
void fdiv_func(num, den, quot, rem)
     int num;
     int den;
     int *quot;
     int *rem;
{
  int temp;
  temp = num * 65536;
  *quot = temp / den;
  *rem = temp - (*quot * den);
}
int main(void)
{
  engine();
  if ((debug_val != 191932) || (debug_base != 3250) || (debug_interval != 418230))
    {
      puts("engine: failed\n");
      return 0;
    }
  puts("engine: success\n");
  return 0;
}
