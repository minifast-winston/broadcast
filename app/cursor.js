import updateReact from 'react-addons-update';

class Cursor {
  constructor(atom, path = []) {
    this.atom = atom;
    this.path = path;
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
    let newData = updateReact(this.atom.deref(), nestedCommand);
    this.atom.replace(newData);
  }
}

export default Cursor;
