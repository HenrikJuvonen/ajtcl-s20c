package org.alljoyn.bus.types;

import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.immutable.*;

import java.lang.String;
import java.lang.StringBuffer;
import java.util.Vector;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Container {
  private final Vector values = new Vector();

  private Container() {
  }

  public static Container create() {
    return new Container();
  }

  public static Container create(Immutable value) {
    return create().add(value);
  }

  public static Container create(ImmutableContainer value) {
    return create().add(value.values);
  }

  public static Container create(Immutable[] values) {
    return create().add(values);
  }

  public static Container create(Immutable key, Immutable value) {
    return create().add(key, value);
  }

  public Container add(Immutable key, Immutable value) {
    this.values.addElement(DictEntry.create(key, value));
    return this;
  }

  public Container add(Immutable value) {
    if (ImmutableContainer.class.isInstance(value)) {
      return add(((ImmutableContainer)value).values);
    }
    this.values.addElement(value);
    return this;
  }

  public Container add(Immutable[] values) {
    for (int i = 0; i < values.length; i++) {
      this.values.addElement(values[i]);
    }
    return this;
  }

  public Container addBool(boolean value) {
    this.values.addElement(Bool.create(value));
    return this;
  }
  public Container addInt16(short value) {
    this.values.addElement(Int16.create(value));
    return this;
  }
  public Container addInt32(int value) {
    this.values.addElement(Int32.create(value));
    return this;
  }
  public Container addInt64(long value) {
    this.values.addElement(Int64.create(value));
    return this;
  }
  public Container addUInt8(int value) {
    this.values.addElement(UInt8.create(value));
    return this;
  }
  public Container addUInt16(short value) {
    this.values.addElement(UInt16.create(value));
    return this;
  }
  public Container addUInt32(int value) {
    this.values.addElement(UInt32.create(value));
    return this;
  }
  public Container addUInt64(long value) {
    this.values.addElement(UInt64.create(value));
    return this;
  }
  public Container addFloat64(double value) {
    this.values.addElement(Float64.create(value));
    return this;
  }
  public Container addWString(String value) {
    this.values.addElement(WString.create(value));
    return this;
  }
  public Container addObjectPath(String value) {
    this.values.addElement(ObjectPath.create(value));
    return this;
  }
  public Container addSignature(String value) {
    this.values.addElement(Signature.create(value));
    return this;
  }
  public Container addVariant(Immutable value) {
    this.values.addElement(Variant.create(value));
    return this;
  }

  public Immutable get(int index) {
    return (Immutable) this.values.elementAt(index);
  }

  public Immutable get(Immutable key) {
    for (int i = 0; i < this.values.size(); i++) {
      Immutable value = (Immutable) this.values.elementAt(i);
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

  public Immutable[] get() {
    return Utils.copy(values);
  }

  public int size() { return values.size(); }

  public ImmutableContainer asImmutable() {
    return new ImmutableContainer(this);
  }

  public Array toArray() {
    return Array.create(this);
  }

  public Struct toStruct() {
    return Struct.create(this);
  }

  public String sig() {
    StringBuffer res = new StringBuffer();
    for (int i = 0; i < values.size(); i++) {
      res.append(((Immutable) values.elementAt(i)).sig());
    }
    return res.toString();
  }
}
