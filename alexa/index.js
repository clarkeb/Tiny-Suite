'use strict';

var Alexa = require('alexa-sdk');
var APP_ID = undefined; //OPTIONAL: replace with 'amzn1.echo-sdk-ams.app.[your-unique-value-here]';
var SKILL_NAME = 'Tiny Suite';
var http = require('http');

exports.handler = function(event, context, callback) {
  var alexa = Alexa.handler(event, context);
  alexa.APP_ID = APP_ID;
  alexa.registerHandlers(handlers);
  alexa.execute();
};

var handlers = {
  'NewSession': function () {
    this.emit('QueryIntent');
  },
  'QueryIntent': function () {
    try {
      var dataType;
      var dataSlot = this.event.request.intent.slots.QueryType;
      if (dataSlot && dataSlot.value) {
        dataType = dataSlot.value.toLowerCase();
      }
    } catch (e) {
      console.log(e);
      var speechOutput = 'Looks like an invocation error ' + e + ' You can ask questions such as, what\'s the temperature in my home, or, you can say exit... '
      var repromptSpeech = 'What else can I help with?';
      speechOutput += repromptSpeech;

      this.attributes['speechOutput'] = speechOutput;
      this.attributes['repromptSpeech'] = repromptSpeech;

      this.emit(':ask', speechOutput, repromptSpeech);
      return;
    }
    var _this = this;
    switch (dataType) {
      case 'gas concentration':
      case 'gas':
      getData('gas', function(response) {
        _this.attributes['speechOutput'] = response;
        _this.emit(':tell', response);
      });
      break;
      case 'temperature':
      getData('temperature', function(response) {
        _this.attributes['speechOutput'] = response;
        _this.emit(':tell', response);
      });
      break;
      case 'humidity':
      getData('humidity', function(response) {
        _this.attributes['speechOutput'] = response;
        _this.emit(':tell', response);
      });
      break;
      case 'motion':
      getData('motion', function(response) {
        _this.attributes['speechOutput'] = response;
        _this.emit(':tell', response);
      });
      break;
      case 'air quality':
      getData('air', function(response) {
        _this.attributes['speechOutput'] = response;
        _this.emit(':tell', response);
      });
      break;
      default:
        var speechOutput = 'This does not appear to be a valid request ';
        var repromptSpeech = 'What else can I help with?';
        speechOutput += repromptSpeech;
        this.attributes['speechOutput'] = speechOutput;
        this.attributes['repromptSpeech'] = repromptSpeech;
        this.emit(':ask', speechOutput, repromptSpeech);
      break;
    }
  },
  'AMAZON.HelpIntent': function () {
    this.attributes['speechOutput'] = 'You can ask questions such as, what\'s the temperature in my home, or, you can say exit... ' +
    'Now, what can I help you with?';
    this.attributes['repromptSpeech'] = 'You can say things like, what\'s the temperature in my home, or you can say exit...' +
    ' Now, what can I help you with?';
    this.emit(':ask', this.attributes['speechOutput'], this.attributes['repromptSpeech'])
  },
  'AMAZON.RepeatIntent': function () {
    this.emit(':ask', this.attributes['speechOutput'], this.attributes['repromptSpeech'])
  },
  'AMAZON.StopIntent': function () {
    this.emit('SessionEndedRequest');
  },
  'AMAZON.CancelIntent': function () {
    this.emit('SessionEndedRequest');
  },
  'SessionEndedRequest':function () {
    this.emit(':tell', 'Goodbye!');
  }
};

function getData(type, callback) {
  var ubidots = 'http://things.ubidots.com/api/v1.6/variables?token=JHsMUIu6LR6uCnWRl7uqqe8hmVAmwi&format=json'
  http.get(ubidots, function(res) {
    var buffer = "";
    res.on("data", function(data){
      buffer += data;
    });
    res.on("end", function() {
      var results = JSON.parse(buffer.toString("ascii")).results;
      var itemFound = false;
      if (!results) {
        callback("Unknown Error");
        return;
      }
      switch (type) {
        case 'gas':
          var name = 'gas vs. air ratio'
        break;
        case 'temperature':
          var name = 'temperature'
        break;
        case 'humidity':
          var name = 'humidity'
        break;
        case 'motion':
          var name = 'motion'
        break;
        case 'air':
          var name = 'gas vs. air ratio'
        break;
        default:
          callback("Invalid Type");
          return;
        break;
      }
      for (var i in results) {
        if (results[i].name === name) {
          itemFound = true;
          var dataValue = results[i].last_value.value;
          switch (type) {
            case 'gas':
              callback("The gas to air ratio in your home is currently " + dataValue + " which is a " + ((dataValue  > 5 && dataValue < 11) ? 'good': 'bad') + ' ratio');
            break;
            case 'temperature':
              callback("The temperature in your home is currently " + dataValue + " degrees fahrenheit");
            break;
            case 'humidity':
              callback("The humidity in your home is currently at " + dataValue + " percent");
            break;
            case 'motion':
              callback("There is currently " + (dataValue === 0 ? 'no' : '') + " motion in your home");
            break;
            case 'air':
              callback("The air quality in your home appears to be at a " + ((dataValue  > 5 && dataValue < 11) ? 'good': 'bad') + ' level');
            break;
            default:
              callback("Unknown error")
            break;
          }
        }
      }
      if (!itemFound){
        callback("Not Found");
      }
  });
});
}
