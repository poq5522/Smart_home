#ifndef __SMART_HOME__
#define __SMART_HOME__

#define SET_MOTOR_OFF _IOR('r',0,int)
#define SET_MOTOR_ON _IOR('r',1,int)

#define GET_CMD_SIZE(cmd)  ((cmd>>16)&0x3fff)

typedef struct {
        volatile unsigned int MODER;   // 0x00
        volatile unsigned int OTYPER;  // 0x04
        volatile unsigned int OSPEEDR; // 0x08
        volatile unsigned int PUPDR;   // 0x0C
        volatile unsigned int IDR;     // 0x10
        volatile unsigned int ODR;     // 0x14
        volatile unsigned int BSRR;    // 0x18
        volatile unsigned int LCKR;    // 0x1C 
        volatile unsigned int AFRL;    // 0x20 
        volatile unsigned int AFRH;    // 0x24
        volatile unsigned int BRR;     // 0x28
        volatile unsigned int res;
        volatile unsigned int SECCFGR; // 0x30
}gpio_t;
#define GPIOF_A  0x50007000

typedef struct{
	volatile unsigned int CR1; 
	volatile unsigned int CR2;
	volatile unsigned int SMCR;
	volatile unsigned int DIER;
	volatile unsigned int SR;
	volatile unsigned int EGR;
	volatile unsigned int CCMR1;
	volatile unsigned int CCMR2;
	volatile unsigned int CCER;
	volatile unsigned int CNT;
	volatile unsigned int PSC;
	volatile unsigned int ARR;
	volatile unsigned int RCR;
	volatile unsigned int CCR1;
	volatile unsigned int CCR2;
	volatile unsigned int CCR3;
	volatile unsigned int CCR4;
	volatile unsigned int BDTR;
	volatile unsigned int DCR;
	volatile unsigned int DMAR;
	volatile unsigned int Res1;
	volatile unsigned int CCMR3;
	volatile unsigned int CCR5;
	volatile unsigned int CCR6;
	volatile unsigned int AF1;
	volatile unsigned int AF2;
	volatile unsigned int TISEL;
} tim1_t;
#define TIM1  0x44000000

typedef struct {
	volatile unsigned int CR1;
	volatile unsigned int CR2;
	volatile unsigned int SMCR;
	volatile unsigned int DIER;
	volatile unsigned int SR;
	volatile unsigned int EGR;
	volatile unsigned int CCMR1;
	volatile unsigned int CCMR2;
	volatile unsigned int CCER;
	volatile unsigned int CNT;
	volatile unsigned int PSC;
	volatile unsigned int ARR;
	volatile unsigned int RES1[1];
	volatile unsigned int CCR1;
	volatile unsigned int CCR2;
	volatile unsigned int CCR3;
	volatile unsigned int CCR4;
	volatile unsigned int RES2[1];
	volatile unsigned int DCR;
	volatile unsigned int DMAR;
	volatile unsigned int RES3[4];
	volatile unsigned int AF1;
	volatile unsigned int RES4[1];
	volatile unsigned int TISEL;
}tim2_3_4_5_t;
#define  TIM4   0x40002000

typedef struct{
	volatile unsigned int CR1;
	volatile unsigned int CR2;
	volatile unsigned int RES1;
	volatile unsigned int DIER;
	volatile unsigned int SR;
	volatile unsigned int EGR;
	volatile unsigned int CCMR1;
	volatile unsigned int RES2;
	volatile unsigned int CCER;
	volatile unsigned int CNT;
	volatile unsigned int PSC;
	volatile unsigned int ARR;
	volatile unsigned int RCR;
	volatile unsigned int CCR1;
	volatile unsigned int RES3[3];
	volatile unsigned int BDTR;
	volatile unsigned int DCR;
	volatile unsigned int DMAR;
	volatile unsigned int RES4[4];
	volatile unsigned int AF1;
	volatile unsigned int RES5;
	volatile unsigned int TISEL;
}tim16_17_t;
#define TIM16_A   0x44007000

#define  RCC_A      0x50000000





#endif