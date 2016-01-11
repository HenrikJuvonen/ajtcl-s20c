package org.alljoyn.bus.types.immutable;

import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public class WString extends Immutable {
  public static final WString Empty = WString.create("");

  public int align() {
    return 4; // 4-byte align for length
  }

  public final String sig() {
    return "s";
  }

  public final String value;

  private WString(String value) {
    this.value = value;
  }

  public static WString create(String value) {
    return new WString(value);
  }
}
