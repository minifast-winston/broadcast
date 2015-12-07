class Ticker {
  constructor(postMessage, addEventListener) {
    this.postMessage = postMessage;
    this.addEventListener = addEventListener;
    this.paused = true;
    this.clock = null;
  }

  start(interval) {
    this.addEventListener('message', ({data}) => this.paused = data);
    this.clock = setInterval(()=> this.onTick(), interval);
  }

  onTick() {
    if (this.paused) { return; }
    this.postMessage(Date.now());
  }

  onMessage({data}) {
    this.paused = data;
  }
}

new Ticker(postMessage.bind(this), addEventListener.bind(this)).start(1000/15);
