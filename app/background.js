import React from 'react';
import ReactDOM from 'react-dom';

import Cursor from 'cursor';
import Atom from 'atom';

class Streamer {
  constructor(cursor) {
    this.$frames = cursor.select('frames');
    this.$paused = cursor.select('paused');
    this.$started = cursor.select('started');
  }

  listen() {
    this.capture = document.getElementById('capture');
    this.capture.addEventListener('message', e => this.onCaptureMessage(e));
    this.$paused.listen(this.onPausedChange.bind(this));
    this.$started.listen(this.onStartedChange.bind(this));
  }

  onCaptureMessage({data}) {
    if (data.name == 'frame') {
      this.$frames.push(data.timestamp)
    }
  }

  onPausedChange(paused) {
    this.capture.postMessage({command: paused ? "pause" : "resume"});
  }

  onStartedChange(started) {
    if (started) {
      this.pickScreen()
          .then(this.getStream.bind(this))
          .then(this.configureStream.bind(this))
          .then(this.startCapture.bind(this));
    } else {
      this.capture.postMessage({command: "stop"});
    }
  }

  pickScreen() {
    return new Promise((resolve, reject) => {
      return chrome.desktopCapture.chooseDesktopMedia(["screen"], resolve);
    });
  }

  getStream(mediaSourceId) {
    return new Promise((resolve, reject) => {
      return navigator.webkitGetUserMedia({
        audio: false,
        video: {
          mandatory: {
            chromeMediaSource: 'desktop',
            chromeMediaSourceId: mediaSourceId
          }
        }
      }, resolve, reject);
    });
  }

  configureStream(stream) {
    return new Promise((resolve, reject) => {
      this.capture.postMessage({
        command: "configure",
        track: stream.getVideoTracks()[0],
        height: 1024,
        width: 768
      });
      resolve(stream);
    })
  }

  startCapture(stream) {
    stream.addEventListener("ended", e => this.$started.set(false));
    this.$started.listen(started => stream.getVideoTracks()[0].end());
    this.capture.postMessage({command: "start"});
  }
}

class Background {
  constructor() {
    this.source = `context.${Date.now()}`;
    this.atom = new Atom(Background.initialState, this.onAtomChange.bind(this));
    this.$loaded = new Cursor(this.atom).select('loaded');
    this.$frames = new Cursor(this.atom).select('frames');
    this.streamer = new Streamer(new Cursor(this.atom));
  }

  getState() {
    return this.atom.deref();
  }

  listen() {
    chrome.runtime.onMessage.addListener(this.onChromeMessage.bind(this));
    this.streamer.listen();
  }

  onAtomChange(state, previous) {
    chrome.runtime.sendMessage({source: this.source, state});
  }

  onChromeMessage({source, state}) {
    if (this.source == source) { return; }
    this.atom.replace(state);
  }
}

Background.initialState = {
  frames: [],
  paused: true,
  started: false,
  loaded: false
};

window.background = new Background();

document.body.addEventListener('load', e => window.background.listen(), true);
document.body.innerHTML = '<embed id="capture" src="native_manifest.nmf" type="application/x-pnacl"></embed>';
