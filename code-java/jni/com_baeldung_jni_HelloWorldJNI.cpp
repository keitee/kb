#include "com_baeldung_jni_HelloWorldJNI.h"
#include <iostream>

JNIEXPORT void JNICALL Java_com_baeldung_jni_HelloWorldJNI_sayHello
  (JNIEnv *, jobject) 
{
  std::cout << "JNI: Hello JNI from C++" << std::endl;
}
