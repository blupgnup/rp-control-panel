#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "main.h"

//Signal size
#define SIGNAL_SIZE_DEFAULT      1024
#define SIGNAL_UPDATE_INTERVAL      100


//Signal
CFloatSignal RTH("RTH", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal TSET("TSET", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal ITEC("ITEC", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal ILAS("ILAS", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> tAct_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> tSet_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> iTec_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> iLas_data(SIGNAL_SIZE_DEFAULT);

//Parameter
CBooleanParameter ledState("LED_STATE", CBaseParameter::RW, false, 0);
CIntParameter GAIN("GAIN", CBaseParameter::RW, 1, 0, 1, 100);
CFloatParameter OFFSET("OFFSET", CBaseParameter::RW, 0.0, 0, 0.0, 5.0);
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0, 0, 0, 1.8);


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
	
    return 0;
}


int rp_app_exit(void)
{
    fprintf(stderr, "Unloading control panel application\n");

    rpApp_Release();
	
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
	rp_AOpinSetValue(0, AMPLITUDE.Value());
	
    // Read values from analog input and convert to physical values
    
    //Read Tact from pin 0
    rp_AIpinGetValue(0, &val);
    //Calculating and pushing it to vector
    //rTh_data.erase(rTh_data.begin());
    //rTh_data.push_back((10 * (10 - val) / (5 * val)));
    tAct_data.erase(tAct_data.begin());
    tAct_data.push_back((25/4 * val) + 10);

    //Read Tset from pin 1
    rp_AIpinGetValue(1, &val);
    //Calculating and pushing it to vector
    tSet_data.erase(tSet_data.begin());
    tSet_data.push_back((25/4 * val) + 10);  // We convert VtSet to tSet same as tAct

    //Read Itec from pin 2
    rp_AIpinGetValue(2, &val);
    //Calculating and pushing it to vector
    iTec_data.erase(iTec_data.begin());
    iTec_data.push_back( 1 * (val - 2.5));

    //Read Ilas from pin 3
    rp_AIpinGetValue(3, &val);
    //Calculating and pushing it to vector
    iLas_data.erase(iLas_data.begin());
    iLas_data.push_back( 0.05 * val);

    //Write data to signal
    for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) 
    {
        RTH[i] = tAct_data[i];
        TSET[i] = tSet_data[i];
        ITEC[i] = iTec_data[i];
        ILAS[i] = iLas_data[i];
    }
}


void UpdateParams(void){}


void OnNewParams(void) {
	ledState.Update();
	
	// If ledState on, we switch the led state
	if (ledState.Value() == false)
	{
		rp_DpinSetState(RP_LED0, RP_LOW);
	}
	else
	{
		rp_DpinSetState(RP_LED0, RP_HIGH);
	}
	
	
	GAIN.Update();
    OFFSET.Update();
	AMPLITUDE.Update();
	
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}