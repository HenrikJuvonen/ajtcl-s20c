package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Int64 extends Immutable {
  public static final Int64 Zero = Int64.create(0);

  public int align() {
    return 8;
  }

  public final java.lang.String sig() {
    return "x";
  }

  public final long value;

  private Int64(long value) {
    this.value = value;
  }

  public static Int64 create(long value) {
    return new Int64(value);
  }
}
