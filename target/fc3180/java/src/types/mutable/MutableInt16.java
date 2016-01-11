package org.alljoyn.bus.types.mutable;

/**
* Created by Henrik on 17.11.2014.
*/
public final class MutableInt16 extends Mutable {
  public short value;

  public MutableInt16() { }

  public MutableInt16(short value) {
    this.value = value;
  }
}
