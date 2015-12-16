import Uploader from '../../app/uploader';
import Atom from '../../app/atom';
import Cursor from '../../app/cursor';

describe('Uploader', function() {
  var uploader, atom, fakeBucket;

  beforeEach(function() {
    atom = new Atom({size: 0});
    fakeBucket = {upload: jasmine.createSpy('upload')};
    uploader = new Uploader(atom, fakeBucket);
  });

  describe('#push', function() {
    it('sets the size to the sum of the concatenated buffer sizes', function() {
      var buffer = new ArrayBuffer(1);
      uploader.push(buffer);
      expect(atom.deref().size).toEqual(1);
      uploader.push(buffer);
      expect(atom.deref().size).toEqual(2);
    });
  });

  describe('#reset', function() {
    it('sets the size to the size of the empty buffer', function() {
      var buffer = new ArrayBuffer(1);
      uploader.push(buffer);
      uploader.reset();
      expect(atom.deref().size).toEqual(0);
    });
  });

  describe('#upload', function() {
    it('uploads the buffer', function(){
      uploader.upload();
      expect(fakeBucket.upload.calls.mostRecent().args[0].Body).toEqual(jasmine.any(ArrayBuffer));
    });

    describe('when the upload is successful', function() {
      beforeEach(function() {
        fakeBucket.upload.and.callFake((params, callback) => {
          callback(null, 'success');
        });
      });

      it('resolves the promise', function(done) {
        uploader.upload().then((result) => {
          expect(result).toEqual('success');
          done();
        });
      });
    });

    describe('when the upload fails', function() {
      beforeEach(function() {
        fakeBucket.upload.and.callFake((params, callback) => {
          callback('nope');
        });
      });

      it('rejects the promise', function(done) {
        uploader.upload().catch((result) => {
          expect(result).toEqual('nope');
          done();
        });
      });
    });
  });
});
