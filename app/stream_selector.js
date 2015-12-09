class StreamSelector {
  static getStream(){
    let selector = new StreamSelector();
    return selector.getSource().then(id => selector.resolveStream(id));
  }

  getSource() {
    return new Promise((resolve) => {
      chrome.desktopCapture.chooseDesktopMedia(["screen"], resolve)
    });
  }

  resolveStream(id){
    return new Promise((resolve, reject) => {
      navigator.webkitGetUserMedia({
        audio: false,
        video: {
          mandatory: {
            chromeMediaSource: 'desktop',
            chromeMediaSourceId: id
          }
        }
      }, resolve, reject);
    });
  }
}

export default StreamSelector;
