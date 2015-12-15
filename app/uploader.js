import {BROADCAST_BUCKET, COGNITO_IDENTITY_POOL} from './env';
import 'aws-sdk/dist/aws-sdk';

class Uploader {
  constructor($size) {
    this.$size = $size;
    this.buffer = new ArrayBuffer(0);
    this.bucket = new window.AWS.S3({
      params: {Bucket: BROADCAST_BUCKET},
      region: 'us-east-1',
      credentials: new window.AWS.CognitoIdentityCredentials({
        IdentityPoolId: COGNITO_IDENTITY_POOL
      })
    });
  }

  push(buffer) {
    let tmp = new Uint8Array(this.buffer.byteLength + buffer.byteLength);
    tmp.set(new Uint8Array(this.buffer), 0);
    tmp.set(new Uint8Array(buffer), this.buffer.byteLength);
    this.set(tmp.buffer);
  }

  reset() {
    this.set(new ArrayBuffer(0));
  }

  set(buffer) {
    this.buffer = buffer;
    this.$size.set(this.buffer.byteLength);
  }

  upload() {
    let params = {Key: `context.${Date.now()}.vp8`, Body: this.buffer};
    return new Promise((resolve, reject) => {
      this.bucket.upload(params, function (err, data) {
        if (err) { reject(data); }
        else { resolve(data); }
      });
    });
  }
}

export default Uploader;
