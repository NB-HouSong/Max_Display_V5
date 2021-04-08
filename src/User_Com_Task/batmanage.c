///******************** (C) COPYRIGHT 2020 Ninebot *******************************
//* File Name     : batmanage.c
//* Author        : meihua
//* Version       : V1.0.0
//* Date          : 2020-9-3
//* Description   : 
//********************************************************************************/
//#include "batmanage.h"

///* Private typedef -----------------------------------------------------------*/
///* Private define ------------------------------------------------------------*/

///* Private macro -------------------------------------------------------------*/
///* Private function prototypes -----------------------------------------------*/
//static void BMS_Query_Init_Info(void);

///* Variables -----------------------------------------------------------------*/
//BATTERY_OB battery_object =
//{
//    .s_battery_pro = BMS_Query_Init_Info,
//    .s_BMS_verify_err_cnt = 0,
//};

//BMS_PARA_TYPE  	g_stBMS_Info;

///*****************************************************************
//* Function Name :  InitBMS1
//* Description   :  BMS��ʼ��
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static u8 InitBMS1(void)
//{
//    u8 temp8 = 0;
//    static s16 s_BMSComTimeout = 0;
//    static u8 s_bSNReaded = 0;

//    if(!battery_object.s_BMS_Init_OK && s_BMSComTimeout < BMS_INT_TIMEOUT)		//BMSû�г�ʼ���ɹ�,δ��ʱ
//    {
//        s_BMSComTimeout += 10;
//        if(!s_bSNReaded)	//û�ж���SN(���к�)
//        {  
//			if(battery_object.havegetinitinfo_flag != 1) //���û�����ӵ�BMS���ȶ�ȡSN
//            {
//                temp8 = BMS_INT_INF_LEN << 1;
//                PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_SN, &temp8, COM_BMS);		//��ȡBMS��SN
//            }
//            else	//��ȡbool����
//            {
//                s_bSNReaded = 1;
//                temp8 = BMS_BOOL_INF_LEN << 1;
//				PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_FUN_BOOLEAN, &temp8, COM_BMS);	//��ȡBMS��Bool		
//            }
//        }
//        else   //��ȡ��SN�Ժ�
//        {
//            if(battery_object.s_BMS_update_flag)		//BMS1�Ѿ��������
//            {
//                battery_object.s_BMS_Init_OK = 1;		//��ʼ�����	
//            }
//            else
//            {
//                temp8 = BMS_BOOL_INF_LEN << 1;
//                PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_FUN_BOOLEAN, &temp8, COM_BMS);
//            }
//        }
//        battery_object.s_BMS_update_flag = 0;
//        return 0;
//    }
//    if(s_BMSComTimeout == BMS_INT_TIMEOUT)
//    {
//        battery_object.s_BMS_OffLine = 1;
//        s_BMSComTimeout++;			//���ڽ����ʾ���OFFLINE
//    }
//    return 1;
//}

///*****************************************************************
//* Function Name :  BBMS_NULL_Pro
//* Description   :  ���ÿ��߳�ȥ��BMS CPID
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_NULL_Pro(void)
//{
////    static u32 s_query_bms_cpid_cnt = 0;
////	u32 pdata;

////    if(battery_object.s_BMS_selected_flag == ON)
////    {
////		memcpy(&pdata, &battery_object.s_identify_array[4], 4);
////        if(pdata!=0)
////        {
////            return;
////        }
////        else
////        {
////            if(++s_query_bms_cpid_cnt > _HALF_ONE_SENCOND)
////            {
////                s_query_bms_cpid_cnt = 0;
////                u8 temp8 = BMS_CPID_LEN << 1;
////				PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_CPID, &temp8, COM_BMS);//��ȡBMS��cpid				
////            }
////        }
////    }
//}
///*****************************************************************
//* Function Name :  BMS_Verify_Pro
//* Description   :  У��
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_Verify_Pro(void)
//{
////	u32 pdata;
////	memcpy(&pdata, &battery_object.s_identify_array[4], 4);
////    if(pdata != 0)
//    {
//		PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, sizeof(battery_object.s_identify_array), \
//			CMD_WR, BAT_VERIFY, battery_object.s_identify_array, COM_BMS);
//    }
//} 

