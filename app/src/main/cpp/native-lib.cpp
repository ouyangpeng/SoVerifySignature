#include <jni.h>
#include <string>
#include"valid_sha1.cpp"

void JNU_ThrowByName(JNIEnv *env, const char* name, const char* msg);

// https://www.kancloud.cn/owenoranba/jni/120509
void JNU_ThrowByName(JNIEnv *env, const char* name, const char* msg){
    jclass cls = (*env).FindClass(name);
    /*if cls is NULL, an exception has already been thrown */
    if(cls){
        (*env).ThrowNew(cls, msg);
    }
    /* free the local ref */
    (*env).DeleteLocalRef(cls);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_csdn_ouyangpeng_jni_SignatureVerificationUtil_getSignaturesSha1FromC(
        JNIEnv *env,
        jobject,
        jobject contextObject) {

    // 返回存在C层的sha1码
    return env->NewStringUTF(app_sha1);
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_csdn_ouyangpeng_jni_SignatureVerificationUtil_checkSha1FromC(
        JNIEnv *env,
        jobject,
        jobject contextObject) {

    // 检验签名是否正确
    char *sha1 = getSha1(env, contextObject);
    jboolean result = checkValidity(env, sha1);
    return result;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_csdn_ouyangpeng_jni_SignatureVerificationUtil_getTokenFromC(
        JNIEnv *env,
        jobject,
        jobject contextObject,
        jstring userId) {
    // 检验签名是否正确
    char *sha1 = getSha1(env, contextObject);
    jboolean result = checkValidity(env, sha1);

    // 如果签名正确，则返回正确的token
    if (result) {
        return env->NewStringUTF("获取Token成功，token为 ouyangpeng");
    } else {
        // 方式一、验证不通过 直接抛异常
        JNU_ThrowByName(env,"java/lang/IllegalArgumentException","Failed to obtain Token, You are a thief, please do not use my so files!");

        // 方式二、如果签名不正确，则返回错误的结果，迷惑窃取方。
//        return env->NewStringUTF("获取Token失败，请检查valid.cpp文件配置的sha1值");
    }
}