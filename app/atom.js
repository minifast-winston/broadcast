class Atom {
  constructor(data, listener) {
    this.data = data;
    this.listener = listener;
  }

  deref() {
    return this.data;
  }

  replace(newData) {
    var oldData = this.data;
    this.data = newData;
    this.listener(newData, oldData);
  }
}

export default Atom;
