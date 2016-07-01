#ifndef SSL_AD12_H
#define SSL_AD12_H


/*----*/
/*��`*/
/*----*/
//�ǂ�ADC���g�p���邩�i�X���[�u�A�h���XW/R�r�b�g���܂ށj
#define ADC_1 0x42	//AD7998-0��U25�i�|�[�g8-15�������Ă���f�o�C�X�j
#define ADC_2 0x44	//AD7998-0��U27�i�|�[�g16-23�������Ă���f�o�C�X�j
#define ADC_3 0x40	//AD7998-0��U29�i�|�[�g24-31�������Ă���f�o�C�X�j
#define ADC_4 0x46	//AD7998-1��U26�i�|�[�g32-39�������Ă���f�o�C�X�j
#define ADC_5 0x48	//AD7998-1��U28�i�|�[�g40-47�������Ă���f�o�C�X�j


#define CONFIGURATION_REGISTER 0x02  
#define SEQUENCE_READ 0x70

#define FILR_BIT 0x04
#define ADCH1 0x10	//configuration register �ł̐ݒ�D����͘_���a�Őݒ肷��Dex.CH1|CH5|CH8
#define ADCH2 0x20
#define ADCH3 0x40
#define ADCH4 0x80
#define ADCH5 0x100
#define ADCH6 0x200
#define ADCH7 0x400
#define ADCH8 0x800
/*--------------------*/
/*Prototype Declataion*/
/*--------------------*/
unsigned char ad12_init(unsigned char select_ch, unsigned char adc_slave_address);
unsigned char ad12_conv(unsigned long int *ad_data, unsigned char ch_num, unsigned char adc_slave_address);



#endif