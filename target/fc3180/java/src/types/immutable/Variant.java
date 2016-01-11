package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Variant extends Immutable {
  public int align() {
    return 1; // 1-byte align for signature
  }

  public final java.lang.String sig() {
    return "v";
  }

  public final Immutable value;

  private Variant(Immutable value) {
    this.value = value;
  }

  public static Variant create(Immutable value) {
    return new Variant(value);
  }
}
