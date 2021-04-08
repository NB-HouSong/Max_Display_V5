#include "CRC.h"

void CRC_Config(uint32_t poly)
{

  /* DeInit CRC peripheral */
  CRC_DeInit();
  
  /* Init the INIT register */
//  CRC_SetInitRegister(0);

  /* Select 32-bit polynomial size */
  CRC_PolynomialSizeSelect(CRC_PolSize_32);
	
  /* Set the polynomial coefficients */
  CRC_SetPolynomial(poly);
	
	CRC_ReverseInputDataSelect(CRC_ReverseInputData_32bits);
	
	CRC_ReverseOutputDataCmd(ENABLE);
}


