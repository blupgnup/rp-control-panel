#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <iostream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <iomanip> // put_time

#include "main.h"

//Signal size
#define SIGNAL_SIZE_DEFAULT      1024
#define SIGNAL_UPDATE_INTERVAL      100

// ofstream object declaration to write onto file
std::ofstream outFile;

//Signal
CFloatSignal AIN_0("AIN_0", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal AIN_1("AIN_1", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal AIN_2("AIN_2", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal AIN_3("AIN_3", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> aIn0_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> aIn1_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> aIn2_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> aIn3_data(SIGNAL_SIZE_DEFAULT);

//Parameter
CBooleanParameter gpioState("GPIO_STATE", CBaseParameter::RW, false, 0);
CFloatParameter AOUT_0_AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0, 0, 0, 1.8);
CIntParameter AIN_0_GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 100);
CFloatParameter AIN_0_OFFSET("OFFSET", CBaseParameter::RW, 0.0, 0, 0.0, 5.0);


const char *rp_app_desc(void)
{
    return (const char *)"Control panel application.\n";
}


int rp_app_init(void)
{
    fprintf(stderr, "Loading control panel application\n");

    // Initialization of API
    if (rpApp_Init() != RP_OK) 
    {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }
    else fprintf(stderr, "Red Pitaya API init success!\n");
	
	//Set signal update interval
    CDataManager::GetInstance()->SetSignalInterval(SIGNAL_UPDATE_INTERVAL);

    // configure DIO7_N to output
    rp_DpinSetDirection (RP_DIO7_N, RP_OUT);

    // Initialize output file
    outFile.open("/opt/data/data.csv", std::ios::app);
    if (!outFile) {
        fprintf(stderr, "Error, could not open output file!\n");
        return EXIT_FAILURE;
    }
    outFile << "Timestamp,Input,Value" << std::endl;
	
    return 0;
}


int rp_app_exit(void)
{
    fprintf(stderr, "Unloading control panel application\n");

    rpApp_Release();
	
    // Closing data file
    outFile.close();

    return 0;
}


int rp_set_params(rp_app_params_t *p, int len)
{
    return 0;
}


int rp_get_params(rp_app_params_t **p)
{
    return 0;
}


int rp_get_signals(float ***s, int *sig_num, int *sig_len)
{
    return 0;
}


void UpdateSignals(void){
	float val;
    
	// Update analog pin value
	rp_AOpinSetValue(0, AOUT_0_AMPLITUDE.Value());
	
    // Read values from analog input and convert to physical values
    
    //Read Ain_0 from pin 0
    rp_AIpinGetValue(0, &val);
    // Write value to data file (without offset or gain applied)
    std::string input = "Ain_0";
	// Get timestamp
	auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    outFile << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "," << input << "," << val << std::endl;
    //Calculating and pushing it to vector
    aIn0_data.erase(aIn0_data.begin());
    aIn0_data.push_back((val * AIN_0_GAIN.Value()) + AIN_0_OFFSET.Value());

    //Read Ain_1 from pin 1
    rp_AIpinGetValue(1, &val);
    // Write value to data file (without offset or gain applied)
    //std::string input = "Ain_1";
    //outFile << "Input: " << input << ", Value: " << val << std::endl;
    //Calculating and pushing it to vector
    aIn1_data.erase(aIn1_data.begin());
    aIn1_data.push_back(val);

    //Read Ain_2 from pin 2
    rp_AIpinGetValue(2, &val);
    // Write value to data file (without offset or gain applied)
    //std::string input = "Ain_2";
    //outFile << "Input: " << input << ", Value: " << val << std::endl;
    //Calculating and pushing it to vector
    aIn2_data.erase(aIn2_data.begin());
    aIn2_data.push_back(val);

    //Read Ain_3 from pin 3
    rp_AIpinGetValue(3, &val);
    // Write value to data file (without offset or gain applied)
    //std::string input = "Ain_3";
    //outFile << "Input: " << input << ", Value: " << val << std::endl;
    //Calculating and pushing it to vector
    aIn3_data.erase(aIn3_data.begin());
    aIn3_data.push_back(val);

    //Write data to signal
    for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) 
    {
        AIN_0[i] = aIn0_data[i];
        AIN_1[i] = aIn1_data[i];
        AIN_2[i] = aIn2_data[i];
        AIN_3[i] = aIn3_data[i];
    }
}


void UpdateParams(void){}


void OnNewParams(void) {
	gpioState.Update();
	
	// If gpioStage on, we switch the laser state
	if (gpioState.Value() == false)
	{
        rp_DpinSetState (RP_DIO7_N, RP_LOW);
        // We also switch on the led 0 as an indicator
		rp_DpinSetState(RP_LED0, RP_LOW);
	}
	else
	{
        rp_DpinSetState (RP_DIO7_N, RP_HIGH);
        // And switching off the led 0
		rp_DpinSetState(RP_LED0, RP_HIGH);
	}
	
	AOUT_0_AMPLITUDE.Update();
    AIN_0_GAIN.Update();
    AIN_0_OFFSET.Update();
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}
