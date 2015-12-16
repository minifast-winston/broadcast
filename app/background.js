import Cursor from 'cursor';
import Atom from 'atom';
import Capturer from 'capturer';

class ChromeListener {
  constructor(atom) {
    this.atom = atom;
    this.source = `context.${Date.now()}`;
  }

  listen() {
    this.atom.onCommand(this.onAtomCommand.bind(this));
    chrome.runtime.onMessage.addListener(this.onChromeMessage.bind(this));
  }

  onAtomCommand(command) {
    chrome.runtime.sendMessage({source: this.source, command});
  }

  onChromeMessage({source, command}) {
    if (this.source == source) { return; }
    this.atom.update(command);
  }
}

window.atom = new Atom({
  frames: [],
  capturing: false,
  requested: false,
  size: 0
});

new ChromeListener(window.atom).listen();
new Capturer(window.atom, document.getElementById('capture')).listen();
