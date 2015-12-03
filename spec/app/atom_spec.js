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

    it('calls the listener', function(){
      atom.replace(newData);
      expect(listener).toHaveBeenCalledWith(newData, data);
    });
  });
});
