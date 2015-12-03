var ReactTestUtils = require('react-addons-test-utils');

import BackgroundContext from '../../app/background_context';
import jsdomify from 'jsdomify';
let React, ReactDOM;

import Atom from '../../app/atom';
import Cursor from '../../app/cursor';

describe('BackgroundContext', function() {
  var subject, data;

  beforeEach(function() {
    jsdomify.create('<!DOCTYPE html><html><head></head><body><div id="container"></div></body></html>');
    React = require('react');
    ReactDOM = require('react-dom');

    data = {tacos: 'pescado'};
    backgroundPage = {background: {getState: function(){ return data }}};
  });

  afterEach(function(){
    jsdomify.destroy();
    chrome.runtime.listeners = [];
  });

  describe('when rendered with a child', function() {
    beforeEach(function() {
      let TacoRenderer = ({cursor}) => <div>{cursor.get('tacos')}</div>;
      let TacoOrderer = ({cursor}) => <button onClick={() => cursor.select('tacos').set('fish')}>Order Fish Taco</button>;
      ReactDOM.render(
        <BackgroundContext>
          <TacoRenderer />
          <TacoOrderer />
        </BackgroundContext>,
        document.getElementById('container')
      );
    });

    it('dumps the contents of the cursor', function() {
      expect(document.body.innerHTML).toContain('pescado');
    });

    describe('when the atom is changed remotely', function(){
      it('re-renders with the new data', function(){
        chrome.runtime.sendMessage({source: 'jasmine', state: {tacos: 'chicharrones'}});
        expect(document.body.innerHTML).toContain('chicharrones');
      });
    });

    describe('when the atom is locally', function(){
      var orderButton;

      beforeEach(function(){
        orderButton = document.querySelector("button");
      });

      it('re-renders with the new data', function(){
        ReactTestUtils.Simulate.click(orderButton);
        expect(document.body.innerHTML).toContain('fish');
      });

      it('sends a message back out to chrome', function(){
        spyOn(chrome.runtime, 'sendMessage');
        ReactTestUtils.Simulate.click(orderButton);
        expect(chrome.runtime.sendMessage).toHaveBeenCalled();
      });
    });

    describe('when the component is about to unmount', function() {
      it("does the job nicely", function(){
        spyOn(chrome.runtime.onMessage, 'removeListener');
        ReactDOM.render(<div>Gone!</div>, document.getElementById('container'));
        expect(chrome.runtime.onMessage.removeListener).toHaveBeenCalled();
      })
    });
  });
});
