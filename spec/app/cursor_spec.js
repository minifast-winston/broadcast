import Cursor from '../../app/cursor';
import Atom from '../../app/atom';
import deepFreeze from 'deep-freeze';

describe('Cursor', function() {
  var cursor, data, atom, callback;

  beforeEach(function() {
    callback = jasmine.createSpy("callback");
    data = {taco: "chicken", beans: ['black', 'pinto']};
    deepFreeze(data);
    atom = new Atom(data, callback);
    cursor = new Cursor(atom);
  });

  describe('#get', function() {
    it("returns the value for a key", function(){
      expect(cursor.get('taco')).toEqual('chicken');
    });
    it("returns the nth value for an array", function(){
      expect(cursor.get('beans', 1)).toEqual('pinto');
    });
  });

  describe('#select', function() {
    it("returns a cursor rooted at the key", function(){
      expect(cursor.select('taco') instanceof Cursor).toBe(true);
      expect(cursor.select('taco').get()).toEqual("chicken");
    });
    it("returns a cursor rooted at the nth key", function(){
      expect(cursor.select('beans', 1).get()).toEqual("pinto");
      expect(cursor.select('beans').select(1).get()).toEqual("pinto");
    });
  });

  describe('#apply', function() {
    it('should apply the function to the data in the context of the cursor', function() {
      cursor.select('taco').apply(tacoType => tacoType + 1);
      expect(cursor.select('taco').get()).toEqual("chicken1");
    });
  });

  describe('#set', function() {
    it('sets the data at the context of the cursor', function() {
      cursor.select('taco').set('fish');
      expect(cursor.select('taco').get()).toEqual("fish");
    });
  });

  describe('#push', function() {
    describe('when the value is an array', function() {
      it('concatenates the value to the array at the cursor context', function() {
        cursor.select('beans').push(['refried', 'suspicious']);
        expect(cursor.select('beans').get()).toEqual([
          'black', 'pinto', 'refried', 'suspicious'
        ]);
      });
    });

    describe('when the value is not an array', function() {
      it('concatenates the value to the array at the cursor context', function() {
        cursor.select('beans').push('refried');
        expect(cursor.select('beans').get()).toEqual([
          'black', 'pinto', 'refried'
        ]);
      });
    });
  });

  describe('#update', function() {
    it('calls the callback', function() {
      cursor.update({taco: {$set: 'pork'}});
      expect(callback).toHaveBeenCalledWith(
        {taco: "pork", beans: ['black', 'pinto']},
        data
      );
    });

    describe('on a selected cursor', function() {
      it('calls the callback', function() {
        cursor.select('beans').update({$push: ['refried']});
        expect(callback).toHaveBeenCalledWith(
          {taco: "chicken", beans: ['black', 'pinto', 'refried']},
          data
        );
      });
    });
  });

  describe('#listen', function() {
    var listener, subCursor;

    beforeEach(function() {
      subCursor = cursor.select('taco');
      listener = jasmine.createSpy("callback");
      subCursor.listen(listener);
    });

    describe('when the data at the cursor context is updated', function(){
      it('calls the listener with those values', function(){
        subCursor.set('fish');
        expect(listener).toHaveBeenCalledWith('fish', 'chicken');
      });
    });

    describe('when the data at the cursor context is not substantially updated', function(){
      it('does not call the listener', function(){
        subCursor.set('chicken');
        expect(listener).not.toHaveBeenCalled();
      });
    });
  });
});
