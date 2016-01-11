package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Int16 extends Immutable {
  public static final Int16 Zero = Int16.create(0);

  public int align() {
    return 2;
  }

  public final java.lang.String sig() {
    return "n";
  }

  public final short value;

  private Int16(short value) {
    this.value = value;
  }

  public static Int16 create(short value) {
    return new Int16(value);
  }

  public static Int16 create(int value) {
    return new Int16((short) value);
  }
}
