package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class UInt32 extends Immutable {
  public static final UInt32 Zero = UInt32.create(0);

  public int align() {
    return 4;
  }

  public final java.lang.String sig() {
    return "u";
  }

  public final int value;

  private UInt32(int value) {
    this.value = value;
  }

  public static UInt32 create(int value) {
    return new UInt32(value);
  }
}
