#include <iostream>
using namespace std;
#include <i2cInterface.h>
i2cInterfaceClass *i2cobj[8];

#define FULLBUS 1
char test_addr[] = {0x48, 0x49, 0x4a, 0x4b};


FILE *fp;

int main()
{
	seteuid(0);
	int i=0;
	float adc_val[16];
	for(i=0; i<sizeof(test_addr); i++)
	{
		i2cobj[i] = new i2cInterfaceClass(0, test_addr[i]);
		i2cobj[i]->setupI2CInterface();
#if FULLBUS
		i2cobj[4+i] = new i2cInterfaceClass(1, test_addr[i]);
		i2cobj[4+i]->setupI2CInterface();
#endif
	}
	
	fp = fopen("adc-test.log", O_RDWR|O_CREAT);
	if(fp < 0)
	{
		printf("FILE NOT OPEN \n");
		perror("");
		exit(0);
	}
	while(1)
	{
#if FULLBUS
		for(i=0; i<8; i++){
#else
		for(i=0; i<4; i++){
#endif
			adc_val[2*i]= i2cobj[i]->readADC_Differential_0_1();
			adc_val[2*i+1] = i2cobj[i]->readADC_Differential_2_3();
			if(i==1 && i==5){
				fprintf(fp, " ch[%d]:%f ch[%d]:%f ", 2*i, adc_val[2*i] * 12,  2*i+1, adc_val[2*i +1] * 12 );
			}
			else{
				fprintf(fp," ch[%d]:%f ch[%d]:%f ", 2*i, adc_val[2*i] * 7,  2*i+1, adc_val[2*i +1] *7 );
			}
			//printf(" press any key..!!\n");
			//getchar();
		}
		fprintf(fp, "\n");
		fflush(fp);
		sleep(1);

	}
	seteuid(getpid());
	return 0;
}
