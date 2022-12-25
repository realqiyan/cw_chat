#ifndef _MORSE_CODE_H
#define _MORSE_CODE_H
#define MORSE_CODE_SIZE 54

char getLetter(String val);
String getMorseCode(char val);

struct MorseCode {
  //字符
  char letter;
  //莫尔斯码
  String code;
};

MorseCode morseCodeArr[] = {
  { 'A', ".-" },
  { 'B', "-..." },
  { 'C', "-.-." },
  { 'D', "-.." },
  { 'E', "." },
  { 'F', "..-." },
  { 'G', "--." },
  { 'H', "...." },
  { 'I', ".." },
  { 'J', ".---" },
  { 'K', "-.-" },
  { 'L', ".-.." },
  { 'M', "--" },
  { 'N', "-." },
  { 'O', "---" },
  { 'P', ".--." },
  { 'Q', "--.-" },
  { 'R', ".-." },
  { 'S', "..." },
  { 'T', "-" },
  { 'U', "..-" },
  { 'V', "...-" },
  { 'W', ".--" },
  { 'X', "-..-" },
  { 'Y', "-.--" },
  { 'Z', "--.." },
  { '1', ".----" },
  { '2', "..---" },
  { '3', "...--" },
  { '4', "....-" },
  { '5', "....." },
  { '6', "-...." },
  { '7', "--..." },
  { '8', "---.." },
  { '9', "----." },
  { '0', "-----" },
  { '.', ".-.-.-" },
  { ',', "--..--" },
  { '?', "..--.." },
  { '\'', ".----." },
  { '!', "-.-.--" },
  { '/', "-..-." },
  { '(', "-.--." },
  { ')', "-.--.-" },
  { '&', ".- ..." },
  { ':', "---..." },
  { ';', "-.-.-." },
  { '=', "-...-" },
  { '+', ".-.-." },
  { '-', "-....-" },
  { '_', "..--.-" },
  { '"', ".-..-." },
  { '$', "...-..-" },
  { '@', ".--.-." }
};

char getLetter(String val) {
  for (int i = 0; i < MORSE_CODE_SIZE; i++) {
    if (morseCodeArr[i].code == val) {
      return morseCodeArr[i].letter;
    }
  }
  return '*';
}

String getMorseCode(char val) {
  for (int i = 0; i < MORSE_CODE_SIZE; i++) {
    if (morseCodeArr[i].letter == val) {
      return morseCodeArr[i].code;
    }
  }
  return "*";
}
#endif