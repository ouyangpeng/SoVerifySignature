package com.csdn.ouyangpeng.jni

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.util.Log
import android.view.View
import android.widget.Button
import android.widget.TextView
import android.widget.Toast

class MainActivity : AppCompatActivity(), View.OnClickListener {
    var appSignaturesTv: TextView? = null
    var jniSignaturesTv: TextView? = null
    var checkBtn: Button? = null
    var tokenBtn: Button? = null
    private var signatureVerificationUtil: SignatureVerificationUtil? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        super.setContentView(R.layout.activity_main)
        initView()
        signatureVerificationUtil = SignatureVerificationUtil()
        signatureVerificationUtil?.let {
            val sha1ValueFromJava = it.getSha1ValueFromJava(this@MainActivity)
            appSignaturesTv!!.text = sha1ValueFromJava
            Log.d(TAG, "sha1ValueFromJava =$sha1ValueFromJava")

            val sha1ValueFromC = it.getSignaturesSha1FromC(this@MainActivity)
            jniSignaturesTv!!.text = sha1ValueFromC
            Log.d(TAG, "sha1ValueFromC =$sha1ValueFromC")
        }
    }

    private fun initView() {
        appSignaturesTv = findViewById(R.id.app_signatures_tv) as TextView
        jniSignaturesTv = findViewById(R.id.jni_signatures_tv) as TextView
        checkBtn = findViewById(R.id.check_btn) as Button
        tokenBtn = findViewById(R.id.token_btn) as Button
        checkBtn!!.setOnClickListener(this)
        tokenBtn!!.setOnClickListener(this)
    }

    override fun onClick(v: View) {
        val id = v.id
        if (id == R.id.check_btn) {
            val result = signatureVerificationUtil!!.checkSha1FromC(this@MainActivity)
            if (result) {
                Toast.makeText(applicationContext, "验证通过", Toast.LENGTH_LONG).show()
            } else {
                Toast.makeText(applicationContext, "验证不通过，请检查valid.cpp文件配置的sha1值", Toast.LENGTH_LONG).show()
            }
        } else if (id == R.id.token_btn) {
            val result = signatureVerificationUtil!!.getTokenFromC(this@MainActivity, "12345")
            Toast.makeText(applicationContext, result, Toast.LENGTH_LONG).show()
        }
    }

    companion object {
        private const val TAG = "MainActivity"
    }
}