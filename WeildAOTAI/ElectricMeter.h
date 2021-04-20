#pragma once

#include "SPI_C.h"
#include <iostream>
#include<cmath>
#include "pugixml.hpp"
#define CONVERT_FREQ  (x) (x*0.0625)
#define CONVERT_PERuMS  (x) (x*9.6)
#define CONVERT_CEL (x)(x*3)
#define AWATTHR   0x01
#define BWATTHR   0x02
#define CWATTHR   0x03
#define AVARHR    0x04
#define BVARHR    0x05
#define CVARHR    0x06
#define AVAHR     0x07
#define BVAHR     0x08
#define CVAHR     0x09
#define AIRMS     0x0A
#define BIRMS     0x0B
#define CIRMS     0x0C
#define AVRMS     0x0D
#define BVRMS     0x0E
#define CVRMS     0x0F
#define FREQ      0x10
#define TEMP      0x11
#define WFORM     0x12
#define OPMODE    0x13
#define MMODE     0x14
#define WAVMODE   0x15
#define COMPMODE  0x16
#define LCYCMODE  0x17
#define MASK      0x18
#define STATUS    0x19
#define RSTATUS   0x1A
#define ZXTOUT    0x1B
#define LINECYC   0x1C
#define SAGCYC    0x1D
#define SAGLVL    0x1E
#define VPINTLVL  0x1F
#define IPINTLVL  0x20
#define VPEAK     0x21
#define IPEAK     0x22
#define GAIN      0x23
#define AVRMSGAIN 0x24
#define BVRMSGAIN 0x25
#define CVRMSGAIN 0x26
#define AIGAIN    0x27
#define BIGAIN    0x28
#define CIGAIN    0x29
#define AWG       0x2A
#define BWG       0x2B
#define CWG       0x2C
#define AVARG     0x2D
#define BVARG     0x2E
#define CVARG     0x2F
#define AVAG      0x30
#define BVAG      0x31
#define CVAG      0x32
#define AVRMSOS   0x33
#define BVRMSOS   0x34
#define CVRMSOS   0x35
#define AIRMSOS   0x36
#define BIRMSOS   0x37
#define CIRMSOS   0x38
#define AWAITOS   0x39
#define BWAITOS   0x3A
#define CWAITOS   0x3B
#define AVAROS    0x3C
#define BVAROS    0x3D
#define CVAROS    0x3E
#define APHCAL    0x3F
#define BPHCAL    0x40
#define CPHCAL    0x41
#define WDIV      0x42
#define VADIV     0x44
#define VARDIV    0x43
#define APCFNUM   0x45
#define APCFDEN   0x46
#define VARCFNUM  0x47
#define VARCFDEN  0x48
#define CHKSUM    0x7E
#define VERSION   0x7F
#define REG_READ(reg) reg
#define REG_WRITE(reg)  (unsigned char)((reg) | 0x80)
//WAV_SEL
#define CURRENT     0
#define VOLTAGE     1
#define ACT_PWR     2
#define REACT_PWR   3
#define APP_PWR     4
//interrupt mask/status bit
#define AEHF            0
#define REHF            1
#define VAEHF           2
#define SAGA            3
#define SAGB            4
#define SAGC            5
#define ZXTOA           6
#define ZXTOB           7
#define ZXTOC           8
#define ZXA             9
#define ZXB             10
#define ZXC             11
#define LENERGY         12
#define RESET           13
#define PKV             14
#define PKI             15
#define WFSM            16
#define REVPAP          17
#define REVPRP          18
#define SEQERR          19
#define WAVMODE_VALUE(phase, wave)   (((wave)<<2)|(phase))
#define PHASE_A     0
#define PHASE_B     1
#define PHASE_C     2


typedef struct 
{
	double AWhLSB = -0.000011525;
	double BWhLSB = -0.000011554;
	double CWhLSB = -0.000011575;

	double AVARhLSB = 0.000000011;
	double BVARhLSB = 0.000000013;
	double CVARhLSB = 0.000000008;

	double AVAhLSB = 0.000011422;
	double BVAhLSB = 0.000011475;
	double CVAhLSB = 0.000011464;
}Kf_Meter;

using namespace std;
class ElectricMeter
{
public:
	uint32_t status;
	double SumWatA = 0;
	double SumWatB = 0;
	double SumWatC = 0;
	double SumVarA = 0;
	double SumVarB = 0;
	double SumVarC = 0;
	double SumVaA = 0;
	double SumVaB = 0;
	double SumVaC = 0;
	double SumWAT = 0;
	double SumVAR = 0;
	double SumVA = 0;
	int SumEnergyWat = 0;
	int SumEnergyVar = 0;
	int SumEnergyVa = 0;
	int SumPowerVa = 0;
	int SumPowerWat = 0;
	int SumPowerVar = 0;
	ElectricMeter(uint8_t cs_pin, uint32_t Cicle,string FileConfig,string NameConfig);
	void ReadValue();
	
private:

	uint32_t CS_PIN;
	uint32_t ReadReg24(uint8_t adr);
	uint16_t Read16bit(uint8_t adr);
	void Write8bit(uint8_t adr, uint8_t value);
	void Write16bit(uint8_t adr, uint16_t value);
	uint32_t CicleMeter;
	void GetConfig(string FileConfig, string NameConfig);
	Kf_Meter KF;
	double ValueRead;
	double PowerVa = 0;
	double PowerVar = 0;
	double PowerWath = 0;
};

