import Cursor from 'cursor';
import Atom from 'atom';
import Capturer from 'capturer';

class Background {
  constructor(atom, capture) {
    this.atom = atom;
    this.source = `context.${Date.now()}`;
    this.cursor = new Cursor(this.atom);
  }

  listen() {
    this.atom.listen(this.onAtomChange.bind(this));
    chrome.runtime.onMessage.addListener(this.onChromeMessage.bind(this));
  }

  onAtomChange(state, previous) {
    chrome.runtime.sendMessage({source: this.source, state});
  }

  onChromeMessage({source, state}) {
    if (this.source == source) { return; }
    this.atom.replace(state);
  }
}

let background = new Background(window.atom);
background.listen();

window.atom = new Atom({
  frames: [],
  capturing: false,
  requested: false
});

new Capturer(window.atom, document.getElementById('capture')).listen();
