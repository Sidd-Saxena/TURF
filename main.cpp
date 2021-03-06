/* ESE 519 : Final Project TURF
demonstrates the main function to cntrol both the bots
Author :Siddharth Saxena, University of Pennsylvania*/


#include <sstream>
#include "robot.h"

const char * dataToString(DataSample);
void checkLidarThread(void const *args);
void reverse(int size, string* rpath);
int TemperatureRead();

LidarLite _lidar(p9,p10);
AnalogIn Temp(p16);
//declaring global variables
float dis;
int path;
string dir;
int flowcount=0;
string rpath[4];
void goback();
int counttime=0;
int godist;
string godista;
int main()
{
    std::stringstream ss;
    std::stringstream st;
    Robot r;
    //callibrate all the sensors and servos
    r.calibrate(true, true);
    //r.xbee.sendData(dataToString(r.scan()));
    //Thread lidarThread(checkLidarThread);   
    while(1)
    {
        dis = r.range();
       // printf("%f",dis);
        //distance calculated to move forward
        r.forward(dis,0.1);
        
        //call function to read sorrounding temperature 
        int mytemp = TemperatureRead();
        
         // Add the robot's position to the string      
        ss << '<';
        ss << r._x << " " << r._y << " " << r._th <<" "<<mytemp<<'\n'<<'\r';
        ss << '>'; 
        //send data about the co ordinates of the bot on xbee
        r.xbee.sendData(ss.str().c_str());
        //counttime++;
    
        //ss << '<';
//        ss <<path<<","<<"F";
//        ss << '>';
        //r.xbee.sendData(ss.str().c_str());
        //st << 'f'<<dis;
        //rpath[flowcount] = st.str();
//        flowcount++;
        dir = r.servo_scan();
        //Logic for selecting left or right depending on servo scan 
        if (dir == "R")
        {
            r.right(0.1);
            //keeping the record of the path travelled
           ss << '<';
           ss <<path<<","<<"R";
           ss << '>'; 
           rpath[flowcount]="r0";
           //count the number of steps taken
           flowcount++;
        }
        else if (dir == "L")
        {
            r.left(0.1);
            ss << '<';
            ss <<path<<","<<"L";
            ss << '>'; 
            rpath[flowcount]="l0";
            flowcount++;
        }
    }
    
    //demonstating the going following the other bot functionality
        if (flowcount == 4)
        {
//          printf("Entered reverse");
//enter the path travelled into an array
          reverse(4,rpath); 
//       printf("Array: %s %s %s %s  \n \r" , rpath[0],rpath[1], rpath[2], rpath[3]);
        }
        logic to follow the path travelled
         for (int i=0; i<4; i++){
                        //printf("looking forward");
                        if(rpath[i].at(0)=='f')
                        {
                         printf("Entered forward");
                         godista=rpath[i].substr(1,2);
                         godist=atoi(godista.c_str());
                         r.forward(godist,0.1);   
                        }
                      //  printf("looking right");
                         else if(rpath[i].at(0)=='r')
                        {
                            printf("Entered right");
                            r.right(0.1);
                        }
                        //printf("looking left");
                         else if(rpath[i].at(0)=='l')
                        {
                            printf("Entered left");
                            r.left(0.1);
                        }
                }         
    }

/* Convert a data sample into a string to send over the xbee. */
const char * dataToString(DataSample ds)
{
    std::stringstream ss;
    ss << '<';
    // Add the robot's position to the string
    ss << ds.x << " " << ds.y << " " << ds.th << '\n';
    // Iterate through each data point and add it to the string
    for (unsigned int i = 0; i < ds.data.size(); i++) {
        ss << ds.data[i].first << " " << ds.data[i].second << '\n';
    }
    ss << '>';
    return ss.str().c_str();
}

//Multithreading to allow reading of LIDAR even when it's travelling

void checkLidarThread(void const *args) {
    int countIf = 0, countElse = 0;
    while (true) {
        printf("In Lidar thread\n\r");
        printf("%f \n\r",_lidar.getRange_cm());
        if(_lidar.getRange_cm() < 50){
            countIf++;   
            countElse = 0; 
            Thread::wait(1000);
       }
       else if(_lidar.getRange_cm() > 10){
            countElse++;
            countIf = 0;
       }    
        _lidar.refreshRange();
    }
}


//logic to travel back to the battery station
void reverse(int size, string* rpath){
    int start=0;
    int size_minus1=size-1;
    //reverse order
    string temp;
    while(start < size_minus1)
    {
    temp = rpath[start];   
    rpath[start] = rpath[size_minus1];
    rpath[size_minus1] = temp;
    start++;
    size_minus1--;
    }   
    //pc.printf("Fliiped Array: %s %s %s %s  \n \r" , rpath[0],rpath[1], rpath[2], rpath[3]);
    //change r to l and l to r
    for (int i=0; i<4; i++)
    {
    if (rpath[i].at(0)=='r')
        {
        rpath[i]='l'+rpath[i].erase(0,1);
        }
    else if (rpath[i].at(0)=='l')
        {
        rpath[i]='r'+rpath[i].erase(0,1);
        }
    }
    //pc.printf("Switched: %s %s %s %s  \n \r" , rpath[0],rpath[1], rpath[2], rpath[3]);   
       
}

//function to read tempertaure (proof of idea about collecting senosr data)
int TemperatureRead()
{
          float sensorReading = Temp.read(); //read temperature from the sensor
          sensorReading=(((sensorReading*5000)/1024)-500)/10;
          int degreeF = ((sensorReading*9)/5)+32;
          return sensorReading;
}
