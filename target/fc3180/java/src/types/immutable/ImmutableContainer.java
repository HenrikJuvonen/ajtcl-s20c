package org.alljoyn.bus.types.immutable;

import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.Container;

import java.lang.String;
import java.lang.StringBuffer;

/**
 * Created by Henrik on 16.11.2014.
 */
public class ImmutableContainer extends Immutable {
  public int align() {
    return 0;
  }

  public String sig() {
    StringBuffer res = new StringBuffer();
    for (int i = 0; i < values.length; i++) {
      res.append(((Immutable) values[i]).sig());
    }
    return res.toString();
  }

  public int size() {
    return this.values.length;
  }

  public Immutable get(int index) {
    return (Immutable) this.values[index];
  }

  public Immutable get(Immutable key) {
    for (int i = 0; i < this.values.length; i++) {
      Immutable value = this.values[i];
      if (!DictEntry.class.isInstance(value)) {
        continue;
      }
      DictEntry entry = value.asDictEntry();
      if (entry.key.equals(key)) {
        return entry.value;
      }
    }
    return Immutable.None;
  }

  public final Immutable[] values;

  public ImmutableContainer(Immutable[] values) {
    this.values = Utils.copy(values);
  }

  public ImmutableContainer(Container values) {
    this.values = values.get();
  }

  public static ImmutableContainer create(Immutable[] values) {
    return new ImmutableContainer(values);
  }
}
