package org.alljoyn.bus.types.mutable;

/**
* Created by Henrik on 17.11.2014.
*/
public final class MutableInt32 extends Mutable {
  public int value;

  public MutableInt32() { }

  public MutableInt32(int value) {
    this.value = value;
  }
}
