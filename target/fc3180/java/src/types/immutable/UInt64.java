package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class UInt64 extends Immutable {
  public static final UInt64 Zero = UInt64.create(0);

  public int align() {
    return 8;
  }

  public final java.lang.String sig() {
    return "t";
  }

  public final long value;

  private UInt64(long value) {
    this.value = value;
  }

  public static UInt64 create(long value) {
    return new UInt64(value);
  }
}
