#include "mbed.h"
#include "math.h"

#define UPPERTHRESHOLD 610
#define UNDERTHRESHOLD 480
#define MAXIMUM_HEARTRATE 360//max heart rate in 2 min
#define MINIMUM_HEARTRATE 60//min heart rate in 2 min

Ticker Sample;
Timer IntervalTimer;
Timeout HrvCalculationTimeout;//2-minute timeout to time the HRV calculation
Timeout MinimumIntervalTimeout;//timeout controlling minimum interval
AnalogIn input(PA_0);
DigitalIn userbutton(PC_13);
DigitalOut led(LED1);

//variables for hrv calculation
float NN_interval[MAXIMUM_HEARTRATE];//array for storing IBI
int beatcount = 0;//heart beat count in 2 min
int THRESHOLD = 550;//initial threshold

//variables for threshold determination
float raw_data[(120/MINIMUM_HEARTRATE)*50];//array for storing raw sampling data for a single heartbeat
int datacount = 0;

bool sample;

static UnbufferedSerial pc(USBTX,USBRX,115200);
//UnbufferedSerial hm10(PB_11, PB_10); //UART6 TX,RX

void sendData()//ticker can only be used to set flag, print and sample need to be done in the main loop
{
    sample = 0;
}

void writearray(int size, float dataarray[size], float newdata, int Counter, int state){//state 1 represent NN_interval array, state 2 represent raw_data array
    
    if (state == 1){
        NN_interval[Counter] = newdata;//write IBI into interval array and increment the count
    }
    else if(state == 2){
        raw_data[Counter] = newdata;
    }
    Counter++;//point to next element, this value is also the number of beats since it start from 0
    if(Counter >= size)//if count == size, wrap over to 0 to avoid overflow
    {
        Counter = 0;
    }
    if (state == 1){
        beatcount = Counter;//update global beatcount variable
    }
    else if(state == 2){
        datacount = Counter;
    }
}

bool hrvcalculationflag = 0;
void sethrvcalculationflag(){
    hrvcalculationflag = 1;
}

float avgibicalculation(int size, float array[size]){//average
    float avg;
    float sum = 0;
    for (int i=0; i<size; i++){
            sum += NN_interval[i];
        }
    avg = sum/size;
    return avg;
}

float hrvcalculationfunc(int size, float array[size], float avg){//SDNN
    float sdnn = 0;
    float variance = 0;
    float sum = 0;
    for (int i=0; i<size; i++){
            float difference = pow(NN_interval[i]-avg,2);
            sum += difference;
        }
    variance = (1.0/(size-1))*sum;
    sdnn = sqrt(variance);
    return sdnn;
}

float maxValue(float myArray[], size_t size) {
    assert(myArray && size);
    size_t i;
    float maxValue = myArray[0];

    for (i = 0; i < size; i++) {
        if ( myArray[i] > maxValue ) {
            maxValue = myArray[i];
        }
    }
    return maxValue;
}

float minValue(float myArray[], size_t size) {
    assert(myArray && size);
    size_t i;
    float minValue = myArray[0];

    for (i = 0; i < size; i++) {
        if ( myArray[i] < minValue ) {
            minValue = myArray[i];
        }
    }
    return minValue;
}

static bool IBIProtected = false;
void SetIBIProtectedFalse(){
    IBIProtected = false;//release protect
}

int main() {
    
    Sample.attach(&sendData, 20ms);//Sample from Pulsesensor, specifing sample rate at 50 Hz

    while(1) {
        if(sample == 0){//sample function
            sample = 1;
            int data = input.read()*1024-300;
            
            if (data > THRESHOLD){//A heartbeat happened
                if (IBIProtected == false){
                    //Determine if this is a new heartbeat
                    //if not in minimum interval, 1.store data into array 2.reset and start timer
                    IBIProtected = true;
                    MinimumIntervalTimeout.attach(&SetIBIProtectedFalse, 450ms);
                    float minData,maxData;
                    minData = minValue(raw_data, datacount);
                    maxData = maxValue(raw_data, datacount);
                    THRESHOLD = (0.6 * maxData + 0.4 * minData)*0.3+0.7*THRESHOLD;
                    if (THRESHOLD<UNDERTHRESHOLD){
                        THRESHOLD = UNDERTHRESHOLD;
                    }
                    else if (THRESHOLD>UPPERTHRESHOLD){
                        THRESHOLD = UPPERTHRESHOLD;
                    }
                    datacount = 0;//reset datacount to 0

                    static bool firstturn = 1;
                    if (firstturn == 1){
                        firstturn = 0;
                        IntervalTimer.start();
                        THRESHOLD = 550;
                    }
                    else{
                    float IBI = IntervalTimer.read_ms();
                    writearray(MAXIMUM_HEARTRATE, NN_interval, IBI, beatcount, 1);//write the measured interval into NN_interval array
                    printf("B%d\n", int(60000.0/IBI));//send HR to serial, comment here to implement the aggregated data calculation.
                    printf("Q%d\n", int(IBI));//send IBI to serial, comment here to implement the aggregated data calculation.
                    led = 0;//light up when storing data
                    IntervalTimer.reset();}
                }
                //led = 0;light up as long as the data is higher than threshold
            }
            else{led = 1;}
            writearray((120/MINIMUM_HEARTRATE)*50, raw_data, data, datacount, 2);//write the measured sampling data into raw_data array

            static bool sentFlag = 0; //Sends data every two samples to avoid data lost
            if(sentFlag == 1){// 
                printf("S%d\n", data);//comment if sent data every two samples, comment here to implement the aggregated data calculation.
                sentFlag = 0;
            }
            else{
                printf("S%d\n", data);//send data to serial, comment here to implement the aggregated data calculation.
                sentFlag = 1;
            }
        }
        if(userbutton){//if user pressed button start timeout for HRV calculation
        beatcount = 0;//reset the counter
        HrvCalculationTimeout.attach(&sethrvcalculationflag, 30s);//30 s record length
        }//HRV calculation

        if(hrvcalculationflag == 1){
            hrvcalculationflag = 0;
            int N = beatcount;//number of beats(data block size)
            float data_array[N];
            for (int i=0; i<N; i++){
                data_array[i] = NN_interval[i];
            }
            float avgibi = avgibicalculation(N, data_array);//in ms
            //printf("heartrate:%d\n", int(60000.0/avgibi));//send avgdata to serial. Uncomment here to implement the aggregated data calculation.
            float hrv_sdnn = hrvcalculationfunc(N, data_array, avgibi);
            //printf("HRV:%d ms", int(hrv_sdnn));//send HRV to serial. Uncomment here to implement the aggregated data calculation.
            //printf("ms\n");
        }
    }
}