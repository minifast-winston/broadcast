import Cursor from './cursor';
import {BROADCAST_BUCKET, COGNITO_IDENTITY_POOL} from './env';
import 'aws-sdk/dist/aws-sdk';

const _private = new WeakMap();

class Uploader {
  constructor(atom, bucket = new AWS.S3({Bucket: BROADCAST_BUCKET})) {
    this.$size = new Cursor(atom).select('size');
    _private.set(this, {bucket, buffer: new ArrayBuffer(0)});
  }

  static configureAws() {
    if (typeof(window) === 'undefined') { return; }
    window.AWS.config.region = 'us-east-1';
    window.AWS.config.credentials = new window.AWS.CognitoIdentityCredentials({
      IdentityPoolId: COGNITO_IDENTITY_POOL
    });
  }

  push(newBuffer) {
    let {buffer} = _private.get(this);
    let array = new Uint8Array(buffer.byteLength + newBuffer.byteLength);
    array.set(new Uint8Array(buffer), 0);
    array.set(new Uint8Array(newBuffer), buffer.byteLength);
    _private.set(this, {buffer: array.buffer});
    this.$size.set(array.buffer.byteLength);
  }

  reset() {
    _private.set(this, {buffer: new ArrayBuffer(0)});
    this.$size.set(0);
  }

  upload() {
    let {bucket, buffer} = _private.get(this);
    let params = {Key: `context.${Date.now()}.vp8`, Body: buffer};
    return new Promise((resolve, reject) => {
      bucket.upload(params, function (error, data) {
        if (error) { reject(error); }
        else { resolve(data); }
      });
    });
  }
}

Uploader.configureAws();

export default Uploader;
