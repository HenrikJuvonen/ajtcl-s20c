package org.alljoyn.bus.types.immutable;

import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public class ObjectPath extends Immutable {
  public int align() {
    return 4; // 4-byte align for length
  }

  public final java.lang.String sig() {
    return "o";
  }

  public final String value;

  private ObjectPath(String value) {
    this.value = value;
  }

  public static ObjectPath create(String value) {
    return new ObjectPath(value);
  }
}
