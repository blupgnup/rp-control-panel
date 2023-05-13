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
	
	//LED state
    APP.led_state = false;




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


    // Processes newly received data for signals
    APP.processSignals = function(new_signals) {

        var pointArr = [];
        var data_1 = [];
        var data_2 = [];
        var data_3 = [];
        var data_4 = [];

        // Draw signals
        for (sig_name in new_signals) {
            
            // Ignore empty signals
            if (new_signals[sig_name].size == 0) continue;

            var points = [];
            for (var i = 0; i < new_signals[sig_name].size; i++) {
                    points.push([i, new_signals[sig_name].value[i]]);
            }

            pointArr.push(points);
        }

        var data_pool = [{
            data: pointArr[0],
            label: "Signal 1"
        }, {
            data: pointArr[1],
            label: "Signal 2"
        }, {
            data: pointArr[2],
            label: "Signal 3",
            yaxis: 2
        }, {
            data: pointArr[3],
            label: "Signal 4"
        }];

        //console.log(pointArr[0])

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
	// program checks if led_state switch was clicked
   $('#flexSwitchLedState').click(function() {

       // changes local led state
       if (APP.led_state == true){
           $('#led_on').hide();
           $('#led_off').show();
           APP.led_state = false;
       }
       else{
           $('#led_off').hide();
           $('#led_on').show();
           APP.led_state = true;
       }

       // sends current led state to backend
       var local = {};
       local['LED_STATE'] = { value: APP.led_state };
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
                yaxis: {
                    min: 0,
                    max: 35
                },
                y2axis: {
                    autoscale: true
                },
                xaxis: {
                    min: 0,
                    max: 1024,
                    show: false
                },
                xaxes: [ { } ],
                yaxes: [ { }, { position: "right", min: 20 } ]
    });
		
	
	// Amplitude change
    $("#amplitude_set").on("change input", function() {
        APP.setAmplitude(); 
    });
	
   
    // Start application
    APP.startApp();
});
