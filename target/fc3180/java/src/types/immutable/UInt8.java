package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class UInt8 extends Immutable {
  public static final UInt8 Zero = UInt8.create(0);

  public int align() {
    return 1;
  }

  public final java.lang.String sig() {
    return "y";
  }

  public final byte value;

  private UInt8(byte value) {
    this.value = value;
  }

  public static UInt8 create(byte value) {
    return new UInt8(value);
  }

  public static UInt8 create(int value) {
    return new UInt8((byte) value);
  }
}
