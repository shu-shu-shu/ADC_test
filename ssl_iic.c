/*-------------------------*/
/* SSL OPEN SOURCE PROJECT */
/*----------------------------------------------------------------------*/
/* ABSTRACT   : iic communication                                       */
/* LAST UPDATE: 2016/06/22                                              */
/* CREATOR    : S.HOTTA                                                 */
/* NOTICE:                                                              */
/* ��MCP2515�ɓ����������̂ł���D                                      */
/* ��II�{�[���[�g��97.7kHz�ɐݒ�D                                      */
/* ��IIC�o�X�C���^�t�F�[�X�d�l�ł͒ʏ탂�[�h���ő�100kHZ                */
/* ���������[�h���ő�400kHZ                                             */
/*----------------------------------------------------------------------*/


#include "iodefine.h"
#include "ssl_iic.h"

/************************************************/
/* dummy interrput function(�_�~�[���[�h�p�֐�) */
/************************************************/
#pragma interrupt(dummy)
void dummy(void)
{
// Interrput error
}
/******************************************/
/* iic_init iic�������̊֐�               */
/******************************************/

void iic_init(void)
{

    MST.CR1.BIT._IIC=0;		//���W���[���X�g�b�v������
	IIC.SCRX.BIT.IICE = 1; // IIC�̐��䃌�W�X�^�C�f�[�^���W�X�^��CPU�A�N�Z�X������
	IIC.ICCR.BYTE = 0x89;	//2�i����1000100
		// ICE(7)=b'1 Enable I2C bus interface
		// IEIC(6)=b'0 Disables the interrupt
		// MST(5)=b'0 Slave mode
		// TRS(4)=b'0 Receive mode
		// ACKE(3)=b'1 Continuous data transfer is halted
		// BBSY(2)=b'0
		// IRIC(1)=b'0
		// SCP(0)=b'1 Start/stop condition issuance disabling
		// set I2C pin function
	//������SH���X���[�u�f�o�C�X�Ƃ��Ĉ����ꍇ�ɂ͑��X���[�u�A�h���X�Ƒ��X���[�u�A�h���X�̐ݒ���s���D����̓}�X�^�[�f�o�C�X�Ƃ��Ă����g��Ȃ��Ƒz�肵�Ă��邽�ߎ��������D
	PFC.PBCR1.WORD = 0x0c00; // SCL�CSDA��IIC�|�[�g�Ƃ��Ďg�p
	PFC.PBCR2.WORD = 0x0000;
 	IIC.ICMR.BYTE = 0x38;	//�]�����[�g��97.7kHz�ɐݒ�,���ʊ��SH�̃N���b�N�ɑ΂��Ĉ�Ԓx���l���g�p�D
		// MILS(7)=b'0 MSB first
		// WAIT(6)=b'0 A wait state is inserted between DATA and ACK
		// CKS2[2:0](5:3)=b'111 Transfer clock select
		// 156kH@(@P-fai40MHz,IICX=1)
		// 97.7kHz(@P-fai25MHz,IICX=1)�����ݒ�
		// 39.1kH@(@P-fai10MHz,IICX=1)
	IIC.SCRX.BYTE = 0x39;	//IIC���W�X�^�ւ�CPU�A�N�Z�X���C�]���r�b�g���[�g�֘A�̃r�b�g��ݒ�
		// IICX(5)=b'1 transfer-rate select,reference CKS bit
		// IICE(4)=b'1 Enables CPU access to the register
		// HNDS(3)=b'1 Set this bit to 1
		// STOPIM(0)=b'1 disables interrupt requests
		// Set device code,EEPROM address
	return ;

}




