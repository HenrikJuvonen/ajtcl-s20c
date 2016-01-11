package org.alljoyn.bus.types.immutable;

import org.alljoyn.bus.core.UnsupportedOperationException;

import java.lang.Object;
import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public abstract class Immutable {
  public static final Immutable None = new Immutable() {
    public int align() {
      return 0;
    }

    public String sig() {
      return "";
    }

    public boolean equals(Object obj) {
      return obj == Immutable.None;
    }
  };

  public abstract int align();

  public abstract String sig();

  public Bool asBool() {
    return (Bool) this;
  }

  public UInt8 asInt8() {
    return (UInt8) this;
  }

  public Int16 asInt16() {
    return (Int16) this;
  }

  public Int32 asInt32() {
    return (Int32) this;
  }

  public Int64 asInt64() {
    return (Int64) this;
  }

  public UInt16 asUInt16() {
    return (UInt16) this;
  }

  public UInt32 asUInt32() {
    return (UInt32) this;
  }

  public UInt64 asUInt64() {
    return (UInt64) this;
  }

  public Float64 asFloat64() {
    return (Float64) this;
  }

  public WString asWString() {
    return (WString) this;
  }

  public Array asArray() {
    return (Array) this;
  }

  public Struct asStruct() {
    return (Struct) this;
  }

  public DictEntry asDictEntry() {
    return (DictEntry) this;
  }

  public Variant asVariant() {
    return (Variant) this;
  }

  public ObjectPath asObjectPath() {
    return (ObjectPath) this;
  }

  public Signature asSignature() {
    return (Signature) this;
  }

  public ImmutableContainer asImmutableContainer() {
    return (ImmutableContainer) this;
  }

  public boolean toBoolean() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return asBool().value;
    if (UInt8.class.isInstance(this)) return asInt8().value == 1;
    if (Int16.class.isInstance(this)) return asInt16().value == 1;
    if (UInt16.class.isInstance(this)) return asUInt16().value == 1;
    if (Int32.class.isInstance(this)) return asInt32().value == 1;
    if (UInt32.class.isInstance(this)) return asUInt32().value == 1;
    if (Int64.class.isInstance(this)) return asInt64().value == 1;
    if (UInt64.class.isInstance(this)) return asUInt64().value == 1;
    if (Float64.class.isInstance(this)) return asFloat64().value == 1;
    throw new UnsupportedOperationException();
  }

  public byte toByte() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return (byte) (asBool().value ? 1 : 0);
    if (UInt8.class.isInstance(this)) return asInt8().value;
    if (Int16.class.isInstance(this)) return (byte) asInt16().value;
    if (UInt16.class.isInstance(this)) return (byte) asUInt16().value;
    if (Int32.class.isInstance(this)) return (byte) asInt32().value;
    if (UInt32.class.isInstance(this)) return (byte) asUInt32().value;
    if (Int64.class.isInstance(this)) return (byte) asInt64().value;
    if (UInt64.class.isInstance(this)) return (byte) asUInt64().value;
    if (Float64.class.isInstance(this)) return (byte) asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public short toShort() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return (short) (asBool().value ? 1 : 0);
    if (UInt8.class.isInstance(this)) return asInt8().value;
    if (Int16.class.isInstance(this)) return asInt16().value;
    if (UInt16.class.isInstance(this)) return asUInt16().value;
    if (Int32.class.isInstance(this)) return (short) asInt32().value;
    if (UInt32.class.isInstance(this)) return (short) asUInt32().value;
    if (Int64.class.isInstance(this)) return (short) asInt64().value;
    if (UInt64.class.isInstance(this)) return (short) asUInt64().value;
    if (Float64.class.isInstance(this)) return (short) asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public int toInt() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return asBool().value ? 1 : 0;
    if (UInt8.class.isInstance(this)) return asInt8().value;
    if (Int16.class.isInstance(this)) return asInt16().value;
    if (UInt16.class.isInstance(this)) return asUInt16().value;
    if (Int32.class.isInstance(this)) return asInt32().value;
    if (UInt32.class.isInstance(this)) return asUInt32().value;
    if (Int64.class.isInstance(this)) return (int) asInt64().value;
    if (UInt64.class.isInstance(this)) return (int) asUInt64().value;
    if (Float64.class.isInstance(this)) return (int) asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public long toLong() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return asBool().value ? 1 : 0;
    if (UInt8.class.isInstance(this)) return asInt8().value;
    if (Int16.class.isInstance(this)) return asInt16().value;
    if (UInt16.class.isInstance(this)) return asUInt16().value;
    if (Int32.class.isInstance(this)) return asInt32().value;
    if (UInt32.class.isInstance(this)) return asUInt32().value;
    if (Int64.class.isInstance(this)) return asInt64().value;
    if (UInt64.class.isInstance(this)) return asUInt64().value;
    if (Float64.class.isInstance(this)) return (long) asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public float toFloat() throws UnsupportedOperationException {
    if (Float64.class.isInstance(this)) return (float) asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public double toDouble() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return asBool().value ? 1 : 0;
    if (UInt8.class.isInstance(this)) return asInt8().value;
    if (Int16.class.isInstance(this)) return asInt16().value;
    if (UInt16.class.isInstance(this)) return asUInt16().value;
    if (Int32.class.isInstance(this)) return asInt32().value;
    if (UInt32.class.isInstance(this)) return asUInt32().value;
    if (Int64.class.isInstance(this)) return asInt64().value;
    if (UInt64.class.isInstance(this)) return asUInt64().value;
    if (Float64.class.isInstance(this)) return asFloat64().value;
    throw new UnsupportedOperationException();
  }

  public String toUTF() throws UnsupportedOperationException {
    if (Bool.class.isInstance(this)) return String.valueOf(asBool().value);
    if (UInt8.class.isInstance(this)) return String.valueOf(asInt8().value);
    if (Int16.class.isInstance(this)) return String.valueOf(asInt16().value);
    if (UInt16.class.isInstance(this)) return String.valueOf(asUInt16().value);
    if (Int32.class.isInstance(this)) return String.valueOf(asInt32().value);
    if (UInt32.class.isInstance(this)) return String.valueOf(asUInt32().value);
    if (Int64.class.isInstance(this)) return String.valueOf(asInt64().value);
    if (UInt64.class.isInstance(this)) return String.valueOf(asUInt64().value);
    if (Float64.class.isInstance(this)) return String.valueOf(asFloat64().value);
    if (WString.class.isInstance(this)) return asWString().value;
    if (ObjectPath.class.isInstance(this)) return asObjectPath().value;
    if (Signature.class.isInstance(this)) return asSignature().value;
    throw new UnsupportedOperationException();
  }
}
