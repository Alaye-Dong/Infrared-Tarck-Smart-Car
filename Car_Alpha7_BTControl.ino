#include <SoftwareSerial.h>
SoftwareSerial BT(0, 1);

#define Left_IN3 9
#define Left_IN4 10
#define Right_IN1 5
#define Right_IN2 6 //定义连接L298N的IN1，IN2，IN3，IN4的四个针脚

void setup() {  
    pinMode(Left_IN3,OUTPUT);
    pinMode(Left_IN4,OUTPUT);
    pinMode(Right_IN1,OUTPUT);
    pinMode(Right_IN2,OUTPUT);  //设定各针脚的输入输出模式
    
    Serial.begin(9600); //设置串口波特率用于调试
}

void loop(){
    char receiveCode = Serial.read();  //读取蓝牙模块获得的数据

    if(receiveCode =='a'){
      forward();
    }
    if(receiveCode =='b'){
      back();
    }
    if(receiveCode =='c'){
      left();
    }
    if(receiveCode =='d'){
      right();
    }
    if(receiveCode =='e'){
      turnLeftOrigin();
    }
    if(receiveCode =='f'){
      turnRightOrigin();
    }
    if(receiveCode =='g'){
      _stop();
    }

    //}
    
//    Print();
}

void Motor_Speed(int Left1_PWM, int Left2_PWM, int Right1_PWM, int Right2_PWM) {  //信号输出为PWM至L298N的四个针脚
    analogWrite(Left_IN3, Left1_PWM);
    analogWrite(Left_IN4, Left2_PWM); //小车自身左轮的正负PWM
    analogWrite(Right_IN1, Right1_PWM);
    analogWrite(Right_IN2, Right2_PWM); //右轮的正负PWM
}

void forward(){
    Motor_Speed(100,0,100,0);
}

void back(){
    Motor_Speed(0,100,0,100);
}

void left(){
    Motor_Speed(60,0,120,0);
}

void right(){
    Motor_Speed(120,0,60,0);
}

void turnLeftOrigin(){
    Motor_Speed(0,90,90,0);
}

void turnRightOrigin(){
    Motor_Speed(90,0,0,90);
}

void _stop(){
    Motor_Speed(0,0,0,0);
}


//void Print() {  //串口打印，用于调试
//
//}
