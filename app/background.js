import Cursor from 'cursor';
import Atom from 'atom';

class Background {
  constructor(ticker) {
    this.source = `context.${Date.now()}`;
    this.ticker = ticker;
    this.atom = new Atom(Background.initialState, this.onAtomChange.bind(this));
  }

  getState() {
    return this.atom.deref();
  }

  listen() {
    chrome.runtime.onMessage.addListener(this.onChromeMessage.bind(this));
    this.ticker.onmessage = this.onTickerMessage.bind(this);
  }

  onAtomChange(state, previous) {
    chrome.runtime.sendMessage({source: this.source, state});
    this.ticker.postMessage(new Cursor(this.atom).select('paused').get());
  }

  onTickerMessage({data}) {
    new Cursor(this.atom).select('frames').push(data);
  }

  onChromeMessage({source, state}) {
    if (this.source == source) { return; }
    this.atom.replace(state);
  }
}

Background.initialState = {
  frames: [],
  paused: true
};

var ticker = new Worker("ticker.bundle.js");
window.background = new Background(ticker);
window.background.listen();
