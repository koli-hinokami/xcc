
extern void puts(char* str);
extern void putchar(char ch);
extern void memset(void* area, uint8_t byte, size_t size);
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
