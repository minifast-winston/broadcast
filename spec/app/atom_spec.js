import Atom from '../../app/atom'

describe('Atom', function() {
  var atom, data, listener;

  beforeEach(function() {
    data = {};
    listener = jasmine.createSpy("listener");
    atom = new Atom(data, listener);
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

    describe('when the atom is created with a listener', function() {
      it('calls the listener', function(){
        atom.replace(newData);
        expect(listener).toHaveBeenCalledWith(newData, data);
      });
    });

    describe('when the atom is created without a listener', function() {
      beforeEach(function() {
        atom = new Atom(data);
      });

      it('has no way of calling the listener', function(){
        atom.replace(newData);
        expect(listener).not.toHaveBeenCalled();
      });

      describe('when the atom is fed a listener', function() {
        beforeEach(function() {
          atom.listen(listener);
        });

        it('calls the listener', function(){
          atom.replace(newData);
          expect(listener).toHaveBeenCalledWith(newData, data);
        });
      });
    });
  });
});
