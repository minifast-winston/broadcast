import React from 'react';
import Cursor from './cursor';
import Atom from './atom';

const BackgroundContext = React.createClass({
  getDefaultProps: function() {
    return {
      source: `context.${Date.now()}`
    };
  },

  getInitialState: function() {
    return {};
  },

  componentDidMount: function() {
    chrome.runtime.onMessage.addListener(this.onMessage);
    chrome.runtime.getBackgroundPage(page => this.setState(page.atom.deref()));
  },

  componentWillUnmount: function(){
    chrome.runtime.onMessage.removeListener(this.onMessage);
  },

  onMessage: function({source, command}) {
    if (this.props.source === source) { return; }
    let atom = new Atom(this.state);
    atom.onReplace(this.onAtomReplace);
    atom.update(command);
  },

  onAtomReplace: function(state) {
    this.setState(state);
  },

  onAtomCommand: function(command) {
    chrome.runtime.sendMessage({source: this.props.source, command});
  },

  render: function() {
    let atom = new Atom(this.state);
    let cursor = new Cursor(atom);
    atom.onCommand(this.onAtomCommand);
    atom.onReplace(this.onAtomReplace);
    return (<div>
      {React.Children.map(this.props.children, child => React.cloneElement(child, {cursor}))}
    </div>);
  }
});

export default BackgroundContext;
