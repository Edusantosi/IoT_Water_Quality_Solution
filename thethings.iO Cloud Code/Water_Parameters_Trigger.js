function trigger(params, callback) {
  // Trigger handle only when we write
  if (params.action == 'write') {
    var values = params.values;
  	for (var i = 0; i < values.length; ++i) {
  		if (values[i].key == "pH") {
          var param = { key: values[i].key, thingToken: params.thingToken };
          thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
     	else if (values[i].key == "od") {
          var param = { key: values[i].key, thingToken: params.thingToken };
          thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
     	else if (values[i].key == "tds") {
          var param = { key: values[i].key, thingToken: params.thingToken };
          thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
      	 else if (values[i].key == "wTemp") {
           var param = { key: values[i].key, thingToken: params.thingToken };
           thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
      	else if (values[i].key == "temp") {
          var param = { key: values[i].key, thingToken: params.thingToken };
          thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
      	else if (values[i].key == "hum") {
          var param = { key: values[i].key, thingToken: params.thingToken };
          thethingsAPI.cloudFunction('check_parameters', param, callback);
  		}
  	}

  }
  callback();
  }
