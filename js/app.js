/*
 * Red Pitaya Template Application
 *
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 */


(function(APP, $, undefined) {
    
    // App configuration
    APP.config = {};
    APP.config.app_id = 'control_panel';
    APP.config.app_url = '/bazaar?start=' + APP.config.app_id + '?' + location.search.substr(1);
    APP.config.socket_url = 'ws://' + window.location.hostname + ':9002';

    // WebSocket
    APP.ws = null;
	
	// Plot
    APP.plot = {};
	
	// Signal stack
    APP.signalStack = [];
	
	// Parameters
    APP.processing = false;
	
	//Laser state
    APP.laser_state = false;




    // Starts template application on server
    APP.startApp = function() {

        $.get(APP.config.app_url)
            .done(function(dresult) {
                if (dresult.status == 'OK') {
                    APP.connectWebSocket();
                } else if (dresult.status == 'ERROR') {
                    console.log(dresult.reason ? dresult.reason : 'Could not start the application (ERR1)');
                    APP.startApp();
                } else {
                    console.log('Could not start the application (ERR2)');
                    APP.startApp();
                }
            })
            .fail(function() {
                console.log('Could not start the application (ERR3)');
                APP.startApp();
            });
    };




    APP.connectWebSocket = function() {

        //Create WebSocket
        if (window.WebSocket) {
            APP.ws = new WebSocket(APP.config.socket_url);
            APP.ws.binaryType = "arraybuffer";
        } else if (window.MozWebSocket) {
            APP.ws = new MozWebSocket(APP.config.socket_url);
            APP.ws.binaryType = "arraybuffer";
        } else {
            console.log('Browser does not support WebSocket');
        }


        // Define WebSocket event listeners
        if (APP.ws) {

            APP.ws.onopen = function() {
				var element = document.getElementById("loader-wrapper");
                element.parentNode.removeChild(element);
                console.log('Socket opened');               
            };

            APP.ws.onclose = function() {
                console.log('Socket closed');
            };

            APP.ws.onerror = function(ev) {
                console.log('Websocket error: ', ev);         
            };

            APP.ws.onmessage = function(ev) {
				console.log('Message recieved');


                //Capture signals
                if (APP.processing) {
                    return;
                }
                APP.processing = true;
				
				try {
					var data = new Uint8Array(ev.data);
                    var inflate = pako.inflate(data);
                    var text = String.fromCharCode.apply(null, new Uint8Array(inflate));
                    var receive = JSON.parse(text);

                    if (receive.parameters) {
                        
                    }

                    if (receive.signals) {
                        APP.signalStack.push(receive.signals);
                    }
                    APP.processing = false;
                } catch (e) {
                    APP.processing = false;
                    console.log(e);
                } finally {
                    APP.processing = false;
                }
            };
        }
    };

	
	// Set Amplitude
	APP.setAmplitude = function() {
		APP.amplitude = $('#amplitude_set').val();
		var local = {};
		local['AMPLITUDE'] = { value: APP.amplitude };
		APP.ws.send(JSON.stringify({ parameters: local }));
		$('#amplitude_value').text(APP.amplitude);
	};


    // Set parameters for Fast output generator OUT1
    APP.setFrequencyCh1 = function() {
        APP.frequency_ch1 = $('#frequency_set_ch1').val();
        APP.frequency_unit_ch1 = $('#frequency_unit_set_ch1').val();
        var local = {};
        local['FREQUENCY_CH1'] = { value: APP.frequency_ch1 * APP.frequency_unit_ch1};
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value_ch1').text(APP.frequency_ch1);
    };
    
    APP.setAmplitudeCh1 = function() {
        APP.amplitude_ch1 = $('#amplitude_set_ch1').val();
        var local = {};
        local['AMPLITUDE_CH1'] = { value: APP.amplitude_ch1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value_ch1').text(APP.amplitude_ch1);
    };

    APP.setOffsetCh1 = function() {
        APP.offset_ch1 = $('#offset_set_ch1').val();
        var local = {};
        local['OFFSET_CH1'] = { value: APP.offset_ch1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value_ch1').text(APP.offset_ch1);
    };
    
    APP.setWaveformCh1 = function() {
        APP.waveform_ch1 = $('#waveform_set_ch1').val();
        console.log('Set to ' + APP.waveform_ch1);
        var local = {};
        local['WAVEFORM_CH1'] = { value: APP.waveform_ch1 };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };


    // Set parameters for Fast output generator OUT2
    APP.setFrequencyCh2 = function() {
        APP.frequency_ch2 = $('#frequency_set_ch2').val();
        APP.frequency_unit_ch2 = $('#frequency_unit_set_ch2').val();
        var local = {};
        local['FREQUENCY_CH2'] = { value: APP.frequency_ch2 *  APP.frequency_unit_ch2 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#frequency_value_ch2').text(APP.frequency_ch2);
    };
    
    APP.setAmplitudeCh2 = function() {
        APP.amplitude_ch2 = $('#amplitude_set_ch2').val();
        var local = {};
        local['AMPLITUDE_CH2'] = { value: APP.amplitude_ch2 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#amplitude_value_ch2').text(APP.amplitude_ch2);
    };

    APP.setOffsetCh2 = function() {
        APP.offset_ch2 = $('#offset_set_ch2').val();
        var local = {};
        local['OFFSET_CH2'] = { value: APP.offset_ch2 };
        APP.ws.send(JSON.stringify({ parameters: local }));
        $('#offset_value_ch2').text(APP.offset_ch2);
    };
    
    APP.setWaveformCh2 = function() {
        APP.waveform_ch2 = $('#waveform_set_ch2').val();
        console.log('Set to ' + APP.waveform_ch2);
        var local = {};
        local['WAVEFORM_CH2'] = { value: APP.waveform_ch2 };
        APP.ws.send(JSON.stringify({ parameters: local }));
    };


    // Processes newly received data for signals
    APP.processSignals = function(new_signals) {

        var data_pool = [];
        var axis = 0;

        // Draw signals
        for (sig_name in new_signals) {
            // Incrementing axis number (axis starts at 1)
            axis += 1;

            // Ignore empty signals
            if (new_signals[sig_name].size == 0) continue;

            var points = [];
            for (var i = 0; i < new_signals[sig_name].size; i++) {
                    points.push([i, new_signals[sig_name].value[i]]);
            }

            //Pushing data into the data_pool
            data_pool.push({
                data: points,
                label: sig_name,
                yaxis: axis
            })            
        }

        // Update graph
        APP.plot.setData(data_pool);
        APP.plot.resize();
        APP.plot.setupGrid();
        APP.plot.draw();
    };
	
	//Handler
    APP.signalHandler = function() {
        if (APP.signalStack.length > 0) {
            APP.processSignals(APP.signalStack[0]);
            APP.signalStack.splice(0, 1);
        }
        if (APP.signalStack.length > 2)
            APP.signalStack.length = [];
    }
    setInterval(APP.signalHandler, 15);
	
	

}(window.APP = window.APP || {}, jQuery));


// Page onload event handler
$(function() {
	// program checks if laser_state switch was clicked
    $('#flexSwitchLaserState').click(function() {

       // changes local laser state
       if (APP.laser_state == true){
           $('#laser_on').hide();
           $('#laser_off').show();
           APP.laser_state = false;
       }
       else{
           $('#laser_off').hide();
           $('#laser_on').show();
           APP.laser_state = true;
       }

       // sends current laser state to backend
       var local = {};
       local['LED_STATE'] = { value: APP.laser_state };
       APP.ws.send(JSON.stringify({ parameters: local }));
    });
   
   // Button click func
    $("#read_button").click(function() {

        APP.readValue(); 
    });
	

	//Init plot
    APP.plot = $.plot($("#placeholder"), [], { 
                series: {
                    shadowSize: 0, // Drawing is faster without shadows
                },
                xaxis: {
                    min: 0,
                    max: 512,
                    show: false
                },
                xaxes: [
                    { }
                ],
                yaxes: [
                    { position: 'left' , min: 10, max: 35},
                    { position: 'left' , min: 10, max: 35},
                    { position: 'right' , min: -0.5, max: 0.5},
                    { position: 'right' , min: 0, max: 0.100}
                ]
    });
		
	
	// Amplitude change
    $("#amplitude_set").on("change input", function() {
        APP.setAmplitude(); 
    });

    // Detect any changes in fast outputs
    $("#ch1_setup").on("change input", function() {
        APP.CH1_UPDATED = true;

        // sends current Channel 1 update to backend
        var local = {};
        local['CH1_UPDATED'] = { value: APP.CH1_UPDATED };
        APP.ws.send(JSON.stringify({ parameters: local }));
    });

    $("#ch2_setup").on("change input", function() {
        APP.CH2_UPDATED = true;

        // sends current Channel 2 update to backend
        var local = {};
        local['CH2_UPDATED'] = { value: APP.CH2_UPDATED };
        APP.ws.send(JSON.stringify({ parameters: local }));
    });
	
    // Parameters change Ch1
    $("#frequency_set_ch1").on("change input", function() {
        APP.setFrequencyCh1();
    });
    $("#frequency_unit_set_ch1").on("change input", function() {
        APP.setFrequencyCh1();
    });
    $("#amplitude_setup_ch1").on("change input", function() {
        APP.setAmplitudeCh1();
    });
    $("#offset_setup_ch1").on("change input", function() {
        APP.setOffsetCh1();
    });
    $("#waveform_set_ch1").on("change input", function() {
        APP.setWaveformCh1();
    });

    // program checks if ch1_state switch was clicked
    $('#flexSwitchCh1State').click(function() { 
        // changes local laser state
        if (APP.ch1_state == true){
            $('#ch1_on').hide();
            $('#ch1_off').show();
            APP.ch1_state = false;
        }
        else{
            $('#ch1_off').hide();
            $('#ch1_on').show();
            APP.ch1_state = true;
        }

        // sends current Channel 1 state to backend
        var local = {};
        local['CH1_STATE'] = { value: APP.ch1_state };
        APP.ws.send(JSON.stringify({ parameters: local }));
    });

    // Parameters change Ch2
    $("#frequency_set_ch2").on("change input", function() {
        APP.setFrequencyCh2();
    });
    $("#frequency_unit_set_ch2").on("change input", function() {
        APP.setFrequencyCh2();
    });
    $("#amplitude_setup_ch2").on("change input", function() {
        APP.setAmplitudeCh2();
    });
    $("#offset_setup_ch2").on("change input", function() {
        APP.setOffsetCh2();
    });
    $("#waveform_set_ch2").on("change input", function() {
        APP.setWaveformCh2();
    });

    // program checks if ch2_state switch was clicked
    $('#flexSwitchCh2State').click(function() { 
        // changes local laser state
        if (APP.ch2_state == true){
            $('#ch2_on').hide();
            $('#ch2_off').show();
            APP.ch2_state = false;
        }
        else{
            $('#ch2_off').hide();
            $('#ch2_on').show();
            APP.ch2_state = true;
        }

        // sends current Channel 2 state to backend
        var local = {};
        local['CH2_STATE'] = { value: APP.ch2_state };
        APP.ws.send(JSON.stringify({ parameters: local }));
    });


   
    // Start application
    APP.startApp();
});
