package org.alljoyn.bus.messaging;

import org.alljoyn.bus.types.immutable.Immutable;
import org.alljoyn.bus.types.immutable.ImmutableContainer;
import org.alljoyn.bus.core.UnsupportedOperationException;
import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.*;
import org.alljoyn.bus.types.immutable.*;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.lang.String;
import java.lang.StringIndexOutOfBoundsException;
import java.util.Vector;

/**
 * Created by Henrik on 17.11.2014.
 */
public final class Unpacker {
  private static class UnmarshalResult {
    public final int size;
    public final Immutable value;

    private UnmarshalResult(int size, Immutable value) {
      this.size = size;
      this.value = value;
    }

    public static final UnmarshalResult create(int size, Immutable value) {
      return new UnmarshalResult(size, value);
    }
  }

  private static int findEnd(int idx, int b, int e, String sig) throws StringIndexOutOfBoundsException {
    int end = sig.length();
    int depth = 1;
    while (idx < end) {
      char subc = sig.charAt(idx);
      if (subc == b) {
        depth++;
      } else if (subc == e) {
        depth--;
        if (depth == 0)
          return idx;
      }
      idx++;
    }
    throw new StringIndexOutOfBoundsException();
  }

  public static String[] genCompleteTypes(String sig) {
    int i = 0;
    int end = sig.length();
    Vector vector = new Vector();
    while (i < end) {
      char c = sig.charAt(i);
      if (c == '(') {
        int x = findEnd(i + 1, '(', ')', sig);
        vector.addElement(sig.substring(i, x + 1));
        i = x;
      } else if (c == '{') {
        int x = findEnd(i + 1, '{', '}', sig);
        vector.addElement(sig.substring(i, x + 1));
        i = x;
      } else if (c == 'a') {
        String ct = genCompleteTypes(sig.substring(i + 1))[0];
        i += ct.length();
        vector.addElement("a" + ct);
      } else {
        vector.addElement(String.valueOf(c));
      }
      i += 1;
    }
    String[] result = new String[vector.size()];
    vector.copyInto(result);
    return result;
  }

  private static UnmarshalResult unmarshalByte(byte[] data, int offset) throws IOException {
    ByteArrayInputStream bais = new ByteArrayInputStream(data, offset, 1);
    DataInputStream dis = new DataInputStream(bais);
    Immutable result = UInt8.create(dis.readByte());
    dis.close();
    bais.close();
    return UnmarshalResult.create(1, result);
  }

