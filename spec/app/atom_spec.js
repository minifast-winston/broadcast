import Atom from '../../app/atom'

describe('Atom', function() {
  var atom, data, listener;

  beforeEach(function() {
    data = {};
    listener = jasmine.createSpy("listener");
    atom = new Atom(data);
  });

  describe('#deref', function() {
    it('returns the data', function() {
      expect(atom.deref()).toBe(data)
    });
  });

  describe('#replace', function() {
    var newData;

    beforeEach(function() {
      newData = {datums: 'newdata'};
    });

    it('changes the value', function() {
      atom.replace(newData)
      expect(atom.deref()).toBe(newData)
    });

    describe('when the atom is fed a listener', function() {
      beforeEach(function() {
        atom.onReplace(listener);
      });

      it('calls the listener', function(){
        atom.replace(newData);
        expect(listener).toHaveBeenCalledWith(newData, data);
      });
    });
  });

  describe('#update', function() {
    it('calls the callback with the command', function() {
      atom.onCommand(listener)
      atom.update({taco: {$set: 'pork'}});
      expect(listener).toHaveBeenCalledWith({taco: {$set: 'pork'}});
    });

    it('calls the callback with the update', function() {
      atom.onReplace(listener)
      atom.update({taco: {$set: 'pork'}});
      expect(listener).toHaveBeenCalledWith({taco: 'pork'}, {});
    });
  });
});
