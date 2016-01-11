package org.alljoyn.bus.types.immutable;

import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.Container;

import java.lang.StringBuffer;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Struct extends Immutable {
  public int align() {
    return 8;
  }

  public final java.lang.String sig() {
    StringBuffer res = new StringBuffer();
    for (int i = 0; i < values.length; i++) {
      res.append(((Immutable) values[i]).sig());
    }
    return "(" + res + ")";
  }

  public final Immutable[] values;

  private Struct(Immutable[] values) {
    this.values = Utils.copy(values);
  }

  private Struct(Container values) {
    this.values = values.get();
  }

  public static Struct create(Immutable[] values) {
    return new Struct(values);
  }

  public static Struct create(Container values) {
    return new Struct(values);
  }
}
