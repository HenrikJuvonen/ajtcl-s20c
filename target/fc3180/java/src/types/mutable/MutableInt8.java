package org.alljoyn.bus.types.mutable;

/**
* Created by Henrik on 17.11.2014.
*/
public final class MutableInt8 extends Mutable {
  public byte value;

  public MutableInt8() { }

  public MutableInt8(byte value) {
    this.value = value;
  }
  public MutableInt8(int value) {
    this.value = (byte) value;
  }
}
