/* ram_image.c by Steve Rhoads 11/7/05 
 * This program take the ram_xilinx.vhd file as input
 * and the code.txt file as input.
 * It then creates ram_image.vhd as output with the
 * initialization vectors set to the contents of code.txt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE (1024*1024)

int main(int argc, char *argv[])
{
   FILE *file;
   int i, j, index, size, count;
   char *buf, *ptr, *ptr_list[64*4], text[80];
   unsigned int *code;

   if(argc < 4)
   {
      printf("Usage: ram_image <in.vhd> <in_code.txt> <out.vhd>\n");
      printf("Usage: ram_image ram_xilinx.vhd code.txt ram_image.vhd\n");
      return 0;
   }

   buf = (char*)malloc(BUF_SIZE);
   code = (unsigned int *)malloc(BUF_SIZE);

   //Read ram_xilinx.vhd
   file = fopen(argv[1], "rb");
   if(file == NULL)
   {
      printf("Can't open %s!\n", argv[1]);
      return -1;
   }
   size = fread(buf, 1, BUF_SIZE, file);
   fclose(file);

   //Read code.txt
   file = fopen(argv[2], "r");
   if(file == NULL)
   {
      printf("Can't open %s!\n", argv[2]);
      return -1;
   }
   for(count = 0; count < 16*1024; ++count)
   {
      if(feof(file))
         break;
      fscanf(file, "%x", &code[count]);
   }
   fclose(file);

   //Find 'INIT_00 => X"'
   ptr = buf;
   for(i = 0; i < 64*4; ++i)
   {
      sprintf(text, "INIT_%2.2X => X\"", i % 64);
      ptr = strstr(ptr, text);
      if(ptr == NULL)
      {
         printf("ERROR:  Can't find '%s' in file!\n", text);
         return -1;
      }
      ptr_list[i] = ptr + strlen(text);
   }

   //Modify vhdl source code
   j = 62;
   for(i = 0; i < count; ++i)
   {
      sprintf(text, "%8.8x", code[i]);
      index = i / 32;
      ptr_list[index][j] = text[0];
      ptr_list[index][j+1] = text[1];
      ptr_list[index+64][j] = text[2];
      ptr_list[index+64][j+1] = text[3];
      ptr_list[index+128][j] = text[4];
      ptr_list[index+128][j+1] = text[5];
      ptr_list[index+192][j] = text[6];
      ptr_list[index+192][j+1] = text[7];
      j -= 2;
      if(j < 0)
         j = 62;
   }

   //Write ram_image.vhd
   file = fopen(argv[3], "wb");
   if(file == NULL)
   {
      printf("Can't write %s!\n", argv[3]);
      return -1;
   }
   fwrite(buf, 1, size, file);
   fclose(file);
   free(buf);
   free(code);
   return 0;
}
