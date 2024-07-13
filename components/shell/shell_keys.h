#ifndef __SHELL_KEYS_H__
#define __SHELL_KEYS_H__

// Ctrl + D  删除一个字符，相当于通常的Delete键（命令行若无任何字符，则相当于exit；处理多行标准输入时也表示eof）
// Ctrl + H  退格删除一个字符，相当于通常的Backspace键
// Ctrl + U  删除光标之前到行首的字符
// Ctrl + K  删除光标之前到行尾的字符
// Ctrl + C  取消当前行输入的命令，相当于Ctrl + Break
// Ctrl + A  光标移动到行首（Ahead of line），相当于通常的Home键
// Ctrl + E  光标移动到行尾（End of line）
// Ctrl + F  光标向前(Forward)移动一个字符位置
// Ctrl + B  光标往回(Backward)移动一个字符位置
// Ctrl + L  清屏，相当于执行clear命令
// Ctrl + P  调出命令历史中的前一条（Previous）命令，相当于通常的上箭头
// Ctrl + N  调出命令历史中的下一条（Next）命令，相当于通常的上箭头
// Ctrl + R  显示：号提示，根据用户输入查找相关历史命令（reverse-i-search）
// Alt  + F   光标向前（Forward）移动到下一个单词
// Alt  + B   光标往回（Backward）移动到前一个单词
// Ctrl + W  删除从光标位置前到当前所处单词（Word）的开头
// Alt  + D   删除从光标位置到当前所处单词的末尾
// Ctrl + Y  粘贴最后一次被删除的单词

/*
 *  Key Binding   Editor Action
 *     Ctrl A     Move cursor to start of the line
 *     Ctrl B     Move left one character
 *     Ctrl D     Delete a single character at the cursor position
 *     Ctrl E     Move cursor to end of current line
 *     Ctrl F     Move right one character
 *     Ctrl H     Delete character, left
 *     Ctrl K     Delete to the end of the line
 *     Ctrl U     Delete the entire line
 */

#define KEY_TAB         0x09000000  // \t
#define KEY_DELETE      0x1B5B337E  // Delete
#define KEY_BACKSPACE   0x08000000  // Backspace
#define KEY_ENTER_LF    0x0A000000  // \r
#define KEY_ENTER_CR    0x0D000000  // \n
#define KEY_ENTER_CRLF  0x0D0A0000  // \r\n

#define KEY_CTRL_A      0x01000000  // Ctrl+A
#define KEY_CTRL_B      0x02000000  // Ctrl+B
#define KEY_CTRL_D      0x04000000  // Ctrl+D
#define KEY_CTRL_E      0x05000000  // Ctrl+E
#define KEY_CTRL_F      0x06000000  // Ctrl+F
#define KEY_CTRL_H      0x7F000000  // Ctrl+H
#define KEY_CTRL_K      0x0B000000  // Ctrl+K
#define KEY_CTRL_U      0x15000000  // Ctrl+U

#define KEY_UP_ARROW    0x1B5B4100  // \e[A
#define KEY_DOWN_ARROW  0x1B5B4200  // \e[B
#define KEY_RIGHT_ARROW 0x1B5B4300  // \e[C
#define KEY_LEFT_ARROW  0x1B5B4400  // \e[D

#endif  // !__SHELL_KEYS_
