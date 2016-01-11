package org.alljoyn.bus.types.immutable;

import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.Container;

import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Array extends Immutable {
  public int align() {
    return 4; // 4-byte align for length
  }

  public final String sig() {
    String res = values[0].sig();
    for (int i = 1; i < values.length; i++) {
      if (!res.equals(values[i].sig())) {
        res = "v";
        break;
      }
    }
    return "a" + res;
  }

  public final Immutable[] values;

  private Array(Immutable[] values) {
    this.values = Utils.copy(values);
  }

  private Array(Container values) {
    this.values = values.get();
  }

  public static Array create(Immutable[] values) {
    return new Array(values);
  }

  public static Array create(Container values) {
    return new Array(values);
  }
}
