/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <iostream>

/* Header for class HelloWorldJNI */

#ifndef _Included_HelloWorldJNI
#define _Included_HelloWorldJNI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     HelloWorldJNI
 * Method:    sayHello
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_HelloWorldJNI_sayHello
  (JNIEnv *, jobject)
{
  std::cout << "JNI: Hello JNI from C++" << std::endl;
}

JNIEXPORT jlong JNICALL Java_HelloWorldJNI_sumIntegers
  (JNIEnv* env, jobject thisObject, jint first, jint second) {
    std::cout << "C++: The numbers received are : " << first << " and " << second << std::endl;
    return (long)first + (long)second;
}

JNIEXPORT jstring JNICALL Java_HelloWorldJNI_sayHelloToMe
  (JNIEnv* env, jobject thisObject, jstring name, jboolean isFemale) {
    const char* nameCharPointer = env->GetStringUTFChars(name, NULL);
    std::string title;
    if(isFemale) {
        title = "Ms. ";
    }
    else {
        title = "Mr. ";
    }
 
    std::string fullName = title + nameCharPointer;
    std::cout << "C++: fullname is : " << fullName << std::endl;
    return env->NewStringUTF(fullName.c_str());
}

#ifdef __cplusplus
}
#endif
#endif
