extern void puts(char* str);
typedef uint16_t fixed16;
extern fixed16 fixed16_mul(fixed16 a, fixed16 b);
extern fixed16 fixed16_divide(fixed16 numerator, fixed16 denominator);
extern fixed16 fixed16_reciprocal(fixed16 num);
extern fixed16 fixed16_sinetab[256];
extern void printf(char* formatstring, ...);
extern void puts(char* str);
extern void putchar(char ch);
extern void memset(void* area, uint8_t byte, size_t size);
extern uint16_t math_divide_16_8(uint16_t a, uint8_t b);
extern uint16_t math_abs(uint16_t val);
void rtl_swap_u16(uint16_t* a, uint16_t* b){
	uint16_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
};
