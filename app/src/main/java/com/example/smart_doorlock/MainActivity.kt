package com.example.smart_doorlock

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import android.widget.ImageButton
import android.widget.Toast
import com.google.firebase.database.database
import com.google.firebase.database.ktx.database
import com.google.firebase.ktx.Firebase

class MainActivity : AppCompatActivity() {

    val database = Firebase.database
    val myRef = database.getReference("unlock")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val unlockButton: ImageButton = findViewById(R.id.unlockButton)
        unlockButton.setOnClickListener {
            // Add your unlock logic here
            myRef.setValue("True")
            Toast.makeText(this@MainActivity, "Door unlocked.", Toast.LENGTH_SHORT).show()
        }
    }
}

