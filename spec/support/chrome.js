global.backgroundPage = {};

global.chrome = {
  extension: {
    getBackgroundPage: function(){
      return global.backgroundPage;
    }
  },
  runtime: {
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
