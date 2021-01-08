package com.csdn.ouyangpeng.jni

import android.content.Context
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.util.Log
import com.csdn.ouyangpeng.jni.SignatureVerificationUtil
import java.lang.Exception
import java.lang.StringBuilder
import java.security.MessageDigest
import java.util.*

class SignatureVerificationUtil {
    companion object {
        private const val TAG = "SignatureVerificationUtil"

        init {
            System.loadLibrary("native-lib")
        }
    }

    /**
     * 从C层拿预置的SHA1码
     */
    external fun getSignaturesSha1FromC(context: Context?): String?

    /**
     * 从C层验证签名是否正确
     */
    external fun checkSha1FromC(context: Context?): Boolean

    /**
     * 从C层去做具体的业务，做之前要验证签名是否正确，如果不正确，则获取Token失败
     */
    external fun getTokenFromC(context: Context?, userId: String?): String?

    /**
     * 获得APK的签名文件的SHA1码
     */
    fun getSha1ValueFromJava(context: Context): String? {
        try {
            val info = context.packageManager.getPackageInfo(
                    context.packageName, PackageManager.GET_SIGNATURES)
            val cert = info.signatures[0].toByteArray()
            val md = MessageDigest.getInstance("SHA1")
            val publicKey = md.digest(cert)
            val hexString = StringBuilder()
            for (b in publicKey) {
                val appendString = Integer.toHexString(0xFF and b.toInt())
                        .toUpperCase(Locale.US)
                if (appendString.length == 1) hexString.append("0")
                hexString.append(appendString)
            }
            return hexString.toString()
        } catch (e: Exception) {
            Log.d(TAG, "getSha1Value() crashed , e =" + Log.getStackTraceString(e))
        }
        return null
    }
}