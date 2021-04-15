#include "HandleTemperature.h"

#define HANDLE_RES              1000
#define HANDLE_INPUT_REFERENCE  3300

typedef struct temperture_t
{
    s16 *x_temp_list;
    u16 *Y_res_list;
    u8 list_len;
}_TEMPERATURE_OB;

// mos管温度表
s16 X_Temp_Handle[24] = {-15,-10,-5,0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100};
u16 Y_Res_Handle[24] = {7340,5559,4249,3275,2545,1993,1573,1250,1000,805,652,531,435,358,297,247,206,173,146,124,105,90,77,66};

/**@brief     temperature object.
 * @details    
 *
 */
_TEMPERATURE_OB _temperature_object =
{
    .x_temp_list = X_Temp_Handle,
    .Y_res_list =Y_Res_Handle,
    .list_len = sizeof(X_Temp_Handle)/2,   
};

/**@brief     Function for Calculate the Slope
 * @details   Three points to Calculate the slope 
* @param[in]   
 */
static s16 Linear_Table(u16 x1,u16 y1,u16 x2,u16 y2,u16 n,s8 factor)
{
    s16 k,b;
    k = (((s32)y1-(s32)y2)/((s32)x1-(s32)x2));
    b = y1-x1*k;
    if(factor == 1)
    return ((s16)n-b)/k;
    else
    return (k*n+b);
}

static s16 Calculate_Value_By_Linertable(u16 param,const s16 *x_list,const u16 *y_list,u8 list_len ,s8 calcu_factor)
{
    u8 Loop,StartPoint =0 ;
    s16 tar_value;
    s16 *cmp_list = 0;
    s16 *result_list =0;
    
    if(calcu_factor == 1)
    {
        cmp_list = (s16*)y_list;
        result_list = (s16*)x_list;
    }       
    else 
    {
        cmp_list = (s16*)x_list;
        result_list = (s16*)y_list;
    }
    
    for(Loop = 0; Loop < list_len; Loop++)
    {
        if(param > cmp_list[Loop])
        {
            StartPoint = Loop;
            break;
        }
    }
    if(Loop >=list_len)
    {
        return tar_value = result_list[list_len -1];       
    }
    else if(Loop == 0)
    {
        return tar_value = result_list[0];      
    }
    
    tar_value = Linear_Table(x_list[StartPoint],y_list[StartPoint],x_list[StartPoint-1],y_list[StartPoint-1],param,calcu_factor);
    
    return tar_value;
}

/**@brief     Function for get temperature
 * @details   
* @param[in]   
 */
static s16 NTC_GET_Temperature(u16 adcValue)
{
    s16 handle_temperture;

    adcValue = (adcValue*3300)>>12;               //计算电压

    if(adcValue > 3299) 
        adcValue = 3299;
    
    adcValue = HANDLE_RES * adcValue /(HANDLE_INPUT_REFERENCE - adcValue);      //计算采样电阻   235*temp32/(5800-temp32) or  1000*temp32/(3300-temp32) 
  
    handle_temperture = Calculate_Value_By_Linertable(adcValue,_temperature_object.x_temp_list,_temperature_object.Y_res_list,_temperature_object.list_len, 1);

    return handle_temperture;
}

/*****************************************************************
* Function Name : Handle_Temp_Ctr
* Description   : 把套加热控制函数  
* Input         : None
* Output        : None
* Notes         :
******************************************************************/
void Handle_Temp_Ctr(void)
{
    int16_t leftHandletemp = 0;
    int16_t rightHandletemp = 0;
    
//    uint16_t handleFault = 0;
    
    g_myself_data.Handle_Bar_Info.HandleLeftTemp;
    g_myself_data.Handle_Bar_Info.HandleRightTemp;

    leftHandletemp  =  ADC_Get_ConversionValue(AdcChanelLeftHandleTemp);        // 左把套温度AD值
    rightHandletemp =  ADC_Get_ConversionValue(AdcChanelRightHandleTemp);       // 右把套温度AD值
    
    g_myself_data.Handle_Bar_Info.HandleLeftTemp = NTC_GET_Temperature(leftHandletemp);
    g_myself_data.Handle_Bar_Info.HandleRightTemp = NTC_GET_Temperature(rightHandletemp);


#ifdef DEBUG_1
    g_myself_data.Scooter_Info.HandleTempSetValue = 30;
    g_myself_data.Scooter_Info.HandleTempMode = 1;
#endif
    
    //左右把套加热硬件好坏先不考虑
    //认为超过36V标准
//    if(handleFault > 2450 || handleFault < 1000)
//    {
//    
//    }

    g_myself_data.Handle_Bar_Info.HandleTempStatus = HANDLE_TEMP_OFF;
    
    if(ABS(g_myself_data.Handle_Bar_Info.HandleLeftTemp - g_myself_data.Handle_Bar_Info.HandleRightTemp) > 10)
    {
        //关闭把套加热
        Handle_Temp_Set(0);
        g_myself_data.Handle_Bar_Info.HandleTempStatus |= HANDLE_TEMP_DIFF_LARGE;
    }
    else if(g_myself_data.Handle_Bar_Info.HandleLeftTemp > 70 || g_myself_data.Handle_Bar_Info.HandleLeftTemp < -30)
    {
        //关闭加热
        Handle_Temp_Set(0);
        g_myself_data.Handle_Bar_Info.HandleTempStatus |= HANDLE_TEMP_LEFT_BROKN;
    }
    else if(g_myself_data.Handle_Bar_Info.HandleRightTemp > 70 || g_myself_data.Handle_Bar_Info.HandleRightTemp < -30)
    {
        //关闭加热
        Handle_Temp_Set(0);
        g_myself_data.Handle_Bar_Info.HandleTempStatus |= HANDLE_TEMP_RIGHT_BROKN;
    }
    else
    {
        if(g_myself_data.Scooter_Info.HandleTempMode == 1)
        {
            //小于设定温度使能加热
            if( ((g_myself_data.Handle_Bar_Info.HandleLeftTemp + g_myself_data.Handle_Bar_Info.HandleRightTemp) / 2) 
                < g_myself_data.Scooter_Info.HandleTempSetValue)
            {
                //使能加热
                Handle_Temp_Set(40);
                g_bool[B_HANDLE_TEMP_STATUS] = ON;
            }
            //大于设定温度关闭加热
            else if( ((g_myself_data.Handle_Bar_Info.HandleLeftTemp + g_myself_data.Handle_Bar_Info.HandleRightTemp) / 2) 
                      > (g_myself_data.Scooter_Info.HandleTempSetValue + 2) )
            {
                //关闭加热
                Handle_Temp_Set(0);
                g_bool[B_HANDLE_TEMP_STATUS] = OFF;
            }
        }
        else
        {
            //关闭加热
            Handle_Temp_Set(0);
            g_bool[B_HANDLE_TEMP_STATUS] = OFF;
        }
        
        if(g_bool[B_HANDLE_TEMP_STATUS] == ON)
        {
            g_myself_data.Handle_Bar_Info.HandleTempStatus |= HANDLE_TEMP_ON;
        }
        else
        {
            g_myself_data.Handle_Bar_Info.HandleTempStatus &= ~HANDLE_TEMP_ON;
        }
    }
}