///*****************************************************************
//* Function Name :  BMS_Update_Verifycode_Pro
//* Description   :  ������֤��
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_Update_Verifycode_Pro(void)
//{
//    PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID,4, 0x80, 0, &battery_object.s_identify_array[4],COM_BMS);
//}
///*****************************************************************
//* Function Name :  BMS_Query_Init_Info
//* Description   :  ��ʼ��״̬��ѯ
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_Query_Init_Info(void)
//{
//    battery_object.s_battery_init_status = InitBMS1();
//}
///*****************************************************************
//* Function Name :  BMS_Query_Bool_Info
//* Description   :  Bool״̬��ѯ
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_Query_Bool_Info(void)
//{
//    if( battery_object.s_BMS_IAP_Mode == 0 )
//    {
//        u8 temp8 = BMS_BOOL_INF_LEN << 1;
//        PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_FUN_BOOLEAN, &temp8,COM_BMS); //��ȡBMS��Bool //3
//        battery_object.s_BMS_update_flag = 0; //�Ƿ��ط�������
//    }
//}

///*****************************************************************
//* Function Name :  BMS_Analysis_Info
//* Description   :  ����
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void BMS_Analysis_Info(void)
//{
//    static u16 s_OffLineCnt_BMS1 = 0;
//    static u8 s_bFristCheckBMS1 = 0;
//    char  defaultsBMSSN[15] = {"3GA0016A1A0001"};
//    
//    if(!battery_object.s_BMS_update_flag && !g_bool[B_UPD_IAP])
//    {
//        s_OffLineCnt_BMS1++;
//        if(s_OffLineCnt_BMS1 >= 25)			//5s�ղ���������ΪBMS1���� 3s->5s,��bms����һ��
//        {
//			s_OffLineCnt_BMS1 = 20;
//            battery_object.s_BMS_OffLine = 1;
//            battery_object.s_BMS_Is_Commu = 0;
//			SetErrorByte(MINI_BMS1_ERROR_COM, MySelf_Err);
//        }
//        return;
//    }
//    //BMS1����ͨ��

//    battery_object.s_BMS_Is_Commu = 1;
//    if(g_myself_data.DIS_Version_sw.BMS_SoftwareVersion > BMS_OLD_VERSION)//�̼��汾ʶ��
//    {
//        battery_object.s_BMS_selected_flag = ON;
//    }
//    else
//    {
//        battery_object.s_BMS_selected_flag = OFF;
//    }
//    if(!s_bFristCheckBMS1)		//0.2.8����BMS�̼��ž��������к�
//    {
//        s_bFristCheckBMS1 = 1;

//        //�ж��Ƿ��ǳ�ʼ���к�
//        if(!memcmp(defaultsBMSSN, (u8*)&g_stBMS_Info.sn,SN_LEN))
//        {
//            SetErrorByte(MINI_BMS1_ERROR_INIT_SN, MySelf_Err);
//        }
//        else
//        {
//            ClearErrorByte(MINI_BMS1_ERROR_INIT_SN);
//        }
//		
//		
//        if((g_stBMS_Info.boolean & BMS_BOOLMARK_PASSWORD) == 0)	//BMS���벻��ȷ
//            SetErrorByte(MINI_BMS1_ERROR_PASSWORD, MySelf_Err);
//        else
//            ClearErrorByte(MINI_BMS1_ERROR_PASSWORD);
//       
//        battery_object.s_BMS_SN_Checked = 1;
//    }
//    battery_object.s_BMS_OffLine = 0;
//    s_OffLineCnt_BMS1 = 0;
//    ClearErrorByte(MINI_BMS1_ERROR_COM);
//}

///*****************************************************************
//* Function Name :  Battery_Event_Update
//* Description   :  �¼�����
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//static void Battery_Event_Update(void)
//{
//    static u16 s_update_key_cnt = 0;
//    static u8  s_send_verifypro_cnt = 0;
//    static u16 s_verifypreiod = 200;
//	u16  full_battery_capacity = FULL_BATTERY_CAPACITY;

//    if(!battery_object.s_battery_init_status)
//    {
//        battery_object.s_battery_pro = BMS_Query_Init_Info;
//        return;
//    }