  private static UnmarshalResult unmarshalBytes2(char tcode, byte[] data, int offset, boolean lendian) throws IOException {
    byte[] bytes = Utils.slice(data, offset, 2);
    if (lendian) {
      bytes = Utils.reverse(bytes);
    }
    ByteArrayInputStream bais = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bais);
    Immutable result = Immutable.None;
    switch (tcode) {
      case 'n':
        result = Int16.create(dis.readShort());
        break;
      case 'q':
        result = UInt16.create(dis.readShort());
        break;
    }
    dis.close();
    bais.close();
    return UnmarshalResult.create(2, result);
  }

  private static UnmarshalResult unmarshalBytes4(char tcode, byte[] data, int offset, boolean lendian) throws IOException {
    byte[] bytes = Utils.slice(data, offset, 4);
    if (lendian) {
      bytes = Utils.reverse(bytes);
    }
    ByteArrayInputStream bais = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bais);
    Immutable result = Immutable.None;
    switch (tcode) {
      case 'b':
        result = Bool.create(dis.readInt() == 1);
        break;
      case 'i':
        result = Int32.create(dis.readInt());
        break;
      case 'u':
      case 'h':
        result = UInt32.create(dis.readInt());
        break;
    }
    dis.close();
    bais.close();
    return UnmarshalResult.create(4, result);
  }

  private static UnmarshalResult unmarshalBytes8(char tcode, byte[] data, int offset, boolean lendian) throws IOException {
    byte[] bytes = Utils.slice(data, offset, 8);
    if (lendian) {
      bytes = Utils.reverse(bytes);
    }
    ByteArrayInputStream bais = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bais);
    Immutable result = Immutable.None;
    switch (tcode) {
      case 'x':
        result = Int64.create(dis.readLong());
        break;
      case 't':
        result = UInt64.create(dis.readLong());
        break;
      case 'd':
        result = Float64.create(dis.readDouble());
        break;
    }
    dis.close();
    bais.close();
    return UnmarshalResult.create(8, result);
  }

  private static UnmarshalResult unmarshalString(byte[] data, int offset, boolean lendian) throws IOException, UnsupportedOperationException {
    int slen = unmarshalBytes4('u', data, offset, lendian).value.toInt();
    Immutable result = WString.create(new String(data, offset + 4, slen, "UTF8"));
    return UnmarshalResult.create(4 + slen + 1, result);
  }

  private static UnmarshalResult unmarshalSignature(byte[] data, int offset) throws IOException, UnsupportedOperationException {
    int slen = unmarshalByte(data, offset).value.toInt();
    Immutable result = Signature.create(new String(data, offset + 1, slen, "ISO8859_1"));
    return UnmarshalResult.create(1 + slen + 1, result);
  }

  private static UnmarshalResult unmarshalArray(String signature, byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    int startOffset = offset;
    Container container = Container.create();
    int dataLen = unmarshalBytes4('u', data, offset, lendian).value.toInt();
    String tsig = signature.substring(1);
    char tcode = tsig.charAt(0);

    offset += 4;
    offset += Padder.pad(tcode, msgOffset + offset).length;
    int endOffset = offset + dataLen;
    while (offset < endOffset) {
      offset += Padder.pad(tcode, msgOffset + offset).length;
      UnmarshalResult res = unmarshalAny(tsig, data, offset, msgOffset, lendian);
      offset += res.size;
      container.add(res.value);
    }
    if (offset != endOffset) {
      throw new UnsupportedOperationException();
    }

    return UnmarshalResult.create(offset-startOffset, container.toArray());
  }

  private static UnmarshalResult unmarshalStruct(String signature, byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    UnmarshalResult res = unmarshal(signature.substring(1, signature.length() - 1), data, offset, msgOffset, lendian);
    return UnmarshalResult.create(res.size, Container.create(res.value).toStruct());
  }

  private static UnmarshalResult unmarshalDictEntry(String signature, byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    UnmarshalResult res = unmarshal(signature.substring(1, signature.length() - 1), data, offset, msgOffset, lendian);
    ImmutableContainer container = res.value.asImmutableContainer();
    return UnmarshalResult.create(res.size, DictEntry.create(container.values[0], container.values[1]));
  }

  private static UnmarshalResult unmarshalVariant(byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    int startOffset = offset;
    UnmarshalResult sigRes = unmarshalSignature(data, offset);
    String sig = sigRes.value.toUTF();
    offset += sigRes.size;
    offset += Padder.pad(sig.charAt(0), msgOffset + offset).length;
    UnmarshalResult res = unmarshalAny(sig, data, offset, msgOffset, lendian);
    offset += res.size;
    return UnmarshalResult.create(offset - startOffset, Variant.create(res.value));
  }

  private static UnmarshalResult unmarshalAny(String signature, byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    char tcode = signature.charAt(0);
    UnmarshalResult res;
    switch (tcode) {
      case 'y':
        res = unmarshalByte(data, offset);
        break;
      case 'n':
      case 'q':
        res = unmarshalBytes2(tcode, data, offset, lendian);
        break;
      case 'b':
      case 'i':
      case 'u':
      case 'h':
        res = unmarshalBytes4(tcode, data, offset, lendian);
        break;
      case 'x':
      case 't':
      case 'd':
        res = unmarshalBytes8(tcode, data, offset, lendian);
        break;
      case 's':
      case 'o':
        res = unmarshalString(data, offset, lendian);
        break;
      case 'g':
        res = unmarshalSignature(data, offset);
        break;
      case 'a':
        res = unmarshalArray(signature, data, offset, msgOffset, lendian);
        break;
      case '(':
        res = unmarshalStruct(signature, data, offset, msgOffset, lendian);
        break;
      case '{':
        res = unmarshalDictEntry(signature, data, offset, msgOffset, lendian);
        break;
      case 'v':
        res = unmarshalVariant(data, offset, msgOffset, lendian);
        break;
      default:
        throw new UnsupportedOperationException();
    }
    return res;
  }

  private static UnmarshalResult unmarshal(String signature, byte[] data, int offset, int msgOffset, boolean lendian) throws IOException, UnsupportedOperationException {
    int startOffset = offset;
    Container container = Container.create();
    String[] completeTypes = genCompleteTypes(signature);
    for (int i = 0; i < completeTypes.length; i++) {
      char tcode = completeTypes[i].charAt(0);
      offset += Padder.pad(tcode, msgOffset + offset).length;
      UnmarshalResult res = unmarshalAny(completeTypes[i], data, offset, msgOffset, lendian);
      offset += res.size;
      container.add(res.value);
    }
    return UnmarshalResult.create(offset - startOffset, container.asImmutable());
  }

  public static Immutable unpack(String signature, byte[] data, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    return unmarshal(signature, data, 0, totalLen - data.length, lendian).value;
  }
}
