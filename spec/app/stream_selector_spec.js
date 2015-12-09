import StreamSelector from '../../app/stream_selector';
import jsdomify from 'jsdomify';

describe('StreamSelector', function() {
  var selector;

  beforeEach(function() {
    jsdomify.create('<!DOCTYPE html><html><head></head><body><div id="container"></div></body></html>');
    selector = new StreamSelector();
    chrome.desktopCapture.chooseDesktopMedia = (_, callback) => { callback("id"); }
  });

  describe('.getStream', function() {
    beforeEach(function() {
      navigator.webkitGetUserMedia = (constraints, resolve) => {
        resolve("stream" + constraints.video.mandatory.chromeMediaSourceId);
      }
    });

    it('retrieves the stream', function(done) {
      StreamSelector.getStream().then((stream) => {
        expect(stream).toEqual("streamid");
        done();
      });
    });
  });

  describe("#getSource", function(){
    it('requests the desktop as a video stream', function(){
      spyOn(chrome.desktopCapture, 'chooseDesktopMedia');
      expect(selector.getSource()).toBeDefined();
      expect(chrome.desktopCapture.chooseDesktopMedia).toHaveBeenCalledWith(
        ["screen"], jasmine.any(Function));
    });

    describe('when the promise is resolved', function() {
      it('gets the source id for the given source', function(done){
        selector.getSource().then((id) => {
          expect(id).toEqual("id");
          done();
        });
      });
    });
  });

  describe("#resolveStream", function(){
    it('configures the webkit user media stream', function(){
      navigator.webkitGetUserMedia = jasmine.createSpy("webkitGetUserMedia");
      expect(selector.resolveStream('flying-fish')).toBeDefined();
      expect(navigator.webkitGetUserMedia).toHaveBeenCalledWith({
        audio: false,
        video: {
          mandatory: {
            chromeMediaSource: 'desktop',
            chromeMediaSourceId: 'flying-fish'
          }
        }
      },jasmine.any(Function), jasmine.any(Function))
    });

    describe('when the promise is resolved', function() {
      beforeEach(function() {
        navigator.webkitGetUserMedia = (constraints, resolve) => {
          resolve("stream" + constraints.video.mandatory.chromeMediaSourceId);
        }
      });

      it('gets user media for the provided id', function(done){
        selector.resolveStream('ed').then((stream) => {
          expect(stream).toEqual('streamed')
          done();
        });
      });
    });

    describe('when the promise is rejected', function() {
      beforeEach(function() {
        navigator.webkitGetUserMedia = (constraints, _, reject) => {
          reject("stream" + constraints.video.mandatory.chromeMediaSourceId);
        }
      });

      it('gets user media for the provided id', function(done){
        selector.resolveStream('ed').catch((stream) => {
          expect(stream).toEqual('streamed')
          done();
        });
      });
    });
  })
});
