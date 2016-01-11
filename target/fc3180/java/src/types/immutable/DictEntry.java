package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class DictEntry extends Immutable {
  public int align() {
    return 8;
  }

  public final java.lang.String sig() {
    return "{" + key.sig() + value.sig() + "}";
  }

  public final Immutable key;
  public final Immutable value;

  private DictEntry(Immutable key, Immutable value) {
    this.key = key;
    this.value = value;
  }

  public static DictEntry create(Immutable key, Immutable value) {
    return new DictEntry(key, value);
  }
}
