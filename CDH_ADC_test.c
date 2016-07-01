#include <stdio.h>
#include "iodefine.h"
#include "ssl_orbis.h"
#include "ssl_sci.h"
#include "ssl_wait.h"
#include "ssl_ad12.h"

/*
void main(void)
{
	unsigned char ack = 0;
	unsigned long int ad_data[48];
	unsigned char comd[2]={0x41};	//���Ƃō폜
	iic_init();		//���Ƃō폜
	ack = iic_write(comd, 1,0x42); //���Ƃō폜
	ack = ad12_init(ADCH1, ADC_1);
	while(ack != 1){	//adc��iic�ʐM���������āC�ݒ肪���������甲����
		ack = ad12_init(ADCH1, ADC_1);
	}
	while(1){
		ack = ad12_conv(ad_data, 1, ADC_1);
		while(ack != 1){
			ack = ad12_conv(ad_data, 1, ADC_1);
		}
	}
}
*/
#define CMPS_SLAVE_ADRS 0x42 	//SLAVE ADRS:b'000	�f�B�W�^���R���p�X�X���[�u�A�h���X
#define DATA_NUM 2 		//data size 2Byte
#define CMPS_COMD 0x41		//�f�W�^���R���p�X�p'A'�R�}���h
#define NULL 0
#define MAG_N_OFFSET	6				//���k�␳���i�ܓx�ɂ���ĕς��j

double get_compass_data(void)
{
	unsigned char read_data[5];		//�f�W�^���R���p�X�f�[�^�p�z��
	int code;				//�[���ǂݍ��ݗp
	long compass_data;			//I2C����̃f�W�^���R���p�X�f�[�^
	double return_compass_data =0;		//���^�[���p�f�W�^���R���p�X�f�[�^
	unsigned char comd[2]={CMPS_COMD};
	unsigned char ack;
//	Set_standby();
	iic_init();
	ack = iic_write(comd, 1,CMPS_SLAVE_ADRS); // set device code,address
//	ack = Set_comd_slave(comd, 1, CMPS_SLAVE_ADRS); // set device code,address
	if(ack==1){
//		ack = Read_page_slave(read_data,DATA_NUM,CMPS_SLAVE_ADRS);	//�f�W�^���R���p�X�f�[�^�擾
		ack = iic_read(read_data,DATA_NUM,CMPS_SLAVE_ADRS);	//�f�W�^���R���p�X�f�[�^�擾
		compass_data = ((read_data[0]*0x100) + (read_data[1])); 	//�f�W�^���R���p�X�f�[�^10�i���ϊ�
		return_compass_data = (double)compass_data / 10.0;		//long��double�ϊ�

		if(return_compass_data >= 0 && 354 > return_compass_data){							//���k�̕␳
			return_compass_data += MAG_N_OFFSET;
		}else if(return_compass_data >= 354 && 360 > return_compass_data){
			return_compass_data = MAG_N_OFFSET - (360 - return_compass_data);
		}else;
	}
	return (return_compass_data);		
}

void main()
{
	double compass;
	char COMPASS[20];

//	wait_init();
//	rs_Open(SCI_DBG, ASYNC, 38400);
//	rs_Puts(SCI_DBG,"compass...OK");
//	rs_Putc(SCI_DBG, '\n');
	compass = get_compass_data();
	while(1){
//		wait_msec(10);
		compass = get_compass_data();
//		ftoa(COMPASS, compass);
//		rs_Puts(SCI_DBG, COMPASS);
//		rs_Putc(SCI_DBG, '\n');
	}
}