// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

var _callbacks = {};
var _next_reply_id = 0;
var _listeners = {};
var _next_listener_id = 0;

var Promise = function() {

  var State = {
    PENDING: 0,
    FULFILLED: 1,
    REJECTED: 2
  };

  var Promise = {
    state: State.PENDING,
    changeState: function( state, value ) {

      // catch changing to same state (perhaps trying to change the value)
      if ( this.state == state ) {
        throw new Error("can't transition to same state: " + state);
      }

      // trying to change out of fulfilled or rejected
      if ( this.state == State.FULFILLED || this.state == State.REJECTED ) {
        throw new Error("can't transition from current state: " + state);
      }

      // if second argument isn't given at all (passing undefined allowed)
      if ( state == State.FULFILLED && arguments.length < 2 ) {
        throw new Error("transition to fulfilled must have a non null value");
      }

      // if a null reason is passed in
      if ( state == State.REJECTED && value == null ) {
        throw new Error("transition to rejected must have a non null reason");
      }

      //change state
      this.state = state;
      this.value = value;
      this.resolve();
      return this.state;
    },
    fulfill: function( value ) {
      this.changeState( State.FULFILLED, value );
    },
    reject: function( reason ) {
      this.changeState( State.REJECTED, reason );
    },
    then: function( onFulfilled, onRejected ) {
      // initialize array
      this.cache = this.cache || [];
      var promise = Object.create(Promise);
      var that = this;

      this.async( function() {
        that.cache.push({
          fulfill: onFulfilled,
          reject: onRejected,
          promise: promise
        });
        that.resolve();
      });
      return promise;
    },
    resolve: function() {
      // check if pending
      if ( this.state == State.PENDING ) {
        return false;
      }

      // for each 'then'
      while ( this.cache && this.cache.length ) {
        var obj = this.cache.shift();
        var fn = this.state == State.FULFILLED ? obj.fulfill : obj.reject;

        if ( typeof fn != 'function' ) {
          obj.promise.changeState( this.state, this.value );
        } else {
          try { // fulfill promise with value or reject with error
            var value = fn( this.value );
            // deal with promise returned
            if ( value && typeof value.then == 'function' ) {
              value.then( function( value ) {
                obj.promise.changeState( State.FULFILLED, value );
                }, function( error ) {
                obj.promise.changeState( State.REJECTED, error );
              });
            } else { // deal with other value returned
              obj.promise.changeState( State.FULFILLED, value );
            }
          } catch (error) { // deal with error thrown
            obj.promise.changeState( State.REJECTED, error );
          }
        }
      }
    },
    async: function(fn) {
      setTimeout(fn, 5);
    }
  };
  return Object.create(Promise);
}; // End of Promise definition

var postMessage = function(msg) {
  var promiseGet = new Promise();
  var reply_id = _next_reply_id;
  _next_reply_id += 1;
  _callbacks[reply_id] = promiseGet;
  msg._reply_id = reply_id.toString();
  extension.postMessage(JSON.stringify(msg));
  return promiseGet;
};

exports.getCPUInfo = function() {
  var msg = {
    'cmd': 'getCPUInfo'
  };
  return postMessage(msg);
};

exports.getDisplayInfo = function() {
  var msg = {
    'cmd': 'getDisplayInfo'
  };
  return postMessage(msg);
};

exports.getMemoryInfo = function() {
  var msg = {
    'cmd': 'getMemoryInfo'
  };
  return postMessage(msg);
};

exports.getStorageInfo = function() {
  var msg = {
    'cmd': 'getStorageInfo'
  };
  return postMessage(msg);
};

function _addConstProperty(obj, propertyKey, propertyValue) {
  Object.defineProperty(obj, propertyKey, {
    configurable: false,
    writable: false,
    value: propertyValue
  });
}

function _createConstClone(obj) {
  var const_obj = {};
  for (var key in obj) {
    if (Array.isArray(obj[key])) {
      var obj_array = obj[key];
      var const_obj_array = [];
      for (var i = 0; i < obj_array.length; ++i) {
        var const_sub_obj = {};
        for (var sub_key in obj_array[i]) {
          _addConstProperty(const_sub_obj, sub_key, obj_array[i][sub_key]);
        }
        const_obj_array.push(const_sub_obj);
      }
      _addConstProperty(const_obj, key, const_obj_array);
    } else {
      _addConstProperty(const_obj, key, obj[key]);
    }
  }
  return const_obj;
}

extension.setMessageListener(function(json) {
  var msg = JSON.parse(json);

  if (msg.cmd == 'attachStorage' ||
      msg.cmd == 'detachStorage' ||
      msg.cmd == 'connectDisplay' ||
      msg.cmd == 'disconnectDisplay') {
      for (var id in _listeners) {
        if (_listeners[id]['eventName'] === msg.eventName) {
          _listeners[id]['callback'](_createConstClone(msg));
        }
      }
    return;
  }

  var reply_id = msg._reply_id;
  var promiseGet = _callbacks[reply_id];
  delete msg.reply_id;
  delete _callbacks[reply_id];
  // FIXME (qjia7) here need to check msg.message's value to detemine to call 'fulfill' or 'reject' 
  promiseGet.fulfill(msg);
});

var _hasListener = function(eventName) {
  var count = 0;

  for (var i in _listeners) {
    if (_listeners[i]['eventName'] === eventName) {
      count += 1;
    }
  }

  return (0 !== count);
};

exports.addEventListener = function(eventName, callback) {
  if (typeof eventName !== 'string') {
    console.log("Invalid parameters (*, -)!");
    return -1;
  }

  if (typeof callback !== 'function') {
    console.log("Invalid parameters (-, *)!");
    return -1;
  }

  if (!_hasListener(eventName)) {
    var msg = {
      'cmd': 'addEventListener',
      'eventName': eventName
    };
    extension.postMessage(JSON.stringify(msg));
  }

  var listener = {
    'eventName': eventName,
    'callback': callback
  };

  var listener_id = _next_listener_id;
  _next_listener_id += 1;
  _listeners[listener_id] = listener;

  return listener_id;
};

var _sendSyncMessage = function(msg) {
  return extension.internal.sendSyncMessage(JSON.stringify(msg));
};
