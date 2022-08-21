#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include "smart_home.h"
#include "/home/ubuntu/S_H/smart_home_m/smart_home/led_core/led_up.h"
#include "/home/ubuntu/S_H/smart_home_m/smart_home/bee_core/bee_up.h"
#include "/home/ubuntu/S_H/smart_home_m/smart_home/spi_core/spi_up.h"

int fd_fan;

struct num
{
    int tmp_1; //温度 十
    int tmp_2; //温度 个
    int tmp_3; //小数点后一位
    int tmp_4; //小数点后二位
    int hum_1; //湿度 十
    int hum_2; //湿度 个
    int hum_3; //小数点后一位
    int hum_4; //小数点后二位
    int mod;   //显示模式
    int fd;
    int fd_iic;
    int fd_motor;
    
    int *wall;
};

void* show_spi(void *arg)
{
    while(1)
    {
        struct num* data=(struct num*)arg;

        //清零消隐
        spi_ctl(data->fd,0,0x20);
        spi_ctl(data->fd,1,0x20);
        spi_ctl(data->fd,2,0x20);
        spi_ctl(data->fd,3,0x20);

        switch (data->mod)
        {
        case 1:
            spi_ctl(data->fd,0,data->tmp_1);
            usleep(4000);
            spi_ctl(data->fd,1,data->tmp_2);
            usleep(4000);
            spi_ctl(data->fd,2,data->hum_1);
            usleep(4000);
            spi_ctl(data->fd,3,data->hum_2);
            usleep(4000);
            break;
        case 2:
            spi_ctl(data->fd,0,data->tmp_1);
            usleep(4000);
            spi_ctl(data->fd,1,data->tmp_2 | 0x10);
            usleep(4000);
            spi_ctl(data->fd,2,data->tmp_3);
            usleep(4000);
            spi_ctl(data->fd,3,data->tmp_4);
            usleep(4000);
            break;
        case 3:
            spi_ctl(data->fd,0,data->hum_1);
            usleep(4000);
            spi_ctl(data->fd,1,data->hum_2 | 0x10);
            usleep(4000);
            spi_ctl(data->fd,2,data->hum_3);
            usleep(4000);
            spi_ctl(data->fd,3,data->hum_4);
            usleep(4000);
            break;
        
        default:
            break;
        }
    }
}

void* read_iic(void *arg)
{
    int tmp, hum;
    float rtmp, rhum;
    int flag_num[2] = {35,20};
    int witch;
    while(1)
    {
        struct num* data=(struct num*)arg;

        ioctl(data->fd_iic, GET_SI7006_TMP, &tmp);
        ioctl(data->fd_iic, GET_SI7006_HUM, &hum);
        rtmp = 175.72 * tmp / 65536 - 46.85;
        rhum = 125 * hum / 65536 - 6;  

        data->tmp_1 = (int)rtmp/10;
        data->tmp_2 = (int)rtmp%10;
        data->tmp_3 = (int)(rtmp*10)%10;
        data->tmp_4 = (int)(rtmp*100)%10;
        data->hum_1 = (int)rhum/10;
        data->hum_2 = (int)rhum%10;
        data->hum_3 = (int)(rhum*10)%10;
        data->hum_4 = (int)(rhum*100)%10;
        
                      
        printf("tmp = %.2f,hum=%.2f\n", rtmp, rhum);  
        printf("max=%d min=%d\n",data->wall[0],data->wall[1]);
        if(rtmp < data->wall[1])
        {
            ioctl(data->fd_motor, SET_MOTOR_ON, &witch);
        }     
        if(rtmp > data->wall[0])
        {
            ioctl(fd_fan, SET_FAN_ON, &witch);
        }  
        
        
    }
}

