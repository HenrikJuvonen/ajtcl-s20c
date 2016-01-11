package org.alljoyn.bus.types.immutable;

/**
 * Created by Henrik on 15.11.2014.
 */
public class Float64 extends Immutable {
  public int align() {
    return 8;
  }

  public final java.lang.String sig() {
    return "d";
  }

  public final double value;

  private Float64(float value) {
    this.value = value;
  }

  private Float64(double value) {
    this.value = value;
  }

  public static Float64 create(float value) {
    return new Float64(value);
  }

  public static Float64 create(double value) {
    return new Float64(value);
  }
}
