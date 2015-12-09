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

  onMessage: function({source, state}) {
    if (this.props.source === source) { return; }
    this.setState(state);
  },

  onAtomChange: function(state) {
    chrome.runtime.sendMessage({source: this.props.source, state});
    this.setState(state);
  },

  render: function() {
    let cursor = new Cursor(new Atom(this.state, this.onAtomChange));
    return (<div>
      {React.Children.map(this.props.children, child => React.cloneElement(child, {cursor}))}
    </div>);
  }
});

export default BackgroundContext;
