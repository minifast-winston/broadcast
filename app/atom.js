import updateReact from 'react-addons-update';

class Atom {
  constructor(data) {
    this.data = data;
    this.commandListeners = [];
    this.replaceListeners = [];
  }

  onCommand(listener) {
    if (listener) this.commandListeners.push(listener);
  }

  onReplace(listener) {
    if (listener) this.replaceListeners.push(listener);
  }

  deref() {
    return this.data;
  }

  replace(newData) {
    var oldData = this.data;
    this.data = newData;
    this.replaceListeners.forEach(listener => listener(newData, oldData));
  }

  update(command) {
    let newData = updateReact(this.data, command);
    this.commandListeners.forEach(listener => listener(command));
    this.replace(newData);
  }
}

export default Atom;
