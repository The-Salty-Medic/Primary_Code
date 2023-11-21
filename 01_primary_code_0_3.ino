// Written by Aaron Hope and ripped off from everywhere
// This is built to run without dependencies
// This vision has two transmission functions, short wait and long wait
// The short wait transmission is the initial CW message
// The long wait transmission is for the periodic CW requirement

// defining pins
#define pin_audio_input A1
#define pin_audio_output 2
#define pin_ptt_switch 3

// begin constant parameters
const int CW_Word_Per_Minute = 20;                             // sets pace for all subsequent tone output
const int Dit_ms = CW_Word_Per_Minute * 3;                     // based on "Paris " (50 dib / sec)
const int Dah_ms = Dit_ms * 3;                                 // confirmed
const int CW_Letter_Gap_ms = Dit_ms;                           // per definitions
const int CW_Space_ms = Dit_ms * 7;                            // space bar
const int Audio_Confirmation_Delay = 100;                      // debounce audio and add to audio confirmation count when appropriate
const long Polite_CW_Delay = 120000;                           // waiting for a pause in the conversation, up to 2 minute, applies to short delay
const long Max_Long_Delay = 900000;                            // maximum long delay of 15 minutes for GMRS compliance, set to 600000 for Ham
const long Min_Long_Delay = Max_Long_Delay - Polite_CW_Delay;  // so we can transmit between the two timelines.
const long Audio_Input_Threshold = 600;                        // likely will need to be adjusted
const String Your_FCC_Call_Sign = "WRXJ407";                   // put your information here, could be used to send whatever you want really
const long tone_hz = 1200;                                     // feel free to change, < https://onlinetonegenerator.com/ > use square type to assess tone output

// begin mutable parameters
int System_Status = 0;         // tracking this as we go for visual display
int Reception_Flag = 0;        // hearing test flashbacks
int Transmission_Flag = 0;     // used to track transmission politeness
long current_audio_level = 0;  // to be modified over time
long Polite_CW_Timer = 0;      // iteratively managed
long Long_CW_Timer = 0;        // iteratively managed

// defining alphabet array
const String Standard_Alphabet[] = {
      "A",
      "B",
      "C",
      "D",
      "E",
      "F",
      "G",
      "H",
      "I",
      "J",
      "K",
      "L",
      "M",
      "N",
      "O",
      "P",
      "Q",
      "R",
      "S",
      "T",
      "U",
      "V",
      "W",
      "X",
      "Y",
      "Z",
      "1",
      "2",
      "3",
      "4",
      "5",
      "6",
      "7",
      "8",
      "9",
      "0",
      "/",
      " "
};
// defining CW array
const String CW_Alphabet[] = {
      ".-",     // A
      "-...",   // B
      "-.-.",   // C
      "-..",    // D
      ".",      // E
      "..-.",   // F
      "--.",    // G
      "....",   // H
      "..",     // I
      ".---",   // J
      "-.-",    // K
      ".-..",   // L
      "--",     // M
      "-.",     // N
      "---",    // O
      ".--.",   // P
      "--.-",   // Q
      ".-.",    // R
      "...",    // S
      "-",      // T
      "..-",    // U
      "...-",   // V
      ".--",    // W
      "-..-",   // X
      "-.--",   // Y
      "--..",   // Z
      ".----",  // 1
      "..---",  // 2
      "...--",  // 3
      "....-",  // 4
      ".....",  // 5
      "-....",  // 6
      "--...",  // 7
      "---..",  // 8
      "----.",  // 9
      "-----",  // 0
      "-..-.",  // forward slash
      " "       // space character
};

void setup() {

      // these things run once, or when you push the reset button
      pinMode(pin_audio_input, INPUT);
      pinMode(pin_audio_output, OUTPUT);
      pinMode(pin_ptt_switch, OUTPUT);

}  // end setup

