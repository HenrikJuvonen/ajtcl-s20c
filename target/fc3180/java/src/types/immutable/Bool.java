package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Bool extends Immutable {
  public static final Bool False = Bool.create(false);
  public static final Bool True = Bool.create(true);

  public int align() {
    return 4;
  }

  public final java.lang.String sig() {
    return "b";
  }

  public final boolean value;

  private Bool(boolean value) {
    this.value = value;
  }

  public static Bool create(boolean value) {
    return new Bool(value);
  }
}
