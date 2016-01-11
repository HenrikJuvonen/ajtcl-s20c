package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class UInt16 extends Immutable {
  public static final UInt16 Zero = UInt16.create(0);

  public int align() {
    return 2;
  }

  public final java.lang.String sig() {
    return "q";
  }

  public final short value;

  private UInt16(short value) {
    this.value = value;
  }

  public static UInt16 create(short value) {
    return new UInt16(value);
  }

  public static UInt16 create(int value) {
    return new UInt16((short) value);
  }
}
