#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

#include "main.h"

//Signal size
#define SIGNAL_SIZE_DEFAULT      512
#define SIGNAL_UPDATE_INTERVAL      20


//Signal
CFloatSignal TACT("TACT", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal TSET("TSET", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal ITEC("ITEC", SIGNAL_SIZE_DEFAULT, 0.0f);
CFloatSignal ILAS("ILAS", SIGNAL_SIZE_DEFAULT, 0.0f);
std::vector<float> tAct_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> tSet_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> iTec_data(SIGNAL_SIZE_DEFAULT);
std::vector<float> iLas_data(SIGNAL_SIZE_DEFAULT);

//Parameter
CBooleanParameter laserState("LED_STATE", CBaseParameter::RW, false, 0);
CFloatParameter AMPLITUDE("AMPLITUDE", CBaseParameter::RW, 0, 0, 0, 1.8);
CBooleanParameter ch1State("CH1_STATE", CBaseParameter::RW, false, 0);
CIntParameter FREQUENCY_CH1("FREQUENCY_CH1", CBaseParameter::RW, 1, 0, 1, 100);
CFloatParameter AMPLITUDE_CH1("AMPLITUDE_CH1", CBaseParameter::RW, 0.5, 0, 0, 2);
CFloatParameter OFFSET_CH1("OFFSET_CH1", CBaseParameter::RW, 0.25, 0, -1, 1);
CIntParameter WAVEFORM_CH1("WAVEFORM_CH1", CBaseParameter::RW, 0, 0, 0, 2);
CBooleanParameter ch2State("CH2_STATE", CBaseParameter::RW, false, 0);
CIntParameter FREQUENCY_CH2("FREQUENCY_CH2", CBaseParameter::RW, 1, 0, 1, 100);
CFloatParameter AMPLITUDE_CH2("AMPLITUDE_CH2", CBaseParameter::RW, 0.5, 0, 0, 2);
CFloatParameter OFFSET_CH2("OFFSET_CH2", CBaseParameter::RW, 0.25, 0, -1, 1);
CIntParameter WAVEFORM_CH2("WAVEFORM_CH2", CBaseParameter::RW, 0, 0, 0, 2);


// Generator config
void set_generator_config()
{
    //Set frequency
    rp_GenFreq(RP_CH_1, FREQUENCY_CH1.Value());
    rp_GenFreq(RP_CH_2, FREQUENCY_CH2.Value());

    //Set offset
    rp_GenOffset(RP_CH_1, OFFSET_CH1.Value());
    rp_GenOffset(RP_CH_2, OFFSET_CH2.Value());

    //Set amplitude
    rp_GenAmp(RP_CH_1, AMPLITUDE_CH1.Value());
    rp_GenAmp(RP_CH_2, AMPLITUDE_CH2.Value());

    //Set waveform
    if (WAVEFORM_CH1.Value() == 0)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
    }
    else if (WAVEFORM_CH1.Value() == 1)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_TRIANGLE);
    }
    else if (WAVEFORM_CH1.Value() == 2)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_RAMP_UP);
    }
    else if (WAVEFORM_CH1.Value() == 3)
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_DC);
    }
    else 
    {
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SQUARE);
    }

    //Set waveform
    if (WAVEFORM_CH2.Value() == 0)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);
    }
    else if (WAVEFORM_CH2.Value() == 1)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_TRIANGLE);
    }
    else if (WAVEFORM_CH2.Value() == 2)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_RAMP_UP);
    }
    else if (WAVEFORM_CH2.Value() == 3)
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_DC);
    }
    else
    {
        rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SQUARE);
    }
}


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

    // Init generator config (without turning it on)
    set_generator_config();
	
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
        TACT[i] = tAct_data[i];
        TSET[i] = tSet_data[i];
        ITEC[i] = iTec_data[i];
        ILAS[i] = iLas_data[i];
    }
}


void UpdateParams(void){}


void OnNewParams(void) {
	laserState.Update();
	
	// If laserState on, we switch the laser state
	if (laserState.Value() == false)
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
	
	AMPLITUDE.Update();

    FREQUENCY_CH1.Update();
    AMPLITUDE_CH1.Update();
    OFFSET_CH1.Update();
    WAVEFORM_CH1.Update();

    FREQUENCY_CH2.Update();
    AMPLITUDE_CH2.Update();
    OFFSET_CH2.Update();
    WAVEFORM_CH2.Update();

    // Set generators config
    set_generator_config();

    ch1State.Update();
    // If Channel 1 is on, we switch the channel 1
	if (ch1State.Value() == false)
	{
        rp_GenOutDisable(RP_CH_1);
    }
    else
    {
        rp_GenReset();
        // Init generator
        rp_GenOutEnable(RP_CH_1);
    }

    ch2State.Update();
    // If Channel 2 is on, we switch the channel 2
	if (ch2State.Value() == false)
	{
        rp_GenOutDisable(RP_CH_2);
    }
    else
        // Init generator
        rp_GenOutEnable(RP_CH_2);
    }
}


void OnNewSignals(void){}


void PostUpdateSignals(void){}
