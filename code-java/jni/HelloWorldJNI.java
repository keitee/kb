// https://www.baeldung.com/jni
//

// hello.java:6: error: class HelloWorldJNI is public, should be declared in a
// file named HelloWorldJNI.java
//
// public class HelloWorldJNI
 
// package com.baeldung.jni;

public class HelloWorldJNI
{
  static {
    System.loadLibrary("native");
  }

  public static void main(String[] args) {
    new HelloWorldJNI().sayHello();

    new HelloWorldJNI().sumIntegers(20, 30);

    new HelloWorldJNI().sayHelloToMe("keitee", false);
  }

  // declare a native sayHello() that receives no arguments and return void
  private native void sayHello();

  private native long sumIntegers(int first, int second);

  private native void singleIntegers(int first);
     
  private native String sayHelloToMe(String name, boolean isFemale);
}
