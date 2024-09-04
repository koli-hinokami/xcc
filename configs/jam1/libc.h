
extern void printf(char* formatstring, ...);
extern void puts(char* str);
extern void putchar(char ch);
extern void memset(void* area, uint8_t byte, size_t size);
extern uint16_t math_divide_16_8(uint16_t a, uint8_t b);
extern uint16_t math_abs(uint16_t val);
void CliWriteinteger(int16_t num){
	if(num<0){
		putchar('-');
		num=-num;
	}else{
		putchar(' ');
	};
	unsigned char i;
	for(i=0;num> 10000;num-= 10000){++i;};putchar(i+'0');
	for(i=0;num>  1000;num-=  1000){++i;};putchar(i+'0');
	for(i=0;num>   100;num-=   100){++i;};putchar(i+'0');
	for(i=0;num>    10;num-=    10){++i;};putchar(i+'0');
	for(i=0;num>     1;num-=     1){++i;};putchar(i+'0');
};
void rtl_swap_u16(uint16_t* a, uint16_t* b){
	uint16_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
};
