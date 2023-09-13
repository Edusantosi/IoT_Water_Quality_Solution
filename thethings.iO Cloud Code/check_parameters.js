// Define filters for data retrieval
const filters = {
  limit: 2
};

// Define minimum and maximum threshold values for different parameters
const thresholds = {
  pH: {
    max: 8,
    min: 5
  },
  od: {
    max: 6.98,
    min: 2.89
  },
  tds: {
    max: 350,
    min: 76
  },
  temp: {
    max: 28,
    min: 24.12
  },
  wTemp: {
    max: 29,
    min: 23
  },
  hum: {
    max: 86,
    min: 67
  }
};

// Main function that processes data and sends notifications if thresholds are exceeded
function main(param, callback) {
  var promise = new Promise(function(resolve, reject) {
    // Read the last two values from the device using thethingsAPI
    thethingsAPI.thingRead(param.thingToken, param.key, filters, function(error, result) {
      if (error) return callback(error);
      var mssg_body = '';
      var status = '';
      var sendFlag = false;

      // Check if the current value exceeds the maximum threshold but the previous value doesn't
      if (result[0].value > thresholds[param.key].max && result[1].value <= thresholds[param.key].max) {
        console.log(param.key + " level increased above the maximum threshold.");
        mssg_body += param.key + ' ALERT: Level just surpassed the maximum threshold of ' + thresholds[param.key].max;
        status += 'ABOVE THRESHOLD';
        sendFlag = true;
      }
      // Check if the current value falls below the minimum threshold but the previous value doesn't
      else if (result[0].value < thresholds[param.key].min && result[1].value >= thresholds[param.key].min) {
        console.log(param.key + " level decreased below the minimum threshold.");
        mssg_body += param.key + ' ALERT: Level just fell below the minimum threshold of ' + thresholds[param.key].min;
        status += 'BELOW THRESHOLD';
        sendFlag = true;
      }
      // Check if the current value is inside the threshold range, but the previous value doesn't
      else if (
        (result[1].value < thresholds[param.key].min || result[1].value > thresholds[param.key].max) &&
        (result[0].value >= thresholds[param.key].min && result[0].value <= thresholds[param.key].max)
      ) {
        console.log(param.key + " level returned to normal values.");
        mssg_body += param.key + ' NOTIFICATION: Level returned to normal values';
        status += 'IN RANGE';
        sendFlag = true;
      }

      // If a notification should be sent, send it
      if (sendFlag === true) {
        sendNotification(mssg_body, callback);

        // Update the status value for the parameter
        let new_value = {
          values: [
            {
              key: param.key + '_status',
              value: status
            }
          ]
        };
        thethingsAPI.thingWrite(param.thingToken, new_value, null);
      }
      return resolve();
    });
  });

  promise
    .then(function() {
      // Stop the execution of the trigger by calling the callback function passed by parameter
      callback();
    })
    .catch(function(error) {
      // Stop the execution of the trigger by calling the callback function passed by parameter
      callback(error);
    });

  // callback();
}

// Function to send notifications (Twilio integration)
function sendNotification(mssg_body, callback) {
  var twilioClient = new Twilio('TWILIO ACCOUNT SID', 'TWILIO AUTH TOKEN');
  var messageConfig = {
    to: '+34XXXXXXXXX',
    from: '+17622254188', //Twilio virtual phone
    body: mssg_body
  };

  // Send an SMS text message using Twilio
  twilioClient.sendMessage(messageConfig, callback);
}
