
class Watch {
  constructor(path, listener) {
    this.path = path;
    this.listener = listener;
  }

  onData(newData, oldData) {
    let newValue = this.path.reduce((memo, key) => memo[key], newData),
        oldValue = this.path.reduce((memo, key) => memo[key], oldData);
    if (newValue !== oldValue) {
      this.listener.call(this, newValue, oldValue);
    }
  }
}

class Cursor {
  constructor(atom, path = []) {
    this.atom = atom;
    this.path = path;
  }

  listen(listener) {
    let watch = new Watch(this.path, listener);
    this.atom.onReplace(watch.onData.bind(watch));
  }

  get(...keys) {
    return this.path.concat(keys).reduce((memo, key) => memo[key], this.atom.deref());
  }

  select(...keys) {
    return new Cursor(this.atom, this.path.concat(keys));
  }

  apply(transducer) {
    this.update({$apply: transducer});
  }

  set(value) {
    this.update({$set: value});
  }

  push(value) {
    if (!Array.isArray(value)) { value = [value]; }
    this.update({$push: value})
  }

  update(command) {
    let nestedCommand = this.path.reduceRight((memo, key) => ({[key]: memo}), command);
    this.atom.update(nestedCommand);
  }
}

export default Cursor;
