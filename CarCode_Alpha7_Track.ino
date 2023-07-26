#define Left_IN3 9
#define Left_IN4 10
#define Right_IN1 5
#define Right_IN2 6 //定义连接L298N的IN1，IN2，IN3，IN4的四个针脚
#define Signal_1 12
#define Signal_2 11
#define Signal_3 7
#define Signal_4 4
#define Signal_5 3  
#define Signal_Rear 2 //定义连接红外传感器的针脚，Signal_Rear为小车后方传感器输出的信号

int Sensor1,Sensor2,Sensor3,Sensor4,Sensor5,Sensor_Rear = 0;//初始化传感器的值
    //小车自身前方左至右传感器依次为为Sensor12345,后方中置传感器为Sensor_Rear
int Start_Time = 0; //起点出库运行单次的辅助计次符
int Line = 0; //黑色横线计数量
int Line_Action_Time; //Line为某值时，执行命令时的运行单次的辅助计次符

void setup() {  
    pinMode(Signal_1,INPUT);
    pinMode(Signal_2,INPUT);
    pinMode(Signal_3,INPUT);
    pinMode(Signal_4,INPUT);
    pinMode(Signal_5,INPUT);
    pinMode(Signal_Rear,INPUT);
    
    pinMode(Left_IN3,OUTPUT);
    pinMode(Left_IN4,OUTPUT);
    pinMode(Right_IN1,OUTPUT);
    pinMode(Right_IN2,OUTPUT);  //设定各针脚的输入输出模式
    
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
    Sensor1 = digitalRead(Signal_1);
    Sensor2 = digitalRead(Signal_2);
    Sensor3 = digitalRead(Signal_3);
    Sensor4 = digitalRead(Signal_4);
    Sensor5 = digitalRead(Signal_5);
    Sensor_Rear = digitalRead(Signal_Rear);
}

void Motor_Speed(int Left1_PWM, int Left2_PWM, int Right1_PWM, int Right2_PWM) {  //信号输出为PWM至L298N的四个针脚
    analogWrite(Left_IN3, Left1_PWM);
    analogWrite(Left_IN4, Left2_PWM); //小车自身左轮的正负PWM
    analogWrite(Right_IN1, Right1_PWM);
    analogWrite(Right_IN2, Right2_PWM); //右轮的正负PWM
}


void Start() {  //出库
    if(Start_Time == 0) {
        Motor_Speed(60,0,60,0); //出库直行
        delay(600);
        Motor_Speed(0,60,120,0);  //出库左转
        delay(600);
        Start_Time++;
    }
}

void Trail_Follow() { //循迹，监测到黑线为1
    if(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0 && Sensor_Rear ==0) //00100&0，直走(监测到黑线为1)
       Motor_Speed(100,0,100,0);
       
    else if(Sensor1== 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0 && Sensor_Rear ==1){ //00100&1，长直线加速
        if(Line + Line_Action_Time == 3) {  //判断已经经过第二个岔路口，可以执行长直线高速模式
        Motor_Speed(230,0,230,0); 
        }
        else Motor_Speed(110,0,110,0);
    }
    
    else if(Sensor1== 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0) //00000，直行
        Motor_Speed(100,0,100,0);
    else if((Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 1)
            ||(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 1 && Sensor5 == 1)) //00001或00011，大右转
        Motor_Speed(170,0,0,170);
    else if((Sensor1 == 1 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0)
            ||(Sensor1 == 1 && Sensor2 == 1 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0)) //10000或11000，大左转
      Motor_Speed(0,170,170,0);
    else if((Sensor1 == 0 && Sensor2 == 1 && Sensor3 == 0 && Sensor4 == 0 && Sensor5 == 0)
            ||(Sensor1 == 0 && Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 0 && Sensor5 == 0)) //01000或01100，小左转
        Motor_Speed(0,60,100,0);
    else if((Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 0 && Sensor4 == 1 && Sensor5 == 0)
            ||(Sensor1 == 0 && Sensor2 == 0 && Sensor3 == 1 && Sensor4 == 1 && Sensor5 == 0)) //00010或00110，小右转
        Motor_Speed(100,0,0,60);
    else 
        Motor_Speed(100,0,100,0);//其它情况，直行
}

void Count() {  //对黑色横线计次并执行岔路拐弯、终点线入库
    //检测黑线,并Line+1  
    if(Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 1 ) { 
        Line++;
        //Motor_Speed(0,0,0,0);
        delay(100);
    }
    
    //↓↓↓第一根小黑横线
    if(Line + Line_Action_Time == 1) {   
        Motor_Speed(0,0,120,0);
        Line_Action_Time++;
        delay(600);
        Motor_Speed(100,0,100,0);
        delay(200);
    }
    //↓↓↓第二根小黑横线
    else if(Line + Line_Action_Time == 3) {   
        Motor_Speed(0,0,120,0);
        Line_Action_Time++;
        delay(600); 
        Motor_Speed(100,0,100,0);
        delay(200);
    }
    //↓↓↓第三根黑线及终点大黑线
    else if((Line >= 3) 
            || (Sensor1 == 1 && Sensor2 == 1 && Sensor3 == 1 && Sensor4 == 1 && Sensor5 == 1 && Line != 1)) {
        Motor_Speed(0,0,0,0);  //如果到达终点，就刹车
        delay(200);
        Motor_Speed(60,0,60,0); //前进一段，准备入库
        delay(600);
        Motor_Speed(0,100,100,0); //左转入库
        delay(550);
        Motor_Speed(60,0,60,0); //前进进库
        delay(1100);
        
        Motor_Speed(0,0,0,0); //停车
        delay(100000);
    }
}

void Print() {  //串口打印，用于调试
  Serial.print(Sensor_Rear);
//    Serial.print(Sensor1);
//    Serial.print("---");
//    Serial.print(Sensor2);
//    Serial.print("---");
//    Serial.print(Sensor3);
//    Serial.print("---");
//    Serial.print(Sensor4);
//    Serial.print("---");
//    Serial.print(Sensor5);
//    Serial.print("\n");
//  
//    Serial.print("-");
//    Serial.print(Line);
//    Serial.print("-");
}
