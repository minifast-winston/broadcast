global.backgroundPage = {};

global.chrome = {
  desktopCapture: {
    chooseDesktopMedia: function(media, callback) {
      callback("id");
    }
  },
  runtime: {
    getBackgroundPage: function(callback){
      callback(global.backgroundPage);
    },
    listeners: [],
    onMessage: {
      addListener: function(listener){
        chrome.runtime.listeners.push(listener)
      },
      removeListener: function(callback){
        chrome.runtime.listeners = chrome.runtime.listeners.filter(listener => listener !== callback);
      }
    },
    sendMessage: function(message) {
      chrome.runtime.listeners.forEach(listener => listener(message));
    }
  }
};
