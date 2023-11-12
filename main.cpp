#include <Arduino.h>

//定义连接L298N的IN1，IN2，IN3，IN4的四个针脚
#define Left_IN3 9
#define Left_IN4 10
#define Right_IN1 5
#define Right_IN2 6 
//定义连接红外传感器的针脚，Signal_Rear为小车后方传感器输出的信号
#define Signal_1 12
#define Signal_2 11
#define Signal_3 7
#define Signal_4 4
#define Signal_5 3  
#define Signal_Rear 2 

//初始化传感器的值
//小车自身前方左至右传感器依次为为Sensor12345,后方中置传感器为Sensor_Rear
int Sensor[7]={0,0,0,0,0,0,0};
int Sensor_Rear = 0;
    
int Start_Flag = 0;
int Line = 0; //黑色横线计数量

void Sensor_Read();
void Motor_Speed(int Left1_PWM, int Left2_PWM, int Right1_PWM, int Right2_PWM);
void Trail_Follow();
void Start();
void Count();
//void Print();

void setup() {  
    //设定各针脚的输入输出模式
    pinMode(Signal_1,INPUT);
    pinMode(Signal_2,INPUT);
    pinMode(Signal_3,INPUT);
    pinMode(Signal_4,INPUT);
    pinMode(Signal_5,INPUT);
    pinMode(Signal_Rear,INPUT);
    
    pinMode(Left_IN3,OUTPUT);
    pinMode(Left_IN4,OUTPUT);
    pinMode(Right_IN1,OUTPUT);
    pinMode(Right_IN2,OUTPUT);  
    
    Serial.begin(9600); //设置串口波特率用于调试
    
    delay(3000);  //开机延迟3s启动
}

void loop(){
    Sensor_Read();
    Start();
    Count();
    Trail_Follow(); 
   // Print();
}

void Sensor_Read() {  //读入各传感器针脚的信号
    Sensor[1] = digitalRead(Signal_1);
    Sensor[2] = digitalRead(Signal_2);
    Sensor[3] = digitalRead(Signal_3);
    Sensor[4] = digitalRead(Signal_4);
    Sensor[5] = digitalRead(Signal_5);
    Sensor[6] = digitalRead(Signal_Rear);
}

//信号输出为PWM至L298N的四个针脚
void Motor_Speed(int Left1_PWM, int Left2_PWM, int Right1_PWM, int Right2_PWM) {  
    analogWrite(Left_IN3, Left1_PWM);
    analogWrite(Left_IN4, Left2_PWM); //小车自身左轮的正负PWM
    analogWrite(Right_IN1, Right1_PWM);
    analogWrite(Right_IN2, Right2_PWM); //右轮的正负PWM
}

//循迹，监测到黑线为1
void Trail_Follow() {
    //循迹，监测到黑线为1
    int i; //定义循环变量
    int sensor_sum = 0; //定义传感器的和
    int sensor_index = 0; //定义传感器的索引
    for(i = 1; i < 6; i++) { //遍历前方传感器数组
        sensor_sum += Sensor[i]; //累加传感器的值
        if(Sensor[i] == 1) { //如果传感器为1
            sensor_index = i; //记录传感器的索引
        }
    }
    switch(sensor_sum) { //根据传感器的和选择不同的情况
        case 0: //00000，直行
            Motor_Speed(100,0,100,0);
            break;
        case 1: //只有一个传感器为1
            switch(sensor_index) { //根据传感器的索引选择不同的情况
                case 1: //10000，大左转
                    Motor_Speed(0,170,170,0);
                    delay(10);
                    break;
                case 2: //01000，小左转
                    Motor_Speed(0,60,100,0);
                    break;
                case 3: //00100，直走或长直线加速
                    if (Sensor[6]==1) {
                        //Motor_Speed(100,0,100,0);
                        Motor_Speed(135,0,135,0);
                    }
                    else{
                        //Motor_Speed(100,0,100,0);
                        Motor_Speed(115,0,115,0);
                    }
                    break;
                case 4: //00010，小右转
                    Motor_Speed(100,0,0,60);
                    break;
                case 5: //00001，大右转
                    Motor_Speed(170,0,0,170);
                    delay(10);
                    break;
            }
            break;
        case 2: //有两个传感器为1
            if(Sensor[0] == 1 && Sensor[1] == 1) { //11000，大左转
                Motor_Speed(0,170,170,0);
            }
            else if(Sensor[1] == 1 && Sensor[2] == 1) { //01100，小左转
                Motor_Speed(0,60,100,0);
            }
            else if(Sensor[2] == 1 && Sensor[3] == 1) { //00110，小右转
                Motor_Speed(100,0,0,60);
            }
            else if(Sensor[3] == 1 && Sensor[4] == 1) { //00011，大右转
                Motor_Speed(170,0,0,170);
            }
            break;
        default: //其它情况，直行
            Motor_Speed(100,0,100,0);
            break;
    }
}

//出库
void Start() {
    while (Start_Flag <= 10)
    {
        if(Sensor_Rear == 0) {
            Start_Flag++;
            delay(100);
        }
        Sensor_Rear = digitalRead(Signal_Rear);
    } 
}


void Count() { 
    //对黑色横线计次 
    if(Sensor[2] == 1 && Sensor[3] == 1 && Sensor[4] == 1) { 
        Line++;
        Motor_Speed(60,0,60,0);
        delay(200);
    }

    //第三根黑线及终点大黑线
    if(Line >= 6) {
        Motor_Speed(60,0,60,0);
        delay(1000);        
        Motor_Speed(0,0,0,0); //停车
        delay(100000);
    }
}

// void Print() {  //串口打印，用于调试
//     Serial.print(Sensor_Rear);
// //    Serial.print(Sensor1);
// //    Serial.print("---");
// //    Serial.print(Sensor2);
// //    Serial.print("---");
// //    Serial.print(Sensor3);
// //    Serial.print("---");
// //    Serial.print(Sensor4);
// //    Serial.print("---");
// //    Serial.print(Sensor5);
// //    Serial.print("\n");
// //  
// //    Serial.print("-");
// //    Serial.print(Line);
// //    Serial.print("-");
// }




