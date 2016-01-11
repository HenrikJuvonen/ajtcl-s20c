package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Int32 extends Immutable {
  public static final Int32 Zero = Int32.create(0);

  public int align() {
    return 4;
  }

  public final java.lang.String sig() {
    return "i";
  }

  public final int value;

  private Int32(int value) {
    this.value = value;
  }

  public static Int32 create(int value) {
    return new Int32(value);
  }
}
