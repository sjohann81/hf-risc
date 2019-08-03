// very simple startup code with definition of handlers for all cortex-m cores
#include <cstdlib>
#include <new>

typedef void (*ptr_func_t)();

// main application
extern void main_app();

// location of these variables is defined in linker script
extern unsigned __data_start;
extern unsigned __data_end;
extern unsigned __data_load;

extern unsigned __bss_start;
extern unsigned __bss_end;

extern unsigned __heap_start;

extern ptr_func_t __preinit_array_start[];
extern ptr_func_t __preinit_array_end[];

extern ptr_func_t __init_array_start[];
extern ptr_func_t __init_array_end[];

extern ptr_func_t __fini_array_start[];
extern ptr_func_t __fini_array_end[];


/** Copy default data to DATA section
 */
void copy_data() {
    unsigned *src = &__data_load;
    unsigned *dst = &__data_start;
    while (dst < &__data_end) {
        *dst++ = *src++;
    }
}

/** Erase BSS section
 */
void zero_bss() {
    unsigned *dst = &__bss_start;
    while (dst < &__bss_end) {
        *dst++ = 0;
    }
}

/** Fill heap memory
 */
void fill_heap(unsigned fill=0x45455246) {
/*    unsigned *dst = &__heap_start;
    unsigned *msp_reg;
    __asm__("mrs %0, msp\n" : "=r" (msp_reg) );
    while (dst < msp_reg) {
        *dst++ = fill;
    }*/
}

/** Call constructors for static objects
 */
void call_init_array() {
    auto array = __preinit_array_start;
    while (array < __preinit_array_end) {
        (*array)();
        array++;
    }

    array = __init_array_start;
    while (array < __init_array_end) {
        (*array)();
        array++;
    }
}

/** Call destructors for static objects
 */
void call_fini_array() {
    auto array = __fini_array_start;
    while (array < __fini_array_end) {
        (*array)();
        array++;
    }
}

extern "C" {
extern void free(void *ptr);
extern void *malloc(unsigned int size);
}
/*
void* operator new(size_t size) noexcept
{
	return malloc(size);
}

void operator delete(void *p) noexcept
{
	free(p);
}

void* operator new[](size_t size) noexcept
{
	return operator new(size); // Same as regular new
}

void operator delete[](void *p) noexcept
{
	operator delete(p); // Same as regular delete
}

void* operator new(size_t size, std::nothrow_t) noexcept
{
	return operator new(size); // Same as regular new
}

void operator delete(void *p,std::nothrow_t) noexcept
{
	operator delete(p); // Same as regular delete
}

void* operator new[](size_t size, std::nothrow_t) noexcept
{
	return operator new(size); // Same as regular new
}
void operator delete[](void *p,std::nothrow_t) noexcept
{
	operator delete(p); // Same as regular delete
}
*/
static volatile unsigned &HALT = *reinterpret_cast<unsigned *>(0xe0000000);

// reset handler
int main() {
    copy_data();
    zero_bss();
    fill_heap();
    call_init_array();
    // run application
    main_app();
    // call destructors for static instances
    call_fini_array();
    // stop
    HALT = 1;

    while (true);

    return 0;
}
