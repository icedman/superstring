import NativeMarkerIndex from '../../build/Release/marker_index'

NativeMarkerIndex.prototype.getRange = function (id) {
  return [this.getStart(id), this.getEnd(id)]
}

NativeMarkerIndex.prototype.findIntersecting = function (start, end) {
  return new Set(this._findIntersecting(start, end))
}

NativeMarkerIndex.prototype.findContaining = function (start, end) {
  return new Set(this._findContaining(start, end))
}

NativeMarkerIndex.prototype.findContainedIn = function (start, end) {
  return new Set(this._findContainedIn(start, end))
}

export default NativeMarkerIndex
