package org.alljoyn.bus;

public class Result {
  public final int status;
  public final Object value;

  public Result(int status, Object value) {
    this.status = status;
    this.value = value;
  }
}
