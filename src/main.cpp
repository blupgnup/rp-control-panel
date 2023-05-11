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
CFloatSignal VOLTAGE("VOLTAGE", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> g_data(SIGNAL_SIZE_DEFAULT);


//Parameter
CBooleanParameter ledState("LED_STATE", CBaseParameter::RW, false, 0);
CBooleanParameter READ_VALUE("READ_VALUE", CBaseParameter::RW, false, 0);
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
	rp_AOpinSetValue(1, AMPLITUDE.Value());
	
    //Read data from pin 1
    rp_AIpinGetValue(1, &val);

    //Push it to vector
    g_data.erase(g_data.begin());
    g_data.push_back(val);

    //Write data to signal
    for(int i = 0; i < SIGNAL_SIZE_DEFAULT; i++) 
    {
        VOLTAGE[i] = g_data[i] * GAIN.Value() + OFFSET.Value();
    }
}


void UpdateParams(void){}


void OnNewParams(void) {
	ledState.Update();
	READ_VALUE.Update();
	
	// If ledState on, we switch the led state
	if (ledState.Value() == false)
	{
		rp_DpinSetState(RP_LED0, RP_LOW);
	}
	else
	{
		rp_DpinSetState(RP_LED0, RP_HIGH);
	}
	
	// If READ_VALUE, we process the analog reading
	if (READ_VALUE.Value() == true)
	{
		float val;

		//Read data from pin 1
		rp_AIpinGetValue(1, &val);

		//Write data to signal
		VOLTAGE[0] = val;

		//Reset READ value
		READ_VALUE.Set(false);
	}
	
	GAIN.Update();
    OFFSET.Update();
	AMPLITUDE.Update();
	
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}