#ifndef __SMART_HOME__
#define __SMART_HOME__

#define GET_SI7006_TMP _IOR('r',0,int)
#define GET_SI7006_HUM _IOR('r',1,int)
#define SET_MOTOR_OFF _IOR('r',0,int)
#define SET_MOTOR_ON _IOR('r',1,int)
#define SET_FAN_ON _IOR('r',0,int)
#define SET_FAN_OFF _IOR('r',1,int)

#define GET_CMD_SIZE(cmd)  ((cmd>>16)&0x3fff)


#endif