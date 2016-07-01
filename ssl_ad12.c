#include <stdio.h>
#include "iodefine.h"
#include "ssl_ad12.h"




//connfiguration register ��D2=b'0,D1=b'0,D0=b'0�ɌŒ肵�C�A���[�g�͎g�p����
unsigned char ad12_init(unsigned char select_ch, unsigned char adc_slave_address)	//�������̊֐�
{
	unsigned char t_data[4];//���M�R�}���h�C[0]�A�h���X���W�X�^�ŃR���t�B�O���W�X�^���w���C[1],[2]�R���t�B�O���W�X�^�ɏ������݁i2�o�C�g�j
	unsigned char ack = 0;
	unsigned char i = 0;

	t_data[0]=CONFIGURATION_REGISTER;
	t_data[1]=(select_ch&0xFF00)>>8;
	t_data[2]=(FILR_BIT|select_ch)&0x00FF;



	//�R���o�[�g�s����������
	PFC.PDIORH.BIT.B25= 0x01;	//�R���o�[�g�s����PD25����o�͂ɐݒ�
	PFC.PDCRH1.BIT.PD25MD= 0x00;		//PD25���o�͂ɐݒ�
	PD.DR.BIT.B25= 0x00;	//�s����Lo��
	
	iic_init();		//���ۂ͉q���S�̂�iic_init���Ǘ������ق��������C�ŏI�I�ɂ͂��̊֐��̒�����O������
	ack = iic_write(t_data, 3, adc_slave_address);
	if(ack==0){
		return 0;
	}
	for(i=0; i <=3; i++){	//�R�}���h�p�z��̏�����
		t_data[i]=0;
	}
	t_data[0]=SEQUENCE_READ;	//���x��1�o�C�g���������s��Ȃ�

	ack = iic_write(t_data, 1, adc_slave_address);	//t_data��4�z��ڂ�SEQUENCE_READ�����Ĉ�x�̏����ɂ��Ă��ǂ����葱���Ƃ��Ă͓��ލs���Ă��邽�ߗ����̂��ߕ���
	if(ack==0){
		return 0;
	}
	else{
		return 1;
	}
	
}

void ad12_set()	//�ǂ�CH���g�p���邩�̐ݒ�
{
	//I2Cwrite
}



//ad_data[]�̔z���[48]�ȏ�ł��邱�ƁC���Ȃ݂�7�ȉ��͂����Ă��Ȃ��C������SH�����̂����Ă��ǂ�
unsigned char ad12_conv(unsigned long int *ad_data, unsigned char ch_num, unsigned char adc_slave_address)		//�ϊ��J�n�C�z��̃T�C�Y��3�ł��̓�2���g�p
{
	unsigned char i;
	unsigned char r_data[3];	//iic_read�p�̓ǂݍ��݃f�[�^�i�[�ϐ�
	for( i=1; i <= ch_num; i++){		//�ݒ肵���`�����l���̐������T�C�N���ϊ��ł̎擾������
		iic_read(r_data, 2, adc_slave_address);		//�X���[�u�ւ̓ǂݍ���
		switch((r_data[0]>>4)&0x07){		//�܂��ǂݍ��񂾃f�[�^����ID���擾����łǂ�CH�̃f�[�^����I��
		case 0x00:
			switch(adc_slave_address){		//����5����ADC����ǂ�ADC���g���Ă��邩�Ń|�[�g���킩�邽�߁C�擾�����f�[�^���|�[�g�P�ʂŊi�[
			case ADC_1:
				ad_data[8]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];		//MSB�f�[�^��AD�f�[�^�Ƃ͊֌W�̂Ȃ����5�r�b�g���}�X�N����8�r�b�g���V�t�g�{LSB
				break;
			case ADC_2:
				ad_data[16]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[24]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[32]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[40]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x01:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[9]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[17]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[25]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[33]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[41]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x02:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[10]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[18]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[26]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[34]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[42]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];		
				break;
			}
			break;
		case 0x03:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[11]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[19]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[27]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[35]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[43]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x04:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[12]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[20]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[28]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[36]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[44]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x05:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[13]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[21]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[29]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[37]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[45]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x06:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[14]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[22]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[30]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[38]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[46]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		case 0x07:
			switch(adc_slave_address){
			case ADC_1:
				ad_data[15]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_2:
				ad_data[23]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_3:
				ad_data[31]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_4:
				ad_data[39]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			case ADC_5:
				ad_data[47]= ((long int)r_data[0]&0x07<<8)+(long int)r_data[1];
				break;
			}
			break;
		}						
	}
	return 1;
}

void ad12_stop()
{
	iic_stop();
}