int main(int argc,const char * argv[])
{
    int fd[6];
    int a,b,i;
    int witch;
    int tmp, hum;
    float rtmp, rhum;
    struct num num;
    int flag_num[2] = {35,20};
    char *name[6] = {"/dev/led","/dev/si7006","/dev/bee","/dev/fan","/dev/motor","/dev/spi"};
    pthread_t tid,tid1;
    
    for(i=0;i<6;i++)
    {
        if ((fd[i] = open(name[i], O_RDWR)) == -1)
        {
            perror("open error");
        } 
    }
    num.fd_iic=fd[1];
    num.fd_motor=fd[4];
    //num.fd_fan=fd[3];
    fd_fan=fd[3];
    num.fd=fd[5];
    num.wall=flag_num;
    /********************创建线程，显示数码管*************************/
    pthread_create(&tid,NULL,show_spi,&num);
    pthread_create(&tid1,NULL,read_iic,&num);


    /*************************************************/
    while(1)
    {
        printf("***********************\n");
        printf("***1.led开关***********\n");
        printf("***2.读取温湿度********\n");
        printf("***3.蜂鸣器开关********\n");
        printf("***4.风扇开关**********\n");
        printf("***5.马达开关**********\n");
        printf("***6.数码管显示********\n");
        printf("***7.温湿度阈值设置****\n");
        printf("***8.退出系统**********\n");
        printf("**********************\n");
        printf("请输入你的选择:");
        scanf("%d", &a);
        getchar();
        switch(a)
        {
            case 1:
                printf("***********************\n");
                printf("*******1.开*******\n");
                printf("*******2.关*******\n");
                printf("*******3.退出***********\n");
                printf("***********************\n");
                printf("请输入你的选择:");
                scanf("%d", &b);
                switch(b)
                {
                    case 1:
                        led_ctl(fd[0],IO_LED_ON,&witch);
                        break;
                    case 2:
                        led_ctl(fd[0],IO_LED_OFF,&witch);                 
                        break;
                    case 3:
                        break;                        
                }    
                break;
            case 2:
                ioctl(fd[1], GET_SI7006_TMP, &tmp);
                ioctl(fd[1], GET_SI7006_HUM, &hum);
                rtmp = 175.72 * tmp / 65536 - 46.85;
                rhum = 125 * hum / 65536 - 6;  

                num.tmp_1 = (int)rtmp/10;
                num.tmp_2 = (int)rtmp%10;
                num.tmp_3 = (int)(rtmp*10)%10;
                num.tmp_4 = (int)(rtmp*100)%10;
                num.hum_1 = (int)rhum/10;
                num.hum_2 = (int)rhum%10;
                num.hum_3 = (int)(rhum*10)%10;
                num.hum_4 = (int)(rhum*100)%10;
                num.fd=fd[5];

                printf("tmp = %.2f,hum=%.2f\n", rtmp, rhum);  
                if(rtmp < flag_num[1])
                {
                    ioctl(fd[4], SET_MOTOR_ON, &witch);
                }     
                if(rtmp > flag_num[0])
                {
                    ioctl(fd[3], SET_FAN_ON, &witch);
                }  
                break;
            case 3:
                printf("***********************\n");
                printf("*******1.开*******\n");
                printf("*******2.关*******\n");
                printf("*******3.退出***********\n");
                printf("***********************\n");
                printf("请输入你的选择:");
                scanf("%d", &b);
                switch(b)
                {
                    case 1:
                        bee_ctl(fd[2],IO_BEE_ON,&witch);
                        break;
                    case 2:
                        bee_ctl(fd[2],IO_BEE_OFF,&witch);                 
                        break;
                    case 3:
                        break;                        
                }    
                break;
            case 4:
            S4:
                printf("***********************\n");
                printf("*******1.打开风扇*******\n");
                printf("*******2.关闭风扇*******\n");
                printf("*******3.退出***********\n");
                printf("***********************\n");
                printf("请输入你的选择:");
                scanf("%d", &b);
                switch(b)
                {
                    case 1:
                        ioctl(fd[3], SET_FAN_ON, &witch);
                        break;
                    case 2:
                        ioctl(fd[3], SET_FAN_OFF, &witch);                  
                        break;
                    case 3:

                        break;                        
                    default:
                        printf("输入错误，请重新输入\n");
                        goto S4;
                        break;
                }    
                break;
            case 5:
            S5:
                printf("***********************\n");
                printf("*******1.打开马达*******\n");
                printf("*******2.关闭马达*******\n");
                printf("*******3.退出***********\n");
                printf("***********************\n");
                printf("请输入你的选择:");
                scanf("%d", &b);
                switch(b)
                {
                    case 1:
                        ioctl(fd[4], SET_MOTOR_ON, &witch);
                        break;
                    case 2:
                        ioctl(fd[4], SET_MOTOR_OFF, &witch);                  
                        break;
                    case 3:
                        break;                        
                    default:
                        printf("输入错误，请重新输入\n");
                        goto S5;
                        break;
                }             
                break;
            case 6:
                printf("***************************\n");
                printf("*******1.全部显示***********\n");
                printf("*******2.温度独显***********\n");
                printf("*******3.湿度独显***********\n");
                printf("*******4.退出***************\n");
                printf("***************************\n");
                printf("请输入你的选择:");
                scanf("%d", &b);
                switch(b)
                {
                    case 1:
                        num.mod = 1;
                        break;
                    case 2:
                        num.mod = 2;               
                        break;
                    case 3:
                        num.mod = 3;
                        break;     
                    case 4:
                        break;                   
                }    
                break;
            case 7:
            S7:
                printf("***********************\n");
                printf("***1.温度最大值*********\n");                
                printf("***2.温度最小值*********\n");
                printf("***3.查看阈值**************\n");
                printf("***4.退出**************\n");
                printf("***********************\n");
                printf("请选择:");
                scanf("%d",&b);
                switch(b)
                {
                    case 1:
                        printf("请输入最大值:");
                        scanf("%d",&flag_num[0]);
                        break;
                    case 2:
                        printf("请输入最小值:");
                        scanf("%d",&flag_num[1]);                    
                        break;
                    case 3:
                        printf("max=%d min=%d\n",flag_num[0],flag_num[1]);
                        break; 
                    case 4:
                        break;                        
                    default:
                        printf("输入错误，请重新输入\n");
                        goto S7;                        
                        break;
                }
                break;
            case 8:
                goto S8;
                break;
            default:
                printf("请重新输入!\n");
                break;
        }


    }
S8:
        for(i=0;i<6;i++)
        {
            close(fd[i]);
        }

    return 0;
}