import Cursor from './cursor';
import Atom from './atom';
import StreamSelector from './stream_selector';
import Uploader from './uploader';

class Capturer {
  constructor(atom, capture, internalAtom = new Atom({video: null}), uploader = new Uploader(atom)) {
    this.$video = new Cursor(internalAtom).select('video');
    this.$frames = new Cursor(atom).select('frames');
    this.$capturing = new Cursor(atom).select('capturing');
    this.$requested = new Cursor(atom).select('requested');
    this.capture = capture;
    this.uploader = uploader;
  }

  listen() {
    this.capture.addEventListener('message', e => this.onCaptureMessage(e));
    this.$capturing.listen(on => on ? this.onResume() : this.onPause());
    this.$requested.listen(on => on ? this.onCapture() : this.onEnd());
    this.$video.listen(this.onVideoStream.bind(this));
  }

  onCaptureMessage({data}) {
    if (data.name == 'frame') {
      this.uploader.push(data.data);
      this.$frames.push(data.timestamp)
    }
  }

  onResume() {
    this.capture.postMessage({command: "resume"});
  }

  onPause() {
    this.capture.postMessage({command: "pause"});
  }

  onCapture() {
    return StreamSelector.getStream()
                         .then(e => this.$video.set(e))
                         .catch(f => this.$requested.set(false));
  }

  onEnd() {
    this.$capturing.set(false);
    this.$video.set(null);
    this.$frames.set([]);
    return this.uploader.upload()
                        .then(this.uploader.reset.bind(this.uploader))
                        .catch(error => console.error('error uploading:', error));
  }

  onVideoStream(stream, oldStream) {
    if (stream === null) {
      this.onVideoStreamStop(oldStream);
    } else {
      this.onVideoStreamStart(stream);
    }
  }

  onVideoStreamStop(stream) {
    this.capture.postMessage({command: "stop"});
    stream.getVideoTracks()[0].stop();
    this.$requested.set(false);
  }

  onVideoStreamStart(stream) {
    stream.addEventListener("ended", () => this.$video.set(null));
    this.capture.postMessage({
      command: "configure",
      track: stream.getVideoTracks()[0],
      width: 1280,
      height: 720
    });
  }
}

export default Capturer;