//    battery_object.s_battery_commu_cnt++;
//    if(battery_object.s_identify_set_flag == ON && battery_object.s_battery_commu_cnt % 200 == 0)
//    {
//        battery_object.s_battery_pro = BMS_Update_Verifycode_Pro;
//        s_update_key_cnt++;
//        battery_object.s_battery_commu_cnt = 0;
//        if(s_update_key_cnt > (10))//10s timeout
//        {
//            battery_object.s_identify_set_flag = OFF;
//            s_update_key_cnt = 0;
//        }
//        return;
//    }

//    if((battery_object.BMS_IAP_IDENT_FLAG == OFF && battery_object.s_battery_commu_cnt % s_verifypreiod == 0 && battery_object.s_battery_commu_cnt!= 0)
//            && battery_object.s_identify_set_flag == OFF && battery_object.s_BMS_selected_flag == ON)
//    {
//        if(s_send_verifypro_cnt < 6)
//        {
//            s_send_verifypro_cnt++;
//            s_verifypreiod = 200; //1s
//        }
//        else
//        {
//            s_verifypreiod = 600; //3s
//        }
//        battery_object.s_battery_commu_cnt = 0;
//        s_update_key_cnt = 0;
//        battery_object.s_battery_pro = BMS_Verify_Pro;
//    }
//    else if(battery_object.s_battery_commu_cnt %20 ==0)   //100ms
//    {
//        if(battery_object.repeatreadver_flag ==1)
//		{
//            u8 temp_len = 2;
//			PushMiniFrame(EBIKE_DIS_ID, EBIKE_BMS_ID, 1, CMD_RD, BAT_SW_VER, &temp_len,COM_BMS);
//        }
//		
//        if(battery_object.s_battery_info_switch ==0)
//        {
//            battery_object.s_battery_pro = BMS_Query_Bool_Info;
//            battery_object.s_battery_info_switch = 1;
//        }
//        else
//        {
//            battery_object.s_battery_pro = BMS_Analysis_Info;
//            battery_object.s_battery_info_switch = 0;
//        }
//    }

////    if( battery_object.s_identify_set_flag == OFF && battery_object.s_BMS_selected_flag == ON && (++battery_object.s_BMS_verify_err_cnt > _1_MIN ))
////    {
////		if(battery_object.s_BMS_verify_err_cnt > _1_MIN + 100 || battery_object.s_BMS_verify_err_cnt == _1_MIN + 1)
////		{
////			battery_object.s_BMS_verify_err_cnt = _1_MIN +1;
////            SetErrorByte(MINI_BMS1_VERIFY_FAILUE, MySelf_Err);
////		}
////    }
////    else
////    {
////        ClearErrorByte(MINI_BMS1_VERIFY_FAILUE);
////    }
//	
//#ifdef VIRTUAL_DISPLAY
//	
//	if(g_myself_data.DIS_Version_hw.BMS_HardwareVersion == BMS_HARD_VERSION_PRO)   //BMS�汾�� == BMS_HARD_VERSION_PRO)
//	   full_battery_capacity = FULL_BATTERY_CAPACITY_PRO;
//	
//    g_stBMS_Info.remain_cap_percent = g_stBMS_Info.virtual_cap_percent;
//	g_stBMS_Info.remain_cap = full_battery_capacity * g_stBMS_Info.remain_cap_percent /100;
//#endif
//	
//    if(battery_object.s_BMS_selected_flag == ON)
//    {
//        BMS_Encrypt_Verifycode_Pro((u8)g_stBMS_Info.virtual_cap_percent,&battery_object.s_identify_array[4],&battery_object.s_identify_set_flag);
//    }
//}

///*****************************************************************
//* Function Name :  BatteryCommu_Pro
//* Description   :  BMSͨѶ�߳�
//* Input         :  None
//* Output        :  None
//* Notes         :
//******************************************************************/
//void BatteryCommu_Pro(void)
//{
//    Battery_Event_Update();
//    battery_object.s_battery_pro();
//    battery_object.s_battery_pro = BMS_NULL_Pro;   
//}

////==================================== The End===============================================/
