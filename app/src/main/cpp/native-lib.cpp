#include <jni.h>
#include <string>
#include"valid_sha1.cpp"

void JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg);

// https://www.kancloud.cn/owenoranba/jni/120509

/*
 * 用来抛出一个指定名字的异常。
 *
 * "JNU" 前缀表示JNI Utilities. JNU_ThrowByName 首先调用FindClass查找到异常对应的类型，如果累查找失败（return NULL), 虚拟机必须抛出一个异常（比如NoClassDefFoundError).在这个示例中，JNU_ThrowByName没有尝试抛出其他的异常。如果FindClass成功，我们会调用ThrowNew抛出指定名字的异常，当JNI_ThrowNew返回的时候，它会保证有一个异常正在抛出，尽管这个异常不必与指定的异常名称相匹配，我们需要保证删除指向在这个函数调用中创建的异常的本地引用。如果传递NULL给DeleteLocalRef，什么都不会执行，所以对于FindClass失败并返回NULL的时候调用这个函数处理返回值，也是可以的。
 */
void JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg) {
    jclass cls = (*env).FindClass(name);
    /*if cls is NULL, an exception has already been thrown */
    if (cls) {
        (*env).ThrowNew(cls, msg);
    }
    /* free the local ref */
    (*env).DeleteLocalRef(cls);
}

/*
 * 从C层拿预置的SHA1码
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_csdn_ouyangpeng_jni_SignatureVerificationUtil_getSignaturesSha1FromC(
        JNIEnv *env,
        jobject,
        jobject contextObject) {

    // 返回存在C层的sha1码
    return env->NewStringUTF(app_sha1);
}

/*
 * 从C层验证签名是否正确
 */
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

/*
 * 从C层去做具体的业务，做之前要验证签名是否正确，如果不正确，则获取Token失败
 */
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
        // 验证不通过 直接抛异常
        JNU_ThrowByName(env, "java/lang/IllegalArgumentException","Failed to obtain Token, You are a thief, please do not use my so files!");

        // 如果不加下面的return，会报异常： A/libc: Fatal signal 4 (SIGILL), code 2 (ILL_ILLOPN), fault addr 0xd1655e40 in tid 9399 (.ouyangpeng.jni), pid 9399 (.ouyangpeng.jni)
        //一定记得有return语句，即使你的return值没有用！

        // 如果签名不正确，则返回错误的结果，迷惑窃取方。
        return env->NewStringUTF("获取Token失败，请检查valid.cpp文件配置的sha1值");
    }
}