void loop() {

      // encoding fcc call sign as CW
      String Encoded_FCC_Call_Sign = Your_FCC_Call_Sign;

      // shifts call sign to upper case to sync with above
      Encoded_FCC_Call_Sign.toUpperCase();
      // for each letter in the standard alphabet
      int alphabet_loop_length = sizeof(Standard_Alphabet) / sizeof(Standard_Alphabet[0]);
      for (int i = 0; i < alphabet_loop_length; i++) {
            // replace the letter with CW
            String replace_this = Standard_Alphabet[i];
            String with_that = CW_Alphabet[i];
            Encoded_FCC_Call_Sign.replace(replace_this, with_that);
      }  // end of CW encoding

      // converting string to array for indexing
      int Buffer_Length = Encoded_FCC_Call_Sign.length() + 1;
      char Call_Sign_Array[Buffer_Length];
      Encoded_FCC_Call_Sign.toCharArray(Call_Sign_Array, Buffer_Length);
      int Encoded_String_Length = Encoded_FCC_Call_Sign.length();
      // establish length of CW to be sent
      int Encoded_CW_Length = Encoded_FCC_Call_Sign.length();


      // open version of case switching based on system status

      if (System_Status == 0) {
            Reception_Flag = 0;
            while (System_Status == 0) {
                  delay(Audio_Confirmation_Delay);

                  // if we detect audio
                  current_audio_level = analogRead(pin_audio_input);
                  if (current_audio_level >= Audio_Input_Threshold && Reception_Flag < 5) {
                        Reception_Flag++;  // increase reception flag with audio
                  } else {
                        // if there isn't any audio, reduce the reception flag until it reaches zero
                        if (Reception_Flag > 0) {
                              Reception_Flag--;
                        }
                  }  // end of debouncing audio

                  // after half of a second of audio, move to System_Status 1
                  if (Reception_Flag >= 5) {
                        System_Status = 1;
                  }
            }
      }  // idle and listening
      // to 1 when triggered

      if (System_Status == 1) {
            while (System_Status = 1) {

                  // if the line is quiet for a full second
                  delay(Audio_Confirmation_Delay);
                  current_audio_level = analogRead(pin_audio_input);
                  if (current_audio_level < Audio_Input_Threshold) {
                        // increase tranmission flag
                        Transmission_Flag++;
                  }

                  // but we add to the polite timer
                  Polite_CW_Timer = Polite_CW_Timer + Audio_Confirmation_Delay;

                  // if we get quiet for a full second or we hit the end of the timer
                  if (Transmission_Flag >= 10 || Polite_CW_Timer >= Polite_CW_Delay) {
                        // we move to transmit
                        System_Status = 2;
                  }
            }
      }  // heard something, waiting for polite send
      // to 2

      if (System_Status == 2) {
            digitalWrite(pin_ptt_switch, HIGH);  // activates relay, forcing PTT function
            delay(Audio_Confirmation_Delay);     // pause before send because reasons

            // for each index in the character array
            for (int i = 0; i < Encoded_CW_Length; i++) {

                  if (Encoded_FCC_Call_Sign[i] == '.') {
                        // dit
                        tone(pin_audio_output, tone_hz, Dit_ms);
                        delay(CW_Letter_Gap_ms);
                  } else if (Encoded_FCC_Call_Sign[i] == '-') {
                        // dah
                        tone(pin_audio_output, tone_hz, Dah_ms);
                        delay(CW_Letter_Gap_ms);
                  } else if (Encoded_FCC_Call_Sign[i] == ' ') {
                        // space break
                        // no tone
                        delay(CW_Space_ms);
                  }  // end of tone selection

            }  // end of send CW loop

            delay(Audio_Confirmation_Delay);    // pause before PTT termination
            digitalWrite(pin_ptt_switch, LOW);  // deactivates relay, forcing PTT termination

            System_Status = 3;
      }  // tranmitting CW
      // to 3

      if (System_Status = 3) {
            Long_CW_Timer = 0;
            Reception_Flag = 0;

            while (System_Status == 3) {

                  delay(Audio_Confirmation_Delay);
                  Long_CW_Timer = Long_CW_Timer + Audio_Confirmation_Delay;
                  current_audio_level = analogRead(pin_audio_input);
                  // if we get audio, we add to the reception flag
                  if (current_audio_level > Audio_Input_Threshold) {
                        // increase tranmission flag
                        Reception_Flag++;
                  }

                  // if we get more than 2 minutes of confirmed audio
                  // 2 minutes >> 120 seconds / 0.1 secons per confirmation
                  if (Reception_Flag >= 1200) {
                        // we wait
                        System_Status = 4;
                  }

                  // if we don't have enough chatter
                  if (Long_CW_Timer >= Min_Long_Delay && Reception_Flag < 200) {
                        // we send the system back to idle without a second transmission
                        System_Status = 0;
                  }
            }
      }  // listening for follow up audio after transmission
      // to 4 if there was enough audio or 0 if not enough audio

      if (System_Status = 4) {
            while (System_Status == 4) {

                  delay(Audio_Confirmation_Delay);
                  // maintaining the Long_CW_Timer
                  Long_CW_Timer = Long_CW_Timer + Audio_Confirmation_Delay;
                  if (Long_CW_Timer >= Min_Long_Delay) {
                        // we move to politely transmit
                        System_Status = 5;
                  }
            }
      }  // clearing the minimum delay threshold before moving on
      // to 5

      if (System_Status = 5) {
            Transmission_Flag = 0;
            while (System_Status == 5) {

                  // keeping the Long_CW_Timer going
                  delay(Audio_Confirmation_Delay);
                  Long_CW_Timer = Long_CW_Timer + Audio_Confirmation_Delay;

                  current_audio_level = analogRead(pin_audio_input);

                  // if there's no audio, add to transmission flag
                  if (current_audio_level < Audio_Input_Threshold) {
                        // increase tranmission flag
                        Transmission_Flag++;
                  }

                  // if we get quiet for a full second
                  if (Transmission_Flag >= 10) {
                        // we move to transmit
                        System_Status = 2;
                  }

                  // if we hit the end of the timer
                  if (Long_CW_Timer >= Max_Long_Delay) {
                        // we move to transmit
                        System_Status = 2;
                  }
            }
      }  // waiting for polite transmit after minimum time and before maximum time
      // to 2 for transmit

}  // end of primary loop
