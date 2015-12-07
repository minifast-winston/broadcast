class Atom {
  constructor(data, listener) {
    this.data = data;
    this.listeners = [];
    this.listen(listener);
  }

  deref() {
    return this.data;
  }

  replace(newData) {
    var oldData = this.data;
    this.data = newData;
    this.listeners.forEach(listener => listener(newData, oldData));
  }

  listen(listener) {
    if (listener) this.listeners.push(listener);
  }
}

export default Atom;
