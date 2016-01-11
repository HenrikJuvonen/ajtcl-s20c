package org.alljoyn.bus.types.immutable;

import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Signature extends Immutable {
  public int align() {
    return 1;
  }

  public final java.lang.String sig() {
    return "g";
  }

  public final String value;

  private Signature(String value) {
    this.value = value;
  }

  public static Signature create(String value) {
    return new Signature(value);
  }
}