/*****************************************************************/
/* iic_write �X���[�u���̑��M�f�[�^�ݒ�		                     */			
/* argument1 ;t_data (unsigned char*)���M�f�[�^������z��	     */
/* argument2 ;t_data_num (unsigned char)���M�f�[�^�� 		     */
/* argument3 ;slave_address (unsigned char)�X���[�u�A�h���X      */
/* return ;1=OK/0=NG slave device NO_ACK(unsigned char)          */
/*****************************************************************/
unsigned char iic_write(unsigned char* t_data, int t_data_num, unsigned char slave_address)
{

	int i=0;
    while( IIC.ICCR.BIT.BBSY !=0 ); // SCL�CSDA���C���̏�Ԕ���
	IIC.ICCR.BYTE |= 0x30; // �}�X�^���M���[�h�ɐݒ�(MST=1,TRS=1)

	IIC.ICCR.BYTE=((IIC.ICCR.BYTE & 0xfe) | 0x04); // �J�n�������s(BBSY=1,SCP=0)
 	while( IIC.ICCR.BIT.IRIC==0 ); // �J�n���������҂�
	
	// Slave address+W Transmission
	IIC.EQU.ICE1.UN_ICDR = (unsigned char)(slave_address|IIC_DATA_W); // ���M�f�[�^�̐ݒ�i�X���[�u�A�h���X��W/R�j
	IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O���N���A�C����͑��M�f�[�^�̐ݒ�̒���ɍs��Ȃ���΂Ȃ�Ȃ�
	while( IIC.ICCR.BIT.IRIC==0 ); // 1�o�C�g���M�����҂�
	
	if( IIC.ICSR.BIT.ACKB!=0 ){ // �X���[�u�f�o�C�X�����ACK�̗L���`�F�b�N
		return (0); // ACK�Ȃ��Ŗ߂�l0��Ԃ�
	}

	for (i=0; i < t_data_num; i++){	//�R�}���h�̐��������M������

		IIC.EQU.ICE1.UN_ICDR = (unsigned char)(t_data[i]); // data set
		IIC.ICCR.BIT.IRIC = 0; // clear IRIC
		while( IIC.ICCR.BIT.IRIC==0 ); // Wait 1byte transmitted
	
		if( IIC.ICSR.BIT.ACKB!=0 ){ // Test the acknowledge bit
			return (0); // no ACK
		}
	}


	return (1); // ACK OK
}


/*********************************************************************/
/* iic_read �f�[�^�ǂݍ��݂̊֐�                                     */	
/* argument1 ;r_data(unsigned char*)�f�[�^������z��	             */
/* argument2 ;r_data_num (unsigned char)�f�[�^�T�C�Y 		         */
/* argument3 ;slave_address (unsigned char)�X���[�u�A�h���X          */
/* return ;1=OK/0=NG slave device NO_ACK(unsigned char)				 */
/*********************************************************************/
unsigned char iic_read(unsigned char *r_data, unsigned char r_data_num, unsigned char slave_address)
{
	unsigned char ack; // ACK�t���O
	unsigned char count; // read data number
	unsigned char dummy; // dummy data

	IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O���N���A
 	while(PB.DR.BIT.B2!=0); // SCL�[�q��Low���x���ɂȂ�܂őҋ@
	IIC.ICCR.BYTE |= 0x30; // �}�X�^�[���M���[�h�ɐݒ�(MST=1,TRS=1)
	IIC.ICCR.BYTE=((IIC.ICCR.BYTE & 0xfe)|0x04); // �J�n�������s(BBSY=1,SCP=0)
	while( IIC.ICCR.BIT.IRIC==0 ); // �J�n���������҂�
	IIC.EQU.ICE1.UN_ICDR = (unsigned char)(slave_address|IIC_DATA_R); // �X���[�u�A�h���X�̃Z�b�g
	IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O�̃N���A

	while( IIC.ICCR.BIT.IRIC==0 ); // 1�o�C�g���M�����҂�
	
	if( IIC.ICSR.BIT.ACKB!=0 ){ // �X���[�u�f�o�C�X�����ACK�̗L���`�F�b�N
		ack = 0; // ACK�Ȃ�
	}
	else{
		ack = 1;
	}
	
	if(ack==1){
		// �}�X�^�[��M���[�h�ɐݒ� (HNDS=1,WAIT=0)
		IIC.ICCR.BIT.TRS = 0; // ��M���[�h�ݒ�(TRS=0)
		IIC.ICMR.BIT.WAIT = 0; // �E�F�C�g�Ȃ�
		IIC.ICSR.BIT.ACKB = 0; // ACK�f�[�^���N���A
		IIC.SCRX.BIT.HNDS = 1; // �A����M����Ȃ�
		IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O���N���A

		// �f�[�^��M�J�n
		if(r_data_num>1){ // n�o�C�g�̃f�[�^��ǂݍ��ޏꍇ (n>1)
			dummy = IIC.EQU.ICE1.UN_ICDR; // �_�~�[���[�h
			while( IIC.ICCR.BIT.IRIC==0 ); // 1�o�C�g��M�҂�
			IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O���N���A
			
			for( count=2; count<r_data_num; count++ ){ 
			//*r_data = IIC.ICDR.BYTE; // read receive data
			*r_data = IIC.EQU.ICE1.UN_ICDR; // ��M�f�[�^�̓ǂݍ���
			while( IIC.ICCR.BIT.IRIC==0 ); // 1�o�C�g��M�҂�
			IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O�̃N���A
			r_data++;
			}
		}  
	
		IIC.ICSR.BIT.ACKB = 1; // �Ō�̎�M�p�A�N�m���b�W�f�[�^�ݒ�
		
		if(r_data_num==1){ // ��M��1�t���[���ڂ��ŏI�f�[�^�̏ꍇ�C��M�J�n�̂��߂̃_�~�[���[�h
			dummy = IIC.EQU.ICE1.UN_ICDR; 
		}
		else{ // ��납��2�t���[���ڂ̃f�[�^�ǂݍ��� (n>1)
			*r_data = IIC.EQU.ICE1.UN_ICDR; // 
			r_data++;
		}
		while( IIC.ICCR.BIT.IRIC==0 ); // 1�o�C�g��M�҂�
		IIC.ICCR.BIT.IRIC = 0; // IRIC�t���O���N���A
		IIC.ICCR.BIT.TRS = 1; // �Ō�̎�M�f�[�^�̓ǂݍ���
		*r_data = IIC.EQU.ICE1.UN_ICDR; 
	}
	return(ack);
}


/******************************************/
/* iic_init iic ��~�̊֐�                */
/******************************************/
void iic_stop(void){
		// ��~�������s
	IIC.ICCR.BYTE = IIC.ICCR.BYTE & 0xfa; // ��~�������s
}
	