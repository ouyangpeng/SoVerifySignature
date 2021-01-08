#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>

#define TAG    "ouyangpeng-jni-log"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

//签名信息
const char *app_sha1 = "8A2BSBCSWSB25097SX110A02B7815537CCASDWR6";
//const char *app_sha1 = "6A68B6BDBBB7E79772B2A075A7815537CCA57F6F";
const char hexcode[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
                        'F'};


// 这个方法和Java层的com.csdn.ouyangpeng.jni.SignatureVerificationUtil.getSha1ValueFromJava()方法是一样的，只是在C层实现

char *getSha1(JNIEnv *env, jobject context_object) {
    //上下文对象
    jclass context_class = env->GetObjectClass(context_object);

    //反射获取PackageManager   对应java层代码为  context.getPackageManager()
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                          "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context_object, methodId);
    if (package_manager == NULL) {
        LOGD("package_manager is NULL!!!");
        return NULL;
    }

    //反射获取包名            对应java层代码为  context.getPackageName()
    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring) env->CallObjectMethod(context_object, methodId);
    if (package_name == NULL) {
        LOGD("package_name is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(context_class);

    //获取PackageInfo对象     对应java层代码为  PackageManager.getPackageInfo()
    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getPackageInfo",
                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    env->DeleteLocalRef(pack_manager_class);

    // 对应java层代码为  PackageInfo info = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);
    // public static final int GET_SIGNATURES          = 0x00000040;
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name, 0x40);
    if (package_info == NULL) {
        LOGD("getPackageInfo() is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(package_manager);


    //获取签名信息
    jclass package_info_class = env->GetObjectClass(package_info);

    // 对应java层代码为  PackageInfo.signatures 属性
    jfieldID fieldId = env->GetFieldID(package_info_class, "signatures",
                                       "[Landroid/content/pm/Signature;");
    env->DeleteLocalRef(package_info_class);
    jobjectArray signature_object_array = (jobjectArray) env->GetObjectField(package_info, fieldId);
    if (signature_object_array == NULL) {
        LOGD("signature is NULL!!!");
        return NULL;
    }
    // 对应java层代码为  PackageInfo.signatures[0]
    jobject signature_object = env->GetObjectArrayElement(signature_object_array, 0);
    env->DeleteLocalRef(package_info);

    //签名信息转换成sha1值

    // 对应java层代码为   info.signatures[0].toByteArray();
    jclass signature_class = env->GetObjectClass(signature_object);
    methodId = env->GetMethodID(signature_class, "toByteArray", "()[B");
    env->DeleteLocalRef(signature_class);
    jbyteArray signature_byte = (jbyteArray) env->CallObjectMethod(signature_object, methodId);

    // 对应java层代码为    byte[] cert = info.signatures[0].toByteArray();
    jclass byte_array_input_class = env->FindClass("java/io/ByteArrayInputStream");
    methodId = env->GetMethodID(byte_array_input_class, "<init>", "([B)V");
    jobject byte_array_input = env->NewObject(byte_array_input_class, methodId, signature_byte);

    // 对应java层代码为   CertificateFactory certfactory = CertificateFactory.getInstance("X.509");
    jclass certificate_factory_class = env->FindClass("java/security/cert/CertificateFactory");
    methodId = env->GetStaticMethodID(certificate_factory_class, "getInstance",
                                      "(Ljava/lang/String;)Ljava/security/cert/CertificateFactory;");
    jstring x_509_jstring = env->NewStringUTF("X.509");
    jobject cert_factory = env->CallStaticObjectMethod(certificate_factory_class, methodId,
                                                       x_509_jstring);

    // 对应的Java层代码为 X509Certificate cert = (X509Certificate) certfactory.generateCertificate(certFileIs);
    methodId = env->GetMethodID(certificate_factory_class, "generateCertificate",
                                ("(Ljava/io/InputStream;)Ljava/security/cert/Certificate;"));
    jobject x509_cert = env->CallObjectMethod(cert_factory, methodId, byte_array_input);
    env->DeleteLocalRef(certificate_factory_class);

    // 对应的Java层代码为  X509Certificate的getEncoded方法
    jclass x509_cert_class = env->GetObjectClass(x509_cert);
    methodId = env->GetMethodID(x509_cert_class, "getEncoded", "()[B");
    jbyteArray cert_byte = (jbyteArray) env->CallObjectMethod(x509_cert, methodId);
    env->DeleteLocalRef(x509_cert_class);

    //对应的Java层代码为    MessageDigest md = MessageDigest.getInstance("SHA1");
    jclass message_digest_class = env->FindClass("java/security/MessageDigest");
    methodId = env->GetStaticMethodID(message_digest_class, "getInstance",
                                      "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    jstring sha1_jstring = env->NewStringUTF("SHA1");
    jobject sha1_digest = env->CallStaticObjectMethod(message_digest_class, methodId, sha1_jstring);

    //对应的Java层代码为 byte[] publicKey = md.digest(cert);
    methodId = env->GetMethodID(message_digest_class, "digest", "([B)[B");
    jbyteArray sha1_byte = (jbyteArray) env->CallObjectMethod(sha1_digest, methodId, cert_byte);
    env->DeleteLocalRef(message_digest_class);

    //转换成char
    jsize array_size = env->GetArrayLength(sha1_byte);
    jbyte *sha1 = env->GetByteArrayElements(sha1_byte, NULL);
    char *hex_sha = new char[array_size * 2 + 1];
    for (int i = 0; i < array_size; ++i) {
        hex_sha[2 * i] = hexcode[((unsigned char) sha1[i]) / 16];
        hex_sha[2 * i + 1] = hexcode[((unsigned char) sha1[i]) % 16];
    }
    hex_sha[array_size * 2] = '\0';

    LOGD("hex_sha  %s ", hex_sha);
    return hex_sha;
}

jboolean checkValidity(JNIEnv *env, char *sha1) {
    //比较签名
    if (strcmp(sha1, app_sha1) == 0) {
        LOGD("Verified successfully!");
        return true;
    }
    LOGD("Verification Failed!");
    return false;
}