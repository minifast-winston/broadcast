import jsdomify from 'jsdomify';

import Capturer from '../../app/capturer';
import StreamSelector from '../../app/stream_selector';
import Atom from '../../app/atom';

describe('Capturer', function() {
  var capturer, atom, internalAtom, captureDom, fakeS3;

  beforeEach(function() {
    jsdomify.create('<!DOCTYPE html><html><head></head><body><div id="container"></div></body></html>');
    atom = new Atom({requested: true, capturing: true, frames: []});
    internalAtom = new Atom({video: null});
    captureDom = {postMessage: jasmine.createSpy('postMessage')};
    fakeS3 = {upload: jasmine.createSpy('upload')};
    var s3Spy = jasmine.createSpy('s3').and.returnValue(fakeS3);
    var credentialsSpy = jasmine.createSpy('CognitoIdentityCredentials');
    global.window.AWS = {CognitoIdentityCredentials: credentialsSpy, S3: s3Spy};
    capturer = new Capturer(atom, captureDom, internalAtom);
  });

  describe('#onResume', function() {
    it('tells the capturer to resume', function() {
      capturer.onResume();
      expect(captureDom.postMessage).toHaveBeenCalledWith({command: "resume"});
    });
  });

  describe('#onCaptureMessage', function() {
    describe('when the message is a frame', function() {
      it('adds the timestamp', function() {
        capturer.onCaptureMessage({data: {name: 'frame', timestamp: 0, data: {byteLength: 0}}});
        expect(atom.deref().frames).toEqual([0]);
      });

      it('adds the data', function() {
        capturer.onCaptureMessage({data: {name: 'frame', timestamp: 0, data: {byteLength: 1}}});
        expect(atom.deref().size).toEqual(1);
      });
    });

    describe('when the message is not handled', function() {
      it('does nothing', function() {
        capturer.onCaptureMessage({data: {}});
        expect(atom.deref().frames).toEqual([]);
      });
    });
  });

  describe('#onPause', function() {
    it('tells the capturer to pause', function() {
      capturer.onPause();
      expect(captureDom.postMessage).toHaveBeenCalledWith({command: "pause"});
    });
  });

  describe('#onEnd', function() {
    beforeEach(function() {
      atom = new Atom({requested: true, capturing: true, frames: [1], size: 123});
      internalAtom = new Atom({video: 'action_films'});
      capturer = new Capturer(atom, captureDom, internalAtom);
      fakeS3.upload.and.callFake((params, callback) => callback());
    });

    it('stops capturing', function() {
      capturer.onEnd();
      expect(atom.deref().capturing).toBe(false);
    });

    it('unsets the video stream', function() {
      capturer.onEnd();
      expect(internalAtom.deref().video).toBe(null);
    });

    it('empties out the frames', function() {
      capturer.onEnd();
      expect(atom.deref().frames).toEqual([]);
    });

    it('uploads the data', function() {
      capturer.onEnd();
      expect(fakeS3.upload).toHaveBeenCalled();
    });

    it('resets the size', function(done) {
      capturer.onEnd();
      setTimeout(() => {
        expect(atom.deref().size).toEqual(0);
        done();
      }, 0);
    });
  });

  describe('#onCapture', function() {
    describe('when the stream is requested successfully', function() {
      beforeEach(function() {
        spyOn(StreamSelector, 'getStream').and.returnValue(
          new Promise(resolve => resolve('yep'))
        );
      });

      it('sets the stream id', function(done){
        capturer.onCapture();
        setTimeout(() => {
          expect(internalAtom.deref()).toEqual({video: 'yep'});
          done();
        }, 0);
      })
    });

    describe('when the stream is requested unsuccessfully', function() {
      beforeEach(function() {
        spyOn(StreamSelector, 'getStream').and.returnValue(
          new Promise((_, reject) => reject('nope'))
        );
      });

      it('un-requests the stream', function(done){
        capturer.onCapture();
        setTimeout(() => {
          expect(atom.deref().requested).toBe(false);
          done();
        }, 0);
      })
    });
  });

  describe('#onVideoStream', function() {
    var track, video;

    beforeEach(function() {
      track = {stop: jasmine.createSpy('stop')};
      video = {
        addEventListener: jasmine.createSpy('addEventListener'),
        getVideoTracks: function(){ return [track]; }
      };
    });

    describe('when the stream is unset', function() {
      var oldVideo;

      beforeEach(function() {
        oldVideo = video;
        video = null;
      });

      it('stops the capture', function() {
        capturer.onVideoStream(video, oldVideo);
        expect(captureDom.postMessage).toHaveBeenCalledWith({
          command: 'stop'
        });
      });

      it("stops the track", function(){
        capturer.onVideoStream(video, oldVideo);
        expect(track.stop).toHaveBeenCalled();
      });

      it("un-requests the video", function(){
        capturer.onVideoStream(video, oldVideo);
        expect(atom.deref().requested).toBe(false);
      });
    });

    describe('when the stream is set', function() {
      it('adds an event listener to the stream', function() {
        capturer.onVideoStream(video);
        expect(video.addEventListener).toHaveBeenCalledWith('ended', jasmine.any(Function));
      });

      it('configures the capture', function() {
        capturer.onVideoStream(video);
        expect(captureDom.postMessage).toHaveBeenCalledWith({
          command: 'configure',
          width: 1280,
          height: 720,
          track: track
        });
      });
    });
  });
});
