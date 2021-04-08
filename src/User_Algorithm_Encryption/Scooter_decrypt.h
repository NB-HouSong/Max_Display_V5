#ifndef _SCOOTER_DECRYPT_CYPT_H
#define _SCOOTER_DECRYPT_CYPT_H


/**@brief     Function for decode the commu data between ECU with Iot.
 * @details
* 	@param[in]  *encryt_data: current Soc
*	@param[in]  *p_key1: pointer to  key1
                *p_key2  pointer to key2
                *return 0: success ff:failure
*/
extern unsigned char  Decrypt_Data_Pro(unsigned char *encryt_data,unsigned char *p_key1, unsigned char *p_key2);
/**@brief     Function for encode the verifycode.
 * @details
* 	@param[in]  cur_capacitance: current Soc
*	@param[in]  *last_key: pointer to current key
*	@param[out] *update_flag:    pointer to update flag
*/
extern void  BMS_Encrypt_Verifycode_Pro(unsigned char cur_soc, unsigned char *last_key, unsigned char *update_flag);


/**@brief Function for IAP Enable decrypt password and verify .
 * @param[in]  encoded_data             encypt data
 * @param[in]  encode_data_size         encypt data length
 * @param[in]  decode_key               decode key
 * @param[in]  decode_key_len           decode key length
 * result 0 correct;1 falut;
 */
extern int IAP_decoded_parse(const unsigned char* encoded_data, unsigned char encode_data_size,const unsigned char *decode_key, unsigned char decode_key_len);


/**@brief Function for genertae key words  .
 * @param[in]  org_pdata  		 original data
 * @param[in]  org_pdata_len     original data size
 * @param[out] encrypt_pdata     cryptcode data
 * @param[out] encrypt_pdata_len cryptcode data size.
 */

extern void Encryption_Seed_Generate(const unsigned char* org_pdata,unsigned char org_pdata_len,unsigned char* encrypt_pdata,unsigned char *encrypt_pdata_len);
#endif


