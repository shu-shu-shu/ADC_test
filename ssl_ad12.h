#ifndef SSL_AD12_H
#define SSL_AD12_H


/*----*/
/*定義*/
/*----*/
//どのADCを使用するか（スレーブアドレスW/Rビットを含む）
#define ADC_1 0x42	//AD7998-0のU25（ポート8-15を持っているデバイス）
#define ADC_2 0x44	//AD7998-0のU27（ポート16-23を持っているデバイス）
#define ADC_3 0x40	//AD7998-0のU29（ポート24-31を持っているデバイス）
#define ADC_4 0x46	//AD7998-1のU26（ポート32-39を持っているデバイス）
#define ADC_5 0x48	//AD7998-1のU28（ポート40-47を持っているデバイス）


#define CONFIGURATION_REGISTER 0x02  
#define SEQUENCE_READ 0x70

#define FILR_BIT 0x04
#define ADCH1 0x10	//configuration register での設定．これは論理和で設定する．ex.CH1|CH5|CH8
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